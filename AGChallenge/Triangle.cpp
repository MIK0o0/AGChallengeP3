#include "Triangle.h"
#include <unordered_map>
using std::pair;

Triangle::Triangle(int iIndex, int jIndex, CMySmartPointer< vector<int>>& nci, 
	CMySmartPointer< vector<int>>& ncj, const vector<vector<int>>* freqTab):ci(nci),cj(ncj),cij(new vector<int>)
{
	ciIndex = iIndex;
	cjIndex = jIndex;
	frequenciesTable = freqTab;

	//tworzenie po³¹czonego zbioru
	cij->insert(cij->end(), ci->begin(), ci->end());
	cij->insert(cij->end(), cj->begin(), cj->end());

	MapCi.push_back(pair<int, unordered_map< int, int>>(ciIndex,{}));
	MapCj.push_back(pair<int, unordered_map< int, int>>(cjIndex, {}));
	MapCi.back().second[(*frequenciesTable)[ciIndex][ciIndex]]++;
	MapCj.back().second[(*frequenciesTable)[cjIndex][cjIndex]]++;

	entropyCi = entropy(ci.getPtr());
	entropyCj = entropy(cj.getPtr());
	entropyCij = entropyMap(cij.getPtr());

	distance = distanceCalculate();

}
Triangle::Triangle(int iIndex, int jIndex, CMySmartPointer< vector<int>>& nci, 
	CMySmartPointer< vector<int>>& ncj,
	double entropyCI, double entropyCJ, vector<pair<int, unordered_map<int, int>>> mapci,
	vector<pair<int, unordered_map<int, int>>> mapcj, const vector<vector<int>>* freqTab)
	:ci(nci),cj(ncj),cij(new vector<int>),MapCi(mapci),MapCj(mapcj)
{
	ciIndex = iIndex;
	cjIndex = jIndex;
	frequenciesTable = freqTab;

	//tworzenie po³¹czonego zbioru	
	cij->insert(cij->end(), ci->begin(), ci->end());
	cij->insert(cij->end(), cj->begin(), cj->end());

	entropyCi = entropyCI;
	entropyCj = entropyCJ;
	entropyCij = entropyMap(cij.getPtr());

	distance = distanceCalculate();

}
Triangle::Triangle(int iIndex, int jIndex, CMySmartPointer< vector<int>>& nci,double entropyCi)
	:ci(nci),cj(nullptr),cij(nullptr)
{
	ciIndex = iIndex;
	cjIndex = jIndex;
	this->entropyCi = entropyCi;
	frequenciesTable = NULL;
}

double Triangle::distanceCalculate() {
	double retValue = 0;

	if (entropyCij == 0)
	{
		retValue += 2;
	}
	else {
		retValue += (2 - (entropyCi + entropyCj) / entropyCij);
	}

	return retValue;
}
double Triangle::entropy(const vector<int>* c) {
	double retValue = 0;
	const vector<int>* ptrRow = frequenciesTable->data();
	for (int i : *c)
	{
		std::unordered_map<int, int> valueCounts;
		const int* ptrKolumn = (ptrRow + i)->data();
		for (int j : *c) {
			valueCounts[*(ptrKolumn + j)]++;
			
		}
		for (std::pair<const int, int> p : valueCounts)
		{
			double probability = (double)(p.second) / (double)(*c).size();
			retValue -= probability * log2(probability);
		}
	}

	return retValue;
}
double Triangle::entropyMap(const vector<int>* c) {
	const vector<int>* ptrRow = frequenciesTable->data();
	double retValue = 0;
	for (pair<int,unordered_map<int,int>> pi: MapCi)
	{
		bool flag = true;
		const int* ptrKolumnPi = (ptrRow + pi.first)->data();
		for (pair<int,unordered_map<int,int>> pj:MapCj)
		{
			if (flag)
			{
				pj.second[(*(ptrRow+pj.first))[pi.first]]++;
				flag = false;
			}
			pi.second[*(ptrKolumnPi + pj.first)]++;
		}
		for (std::pair<const int, int> p : pi.second)
		{
			double probability = (double)(p.second) / (double)(*c).size();
			retValue -= probability * log2(probability);
		}
	}
	for (pair<int, unordered_map<int, int>> pj : MapCj)
	{
		for (std::pair<const int, int> p : pj.second)
		{
			double probability = (double)(p.second) / (double)(*c).size();
			retValue -= probability * log2(probability);
		}
		
	}
	MapCi.insert(MapCi.end(), MapCj.begin(), MapCj.end());
	MapCj.clear();
	return retValue;
}