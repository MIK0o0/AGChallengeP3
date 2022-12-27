#include "Level.h"
#include <unordered_map>
#include <numeric>
#include <algorithm>

Level::Level(int N, CEvaluator& cEvaluator):nrBits(N), frequenciesTable(N,vector<int>(N,0)), c_evaluator(cEvaluator){
	random_device c_seed_generator;
	c_rand_engine.seed(c_seed_generator());
}
Level::~Level() {
	population.clear();
	clusters.clear();
	frequenciesTable.clear();
}
void Level::updateFreq(const vector<int>& solution) {
	for (int i = 0; i < nrBits; i++)
	{
		for (int j = 0; j < nrBits; j++) {
			frequenciesTable.at(i).at(j) += (solution.at(i) == solution.at(j));
		}
	}
}
void Level::addSolution(vector<int>& solution) {
	
	population.push_back(solution);
	
	updateFreq(solution);
	
	createClusters();
	
}
vector<int> Level::cross(vector<int>& solution) {
	vector<int> options(nrBits);
	iota(options.begin(), options.end(), 0);
	random_device rd;
	mt19937 g(rd());
	shuffle(options.begin(), options.end(), g);
	//cout << "shuffle end" << endl;
	
	uniform_int_distribution<int> index(0, population.size()-1);

	for (vector<int> cluster : clusters)
	{	
		vector<int>& donor = population.at(index(c_rand_engine));
		vector<int> copySolution = solution;
		double fitness = c_evaluator.dEvaluate(solution);
		for (int i =0 ;i<cluster.size();i++)
		{
			solution.at(cluster.at(i)) = donor.at(cluster.at(i));
			if (c_evaluator.dEvaluate(solution) < fitness)
			{
				for (int r = i; r >= 0; r--) {
					solution.at(cluster.at(r)) = copySolution.at(cluster.at(r));
				}
				break;
			}
		}
	}
	
	return solution;
}
void Level::createClusters() {
	vector<vector<int>> unmerged;
	unmerged.clear();
	clusters.clear();
	for (int i = 0; i < nrBits; ++i) {
		unmerged.push_back({i});
		clusters.push_back({ i });
	}

	while (unmerged.size()>1)
	{
		double minDistance = DBL_MAX;
		int min_i = -1;
		int min_j = -1;
		for (int i = 0; i < unmerged.size(); i++)
		{
			for (int j = i+1; j < unmerged.size(); j++)
			{
				//cout << "i = " << i << "; j = " << j << endl;
				//printVect(unmerged.at(i));
				//cout << "unmerged.size = " << unmerged.size() << endl;
				double distance = distanceCalculate(unmerged.at(i), unmerged.at(j));
				//cout << distance << endl;
				if (distance< minDistance)
				{
					minDistance = distance;
					min_i = i;
					min_j = j;
				}
			}
		}
		//cout << min_i << " ; " << min_j << endl;
		if (min_i >=0 && min_j >=0)
		{
			vector<int> cij;
			cij.insert(cij.end(), unmerged.at(min_i).begin(), unmerged.at(min_i).end());
			cij.insert(cij.end(), unmerged.at(min_j).begin(), unmerged.at(min_j).end());

			if (minDistance == 0 && !clusters.empty()) {
				clusters.erase(remove(clusters.begin(), clusters.end(), unmerged.at(min_i)), clusters.end());
				clusters.erase(remove(clusters.begin(), clusters.end(), unmerged.at(min_j)), clusters.end());
			}

			unmerged.erase(unmerged.begin() + min_j);
			unmerged.erase(unmerged.begin() + min_i);
			unmerged.push_back(cij);
			clusters.push_back(cij);
		}
		
		
	}
	//cout << "while end " << endl;
	sort(clusters.begin(), clusters.end(), [](const vector<int>& a, const vector<int>& b) {
		return a.size() < b.size();
		});

	//cout << "sort end " << endl;
	clusters.pop_back();
	cout << "after creating cluster" << endl;
}
double Level::distanceCalculate(const vector<int>& ci, const vector<int>& cj) {
	double retValue = 0;
	//printVect(ci);
	//printVect(cj);
	//cout  << endl;
	for (int i = 0; i < ci.size(); i++)
	{
		for (int j = 0; j < cj.size(); j++) {
			vector<int> cij;
			cij.insert(cij.end(), ci.begin(), ci.end());
			cij.insert(cij.end(), cj.begin(), cj.end());
			retValue += (2 - ((entropy(ci) + entropy(cj)) / entropy(cij)));
		}
	}
	//cout << "distance calulate end " << endl;
	return retValue / ((double)ci.size() * (double)cj.size());
}
double Level::entropy(const vector<int>& c) {
	double retValue = 0;
	for (int i : c)
	{
		std::unordered_map<int, int> valueCounts;
		for (int j : c) {
			valueCounts[frequenciesTable.at(i).at(j)]++;
		}
		for (  std::pair<const int,int> p : valueCounts)
		{
			double probability = (double)(p.second) / (double)c.size();
			retValue -= probability * log2(probability);
		}
	}
	//cout << retValue << endl;
	return retValue;
}
void Level::printVect(const vector<int>& v) {
	cout << "\n";
	for (int i : v) {
		cout << i << " ; ";
	}
}

