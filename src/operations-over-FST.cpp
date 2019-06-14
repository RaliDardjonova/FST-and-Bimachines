#include "../lib/operations-over-FST.h"

std::unordered_set<int> kNewStates(int k, int from){
    std::unordered_set<int> result;
    for(int i=from+1; i<from+k+1; i++){
        result.insert(i);
    }
    return result;
}

std::unordered_map<int, std::vector<int>> edgesWithoutLabels(std::unordered_map<int, std::vector<Edge>> rel){
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
    std::vector<std::pair<int, int>> closure = mapToVector(rel);
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
    for(auto& init : t.init){
        for(auto& epsilonEdge : epsilonClosure.at(init)){
            result.init.insert(epsilonEdge.end);
        }
    }
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

    return remap(result, 0);
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
