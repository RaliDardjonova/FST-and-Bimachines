#pragma once
#include<algorithm> // std::find
#include "construct-fst.h"

std::unordered_set<int> kNewStates(int k, int from);
std::unordered_map<int, std::vector<int>> edgesWithoutLabels(std::unordered_map<int, std::vector<Edge>> rel);
template <class T>
std::vector<std::pair<int, T>> mapToVector(std::unordered_map<int, std::vector<T>> rel);
template <class T>
std::unordered_map<int, std::vector<T>> vectorToMap(std::vector<std::pair<int, T>> rel);
std::unordered_map<int, std::vector<int>> transClosure(std::unordered_map<int, std::vector<int>> rel);
std::unordered_map<int, std::vector<Edge>> transEpsilonClosure(std::unordered_map<int, std::vector<Edge>> rel);
std::unordered_map<int, std::vector<Edge>>
transEpsilonUpperTapeClosure(std::unordered_map<int, std::vector<Edge>> epsilonRel, bool& isInf);
std::unordered_set<int> intersectionSets(std::unordered_set<int> first, std::unordered_set<int> second);
Transducer removeEpsilon(Transducer t);
Transducer trim(Transducer t);
std::string sigma(std::string alpha, int i);
Transducer expand(Transducer t);
Transducer removeUpperEpsilon(Transducer t, std::unordered_set<std::string>& words, bool& isInf);
Transducer realTime(Transducer t, std::unordered_set<std::string>& words, bool& isInf);
