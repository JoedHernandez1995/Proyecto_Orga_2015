#pragma once
#include "primaryKey.h"
#include "nodo.h"
#include <iostream>
using namespace std;

class ArbolB{
private:
	Nodo* raiz; //Puntero al nodo raiz
	int grado; //grado minimo
public:
	//Constructor(Inicializa el arbol como vacio)
	ArbolB(int degree);
	//Funcion para recorrer el arbol
	void recorrer();
	//Funcion para buscar una llave en este arbol
	Nodo* buscar(PrimaryKey*);
	//La funcion principal que agrega una nueva llave a este Arbol B
	void insertar(PrimaryKey*);
};
