#include "Pyramid.h"
#include <cfloat>
#include <iostream>
#include <windows.h>
#include <random>
using namespace std;
Pyramid::Pyramid(int N, CEvaluator& cEvaluator):nrBits(N),c_evaluator(cEvaluator),populations() {
    currentFitnessBest = -DBL_MAX;
    populations.emplace_back(nrBits,cEvaluator);
    random_device c_seed_generator;
    c_rand_engine.seed(c_seed_generator());
    nrOfGenerations = 0;
}
Pyramid::~Pyramid() {
    
    cout <<"Number of iterations : " << nrOfGenerations << endl;
    cout << "Best fitness : " << currentFitnessBest << endl;
    cout << "Best solution : " ;
    printVect(currentVectBest);
    for (size_t i = 0; i < populations.size(); i++)
    {
        populations.at(i).deleteDynamicVect();
        delete populations.at(i).population;
    }
    populations.clear();
    allSolutions.clear();
    currentVectBest.clear();
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
        random_device rd;
        mt19937 g(rd());
        shuffle(options.begin(), options.end(), g);
        for (const int index : options) {
            
            if (tried.count(index) == 0) {
                
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
    //cout << "populations size : " << populations.size() << endl;
    //for (size_t i = 0; i < populations.size(); i++)
    //{
        //cout <<"Liczba osobników na poziomie :"<<i<<" : " << populations.at(i).population->size() << endl;
    //}
    nrOfGenerations++;
    vector<int> solution;
    bool unicat = false;
    if (currentFitnessBest<1)
    {
        while (!unicat)
        {
            v_fill_randomly(solution);

            hillClimber(solution);

            unicat = (allSolutions.count(solution) == 0);
        }
        cout << "after unicat" << endl;
        allSolutions.insert(solution);
        
        populations.at(0).addSolution(solution);

        double fitness = c_evaluator.dEvaluate(solution);
        if (fitness > currentFitnessBest)
        {
            currentVectBest = solution;
            currentFitnessBest = fitness;
        }
        int popSize = populations.size();
        for (int i = 0; i < popSize; i++) {
            vector<int> crossSolution = populations.at(i).cross(solution);
            //cout << "after cross" << endl;
            double crossFitness = c_evaluator.dEvaluate(crossSolution);
            if (fitness < crossFitness && allSolutions.count(crossSolution) == 0)
            {
                if (crossFitness > currentFitnessBest)
                {
                    currentVectBest = crossSolution;
                    currentFitnessBest = crossFitness;
                    cout << "Cross solution gurom" << endl;
                }
                if ((i + 1) >= populations.size())
                {
                    //cout << "Dodaje nowy poziom" << endl;
                    populations.emplace_back(nrBits, c_evaluator);
                }
                //cout << "Przed dodaniem crossSolution do nowego poziomu" << endl;
                populations.at(i + 1).addSolution(crossSolution);
                allSolutions.insert(crossSolution);
            }
            fitness = c_evaluator.dEvaluate(solution);

        }
    }
    
    

}
void Pyramid::printVect(const vector<int>& v) {
    cout << "\n";
    for (int i : v) {
        cout << i ;
    }
}