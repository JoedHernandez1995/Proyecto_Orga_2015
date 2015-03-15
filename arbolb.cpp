#include "primaryKey.h"
#include "nodo.h"
#include "arbolb.h"
#include <iostream>
using namespace std;

ArbolB::ArbolB(int degree){
	grado = degree;
	raiz = NULL;
}

void ArbolB::recorrer(){
	if(!(raiz==NULL)){
		raiz->recorrer();
	}
}

Nodo* ArbolB::buscar(PrimaryKey* key){
	return (raiz == NULL)? NULL : raiz->buscar(key);
}
