#pragma once
#include <vector>
#include <iostream>


using std::vector;

class Triangle
{
public:
	Triangle(int iIndex, int jIndex,double distance);
	~Triangle();
	int ciIndex;
	int cjIndex;
	double distance;
	vector<int>* cevSum;

private:


};

