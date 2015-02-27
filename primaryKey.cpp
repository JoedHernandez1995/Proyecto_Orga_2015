#include "primaryKey.h"
#include <string>

PrimaryKey::PrimaryKey(string x, int y){

	this->offset=y;
	this->llavePrimaria=x;
}

string PrimaryKey::getLlave(){
	return this->llavePrimaria;
}

int PrimaryKey::getOffset(){
	return this->offset;
}

bool PrimaryKey::operator ==(const PrimaryKey& otro){
	return this->llavePrimaria == otro.llavePrimaria;
}
