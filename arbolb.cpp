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
	if(raiz!=NULL){
		raiz->recorrer();
	}
}

Nodo* ArbolB::buscar(PrimaryKey* key){
	return (raiz == NULL)? NULL : raiz->buscar(key);
}

void ArbolB::insertar(PrimaryKey* key){
	//Si el arbol esta vacio
	if(raiz == NULL){
		//Reservamos memoria para la raiz
		raiz = new Nodo(grado,true);
		raiz->llaves[0] = key; //insertamos la llave;
		raiz->numLlaves = 1; //Actualizamos el numero de llaves en raiz
	}else{ //Si el arbol no esta vacio
		//Si la raiz esta llena, entonces el arbol crece un nivel
		if(raiz->numLlaves == 2*grado-1){
			//Reservamos memoria para la nueva raiz
			Nodo* nuevaRaiz = new Nodo(grado,false);
			//Hacemos la vieja raiz como un nodo hijo a la nueva raiz
			nuevaRaiz->hijos[0] = raiz;
			//Partimos la vieja raiz y movemos 1 llave a la nueva raiz
			nuevaRaiz->split(0,raiz);

			//La nueva raiz tiene dos hijos ahora. Decidimos cual de los dos
			//va a tener la nueva llave
			int i = 0;
			if(nuevaRaiz->llaves[0]->getLlave() < key->getLlave()){
				i++;
			}
			nuevaRaiz->hijos[i]->insertarNoLleno(key);

			//Cambiamos raiz;
			raiz = nuevaRaiz;
		} else { // Si la raiz no esta llena, llamamos insertarNoLleno para la raiz
			raiz->insertarNoLleno(key);
		}
	}
}
