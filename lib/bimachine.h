#pragma once
#include "construct-fst.h"
#include "../lib/operations-over-FST.h"
#include "check-functionality.h"
#include<boost/functional/hash.hpp> // boos::hash_combine

struct pair_state_hash{
    std::size_t operator () (const std::pair<int,char>& p) const {
        std::size_t seed = 0;
        boost::hash_combine(seed, p.first);
        boost::hash_combine(seed, p.second);

        return seed;
    }
};

struct pair_int_string_hash{
    std::size_t operator () (const std::pair<int,std::string>& p) const {
        std::size_t seed = 0;
        boost::hash_combine(seed, p.first);
        boost::hash_combine(seed, p.second);

        return seed;
    }
};

struct StateTriple{
    int leftState;
    char label;
    int rightState;

/*
    StateTriple(){

    }

    StateTriple(int leftOther, char labelOther, int rightOther){
        leftState = leftOther;
        label = labelOther;
        rightState =
    }
*/

    bool operator==(const StateTriple& other) const{
        return (leftState == other.leftState) &&
                (label == other.label) &&
                (rightState == other.rightState);
    }
};

struct triple_state_hash{
    std::size_t operator () (const StateTriple& p) const {
        std::size_t seed = 0;
        boost::hash_combine(seed, p.leftState);
        boost::hash_combine(seed, p.label);
        boost::hash_combine(seed, p.rightState);

        return seed;
    }
};

struct set_hash{
    std::size_t operator () (const std::unordered_set<int>& s) const {
        std::size_t seed = 0;
        std::set<int> ordered;

        for(auto& elem : s){
            ordered.insert(elem);
        }

        for(auto& elem : ordered){
            boost::hash_combine(seed, elem);
        }

        return seed;
    }
};

typedef std::unordered_map<std::pair<int, char>, int, pair_state_hash> Dtransitions;
typedef std::unordered_map<StateTriple, std::string, triple_state_hash> BMoutput;
typedef std::pair<std::unordered_set<int>, std::unordered_map<int, int>> leftState;

struct leftState_hash{
    std::size_t operator () (const leftState& state) const {
        std::size_t seed = 0;
        std::set<int> ordered_set;
        for(auto& elem : state.first){
            ordered_set.insert(elem);
        }
        for(auto& elem : ordered_set){
            boost::hash_combine(seed, elem);
        }
        std::map<int, int> ordered_map;
        for(auto& p : state.second){
            ordered_map.insert(p);
        }

        for(auto& p : ordered_map){
            boost::hash_combine(seed, p.first);
            boost::hash_combine(seed, p.second);
        }

        return seed;
    }
};

struct DFSA{
    std::unordered_set<char> alphabet;
    std::unordered_set<int> states;
    int init;
    Dtransitions delta;

    void insertEdge(int first, char label, int second){
        std::pair<int, char> p = std::make_pair(first, label);
        delta.insert(std::make_pair(p, second));
    }

    void printEdges(){
        for(auto& edge : delta){
            //printf("deltta.size : %d\n", delta.size());
            //printf("//%s\n", typeid(edge.second).name());
            printf("<%d, %c, %d>\n", edge.first.first, edge.first.second, edge.second);
        }
    }

    std::vector<int> getWordStates(std::string word, bool& isExistant){
        std::vector<int> result;
        result.push_back(1);

        isExistant = true;
        int from = 1;
        int to;
        for(auto& c : word){
            std::pair<int, char> edge = std::make_pair(from, c);
            if(delta.find(edge) != delta.end()){
                to = delta.at(edge);
                result.push_back(to);
                from = to;
            } else {
                isExistant = false;
                break;
            }
        }
        return result;
    }
};

struct Bimachine{
    //std::unordered_set<char> alphabet;
    DFSA leftA;
    DFSA rightA;
    BMoutput output;

    std::string processWord(std::string word, bool& isExistant){
        std::vector<int> leftStates;
        std::vector<int> rightStates;

        bool isLeftExistant;
        bool isRightExistant;
        std::string result;

        std::string forwardWord = word;
        std::string reverseWord = word;
        std::reverse(reverseWord.begin(), reverseWord.end());

        leftStates = leftA.getWordStates(forwardWord, isLeftExistant);
        rightStates = rightA.getWordStates(reverseWord, isRightExistant);
        std::reverse(rightStates.begin(), rightStates.end());

        isExistant = isLeftExistant && isRightExistant;
        if(isExistant == false){
            return result;
        }

        int pathSize = word.size();
        for(int i=0; i< pathSize; i++){
            StateTriple triple = StateTriple();
            triple.leftState = leftStates[i];
            triple.label = word[i];
            triple.rightState = rightStates[i+1];
            result += output.at(triple);
        }

        return result;
    }

    void printFunc(){
        for(auto& triple : output){
            printf("<%d, %c, %d> -> %s\n", triple.first.leftState,
                                           triple.first.label,
                                           triple.first.rightState,
                                           triple.second.c_str());
        }
    }
};

Bimachine FSTtoBM(Transducer t);
