#include "Level.h"
#include <unordered_map>
#include <numeric>
#include <algorithm>
#include "Triangle.h"
#include "CMySmartPointer.h"
using std::tuple;

Level::Level(int N, CEvaluator& cEvaluator):nrBits(N), c_evaluator(cEvaluator) {
	frequenciesTable = new vector<vector<int>>(nrBits, vector<int>(nrBits, 0));
	population = new vector<vector<int>>;
	
	random_device c_seed_generator;
	c_rand_engine.seed(c_seed_generator());
	//cout << frequenciesTable->size() << endl;

}
Level::Level(const Level& pcOther) :nrBits(pcOther.nrBits), c_evaluator(pcOther.c_evaluator) {
	frequenciesTable = pcOther.frequenciesTable;
	population = pcOther.population;
	random_device c_seed_generator;
	c_rand_engine.seed(c_seed_generator());
	//cout << "Level copy" << endl;

}
Level::~Level() {
	//cout << "destruktor level" << endl;
	vector<int>** c = clusters.data();
	for (size_t i = 0; i < clusters.size(); i++)
	{
		delete *(c + i);
	}
	clusters.clear();
	//population->clear();
	
	//frequenciesTable->clear();
	//delete frequenciesTable;
}
void Level::deleteDynamicVect() {
	delete frequenciesTable;
	for (size_t i = 0; i < clusters.size(); i++)
	{
		delete clusters.at(i);
	}
}
void Level::updateFreq(const vector<int>& solution) {
	const int* ptrSolution = solution.data();
	vector<int>* ptrRow = frequenciesTable->data();
	

	for (int i = 0; i < nrBits; i++)
	{
		int* ptrKolumn = (ptrRow + i)->data();
		for (int j = 0; j < nrBits; j++) {
			(*(ptrKolumn + j)) += (*(ptrSolution + i) == *(ptrSolution + j));
		}
		//printVect(*(ptrRow + i));
	}
}
void Level::addSolution(vector<int>& solution) {
	
	population->push_back(solution);
	updateFreq(solution);
	createClusters();
	//cout << "after creating cluster" << endl;
	
}
vector<int> Level::cross(vector<int>& solution) {
	/*
	vector<int> options(nrBits);
	iota(options.begin(), options.end(), 0);
	random_device rd;
	mt19937 g(rd());
	shuffle(options.begin(), options.end(), g);
	//cout << "shuffle end" << endl;*/
	
	uniform_int_distribution<int> index(0, population->size()-1);

	for (vector<int>* cluster : clusters)
	{	
		vector<int>& donor = population->at(index(c_rand_engine));
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
	//usuwamy stare clustery
	vector<int>** c = clusters.data();
	for (size_t i = 0; i < clusters.size(); i++)
	{
		delete *(c + i);
	}
	clusters.clear();

	for (int i = 0; i < nrBits; ++i) {
		clusters.push_back(new vector<int>{ i });
	}

	//tworzymy macierz wskaŸników trójk¹tów
	vector<vector<Triangle*>> matrix;
	//vector u¿ytecznych indexów
	vector<bool> useful;
	//liczniik ile jest unmerged klastrów
	int counter = 0;
	// wektor pary dystansu ze wskaŸnikiem
	vector<tuple<double,int ,int , Triangle*>> distanceVect;
	vector < CMySmartPointer<vector<int>>> allClusters;
	vector<vector<pair<int, unordered_map<int, int>>>> allMaps;
	//wype³niamy macierz pocz¹tkowymi wartoœciami (pojedyñczymi clustrami)
	for (int i = 0; i < nrBits; ++i) {
		//cout << "first loop" << endl;
		useful.push_back(true);
		counter++;
		matrix.push_back({});

		allClusters.push_back(CMySmartPointer<vector<int>>(new vector<int>{ i }));
		allMaps.push_back({ pair<int, unordered_map< int, int>>(i, {}) });
		allMaps.back().back().second[(*frequenciesTable)[i][i]]++;
		for (int j = 0; j < nrBits; j++)
		{
			if (j>=i)
			{
				//cout << "j>=i+1" << endl;
				matrix[i].push_back(new Triangle(i, j, allClusters.at(i),
					CMySmartPointer<vector<int>>(new vector<int>{ j }), frequenciesTable));
				distanceVect.push_back(tuple<double,int,int, Triangle*>((*matrix[i][j]).distance,i,j, matrix[i][j]));
				
			}
			else
			{	
				//cout << "else" << endl;
				//cout << "po tym sie wywale" << endl;
				matrix[i].push_back(NULL);
				//matrix[i].push_back(new Triangle(i, j, CMySmartPointer<vector<int>>(new vector<int>{ i }),
					//CMySmartPointer<vector<int>>(new vector<int>{ j }), &frequenciesTable));
			}
			
		}
	}
	int indexAdd = matrix.size()-1;
	//cout << "before while" << endl;
	while (counter>1)
	{
		//cout << counter << endl;
		double minDistance;
		int min_i;
		int min_j;
		Triangle* minTriangle;
		
		sort(distanceVect.begin(), distanceVect.end(), [](const tuple<double,int,int, Triangle*> a, const tuple<double, int, int, Triangle*> b) {
			return get<0>(a) > get<0>(b);
			});

		minDistance =get<0>( distanceVect.back());
		minTriangle = get<3>(distanceVect.back());
		min_i = minTriangle->ciIndex;
		min_j = minTriangle->cjIndex;
		distanceVect.pop_back();
		//cout << minTriangle->cij->size()<<endl;
		if (minDistance == 0 && !clusters.empty()) {
			cout << "before erase" << endl;
			clusters.erase(clusters.begin() + findidex(clusters, *(minTriangle->ci)));
			clusters.erase(clusters.begin() + findidex(clusters, *(minTriangle->cj)));
			cout << "after erase" << endl;
		}

		useful[min_i] = false;
		counter--;
		useful[min_j] = false;
		counter--;
		useful.push_back(true);
		distanceVect.erase(remove_if(distanceVect.begin(), distanceVect.end(), [min_i,min_j](tuple<double, int, int, Triangle*> a) {
			return ((get<1>(a) == min_i) || (get<1>(a) == min_j) || (get<2>(a) == min_i) || (get<2>(a) == min_j));
			}),distanceVect.end());
		counter++;
		
		allClusters.push_back(minTriangle->cij);
		allMaps.push_back(minTriangle->MapCi);
		indexAdd++;

		matrix.push_back({});

		matrix.back().push_back(new Triangle(matrix.size()-1, matrix.size() - 1, minTriangle->cij,minTriangle->entropyCij));
		
		int mSize = matrix.size();
		//matrix.erase(remove_if(matrix.begin(), matrix.end(), [min_i,min_j](vector<Triangle*> a) {
			//return (a.back()->ciIndex == min_i || a.back()->ciIndex == min_j);
			//}), matrix.end());
		
		vector<Triangle*>* ptrMatrix = matrix.data();
		for (int i = 0;i<mSize-1;i++)
		{
			if (useful.at(i))
			{
				Triangle** ptrTriangle = (ptrMatrix + i)->data();
				
				(ptrMatrix + i)->push_back(new Triangle(i, indexAdd,allClusters.at(i) /*((ptrMatrix + i)->back())->ci*/, minTriangle->cij,
					((ptrMatrix + i)->back())->entropyCi, minTriangle->entropyCij,allMaps.at(i),allMaps.at(i), frequenciesTable));
				distanceVect.push_back(tuple<double,int,int,Triangle*>((ptrMatrix + i)->back()->distance,i,mSize-1, (ptrMatrix + i)->back()));
				
			}
			else
			{
				
				//(ptrMatrix + i)->push_back(NULL);
			}
		}
		clusters.push_back(new vector<int>(*(minTriangle->cij)));
		for (size_t i = 0; i < matrix.at(min_i).size(); i++)
		{
			delete matrix.at(min_i).at(i);
		}
		matrix.at(min_i).clear();
		for (size_t i = 0; i < matrix.at(min_j).size(); i++)
		{
			delete matrix.at(min_j).at(i);
		}
		matrix.at(min_j).clear();
		//cout << "trzy" << endl;
		
	}
	
	sort(clusters.begin(), clusters.end(), [](const vector<int>* a, const vector<int>* b) {
		return (*a).size() < (*b).size();
		});

	for (size_t i = 0; i < matrix.size(); i++)
	{
		for (size_t j = i + 1; j < matrix.at(0).size(); j++) {
			delete matrix.at(i).at(j);
		}
		matrix.at(i).clear();
	}
	matrix.clear();
	useful.clear();
	distanceVect.clear();

	clusters.pop_back();
}
double Level::distanceCalculate(const vector<int>* ci, const vector<int>* cj, const vector<int>* cij) {
	double retValue = 0;
	//printVect(ci);
	//printVect(cj);
	//cout  << endl;
	
			double divideNumber = entropy(cij);
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
	vector<int>* ptrRow = frequenciesTable->data();
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

