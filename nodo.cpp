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
	hijos = new Nodo*[2*grado];

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

int Nodo::encontrarLlave(PrimaryKey* key){
	int i = 0;
	while((i < numLlaves) && (llaves[i]->getLlave() < key->getLlave())){
		++i;
	}
	return i;
}

void Nodo::borrar(PrimaryKey* key){
	int i = encontrarLlave(key);

	//La llave que se vaa  borrar esta presente en este nodo
	if( (i < numLlaves) && (llaves[i]->getLlave() == key->getLlave())){
			//Si este nodo es una hoja, llamamos al metodo correspondiente
			//Sino, llamamos al metodo para borrar del intermedio
		if(isLeaf){
			borrarDeUnaHoja(i);
		}else{
			borrarDeUnIntermedio(i);
		}
	}else{
			//Si este nodo es una hoja, entonces la llave no esta presente en el arbol
		if(isLeaf){
			cout << "La llave no esta presente en el arbol"<<endl;
			return; 
		}

			//La llave que se va a quitar esta presente en el sub-arbol el cual
			//este nodo es raiz. La bandera indica si la llave esta presente en el sub-arbol
			//el cual la raiz es el ultimo hijo de este nodo
		bool bandera = ( (i == numLlaves)? true:false);

			//Si el hijo donde la llave deberia de estar tiene menos de "grado" llaves,
			//entonces llenamos ese hijo
		if(hijos[i]->numLlaves < grado){
			llenar(i);
		}

			//Si el ultimo hijo ya fue unido, entonces se debio
			//haber unido con el hijo anterior entonces hacemos recursion con el 
			//i-esimo hijo. Sino, hacemos recursion con el i-esimo hijo
			//el cual ahora tenga al menos "grado" llaves
		if(bandera && i > numLlaves){
			hijos[i-1]->borrar(key);
		}else{
			hijos[i]->borrar(key);
		}
	}
	return;
}

void Nodo::borrarDeUnaHoja(int i){
	//Movemos todas las llaves despues de la i-esima posicion, un lugar hacia atras
	for(int j = i+1; j < numLlaves; ++j){
		llaves[j-1] = llaves[j];
	}
		//Reducimos la cantidad de llaves
	numLlaves--;
	return;
}

void Nodo::borrarDeUnIntermedio(int i){

	PrimaryKey* key = llaves[i];

		//Si el hijo que procede a key tiene al menos "grado"
		//llaves, entonces encontramos el anterior a key en el 
		//subarbol el cual hijos[i] es raiz. Reemplazamos key con 
		//anterior. Borramos el anterior de forma recursiva de hijos[i]

	if(hijos[i]->numLlaves >= grado){
		PrimaryKey* anterior = getAnterior(i);
		llaves[i] = anterior;
		hijos[i]->borrar(anterior);
	} else if(hijos[i+1]->numLlaves >= grado){
		//Si el hijo tiene menos que "grado" llaves, entonces
		//examinar hijos[i+1]. Si hijos[i+1] tiene al menos
		//"grado" llaves, entonces encontrar el siguiente a key
		//en el subarbol en el cual hijos[i+1] es raiz. Borrar
		//de forma recursiva siguiente en hijos[i+1];
		PrimaryKey* siguiente = getSiguiente(i);
		llaves[i] = siguiente;
		hijos[i+1]->borrar(siguiente);
	}else{

		//Si ambos hijos[i] e hijos[i+1] tienen menos de "grado" llaves
		//entonces unimos key y todo lo de hijos[i+1] en hijos[i]
		//hijos[i] va a contener 2*grado-1 llaves
		//liberamos hijos[i+1] y recursivamente borramos key de hijos[i]

		unir(i);
		hijos[i]->borrar(key);
	}
	return;
}

PrimaryKey* Nodo::getAnterior(int i){
	//Seguir moviendose hasta el nodo mas a la derecha hasta llegar a una hoja
	Nodo* temp = hijos[i];
	while(!temp->isLeaf){
		temp = temp->hijos[temp->numLlaves];
	}
	//retornar la ultima llave de la hoja
	return temp->llaves[temp->numLlaves-1];
}

PrimaryKey* Nodo::getSiguiente(int i){
	//Nos movemos hasta el nodo mas a la izquierda empezando desde hijos[i+1]
	//hasta que llegamos a una hoja
	Nodo* temp = hijos[i+1];
	while(!temp->isLeaf){
		temp = temp->hijos[0];
	}
	//retornar la primera llave de la hoja
	return temp->llaves[0];
}

