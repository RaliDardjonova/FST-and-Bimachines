#include<stdio.h>
#include<unordered_set>
#include<string>
#include<unordered_map>
#include<vector>

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
