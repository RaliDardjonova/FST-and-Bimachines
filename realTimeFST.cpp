#include<stdio.h>
#include<unordered_set>
#include<string>
#include<functional>
#include<unordered_map>
#include<vector>
#include<algorithm>
#include <boost/functional/hash.hpp> // boos::hash_combine

struct Edge{
    std::string firstTape;
    std::string secondTape;
    int end;

    Edge(){
        firstTape = "";
        secondTape = "";
        end = 0;
    }

    Edge(std::string otherFirstTape, std::string otherSecondTape, int otherEnd){
        firstTape = otherFirstTape;
        secondTape = otherSecondTape;
        end = otherEnd;
    }

    Edge(const Edge& other){
        firstTape = other.firstTape;
        secondTape = other.secondTape;
        end = other.end;
    }

    bool operator==(const Edge& other)const{
        return (firstTape == other.firstTape) &&
                (secondTape == other.secondTape) &&
                (end == other.end);
    }

    bool operator<(const Edge& other)const{
        if(end < other.end){
            return true;
        } else if(end > other.end){
            return false;
        }

        if(firstTape < other.firstTape){
            return true;
        } else if(firstTape > other.firstTape){
            return false;
        }

        if(secondTape < other.secondTape){
            return true;
        } else if(secondTape > other.secondTape){
            return false;
        }
        return false;
    }
};

struct Transducer{
    std::unordered_set<char> alphabet;
    std::unordered_set<int> states;
    std::unordered_set<int> init;
    std::unordered_set<int> fin;
    std::unordered_map<int, std::vector<Edge>> delta;

    Transducer(){

    }

    Transducer(const Transducer& other){
        alphabet = other.alphabet;
        init = other.init;
        fin = other.fin;
        states = other.states;
        delta = other.delta;
    }

    Transducer& operator=(const Transducer& other){
        if(this != &other){
            alphabet = other.alphabet;
            init = other.init;
            fin = other.fin;
            delta = other.delta;
            states = other.states;
        }

        return *this;
    }

    void insertState(int start, std::vector<Edge> edges){
        states.insert(start);
        delta.insert(std::make_pair(start, edges));
    }

    void insertEmptyState(int start){
        states.insert(start);
        std::vector<Edge> empty;
        delta.insert(std::make_pair(start, empty));
    }

    void addInitialState(int initialState){
        init.insert(initialState);
    }

    void addFinalState(int finalState){
        fin.insert(finalState);
    }

    void insertEdge(int start, std::string firstTape, std::string secondTape, int end){
        Edge edge = Edge(firstTape, secondTape, end);
        //std::vector<Edge> edges;

        if(delta.find(start) == delta.end()){
            insertEmptyState(start);
        }
        delta.at(start).push_back(edge);
    }

    void insertEdge(int start, Edge edge){
        if(delta.find(start) == delta.end()){
            insertEmptyState(start);
        }
        delta.at(start).push_back(edge);
    }

    std::unordered_set<int> getStates(){
        std::unordered_set<int> states;
        for(auto& state : delta){
            states.insert(state.first);
        }
        return states;
    }

    void printEdges(){
        for(auto& state: delta){
            for(auto& edge : state.second){
                printf("<%d, %s, %s, %d>\n", state.first, edge.firstTape.c_str(), edge.secondTape.c_str(), edge.end);
            }
        }
    }

    void printStates(){
        for(auto& state: states){
            printf("%d ", state);
        }
    }

    size_t numStates(){
        return delta.size();
    }
};

std::unordered_set<int> kNewStates(int k, int from){
    std::unordered_set<int> result;
    for(int i=from+1; i<from+k+1; i++){
        result.insert(i);
    }
    return result;
}

Transducer remap(Transducer& t, int lastState){
    //std::unordered_set<int> states = kNewStates(t.delta.size(), lastState);
    Transducer newT = Transducer();
    newT.alphabet = t.alphabet;

    for(auto& state: t.init){
        newT.init.insert(state + lastState);
    }

    for(auto& state: t.fin){
        newT.fin.insert(state + lastState);
    }

    for(auto& state: t.states){
        newT.states.insert(state + lastState);
    }

    std::unordered_map<int, std::vector<Edge>> newDelta;
    for(auto& state: t.delta){
        std::vector<Edge> newEdges;
        for(auto& edge : state.second){
            newEdges.push_back(Edge(edge.firstTape, edge.secondTape, edge.end + lastState));
        }
        newT.delta.insert(std::make_pair(state.first + lastState, newEdges));
    }

    return newT;
}

