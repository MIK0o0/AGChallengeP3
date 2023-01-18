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
}
Level::Level(const Level& pcOther) :nrBits(pcOther.nrBits), c_evaluator(pcOther.c_evaluator) {
	frequenciesTable = pcOther.frequenciesTable;
	population = pcOther.population;
	random_device c_seed_generator;
	c_rand_engine.seed(c_seed_generator());

}
Level::~Level() {
	vector<int>** c = clusters.data();
	for (size_t i = 0; i < clusters.size(); i++)
	{
		delete *(c + i);
	}
	clusters.clear();
}
void Level::deleteDynamicVect() {
	delete frequenciesTable;
	for (size_t i = 0; i < clusters.size(); i++)
	{
		delete clusters.at(i);
	}
	clusters.clear();
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
	}
}
void Level::addSolution(vector<int>& solution) {
	
	population->push_back(solution);
	updateFreq(solution);
	createClusters();
	
}
vector<int> Level::cross(vector<int>& solution) {
	
	uniform_int_distribution<int> index(0, population->size()-1);

	for (vector<int>* cluster : clusters)
	{	
		vector<int>& donor = population->at(index(c_rand_engine));
		vector<int> copySolution;
		double fitness = c_evaluator.dEvaluate(solution);

		const int* ptrCluster = cluster->data();
		int* ptrSolution = solution.data();
		const int* ptrDonor = donor.data();
		
		bool flagBreak = true;
		for (int i =0 ;i<cluster->size() && flagBreak;i++)
		{
			copySolution.push_back(*(ptrSolution + (*(ptrCluster + i))));
			(*(ptrSolution + (*(ptrCluster + i)))) = (*(ptrDonor + (*(ptrCluster + i))));
			if (c_evaluator.dEvaluate(solution) < fitness)
			{
				for (int r = i; r >= 0; r--) {
					(*(ptrSolution + (*(ptrCluster + r)))) = copySolution.back();
					copySolution.pop_back();
				}
				flagBreak = false;
			}
		}
	}
	
	return solution;
}
void Level::createClusters() {
	
	vector<int>** c = clusters.data();					//usuwamy stare clustery
	for (size_t i = 0; i < clusters.size(); i++)
	{
		delete *(c + i);
	}
	clusters.clear();

	for (int i = 0; i < nrBits; ++i) {
		clusters.push_back(new vector<int>{ i });
	}
	
	vector<vector<Triangle*>> matrix;		//tworzymy macierz wskaŸników trójk¹tów
	matrix.reserve(nrBits * 2);
	
	vector<bool> useful;					//vector u¿ytecznych indexów
	
	int counter = 0;						//liczniik ile jest unmerged klastrów
	
	vector<tuple<double,int ,int , Triangle*>> distanceVect;		// wektor pary dystansu ze wskaŸnikiem
	vector < CMySmartPointer<vector<int>>> allClusters;				//wektor wszystkich clustrów
	allClusters.reserve(nrBits * 2);

	vector<vector<pair<int, unordered_map<int, int>>>> allMaps;		//vector wszystkich mo¿liwych konfiguracji clustrow
	allMaps.reserve(nrBits * 2);

	for (int i = 0; i < nrBits; ++i) {				//wype³niamy macierz pocz¹tkowymi wartoœciami (pojedyñczymi clustrami)
		
		useful.push_back(true);
		counter++;
		matrix.push_back({});

		allClusters.push_back(CMySmartPointer<vector<int>>(new vector<int>{ i }));
		allMaps.push_back({ pair<int, unordered_map< int, int>>(i, {}) });
		allMaps.back().back().second[(*frequenciesTable)[i][i]]++;
		for (int j = i; j < nrBits; j++)
		{	
			matrix[i].push_back(new Triangle(i, j, allClusters.at(i),
					CMySmartPointer<vector<int>>(new vector<int>{ j }), frequenciesTable));
			distanceVect.push_back(tuple<double,int,int, Triangle*>((matrix[i].back())->distance,i,j, matrix[i].back()));
		}
	}
	
	int indexAdd = matrix.size()-1;
	
	while (counter>1)
	{
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
		
		if (minDistance == 0 && !clusters.empty()) {
			clusters.erase(clusters.begin() + findidex(clusters, *(minTriangle->ci)));
			clusters.erase(clusters.begin() + findidex(clusters, *(minTriangle->cj)));
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
		
		vector<Triangle*>* ptrMatrix = matrix.data();
		for (int i = 0;i<mSize-1;i++)
		{
			if (useful.at(i))
			{
				
				(ptrMatrix + i)->push_back(new Triangle(i, indexAdd,allClusters.at(i), minTriangle->cij,
					((ptrMatrix + i)->back())->entropyCi, minTriangle->entropyCij,allMaps.at(i),allMaps.at(i), frequenciesTable));
				distanceVect.push_back(tuple<double,int,int,Triangle*>((ptrMatrix + i)->back()->distance,i,mSize-1, (ptrMatrix + i)->back()));
				
			}
		}
		clusters.push_back(new vector<int>(*(minTriangle->cij)));
		ptrMatrix = matrix.data();
		for (size_t i = 0; i < (ptrMatrix+min_i)->size(); i++)
		{
			delete (ptrMatrix + min_i)->at(i);
		}
		(ptrMatrix + min_i)->clear();
		for (size_t i = 0; i < (ptrMatrix + min_j)->size(); i++)
		{
			delete (ptrMatrix + min_j)->at(i);
		}
		(ptrMatrix + min_j)->clear();
	}
	
	sort(clusters.begin(), clusters.end(), [](const vector<int>* a, const vector<int>* b) {
		return (*a).size() < (*b).size();
		});

	for (size_t i = 0; i < matrix.size(); i++)
	{
		for (size_t j = 0 ; j < matrix.at(i).size(); j++) {
			delete matrix.at(i).at(j);
		}
		matrix.at(i).clear();
	}
	matrix.clear();
	useful.clear();
	distanceVect.clear();

	delete clusters.back();
	clusters.pop_back();
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