void Nodo::llenar(int i){
	//Si el hijo anterior(hijos[i-1]) tiene mas de "grado-1" llaves,
	//pedimos prestada una llave de ese  hijo.
	if((i != 0) && (hijos[i-1]->numLlaves >= grado)){
		pedirPrestadoIzquierda(i);
	} else if ( (i != numLlaves) && (hijos[i+1]->numLlaves>=grado) ){
	//Si el hijo siguiente(hijos[i+1]) tiene mas de "grado-1" llaves,
	//pedimos prestada una llave de ese  hijo.
		pedirPrestadoDerecha(i);
	}else{
		//Unir hijos[i] con su hermano
		//Si hijos[i] es el ultimo hijo, unirlo con su hermano a la izquierda
		//Sino, unirlo con su hermano a la derecha
		if(i != numLlaves){
			unir(i);
		}else{
			unir(i-1);
		}
	}
	return;
}

void Nodo::pedirPrestadoIzquierda(int i){
	Nodo* hijo = hijos[i];
	Nodo* hermano = hijos[i-1];

	//La ultima llave de hijos[i-1] sube hacia el padre y 
	//llaves[i-1] del padre es insertada como la primera llave en 
	// hijos[i]. El hermano pierde una llave y el hijo gana una

	//Movemos todas las llaves en hijos[i] un paso adelante
	for(int j = hijo->numLlaves-1; j >= 0; --j){
		hijo->llaves[j+1] = hijo->llaves[j];
	}

	//Si hijo[i] no es una hoja, entonces movemos todos los punteros un paso adelante
	if(!hijo->isLeaf){
		for(int j = hijo->numLlaves; j >= 0; --j){
			hijo->hijos[j+1] = hijo->hijos[j];
		}
	}

	//Ponemos la primera llave del hijo igual a llaves[i+1] del nodo actual
	hijo->llaves[0] = llaves[i-1];

	//Movemos el ultimo hijo del hermano como el primer hijo de hijos[i]
	if(!isLeaf){
		hijo->hijos[0] = hermano->hijos[hermano->numLlaves];
	}

	//Movemos la llave del hermano al padre
	//Esto reduce el numero de llaves en el hermano
	llaves[i-1] = hermano->llaves[hermano->numLlaves-1];
	hijo->numLlaves += 1;
	hermano->numLlaves -= 1;
	return;
}

void Nodo::pedirPrestadoDerecha(int i){
	Nodo* hijo = hijos[i];
	Nodo* hermano = hijos[i+1];

	//Llaves[i] es insertada como la ultima llave en hijos[i]
	hijo->llaves[(hijo->numLlaves)] = llaves[i];

	//El primer hijo del hermano es insertado como el ultimo
	//hijo dentro de hijos[i]
	if(!(hijo->isLeaf)){
		hijo->hijos[(hijo->numLlaves)+1] = hermano->hijos[0];
	}

	//La primera llave del hermano es insertada dentro de llaves[i]
	llaves[i] = hermano->llaves[0];

	//Movemos todas las llaves en el hermano una posicion hacia atras
	for(int j = 1; j < hermano->numLlaves; ++j){
		hermano->llaves[j-1] = hermano->llaves[j];
	}

	//Movemos los punteros a los hijos un paso hacia atras
	if(!hermano->isLeaf){
		for(int j = 1; j <= hermano->numLlaves; ++j){
			hermano->hijos[j-1] = hermano->hijos[j];
		}
	}

	//Incrementamos y decrementamos el numero de llaves en 
	//hijos[i] y hijos[i+1]
	hijo->numLlaves += 1;
	hermano->numLlaves -= 1;
	return;
}

void Nodo::unir(int i){
	Nodo* hijo = hijos[i];
	Nodo* hermano = hijos[i+1];

	//Agarrando una llave del nodo actual e insertandola en la
	// (grado-1)iesima position de hijos[i]
	hijo->llaves[grado-1] = llaves[i];

	//Copiando las llaves de hijos[i+1] a hijo[i] al final
	for(int j=0; j<hermano->numLlaves; ++j){
		hijo->hijos[j+grado] = hermano->hijos[j];
	}

	//Copiamos los punteros de los hijos de hijos[i+1] a hijos[i]
	if(!hijo->isLeaf){
		for(int j = 0; j <= hermano->numLlaves; ++j){
			hijo->hijos[j+grado] = hermano->hijos[j];
		}
	}

	//Movemos todas las llaves despues de i en el nodo actual
	//una posicion hacia atras para llenar el agujero creado
	//despues de mover llaves[i] hacia hijos[i]
	for(int j=i+1;j<numLlaves;++j){
		llaves[j-1] = llaves[j];
	}

	//Movemos los punteros despues de (i+1) en el nodo actual
	//un paso hacia atras
	for(int j=i+2; j<=numLlaves; ++j){
		hijos[j-1] = hijos[j];
	}

	//Actualizamos el numero de llaves del hijo y el nodo actual
	hijo->numLlaves += hermano->numLlaves+1;
	numLlaves--;

	//Liberamos la memoria ocupada por el hermano
	delete hermano;
	return;
}