Transducer remap2(Transducer& t, int lastState){
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

std::unordered_map<int, std::vector<int>> edgesWithoutLabels(std::unordered_map<int, std::vector<Edge>> rel)
{
    std::unordered_map<int, std::vector<int>> result;

    for(auto& state : rel){
        for(auto& edge : state.second){
            if(result.find(state.first) == result.end()){
                std::vector<int> empty;
                result.insert(std::make_pair(state.first, empty));
            }
            result.at(state.first).push_back(edge.end);
        }
    }
    return result;
}

template <class T>
std::vector<std::pair<int, T>> mapToVector(std::unordered_map<int, std::vector<T>> rel){
    std::vector<std::pair<int, T>> result;

    for(auto& state : rel){
        for(auto& edge : state.second){
            //printf("%d %d\n", state.first, edge);
            result.push_back(std::make_pair(state.first, edge));
        }
    }
    return result;
}

template <class T>
std::unordered_map<int, std::vector<T>> vectorToMap(std::vector<std::pair<int, T>> rel){
    std::unordered_map<int, std::vector<T>> result;

    for(auto& edge : rel){
        if(result.find(edge.first) == result.end()){
            std::vector<T> empty;
            result.insert(std::make_pair(edge.first, empty));
        }
        result.at(edge.first).push_back(edge.second);
    }
    return result;
}

std::unordered_map<int, std::vector<int>> transClosure(std::unordered_map<int, std::vector<int>> rel){
    //std::unordered_map<int, std::vector<int>> rel = edgesWithoutLabels(relWithLabels);
    std::vector<std::pair<int, int>> closure = mapToVector(rel);
    printf("######\n");
    int i = 0;
    while(i != closure.size()){
        int from = closure[i].first;
        int thru = closure[i].second;
        if(rel.find(thru) != rel.end()){
            for(auto& to : rel.at(thru)){
                std::pair<int, int> edge = std::make_pair(from, to);
                if(std::find(closure.begin(), closure.end(), edge) == closure.end()){
                    closure.push_back(std::make_pair(from, to));
                }
            }
        }
        //printf("---- %d %d %d %d----\n", from, thru, i, closure.size());
        i++;
    }
    return vectorToMap(closure);
}

std::unordered_map<int, std::vector<Edge>> transEpsilonClosure(std::unordered_map<int, std::vector<Edge>> rel){
    std::unordered_map<int, std::vector<Edge>> epsilonRel;
    std::vector<std::pair<int, Edge>> closure;
    std::unordered_map<int, std::vector<Edge>> result;

    for(auto& state : rel){
        for(auto& edge : state.second){
            if(edge.firstTape == "" && edge.secondTape == ""){
                closure.push_back(std::make_pair(state.first, edge));
                if(epsilonRel.find(state.first) == epsilonRel.end()){
                    std::vector<Edge> empty;
                    epsilonRel.insert(std::make_pair(state.first, empty));
                }
                epsilonRel.at(state.first).push_back(edge);
            }
        }
    }
    int i = 0;
    while(i != closure.size()){
        std::pair<int, Edge> p = closure[i];
        if(epsilonRel.find(p.second.end) != epsilonRel.end()){
            for(auto& edge : epsilonRel.at(p.second.end)){
                Edge newEdge = Edge("", "", edge.end);
                std::pair<int, Edge> x = std::make_pair(p.first, newEdge);
                if(std::find(closure.begin(), closure.end(), x) == closure.end()){
                    closure.push_back(x);
                }
            }
        }
        i++;
    }
    for(auto& edge : closure){
        if(result.find(edge.first) == result.end()){
            std::vector<Edge> empty;
            result.insert(std::make_pair(edge.first, empty));
        }
        result.at(edge.first).push_back(edge.second);
    }
    return result;
}

Transducer removeEpsilon(Transducer t){
    std::unordered_map<int, std::vector<Edge>> epsilonClosure = transEpsilonClosure(t.delta);
    for(auto& state : t.states){
        if(epsilonClosure.find(state) == epsilonClosure.end()){
            std::vector<Edge> empty;
            epsilonClosure.insert(std::make_pair(state, empty));
        }
        epsilonClosure.at(state).push_back(Edge("", "", state));
    }

    std::unordered_map<int, std::vector<Edge>> newDelta;
    for(auto& state : t.delta){
        for(auto& edge : state.second){
            if(edge.firstTape != "" || edge.secondTape != ""){

                for(auto& epsilonEdge : epsilonClosure.at(edge.end)){
                    if(newDelta.find(state.first) == newDelta.end()){
                        std::vector<Edge> empty;
                        newDelta.insert(std::make_pair(state.first, empty));
                    }
                    newDelta.at(state.first).push_back(Edge(edge.firstTape, edge.secondTape, epsilonEdge.end));
                }
            }
        }
    }
    Transducer result = Transducer(t);
    result.delta  = newDelta;
    //result.printEdges();
    for(auto& init : t.init){
        for(auto& epsilonEdge : epsilonClosure.at(init)){
            result.init.insert(epsilonEdge.end);
        }
    }
    //result = remap(result, 0);
    return result;
}

std::unordered_set<int> intersectionSets(std::unordered_set<int> first, std::unordered_set<int> second){
    std::unordered_set<int> result;

    for(auto& elem : first){
        if(second.find(elem) != second.end()){
            result.insert(elem);
        }
    }
    return result;
}

Transducer trim(Transducer t){
    std::unordered_map<int, std::vector<int>> closure = transClosure(edgesWithoutLabels(t.delta));
    printf("~~~~~~~~~~~\n");
    Transducer result = Transducer();
    result.alphabet = t.alphabet;

    std::unordered_set<int> reachable;
    std::unordered_set<int> coreachable;

    for(auto& init : t.init){
        if(closure.find(init) != closure.end()){
            for(auto& to : closure.at(init)){
                reachable.insert(to);
            }
        }
        reachable.insert(init);
    }


    for(auto& state : closure){
        for(auto& to : state.second){
            if(t.fin.find(to) != t.fin.end()){
                coreachable.insert(state.first);
            }
        }
    }
    for(auto& fin: t.fin){
        coreachable.insert(fin);
    }

    std::unordered_set<int> intersection = intersectionSets(reachable, coreachable);
    result.states = intersection;

    for(auto& state : t.delta){
        if(intersection.find(state.first) != intersection.end()){
            for(auto& edge : state.second){
                if(intersection.find(edge.end) != intersection.end()){
                    result.insertEdge(state.first, edge);
                }
            }
        }
    }

    result.init = intersectionSets(t.init, intersection);
    result.fin = intersectionSets(t.fin, intersection);

    return remap2(result, 0);
}

std::string sigma(std::string alpha, int i){
    if(i < alpha.size()){
        return std::string(1, alpha[i]);
    }
    return "";
}

Transducer expand(Transducer t){
    Transducer result = Transducer(t);

    std::vector<std::pair<int, Edge>> longEdges;

    for(auto& state : t.delta){
        for(auto& edge : state.second){
            if(edge.firstTape.size() > 1 || edge.secondTape.size() > 1){
                longEdges.push_back(std::make_pair(state.first, edge));
            }
        }
    }

    int i = 0;
    int longEdgeslen = longEdges.size();
    while(i != longEdgeslen){
        int from = longEdges[i].first;
        Edge edge = longEdges[i].second;
        int to = edge.end;
        std::string firstTape = edge.firstTape;
        std::string secondTape = edge.secondTape;
        int maxLen = std::max(firstTape.size(), secondTape.size());

        std::vector<int> newPath;
        std::unordered_set<int> newStates = kNewStates(maxLen - 1, result.states.size());
        newPath.push_back(from);
        newPath.insert(newPath.end(), newStates.begin(), newStates.end());
        newPath.push_back(to);

        result.states.insert(newStates.begin(), newStates.end());
        auto it = std::find(result.delta.at(from).begin(), result.delta.at(from).end(), edge);
        result.delta.at(from).erase(it);

        int lenPath = newPath.size();
        for(int i=0; i< lenPath - 1; i++){
            result.insertEdge(newPath[i], sigma(firstTape, i), sigma(secondTape, i), newPath[i+1]);
        }
        i++;
    }
    return result;
}

std::unordered_map<int, std::vector<Edge>>
transEpsilonUpperTapeClosure(std::unordered_map<int, std::vector<Edge>> epsilonRel, bool& isInf){

    std::vector<std::pair<int, Edge>> closure;
    std::unordered_map<int, std::vector<Edge>> result;

    closure = mapToVector(epsilonRel);

    isInf = 0;
    int i = 0;
    while(i < closure.size() && !isInf){
        int from  = closure[i].first;
        Edge edge = closure[i].second;

        isInf =  (from == edge.end) && edge.firstTape == "";
        if(epsilonRel.find(edge.end) != epsilonRel.end()){
            for(auto& nextEdge : epsilonRel.at(edge.end)){
                Edge newEdge = Edge("", edge.secondTape + nextEdge.secondTape, nextEdge.end);
                std::pair<int, Edge> fullEdge = std::make_pair(from, newEdge);
                if(std::find(closure.begin(), closure.end(), fullEdge) == closure.end()){
                    closure.push_back(fullEdge);
                }
            }
        }
        i++;
    }
    result = vectorToMap(closure);
    return result;
}

Transducer removeUpperEpsilon(Transducer t, std::vector<std::string>& words, bool& isInf){
    //bool isFinite = false;
    std::unordered_map<int, std::vector<Edge>> epsilonRel;
    std::unordered_map<int, std::vector<Edge>> epsilonClosure;
    std::unordered_map<int, std::vector<Edge>> revereseEpsilonClosure;
    Transducer result = Transducer();
    result.alphabet = t.alphabet;
    result.states = t.states;
    result.init = t.init;
    result.fin = t.fin;

    for(auto& state : t.delta){
        for(auto& edge : state.second){
            if(edge.firstTape == ""){
                if(epsilonRel.find(state.first) == epsilonRel.end()){
                    std::vector<Edge> empty;
                    epsilonRel.insert(std::make_pair(state.first, empty));
                }
                epsilonRel.at(state.first).push_back(edge);
            }
        }
    }

    epsilonClosure = transEpsilonUpperTapeClosure(epsilonRel, isInf);

    for(auto& state : t.states){
        if(epsilonClosure.find(state) == epsilonClosure.end()){
            std::vector<Edge> empty;
            epsilonClosure.insert(std::make_pair(state, empty));
        }
        epsilonClosure.at(state).push_back(Edge("", "", state));
    }

    for(auto& state : epsilonClosure){
        for(auto& edge : state.second){
            if(revereseEpsilonClosure.find(edge.end) == revereseEpsilonClosure.end()){
                std::vector<Edge> empty;
                revereseEpsilonClosure.insert(std::make_pair(edge.end, empty));
            }
            Edge reverseEdge = Edge(edge.firstTape, edge.secondTape, state.first);
            revereseEpsilonClosure.at(edge.end).push_back(reverseEdge);
        }
    }

    for(auto& state : epsilonClosure){
        if(t.init.find(state.first) != t.init.end()){
            for(auto& edge : state.second){
                if(t.fin.find(edge.end) != t.fin.end()){
                    words.push_back(edge.secondTape);
                }
            }
        }
    }

    for(auto& state : epsilonClosure){
        if(t.init.find(state.first) != t.init.end()){
            for(auto& edge : state.second){
                if(t.fin.find(edge.end) != t.fin.end()){
                    result.fin.insert(state.first);
                }
            }
        }
    }

    for(auto& state : t.delta){
        for(auto& edge : state.second){
            int from = state.first;
            int to = edge.end;

            if(edge.firstTape != ""){
                //result.insertEdge(from, edge.firstTape, edge.secondTape, to);

                for(auto& reverseEdge : revereseEpsilonClosure.at(from) ){
                    for(auto& forwardEdge : epsilonClosure.at(to)){
                        Edge newEdge = Edge(edge.firstTape,
                                            reverseEdge.secondTape +
                                            edge.secondTape +
                                            forwardEdge.secondTape,
                                            forwardEdge.end);
                        result.insertEdge(reverseEdge.end, newEdge);
                    }
                }

            }
        }
    }

    return result;
}

Transducer realTime(Transducer t, std::vector<std::string>& words, bool& isInf){
    Transducer expandedT = expand(trim(removeEpsilon(trim(t))));
    Transducer result = removeUpperEpsilon(expandedT, words, isInf);
    return result;
}

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
    Transducer t = constructFST("<Ca,ca>(<t,t>|<t,s>)");
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
