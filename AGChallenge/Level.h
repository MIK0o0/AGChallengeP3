#pragma once
#include <iostream>
#include <vector>
#include "Evaluator.h"
using std::vector;

class Level
{
public:
	Level(int N, CEvaluator& cEvaluator);
	//~Level();
	void addSolution(vector<int>& solution);
	vector<int> cross(vector<int>& solution);
	vector<vector<int>> population;
private:
	const int nrBits;
	void updateFreq(const vector<int>& solution);
	void createClusters();
	CEvaluator& c_evaluator;
	double distanceCalculate(const vector<int>& ci, const vector<int>& cj);
	double entropy(const vector<int>& cluster);
	vector<vector<int>> clusters;
	vector<vector<int>> frequenciesTable;
};

