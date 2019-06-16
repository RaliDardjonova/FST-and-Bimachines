#include "../lib/bimachine.h"

Bimachine FSTtoBM(Transducer t){
    Bimachine bm = Bimachine();
    bm.leftA.alphabet = t.alphabet;
    bm.rightA.alphabet = t.alphabet;
    bool isInf;
    std::unordered_set<std::string> words;
    Transducer realTimeT = realTime(t, words, isInf);
    std::unordered_map<int, std::vector<std::pair<char, int>>> reverseDelta;

    // get reverse relation
    for(auto& state : realTimeT.delta){
        for(auto& edge : state.second){
            if(reverseDelta.find(edge.end) == reverseDelta.end()){
                std::vector<std::pair<char, int>> empty;
                reverseDelta.insert(std::make_pair(edge.end, empty));
            }
            reverseDelta.at(edge.end).push_back(std::make_pair(edge.firstTape[0], state.first));
        }
    }

    // construct the right automaton of the bimachine
    std::vector<std::unordered_set<int>> rightStates;
    std::unordered_map<std::unordered_set<int>, int, set_hash> rightStateIndices;
    rightStates.push_back(realTimeT.fin);
    rightStateIndices.insert(std::make_pair(realTimeT.fin, 1));

    int rightStateIndex = 2;
    int i = 0;
    while( i != rightStates.size()){
        std::unordered_map<char, std::unordered_set<int>> newRightEdges;
        std::unordered_set<int> currState = rightStates[i];

        // for every symbol get all destination states
        // with this labels in the reverse relation
        for(auto& state : currState){
            if(reverseDelta.find(state) != reverseDelta.end()){
                for(auto& edge : reverseDelta.at(state)){
                    if(newRightEdges.find(edge.first) == newRightEdges.end()){
                        std::unordered_set<int> empty;
                        newRightEdges.insert(std::make_pair(edge.first, empty));
                    }
                    newRightEdges.at(edge.first).insert(edge.second);
                }
            }
        }

        // insert states that are not already in rightStates
        for(auto& edge : newRightEdges){
            auto it = std::find_if(rightStates.begin(), rightStates.end(),
            [&edge](const auto& elem){ return elem ==  edge.second;});

            if(it == rightStates.end()){
                rightStates.push_back(edge.second);
                rightStateIndices.insert(std::make_pair(edge.second, rightStateIndex));
                rightStateIndex++;
            }
        }

        for(auto& edge : newRightEdges){
            for(auto& state: edge.second){
            }
            bm.rightA.insertEdge(i+1, edge.first, rightStateIndices.at(edge.second));
        }
        i++;
    }

    // construct the left automaton of the bimachine
    std::unordered_map<int, std::unordered_set<std::pair<int, char>, pair_state_hash>> reverseRightDelta;

    for(auto& edge : bm.rightA.delta){
        int from = edge.first.first;
        char label = edge.first.second;
        int to = edge.second;
        if(reverseRightDelta.find(to) == reverseRightDelta.end()){
            std::unordered_set<std::pair<int, char>, pair_state_hash> empty;
            reverseRightDelta.insert(std::make_pair(to, empty));
        }
        reverseRightDelta.at(to).insert(std::make_pair(from, label));
    }

    // reform the delta function to be of the form char -> ( state -> (state, word))
    std::unordered_map<char, std::unordered_map<int, std::unordered_set<std::pair<int, std::string>, pair_int_string_hash>>> reformedDelta;
    for(auto& state : realTimeT.delta){
        for(auto& edge : state.second){
            char c = edge.firstTape[0];
            int from = state.first;
            if(reformedDelta.find(c) == reformedDelta.end()){
                std::unordered_map<int, std::unordered_set<std::pair<int, std::string>, pair_int_string_hash>> empty_map;
                reformedDelta.insert(std::make_pair(c, empty_map));
            }
            if(reformedDelta.at(c).find(from) == reformedDelta.at(c).end()){
                std::unordered_set<std::pair<int, std::string>,  pair_int_string_hash> empty_set;
                reformedDelta.at(c).insert(std::make_pair(from, empty_set));
            }
            reformedDelta.at(c).at(from).insert(std::make_pair(edge.end, edge.secondTape));
        }
    }

    std::vector<leftState> leftStates;
    std::unordered_map<leftState, int, leftState_hash> leftStateIndices;
    std::unordered_map<int, int> selector;

    for(auto& state : rightStates){
        for(auto& init : realTimeT.init){
            if(state.find(init) != state.end()){
                selector.insert(std::make_pair(rightStateIndices.at(state), init));
                break;
            }
        }
    }

    leftStates.push_back(std::make_pair(realTimeT.init, selector));
    leftStateIndices.insert(std::make_pair(std::make_pair(realTimeT.init, selector), 1));
    int leftStateIndex = 2;

    int k = 0;
    while(k != leftStates.size()){
        std::unordered_set<int> currState = leftStates[k].first;
        std::unordered_map<int, int> currFunc = leftStates[k].second;
        std::unordered_map<char, leftState> newLeftEdges;
        std::unordered_map<char, std::unordered_map<int, int>> newSelector;

        for(auto& letter : bm.leftA.alphabet){
            if(reformedDelta.find(letter) != reformedDelta.end()){
                std::unordered_set<int> newLeftState;

                for(auto& p : currFunc){
                    if(reverseRightDelta.find(p.first) != reverseRightDelta.end()){
                        for(auto& edge : reverseRightDelta.at(p.first)){
                            if(letter == edge.second){
                                for(auto& to : reformedDelta.at(letter).at(p.second)){
                                    if(rightStates[edge.first - 1].find(to.first) != rightStates[edge.first - 1].end()){

                                        if(newSelector.find(letter) == newSelector.end()){
                                            std::unordered_map<int, int> empty;
                                            newSelector.insert(std::make_pair(letter, empty));
                                        }
                                        newSelector.at(letter).insert(std::make_pair(edge.first, to.first));
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }

                for(auto& state : reformedDelta.at(letter)){
                    if(currState.find(state.first) != currState.end()){
                        for(auto& p : state.second){
                            newLeftState.insert(p.first);
                        }
                    }
                }

                if(newSelector.find(letter) != newSelector.end()){
                    newLeftEdges.insert(
                        std::make_pair(letter,
                                       std::make_pair(newLeftState,
                                                      newSelector.at(letter))));
                }
            }
        }

        //bm.output
        for(auto& edge : newLeftEdges){
            char c = edge.first;
            std::unordered_map<int, int> stateFunc = edge.second.second;
            for(auto& p : stateFunc){
                int rightImage = bm.rightA.delta.at(std::make_pair(p.first, c));
                if(currFunc.find(rightImage) != currFunc.end()){
                    int from = currFunc.at(rightImage);
                    for(auto& to : reformedDelta.at(c).at(from)){
                        if(to.first == p.second){
                            StateTriple triple = StateTriple();
                            triple.leftState = k+1;
                            triple.label = c;
                            triple.rightState = p.first;
                            bm.output.insert(std::make_pair(triple, to.second));
                            break;
                        }
                    }
                }
            }
        }

        //leftStates
        for(auto& edge : newLeftEdges){
            auto it = std::find_if(leftStates.begin(), leftStates.end(),
            [&edge](const auto& elem){ return elem ==  edge.second;});
            if(it == leftStates.end()){
                leftStates.push_back(edge.second);
                leftStateIndices.insert(std::make_pair(edge.second, leftStateIndex));
                leftStateIndex++;
            }
        }

        for(auto& edge : newLeftEdges){
            bm.leftA.insertEdge(k+1, edge.first, leftStateIndices.at(edge.second));
        }
        k++;
    }

    int rightSize = rightStates.size();
    int leftSize = leftStates.size();

    for(int i=1; i<= rightSize; i++){
        bm.rightA.states.insert(i);
    }

    for(int i=1; i<= leftSize; i++){
        bm.leftA.states.insert(i);
    }

    bm.leftA.init = 1;
    bm.rightA.init = 1;
    return bm;
}
