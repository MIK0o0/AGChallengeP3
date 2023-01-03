#include "Triangle.h"
#include <unordered_map>

Triangle::Triangle(int iIndex, int jIndex, CMySmartPointer< vector<int>>& nci, 
	CMySmartPointer< vector<int>>& ncj, const vector<vector<int>>* freqTab):ci(nci),cj(ncj),cij(new vector<int>)
{
	ciIndex = iIndex;
	cjIndex = jIndex;
	frequenciesTable = freqTab;
	//tworzenie po³¹czonego zbioru
	//std::cout << "Laczê i liczê" << std::endl;
	cij->insert(cij->end(), ci->begin(), ci->end());
	cij->insert(cij->end(), cj->begin(), cj->end());

	entropyCi = entropy(ci.getPtr());
	entropyCj = entropy(cj.getPtr());
	entropyCij = entropy(cij.getPtr());

	distance = distanceCalculate();

}
Triangle::Triangle(int iIndex, int jIndex, CMySmartPointer< vector<int>>& nci, 
	CMySmartPointer< vector<int>>& ncj,
	double entropyCI, double entropyCJ, const vector<vector<int>>* freqTab)
	:ci(nci),cj(ncj),cij(new vector<int>)
{
	ciIndex = iIndex;
	cjIndex = jIndex;
	frequenciesTable = freqTab;
	//tworzenie po³¹czonego zbioru
	//std::cout << "Lacze" << std::endl;
	cij->insert(cij->end(), ci->begin(), ci->end());
	cij->insert(cij->end(), cj->begin(), cj->end());

	//std::cout << "licze" << std::endl;

	entropyCi = entropyCI;
	entropyCj = entropyCJ;
	entropyCij = entropy(cij.getPtr());

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

Triangle::~Triangle()
{
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