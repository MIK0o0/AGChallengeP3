#include "Optimizer.h"

#include <cfloat>
#include <iostream>
#include <windows.h>

using namespace std;

COptimizer::COptimizer(CEvaluator &cEvaluator)
	: c_evaluator(cEvaluator)
{
	random_device c_seed_generator;
	c_rand_engine.seed(c_seed_generator());

	d_current_best_fitness = 0;
}
COptimizer::~COptimizer()
{
	delete pyramid;
}
 
void COptimizer::vInitialize()
{	
	pyramid = new Pyramid(c_evaluator.iGetNumberOfBits(), c_evaluator);
	d_current_best_fitness = -DBL_MAX;
	v_current_best.clear();
}//void COptimizer::vInitialize()

void COptimizer::vRunIteration()
{

	pyramid->iteration();

	if (pyramid->getBestFitness() > d_current_best_fitness)
	{
		v_current_best = pyramid->getBestVect();
		d_current_best_fitness = pyramid->getBestFitness();
	}//if (d_candidate_fitness > d_current_best_fitness)
}//void COptimizer::vRunIteration()

