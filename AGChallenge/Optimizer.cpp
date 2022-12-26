#include "Optimizer.h"

#include <cfloat>
#include <iostream>
#include <windows.h>

using namespace std;

COptimizer::COptimizer(CEvaluator &cEvaluator)
	: c_evaluator(cEvaluator),pyramid(c_evaluator.iGetNumberOfBits(), cEvaluator)
{
	random_device c_seed_generator;
	c_rand_engine.seed(c_seed_generator());

	d_current_best_fitness = 0;
}//COptimizer::COptimizer(CEvaluator &cEvaluator)

void COptimizer::vInitialize()
{	
	
	d_current_best_fitness = -DBL_MAX;
	v_current_best.clear();
	cout << "NOWY#############################################################" << endl;
}//void COptimizer::vInitialize()

void COptimizer::vRunIteration()
{
	pyramid.iteration();

	if (pyramid.getDouble() > d_current_best_fitness)
	{
		v_current_best = pyramid.getVect();
		d_current_best_fitness = pyramid.getDouble();

		cout << d_current_best_fitness << "\n"<<endl;
	}//if (d_candidate_fitness > d_current_best_fitness)
}//void COptimizer::vRunIteration()

