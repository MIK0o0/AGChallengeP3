#pragma once
#include "CRefCounter.h"
template<typename T>
class CMySmartPointer
{
public:
	CMySmartPointer(T* pcPointer);
	CMySmartPointer(const CMySmartPointer<T>& pcOther);
	~CMySmartPointer();
	T& operator*() { return(*pc_pointer); };
	T* operator->() { return(pc_pointer); };
	T* getPtr() { return pc_pointer; };
	void operator=(const CMySmartPointer<T>& ptr);
private:
	CRefCounter* pc_counter;
	T* pc_pointer;
};//class CMySmartPointer


template<typename T>
CMySmartPointer<T>::CMySmartPointer(T* pcPointer)
{
	pc_pointer = pcPointer;
	pc_counter = new CRefCounter();
	pc_counter->iAdd();
}
template<typename T>
CMySmartPointer<T>::CMySmartPointer(const CMySmartPointer<T>& pcOther)
{
	pc_pointer = pcOther.pc_pointer;
	pc_counter = pcOther.pc_counter;
	pc_counter->iAdd();
}
template<typename T>
CMySmartPointer<T>::~CMySmartPointer()
{
	if (pc_counter->iDec() == 0)
	{
		delete pc_pointer;
		delete pc_counter;
		//std::cout << "#";
	}
}
template<typename T>
void CMySmartPointer<T>::operator=(const CMySmartPointer& ptr) {
	if (pc_counter->iDec() == 0)
	{
		delete pc_pointer;
		delete pc_counter;
	}
	pc_pointer = ptr.pc_pointer;
	pc_counter = ptr.pc_counter;
	pc_counter->iAdd();

}
