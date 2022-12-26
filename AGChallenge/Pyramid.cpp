#include "Pyramid.h"
using namespace std;
Pyramid::Pyramid(int N, CEvaluator& cEvaluator):nrBits(N),c_evaluator(cEvaluator) {
    currentFitnessBest = -DBL_MAX;
    populations.push_back(Level(nrBits));
}
void Pyramid::v_fill_randomly(vector<int>& vSolution)
{
	uniform_int_distribution<int> c_uniform_int_distribution(iBIT_FALSE, iBIT_TRUE);
	vSolution.resize((size_t)nrBits);

	for (size_t i = 0; i < vSolution.size(); i++)
	{
		vSolution.at(i) = c_uniform_int_distribution(c_rand_engine);
	}
}
void Pyramid::hillClimber(vector<int>& solution) {
    double currentFitness = c_evaluator.dEvaluate(solution);
    vector<int> options(nrBits);
    iota(options.begin(), options.end(), 0);
    double newFitness;
    bool improvement;

    std::unordered_set<int> tried;

    do {
        improvement = false;
        
        shuffle(options.begin(), options.end(), rand);
        for (const int index : options) {
            
            if (tried.count(index) != 0) {
                
                solution.at(index) = (solution.at(index)==1) ? 0:1;
                newFitness = c_evaluator.dEvaluate(solution);
                if (currentFitness < newFitness) {
                    
                    currentFitness = newFitness;
                    improvement = true;
                    tried.clear();
                }
                else {
                    solution.at(index) = (solution.at(index) == 1) ? 0 : 1;
                }
                tried.insert(index);
            } 
        }
    } while (improvement);
}
void Pyramid::iteration() {
    vector<int> solution;
    bool unicat = false;
    while (!unicat)
    {
        v_fill_randomly(solution);
        hillClimber(solution);
        unicat = (allSolutions.count(solution) == 0);
    }
    allSolutions.insert(solution);
    populations.at(0).addSolution(solution);
    double fitness = c_evaluator.dEvaluate(solution);
    if (fitness > currentFitnessBest)
    {
        currentVectBest = solution;
        currentFitnessBest = fitness;
    }
    for (int i = 0;i<populations.size();i++) {
        vector<int> crossSolution = populations.at(i).cross(solution);
        double crossFitness = c_evaluator.dEvaluate(crossSolution);
        if (fitness < crossFitness && allSolutions.count(crossSolution) ==0)
        {
            if (crossFitness >currentFitnessBest)
            {
                currentVectBest = crossSolution;
                currentFitnessBest = crossFitness;
            }
            if (i+1 == populations.size())
            {
                populations.push_back(Level(nrBits));
            }
            populations.at(i + 1).addSolution(crossSolution);
            allSolutions.insert(crossSolution);
        }
    }
    

}