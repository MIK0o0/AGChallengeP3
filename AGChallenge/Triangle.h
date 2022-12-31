#pragma once
#include <vector>
#include <iostream>
#include "CMySmartPointer.h"


using std::vector;

class Triangle
{
public:
	Triangle(int iIndex, int jIndex, CMySmartPointer< vector<int>>& nci,
		CMySmartPointer< vector<int>>& ncj, const vector<vector<int>>* freqTab);
	Triangle(int iIndex, int jIndex, CMySmartPointer< vector<int>>& nci,
		CMySmartPointer< vector<int>>& ncj,double entropyCI,double entropyCJ, const vector<vector<int>>* freqTab);
	Triangle(int iIndex, int jIndex, CMySmartPointer< vector<int>>& nci,double entropyCi);
	~Triangle();
	int ciIndex;
	int cjIndex;
	CMySmartPointer< vector<int>> ci;
	CMySmartPointer< vector<int>> cj;
	CMySmartPointer< vector<int>> cij;
	double distance;
	double entropyCi;
	double entropyCj;
	double entropyCij;

private:
	double distanceCalculate();
	double entropy(const vector<int>* cluster);
	const vector<vector<int>>* frequenciesTable;

};

