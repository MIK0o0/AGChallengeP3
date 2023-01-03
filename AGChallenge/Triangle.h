#pragma once
#include <vector>
#include <iostream>
#include "CMySmartPointer.h"
#include <vector>
#include <unordered_map>


using std::vector;
using std::pair;
using std::unordered_map;
class Triangle
{
public:
	Triangle(int iIndex, int jIndex, CMySmartPointer< vector<int>>& nci,
		CMySmartPointer< vector<int>>& ncj, const vector<vector<int>>* freqTab);
	Triangle(int iIndex, int jIndex, CMySmartPointer< vector<int>>& nci,
		CMySmartPointer< vector<int>>& ncj,double entropyCI,double entropyCJ,vector<pair<int,unordered_map<int,int>>> mapci,
		vector<pair<int, unordered_map<int, int>>> mapcj, const vector<vector<int>>* freqTab);
	Triangle(int iIndex, int jIndex, CMySmartPointer< vector<int>>& nci,double entropyCi);
	~Triangle();
	int ciIndex;
	int cjIndex;
	CMySmartPointer< vector<int>> ci;
	CMySmartPointer< vector<int>> cj;
	CMySmartPointer< vector<int>> cij;
	vector<pair<int, unordered_map< int, int>>> MapCi;
	vector<pair<int, unordered_map<int, int>>> MapCj;

	double distance;
	double entropyCi;
	double entropyCj;
	double entropyCij;

private:
	double distanceCalculate();
	double entropy(const vector<int>* cluster);
	double entropyMap(const vector<int>* c);
	const vector<vector<int>>* frequenciesTable;

};

