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
	vector<Level> populations;
    const int nrBits;
    void iteration();
    vector<int>& getVect() { return currentVectBest; };
    double& getDouble() { return currentFitnessBest; };
private:
    vector<int> currentVectBest;
    double currentFitnessBest;
    MySet allSolutions;
    CEvaluator& c_evaluator;
    void v_fill_randomly(vector<int>& vSolution);
    void hillClimber(vector<int>& solution);
    mt19937 c_rand_engine;

};

