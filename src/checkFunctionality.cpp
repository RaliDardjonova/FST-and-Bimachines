#include<stdio.h>
#include<unordered_set>
#include<string>
#include<unordered_map>
#include<vector>
#include<boost/functional/hash.hpp> // boos::hash_combine
#include "construct-fst.h"
#include "operations-over-FST.h"

struct pair_hash{
    std::size_t operator () (const std::pair<int,int>& p) const {
        std::size_t seed = 0;
        boost::hash_combine(seed, p.first);
        boost::hash_combine(seed, p.second);

        return seed;
    }
};

struct sq_edges_hash{
    std::size_t operator () (const std::pair<std::pair<std::string, std::string>, std::pair<int, int>>& p) const {
        std::size_t seed = 0;
        boost::hash_combine(seed, p.first.first);
        boost::hash_combine(seed, p.first.second);
        boost::hash_combine(seed, p.second.first);
        boost::hash_combine(seed, p.second.first);

        return seed;
    }
};

struct sq_comparator{
    bool operator()(const std::pair<std::pair<std::string, std::string>, std::pair<int, int>>& firstObj,
                    const std::pair<std::pair<std::string, std::string>, std::pair<int, int>>& secondObj) const{
                        if(firstObj.first.first == secondObj.first.first &&
                           firstObj.first.second == secondObj.first.second &&
                           firstObj.second.first == secondObj.second.first &&
                           firstObj.second.second == secondObj.second.second){
                               return true;
                           } else {
                               return false;
                           }
                    }
};

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
    printf("------\n");
    int i = 0;
    while(i != sqStates.size()){
        //printf("## %d %d\n", i, sqStates.size() );
        int firstState = sqStates[i].first;
        int secondState = sqStates[i].second;
        std::vector<Edge> firstEdges;
        std::vector<Edge> secondEdges;
        std::unordered_set<std::pair<std::pair<std::string, std::string>, std::pair<int, int>>, sq_edges_hash, sq_comparator> sqEdges;
        printf("*** %d %d \n", firstState, secondState);

        if(t.delta.find(firstState) != t.delta.end()){
            for(auto& edge : t.delta.at(firstState)){
                firstEdges.push_back(edge);
                printf("edge: %s %s %d\n", edge.firstTape.c_str(), edge.secondTape.c_str(), edge.end);
            }
        }

        if(t.delta.find(secondState) != t.delta.end()){
            for(auto& edge : t.delta.at(secondState)){
                secondEdges.push_back(edge);
            }
        }

        for(auto& firstEdge : firstEdges){
            for(auto& secondEdge : secondEdges){
                printf("^^^ %d %d", firstEdge.end, secondEdge.end);
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
        printf("+++++++++++\n");
        //result.printEdges();

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
    //printf("commonPref: %s\n", commonPref.c_str());
    auto result = std::make_pair(remainderSuffix(commonPref, concatFirst),
                                 remainderSuffix(commonPref, concatSecond));
    //printf("remainderSuffix 1: %s\n", result.first.c_str());
    //printf("remainderSuffix 2: %s\n", result.second.c_str());
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
        printf("i: %d, size: %d\n", i, admissibleAdv.size());
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


int main(){
/*    Transducer t = Transducer();
    std::vector<Edge> emptyEdges;
    t.insertEdge(1, "a", "b", 2);
    t.insertEdge(1, "a", "c", 2);
    t.insertEdge(2, "h", "b", 2);
    t.insertEdge(2, "i", "w", 2);
    t.init.insert(1);
    t.fin.insert(2);
    t.printEdges();
    printf("***********\n" );
    Transducer newT = Transducer();
    newT.insertEdge(1, "a", "b", 2);
    newT.insertEdge(1, "a", "c", 2);
    newT.insertEdge(2, "h", "b", 2);
    newT.insertEdge(2, "i", "w", 2);
    newT.init.insert(1);
    newT.fin.insert(2);
    newT.printEdges();
    printf("***********\n" );
    Transducer unionT = starFST(t);
    unionT.printEdges();
*/

    //Transducer t = constructFST("(((<a,b>*)|<c,ds>)<dd,zz>)*");
    Transducer t = constructFST("<Ca,ca>(<t,t>|<x,s>)");
    //t.printEdges();
    //t.printStates();
    //t.delta = transEpsilonClosure(t.delta);
    //printf("***********\n" );
    //transEpsilonClosure(t.delta).printEdges();
    //printf("***********\n" );
    //t = removeEpsilon(t);
    //t = trim(t);
    //t.printEdges();
    //printf("***********\n" );

    //t = expand(t);
    //t.printEdges();
    //printf("***********\n" );
    //bool isInf = 0;
    //std::vector<std::string> words;
    //t = realTime(t, words, isInf);
    //t.printEdges();
    printf("***********\n" );

    Transducer sqT = squaredOutputTransducer(t);
    sqT.printEdges();
//    t = remap(t, 0);
    //t.printEdges();
    /*std::unordered_map<int, std::vector<int>> closure = transClosure(edgesWithoutLabels(t.delta));

    for(auto& state: closure){
        for(auto& to : state.second){
            printf("<%d, %d>\n", state.first, to);
        }
    }
*/
    for(auto& state: t.states){
        printf("%d ", state);
    }
    printf("--\n" );
    for(auto& init: t.init){
        printf("%d ", init);
    }
    printf("--\n" );
    for(auto& fin: t.fin){
        printf("%d ", fin);
    }

    std::string a = "aasdfz";
    std::string b = "aasdfzq1";

    std::string a2 = "q1wert";
    std::string b2 = "wq1z";

    printf("$$$ %s \n", advance(std::make_pair(a, b), std::make_pair(a2, b2)).second.c_str());

    printf("t is functional: %d\n", testFunctionality(t));
    //t.insertEdge(1, 'a', 'b', 2);
    //t.printEdges();
}
