#pragma once

#include "Evaluator.h"
#include "Pyramid.h" 

#include <random>
#include <vector>

using namespace std;

class COptimizer
{
public:
	COptimizer(CEvaluator &cEvaluator);
	~COptimizer();

	void vInitialize();
	void vRunIteration();

	vector<int> *pvGetCurrentBest() { return &v_current_best; }

private:

	CEvaluator &c_evaluator;
	Pyramid* pyramid;


	double d_current_best_fitness;
	vector<int> v_current_best;

	mt19937 c_rand_engine;
};//class COptimizer