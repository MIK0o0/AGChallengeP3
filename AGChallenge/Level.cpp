#include "Level.h"
#include <unordered_map>

Level::Level(int N):nrBits(N), frequenciesTable(N,vector<int>(N,0)) {
	
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
void Level::createClusters() {
	vector<vector<int>> unmerged(nrBits);
	for (int i = 0; i < nrBits; ++i) {
		unmerged.push_back({ i });
	}
}
double Level::distanceCalculate(const vector<int>& ci, const vector<int>& cj) {
	double retValue = 0;
	for (int i = 0; i < ci.size(); i++)
	{
		for (int j = 0; j < cj.size(); j++) {
			vector<int> cij;
			cij.insert(cij.end(), ci.begin(), ci.end());
			cij.insert(cij.end(), cj.begin(), cj.end());
			retValue += (2 - ((entropy(ci) + entropy(cj)) / entropy(cij)));
		}
	}
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
		for (  std::pair<const int,int>& p : valueCounts)
		{
			double probability = (double)(p.second) / (double)c.size();
			retValue -= probability * log2(probability);
		}
	}
	return retValue;
}

