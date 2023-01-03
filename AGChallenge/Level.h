#pragma once
#include <iostream>
#include <vector>
#include "Evaluator.h"
using std::vector;

class Level
{
public:
	Level(int N, CEvaluator& cEvaluator);
	Level(const Level& pcOther);
	~Level();

	void addSolution(vector<int>& solution);
	vector<int> cross(vector<int>& solution);
	void deleteDynamicVect();

	vector<vector<int>>* population;

private:
	void printVect(const vector<int>& v);
	void updateFreq(const vector<int>& solution);
	void createClusters();
	double distanceCalculate(const vector<int>* ci, const vector<int>* cj,const vector<int>* cij);
	double entropy(const vector<int>* cluster);
	int findidex(const vector<vector<int>*> clu, const vector<int>& v);

	vector<vector<int>*> clusters;
	vector<vector<int>>* frequenciesTable;
	const int nrBits;
	mt19937 c_rand_engine;
	CEvaluator& c_evaluator;
};

