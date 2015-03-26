#pragma once

#include <string>
using namespace std;

class PrimaryKey{
private:
	int offset;
	string llavePrimaria;
public:
	PrimaryKey(string,int);
	int getOffset();
	string getLlave();
};


