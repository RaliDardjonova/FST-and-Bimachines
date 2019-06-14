#pragma once
#include<boost/functional/hash.hpp> // boos::hash_combine

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

Transducer squaredOutputTransducer(Transducer t);
std::string commonPrefix(std::string firstWord, std::string secondWord);
std::string remainderSuffix(std::string prefix, std::string word);
std::pair<std::string, std::string>
advance(std::pair<std::string, std::string> firstPair,
        std::pair<std::string, std::string> secondPair);
bool balancible(std::pair<std::string,  std::string> adv);
bool testFunctionality(Transducer t);
