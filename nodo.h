#pragma once
#include <iostream>
#include "primaryKey.h"
using namespace std;
//Clase Nodo del arbol B
class Nodo{
private:
	PrimaryKey** llaves; //un arreglo de punteros a las llaves
	int grado; //el grado minimo del arbol
	Nodo** hijos; //un arreglo de punteros a los hijos
	int numLlaves; //Numero actual de llaves
	bool isLeaf; //Es verdadero cuando el nodo actual es una hoja
public:
	Nodo(int degree,bool leaf); //Constructor
	//Una funcion que inserta una nueva llave en un subarbol de este nodo.
	//Se asume que el nodo aun no esta lleno cuando se llama la funcion
	void insertarNoLleno(PrimaryKey*);
	//Una funcion para partir el hijo "y" de este nodo. i Es el indice de y en
	//el arreglo numLlaves. El hijo y tiene que estar lleno cuando esta funcion se llame 
	void split(int, Nodo*);
	//Una funcion para recorrer los nodos en un subarbol que esta conectado a este nodo
	void recorrer();
	//Una funcion para buscar una llave en un subarbol conectado a este nodo 
	Nodo* buscar(PrimaryKey*); //retorna NULL si la llave no esta presenta
	friend class ArbolB;
};



