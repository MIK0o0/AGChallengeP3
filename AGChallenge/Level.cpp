#include "Level.h"
#include <unordered_map>
#include <numeric>
#include <algorithm>

Level::Level(int N, CEvaluator& cEvaluator):nrBits(N), frequenciesTable(N,vector<int>(N,0)), c_evaluator(cEvaluator){
	random_device c_seed_generator;
	c_rand_engine.seed(c_seed_generator());
}
Level::~Level() {
	vector<int>** c = clusters.data();
	for (size_t i = 0; i < clusters.size(); i++)
	{
		delete *(c + i);
	}
	//delete c;
	population.clear();
	clusters.clear();
	frequenciesTable.clear();
}
void Level::updateFreq(const vector<int>& solution) {
	const int* ptrSolution = solution.data();
	vector<int>* ptrRow = frequenciesTable.data();
	for (int i = 0; i < nrBits; i++)
	{
		int* ptrKolumn = (ptrRow + i)->data();
		for (int j = 0; j < nrBits; j++) {
			(*(ptrKolumn + j)) += (*(ptrSolution + i) == *(ptrSolution + j));
		}
	}
}
void Level::addSolution(vector<int>& solution) {
	
	population.push_back(solution);
	
	updateFreq(solution);

	createClusters();
	cout << "after creating cluster" << endl;
	
}
vector<int> Level::cross(vector<int>& solution) {
	/*
	vector<int> options(nrBits);
	iota(options.begin(), options.end(), 0);
	random_device rd;
	mt19937 g(rd());
	shuffle(options.begin(), options.end(), g);
	//cout << "shuffle end" << endl;*/
	
	uniform_int_distribution<int> index(0, population.size()-1);

	for (vector<int>* cluster : clusters)
	{	
		vector<int>& donor = population.at(index(c_rand_engine));
		vector<int> copySolution;
		double fitness = c_evaluator.dEvaluate(solution);

		const int* ptrCluster = cluster->data();
		int* ptrSolution = solution.data();
		const int* ptrDonor = donor.data();
		
		for (int i =0 ;i<cluster->size();i++)
		{
			copySolution.push_back(*(ptrSolution + (*(ptrCluster + i))));
			(*(ptrSolution + (*(ptrCluster + i)))) = (*(ptrDonor + (*(ptrCluster + i))));
			if (c_evaluator.dEvaluate(solution) < fitness)
			{
				for (int r = i; r >= 0; r--) {
					(*(ptrSolution + (*(ptrCluster + r)))) = copySolution.back();
					copySolution.pop_back();
				}
				break;
			}
		}
	}
	
	return solution;
}
void Level::createClusters() {
	vector<int>** c = clusters.data();
	for (size_t i = 0; i < clusters.size(); i++)
	{
		delete *(c + i);
	}
	//delete c;
	clusters.clear();
	vector<vector<int>*> unmerged;
	
	
	for (int i = 0; i < nrBits; ++i) {
		unmerged.push_back(new vector<int>{ i });
		clusters.push_back(new vector<int> {i});
	}

	while (unmerged.size()>1)
	{
		double minDistance = DBL_MAX;
		int min_i = -1;
		int min_j = -1;
		vector<int>** indexUnmerged = unmerged.data();
		vector<int> min_cij;
		for (int i = 0; i < unmerged.size(); i++)
		{
			for (int j = i+1; j < unmerged.size(); j++)
			{
				//cout << "i = " << i << "; j = " << j << endl;
				//printVect(unmerged.at(i));
				// 
				vector<int>* cij = new vector<int>;
				(*cij).insert((*cij).end(), (*(*(indexUnmerged + i))).begin(), (*(*(indexUnmerged + i))).end());
				(*cij).insert((*cij).end(), (*(*(indexUnmerged + j))).begin(), (*(*(indexUnmerged + j))).end());
				//cout << "unmerged.size = " << unmerged.size() << endl;
				double distance = distanceCalculate(*(indexUnmerged + i), *(indexUnmerged + j),cij);
				//cout << distance << endl;
				if (distance< minDistance)
				{
					minDistance = distance;
					min_i = i;
					min_j = j;
					min_cij = *cij;
				}
				delete cij;
			}
		}
		//cout << min_i << " ; " << min_j << endl;
		if (min_i >=0 && min_j >=0)
		{
			if (minDistance == 0 && !clusters.empty()) {

				clusters.erase(clusters.begin()+findidex(clusters, (*(*(indexUnmerged + min_i)))));
				clusters.erase(clusters.begin() + findidex(clusters, (*(*(indexUnmerged + min_j)))));
			}

			unmerged.erase(unmerged.begin() + min_j);
			unmerged.erase(unmerged.begin() + min_i);
			unmerged.push_back(new vector<int> (min_cij));
			clusters.push_back(new vector<int>(min_cij));
		}
		min_cij.clear();
		
	}
	
	
	sort(clusters.begin(), clusters.end(), [](const vector<int>* a, const vector<int>* b) {
		return (*a).size() < (*b).size();
		});

	
	clusters.pop_back();
	
	//delete unmerge:
	vector<int>** u = unmerged.data();
	for (size_t i = 0; i < unmerged.size(); i++)
	{
		delete* (u + i);
	}
	//delete u;
	unmerged.clear();
}
double Level::distanceCalculate(const vector<int>* ci, const vector<int>* cj, const vector<int>* cij) {
	double retValue = 0;
	//printVect(ci);
	//printVect(cj);
	//cout  << endl;
	
			int divideNumber = entropy(cij);
			if (divideNumber == 0)
			{
				retValue += 2;
			}
			else {
				retValue += (2 - ((entropy(ci) + entropy(cj)) / divideNumber));
			}
		
	//cout << "distance calulate end " << endl;
	return retValue;
}
double Level::entropy(const vector<int>* c) {
	double retValue = 0;
	vector<int>* ptrRow = frequenciesTable.data();
	for (int i : *c)
	{
		std::unordered_map<int, int> valueCounts;
		int* ptrKolumn = (ptrRow + i)->data();
		for (int j : *c) {
			valueCounts[*(ptrKolumn + j)]++;
		}
		for (  std::pair<const int,int> p : valueCounts)
		{
			double probability = (double)(p.second) / (double)(*c).size();
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
int Level::findidex(const vector<vector<int>*> clu, const vector<int>& v) {
	const vector<int>*const*  ptrC = clu.data();
	for (size_t i = 0; i < clu.size(); i++)
	{
		if (*(*(ptrC + i))==v)
		{
			return i;
		}
	}
	return clu.size();
}

