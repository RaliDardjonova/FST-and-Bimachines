#include "construct-fst.h"


Transducer remap(Transducer& t, int lastState){
    std::unordered_map<int, int> map;
    int i = lastState + 1;
    for(auto& state : t.states){
        map.insert(std::make_pair(state, i));
        i++;
    }

    Transducer newT = Transducer();
    newT.alphabet = t.alphabet;

    for(auto& state: t.init){
        newT.init.insert(map.at(state));
    }

    for(auto& state: t.fin){
        newT.fin.insert(map.at(state));
    }

    for(auto& state: t.states){
        newT.states.insert(map.at(state));
    }

    for(auto& state: t.delta){
        std::vector<Edge> newEdges;
        for(auto& edge : state.second){
            newEdges.push_back(Edge(edge.firstTape, edge.secondTape, map.at(edge.end)));
        }
        newT.delta.insert(std::make_pair(map.at(state.first), newEdges));
    }
    return newT;
}

Transducer unionFST(Transducer first, Transducer second){
    Transducer remapSecond = remap(second, first.numStates());
    Transducer unionT = Transducer(first);
    unionT.init.insert(remapSecond.init.begin(), remapSecond.init.end());
    unionT.fin.insert(remapSecond.fin.begin(), remapSecond.fin.end());
    unionT.delta.insert(remapSecond.delta.begin(), remapSecond.delta.end());
    unionT.states.insert(remapSecond.states.begin(), remapSecond.states.end());

    return unionT;
}

Transducer concatFST(Transducer first, Transducer second){
    Transducer remapSecond = remap(second, first.numStates());
    Transducer concatT = Transducer();
    concatT.alphabet = first.alphabet;
    concatT.init = first.init;
    concatT.fin = remapSecond.fin;
    concatT.delta = first.delta;
    concatT.delta.insert(remapSecond.delta.begin(), remapSecond.delta.end());
    concatT.states.insert(first.states.begin(), first.states.end());
    concatT.states.insert(remapSecond.states.begin(), remapSecond.states.end());

    for(auto& finalState : first.fin){
        for(auto& initialState : remapSecond.init){
            concatT.insertEdge(finalState, "", "", initialState);
        }
    }
    return concatT;
}

Transducer starFST(Transducer t){
    int newState = t.numStates() + 1;
    Transducer starT = Transducer();
    starT.alphabet = t.alphabet;
    starT.init.insert(newState);
    starT.fin.insert(t.fin.begin(), t.fin.end());
    starT.fin.insert(newState);
    starT.states.insert(t.states.begin(), t.states.end());
    starT.states.insert(newState);
    starT.delta.insert(t.delta.begin(), t.delta.end());

    for(auto& initState : t.init){
        starT.insertEdge(newState, "", "", initState);
    }

    for(auto& finalState : t.fin){
        starT.insertEdge(finalState, "", "", newState);
    }
    return starT;
}

Transducer initFST(std::string regex){
    //printf("%s\n", regex.c_str());
    int i = 0;
    while(regex[i] != ','){
        i++;
    }
    std::string firstTape = regex.substr(1, i-1);
    std::string secondTape = regex.substr(i+1, regex.size()-i-2);
    //printf("-%s-\n", firstTape.c_str());
    //printf("-%s-\n", secondTape.c_str());
    Transducer result = Transducer();
    result.insertEmptyState(1);
    result.insertEmptyState(2);
    result.addInitialState(1);
    result.addFinalState(2);
    result.insertEdge(1, firstTape, secondTape, 2);
    result.printStates();
    return result;
}

Transducer constructFST(std::string regex){
    printf("%s\n", regex.c_str());
    int flag = 1;
    int scopeDiff = 1;
    int secondStartPos;
    Transducer firstT = Transducer();
    Transducer result = Transducer();

    if(regex.size() == 0){
        Transducer empty = Transducer();
        return empty;
    }

    if(regex[0] == '('){
        int i = 1;
        while(scopeDiff > 0){
            if(regex[i] == '('){
                scopeDiff++;
            }
            if(regex[i] == ')'){
                scopeDiff--;
            }
            i++;
        }


        std::string firstPart = regex.substr(1, i-2);
        //printf("%s\n", firstPart.c_str());

        if(i < regex.size() && regex[i] == '*'){
            firstT = starFST(constructFST(firstPart));
            secondStartPos = i+1;
            //printf("%s\n", firstPart.c_str());
        } else {
            firstT = constructFST(firstPart);
            secondStartPos = i;
            //printf("%s\n", firstPart.c_str());
            //firstT.printEdges();
        }

    } else {
        int i = 1;
        //printf("%c\n", regex[0]);
        if(regex[0] == '<'){
            while(regex[i] != '>'){
                i++;
            }
            i++;
            //int secondStartPos;
            std::string firstPart = regex.substr(0, i);
            //printf("%s\n", firstPart.c_str());
            if(i < regex.size() && regex[i] == '*'){
                firstT = starFST(initFST(firstPart));
                secondStartPos = i+1;
            } else {
                firstT = initFST(firstPart);
                secondStartPos = i;
                //firstT.printEdges();
            }
        } else {
            return Transducer();
        }
    }

    if(secondStartPos < regex.size()){
        if(regex[secondStartPos] == '|'){
            std::string secondPart = regex.substr(secondStartPos+1);
            Transducer secondT = constructFST(secondPart);
            result = unionFST(firstT, secondT);
            //printf("%s\n", secondPart.c_str());
        } else {
            std::string secondPart = regex.substr(secondStartPos);
            Transducer secondT = constructFST(secondPart);
            result = concatFST(firstT, secondT);
            //printf("%s\n", secondPart.c_str());
        }
    } else {
        result = firstT;
    }
    printf("/////////");
    result.printStates();
    return result;
}
