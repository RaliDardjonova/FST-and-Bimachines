#include "../lib/construct-fst.h"
#include "../lib/operations-over-FST.h"
#include "../lib/check-functionality.h"


Transducer squaredOutputTransducer(Transducer t){
    Transducer result = Transducer();
    result.alphabet = t.alphabet;
    std::vector<std::pair<int, int>> sqStates;
    std::unordered_map<std::pair<int, int>, int, pair_hash> mapSquareStates;
    std::unordered_map<int, std::vector<Edge>> sqDelta;

    int firstFreeState = 1;
    for(auto& firstState : t.init){
        for(auto& secondState : t.init){
            sqStates.push_back(std::make_pair(firstState, secondState));
            result.states.insert(firstFreeState);
            mapSquareStates.insert(std::make_pair(std::make_pair(firstState, secondState), firstFreeState));
            firstFreeState++;
        }
    }
    int i = 0;
    while(i != sqStates.size()){
        int firstState = sqStates[i].first;
        int secondState = sqStates[i].second;
        std::vector<Edge> firstEdges;
        std::vector<Edge> secondEdges;
        std::unordered_set<std::pair<std::pair<std::string, std::string>, std::pair<int, int>>, sq_edges_hash, sq_comparator> sqEdges;

        if(t.delta.find(firstState) != t.delta.end()){
            for(auto& edge : t.delta.at(firstState)){
                firstEdges.push_back(edge);
            }
        }

        if(t.delta.find(secondState) != t.delta.end()){
            for(auto& edge : t.delta.at(secondState)){
                secondEdges.push_back(edge);
            }
        }

        for(auto& firstEdge : firstEdges){
            for(auto& secondEdge : secondEdges){
                if(firstEdge.firstTape == secondEdge.firstTape){
                    std::pair<std::string, std::string> label =
                            std::make_pair(firstEdge.secondTape, secondEdge.secondTape);
                    std::pair<int, int> endState =
                            std::make_pair(firstEdge.end, secondEdge.end);


                    sqEdges.insert(std::make_pair(label, endState));

                    if(std::find(sqStates.begin(), sqStates.end(), endState) == sqStates.end()){
                        sqStates.push_back(endState);
                        result.states.insert(firstFreeState);
                        mapSquareStates.insert(std::make_pair(endState, firstFreeState));
                        firstFreeState++;
                    }
                }
            }
        }

        int from = mapSquareStates.at(sqStates[i]);
        for(auto& sqEdge : sqEdges){
            int to = mapSquareStates.at(sqEdge.second);
            std::string firstTape = sqEdge.first.first;
            std::string secondTape = sqEdge.first.second;
            Edge newEdge = Edge(firstTape, secondTape, to);
            result.insertEdge(from, newEdge);
        }
        i++;
    }

    for(auto& state : sqStates){
        int mappedState = mapSquareStates.at(state);

        bool isFirstInit = t.init.find(state.first) != t.init.end();
        bool isSecondInit = t.init.find(state.second) != t.init.end();
        if(isFirstInit && isSecondInit){
            result.init.insert(mappedState);
        }

        bool isFirstFin = t.fin.find(state.first) != t.fin.end();
        bool isSecondFin = t.fin.find(state.second) != t.fin.end();
        if(isFirstFin && isSecondFin){
            result.fin.insert(mappedState);
        }
    }
    return result;
}

std::string commonPrefix(std::string firstWord, std::string secondWord){
    std::string result;
    size_t lenFirst = firstWord.size();
    size_t lenSecond = secondWord.size();

    int i = 0;
    while(i < lenFirst && i < lenSecond){
        if(firstWord[i] == secondWord[i]){
            result.push_back(firstWord[i]);
        } else {
            break;
        }
        i++;
    }
    return result;
}

std::string remainderSuffix(std::string prefix, std::string word){
    size_t lenPrefix = prefix.size();
    return word.substr(lenPrefix);
}

std::pair<std::string, std::string>
advance(std::pair<std::string, std::string> firstPair,
        std::pair<std::string, std::string> secondPair){
    std::string concatFirst = firstPair.first + secondPair.first;
    std::string concatSecond = firstPair.second + secondPair.second;
    std::string commonPref = commonPrefix(concatFirst, concatSecond);
    auto result = std::make_pair(remainderSuffix(commonPref, concatFirst),
                                 remainderSuffix(commonPref, concatSecond));
    return result;
}

bool balancible(std::pair<std::string,  std::string> adv){
    return (adv.first == "") || (adv.second == "");
}

bool testFunctionality(Transducer t){
    std::vector<std::string> epsilonCorrespondingWords;
    bool isInf;
    std::vector<std::pair<int, std::pair<std::string, std::string>>> admissibleAdv;
    Transducer realTimeT = realTime(t, epsilonCorrespondingWords, isInf);
    Transducer squareT = trim(squaredOutputTransducer(realTimeT));

    if(isInf || epsilonCorrespondingWords.size() > 1){
        return false;
    }

    bool isFunc = true;
    for(auto& init : squareT.init){
        admissibleAdv.push_back(std::make_pair(init, std::make_pair("", "")));
    }

    int i = 0;
    while(isFunc && i != admissibleAdv.size()){
        int from = admissibleAdv[i].first;
        std::pair<std::string, std::string> fromAdvances = admissibleAdv[i].second;

        std::vector<std::pair<int, std::pair<std::string, std::string>>> newAdvances;

        if(squareT.delta.find(from) != squareT.delta.end()){
            for(auto& edge : squareT.delta.at(from)){
                newAdvances.push_back(std::make_pair(
                    edge.end,
                    advance(fromAdvances,
                            std::make_pair(edge.firstTape, edge.secondTape))));
            }
        }

        for(auto& adv : newAdvances){
            isFunc = isFunc && balancible(adv.second);
            if(squareT.fin.find(adv.first) != squareT.fin.end()){
                isFunc = isFunc && adv.second.first == "" && adv.second.second == "";
            }

            auto it = std::find_if(admissibleAdv.begin(), admissibleAdv.end(),
            [&adv](const auto& elem){ return elem.first ==  adv.first;});

            if(it != admissibleAdv.end()){
                isFunc = isFunc && (*it).second.first == adv.second.first;
                isFunc = isFunc && (*it).second.second == adv.second.second;
            }
        }

        if(isFunc){
            for(auto& adv : newAdvances){
                auto it = std::find_if(admissibleAdv.begin(), admissibleAdv.end(),
                [&adv](const auto& elem){ return elem.first ==  adv.first;});
                if(it == admissibleAdv.end()){
                    admissibleAdv.push_back(adv);
                }
            }
        }
        i++;
    }
    return isFunc;
}
