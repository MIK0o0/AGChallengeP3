#pragma once

#include <vector>
#include <iostream>
#include "Level.h"
#include <unordered_set>
#include "Evaluator.h"

#include <random>


struct VectorHash {
    size_t operator()(const std::vector<int>& v) const {
        std::hash<int> hasher;
        size_t seed = 0;
        for (int i : v) {
            seed ^= hasher(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};
using MySet = std::unordered_set<std::vector<int>, VectorHash>;
using std::unordered_set;
using std::vector;
class Pyramid
{
public:
	Pyramid(int N, CEvaluator& cEvaluator);
	~Pyramid();
	
    void iteration();
    vector<int>& getBestVect() { return currentVectBest; };
    double getBestFitness() { return currentFitnessBest; };

    vector<Level> populations;
    const int nrBits;
private:
    
    void v_fill_randomly(vector<int>& vSolution);
    void hillClimber(vector<int>& solution);
    void printVect(const vector<int>& v);

    vector<int> currentVectBest;
    double currentFitnessBest;
    MySet allSolutions;  //hashset wszystkich rozwi¹zañ
    CEvaluator& c_evaluator;
    mt19937 c_rand_engine;
    int nrOfGenerations;
};

