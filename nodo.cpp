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
	int i;
	//Hay n llaves y n+1 hijos, recorre
	//a traves de n llaves y los primeros n hijos.
	for(i = 0; i < numLlaves; i++){
		//Si este nodo no es una hoja, entonces antes
		//de imprimir llave[i], recorremos el subarbol
		//que su raiz sea el hijo hijos[i]
		if(isLeaf == false){
			hijos[i]->recorrer(); //recursion
		}
		cout << "Llave: " << llaves[i]->getLlave();
		cout << " Offset: " << llaves[i]->getOffset()<<endl;
	}
	if(isLeaf == false){
		hijos[i]->recorrer();
	}
}

Nodo* Nodo::buscar(PrimaryKey* key){
	//Encontrar la primera llave mayor o igual a key
	int i = 0;
	while((i < numLlaves) && (key->getLlave() > llaves[i]->getLlave())){
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

//Esto se llama cuando a un nodo todavia le caben mas llaves por insertar
void Nodo::insertarNoLleno(PrimaryKey* key){
	//Inicializamos el indice como el indice del elemento mas a la derecha
	int i = numLlaves-1;

	//Si este nodo es una hoja
	if(isLeaf){
		//Este while hace dos cosas:
		//a)Encuentra la collection de la nueva llave a ser insertada
		//b)Mueve todas las llaves mayores un lugar hacia adelante
		while(i >= 0 && (llaves[i]->getLlave() > key->getLlave()) ){
			llaves[i+1] = llaves[i];
			i--;
		}

		//Insertamos la nueva llave en la posicion encontrada
		llaves[i+1] = key;
		numLlaves = numLlaves+1;
	} else { //Si este nodo no es una hoja
		//Encontramos el hijo que va a tener la nueva llave
		while(i>=0 && (llaves[i]->getLlave() > key->getLlave())){
			i--;
		}
		//Nos verificamos si el nodo hijo esta lleno
		if(hijos[i+1]->numLlaves == 2*grado-1){
			//Si el hijo esta lleno, entonces lo partimos
			split(i+1,hijos[i+1]);

			//Luego de partirlo, la llave de enmedio de hijos[i] sube un nivel
			//e hijos[i] es partido en dos. Vemos cual de los dos va a tener
			//la nueva llave
			if(llaves[i+1]->getLlave() < key->getLlave()){
				i++;
			}
		}
		hijos[i+1]->insertarNoLleno(key);
	}
}

void Nodo::split(int i, Nodo* y){
		//Creamos un nuevo nodo que va a guardar (grado-1) llaves de y
		Nodo* z = new Nodo(y->grado, y->isLeaf);
		z->numLlaves = grado-1;

		//Copiamos las ultimas (grado-1) llaves de y a z
		for(int j = 0; j < grado-1; j++){
			z->llaves[j] = y->llaves[j+grado];
		}

		//Copiamos los ultimos hijos de y a z
		if(y->isLeaf == false){
			for(int j = 0; j < grado; j++){
				z->hijos[j] = y->hijos[j+grado];
			}
		}

		//Reducimos el numero de llaves en y
		y->numLlaves = grado-1;

		//Como este nodo va a tener un nuevo hijo, entonces
		//creamos espacio para el nuevo hijo
		for(int j = numLlaves; j >= i+1; j--){
			hijos[j+1] = hijos[j];
		}

		//Enlazamos el nuevo hijo con este nodo
		hijos[i+1] = z;

		//Una llave de y se va a mover a este nodo. Encontramos
		//La posicion de la nueva llave y movemos todas las llaves
		//a una posicion adelante
		for(int j = numLlaves-1; j >= i; j--){
			llaves[j+1] = llaves[j];
		}

		//copiamos la llave de enmedio de y a este nodo
		llaves[i] = y->llaves[grado-1];
		//Incrementamos el numero de llaves en este nodo
		numLlaves = numLlaves+1;
	}	
