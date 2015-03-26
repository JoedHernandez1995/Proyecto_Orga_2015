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

	//Una funcion que retorna el indice de la primera llave que es
	//mayor o igual a la llave que se le esta enviando a la funcion.
	int encontrarLlave(PrimaryKey*);


	//Una funcion para borrar una llave en un subarbol el cual
	//Este nodo es raiz
	void borrar(PrimaryKey*);

	//Una funcion para borrar na llave ubicada en la i-esima posicion
	//en el nodo que es una hoja
	void borrarDeUnaHoja(int);

	//Una funcion para borrar una llave ubicada en la i-esima posicion
	//en el nodo que sea intermedio
	void borrarDeUnIntermedio(int);

	//Una funcion para obtener el anterior de la llave donde la llave
	//esta presenta en la i-esima posicion del nodo
	PrimaryKey* getAnterior(int);

	//Una funcion para obtener el siguiente de la llave donde la llave
	//esta presente en la i-esima posicion de este nodo
	PrimaryKey* getSiguiente(int);

	//Una funcion para llenar el nodo hijo presente en la i-esima
	//posicion en el arreglo hijos[] si ese hijo tiene menos de (grado-1) llaves
	void llenar(int);

	//Una funcion para pedir prestada una llave del nodo hijos[i-1] y 
	//colocarla en el nodo hijos[i]
	void pedirPrestadoIzquierda(int);

	//Una funcion para pedir presta una llave del nodo hijos[i+1] y
	//colocarla en el nodo hijos[i]
	void pedirPrestadoDerecha(int);

	//Una funcion para unir el i-esimo hijo del nodo con el (i+1)iesimo
	//hijo del nodo
	void unir(int);

	friend class ArbolB;
};





