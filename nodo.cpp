#include "nodo.h"
#include "primaryKey.h"
#include <iostream>
using namespace std;

Nodo::Nodo(int degree,bool leaf){
	//Copiamos el grado minimo dado y si el nodo es hoja o no
	grado = degree;
	isLeaf = leaf;

	//Reservamos memoria para el maximo numero de posibles
	//llaves y punteros a los hijos
	llaves = new PrimaryKey*[2*grado-1];
	hijos = new Nodo *[2*grado];

	//Inicializamos el numero de llaves en cero
	numLlaves = 0;
}

//Funcion para recorrer todos los nodos en un subarbol cuyo este nodo es raiz
void Nodo::recorrer(){
	int n = numLlaves;
	int i;
	//Hay n llaves y n+1 hijos, recorre
	//a traves de n llaves y los primeros n hijos.
	for(i = 0; i < n; i++){
		//Si este nodo no es una hoja, entonces antes
		//de imprimir llave[i], recorremos el subarbol
		//que su raiz sea el hijo hijos[i]
		if(!isLeaf){
			hijos[i]->recorrer(); //recursion
		}
		cout << "Llave: " << llaves[i]->getLlave();
		cout << "Offset: " << llaves[i]->getOffset();
	}
	if(!isLeaf){
		hijos[i]->recorrer();
	}
}

Nodo* Nodo::buscar(PrimaryKey* key){
	//Encontrar la primera llave mayor o igual a key
	int i = 0;
	int n = numLlaves;
	while((i < n) && (key->getLlave() > llaves[i]->getLlave())){
		i++;
	}
	//Si la llave encontrada es igual a key, retornar este nodo
	if(llaves[i]->getLlave() == key->getLlave()){
		return this;
	}
	//Si la llave no fue encontrada y este nodo es hoja
	if(isLeaf){
		return NULL;
	}
	//Ir al hijo apropiado
	return hijos[i]->buscar(key);

}
