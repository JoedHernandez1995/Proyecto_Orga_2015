#include <iostream>
#include <stack>
#include <cstdio>
#include <vector>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include "primaryKey.h"
#include "nodo.h"
#include "arbolb.h"

using namespace std;

struct Campo{
	string nombre;
	int tipo;
	int longitud;
	bool isKey;
};

struct Informacion{
	string texto;
	int value;
};

struct Registro{
	vector<Campo> estructura;
	vector<Informacion> informacion;
};


int menu();
int menuIndices();
void cargarIndices(string);
void imprimir(string);
void text2bin(string);
int inicioRegistros(string);
vector<int> cargarAvailList(string);

vector<Campo> cargarEstructura(string);
map<string,PrimaryKey*> indice;

ArbolB btree(16);

int main(int argc, char*argv[]){

	int cantCampos;
	string filename;
	vector<Campo> campos;
	vector<Informacion> info;
	vector<Registro> registros;
	bool continuar = true;
	int useIndex;
	int indexType;
	int numPrimaryKeys = 0;
	while(continuar){
		int opcion = menu();
		if(opcion == 1){
			int usaIndice;
			int tipoIndice;
			cout << "Ingrese el nombre del archivo a abrir: ";
			cin >> filename;
			ifstream fileExists(filename,ios::binary);
			if(fileExists){
				fileExists.seekg(sizeof(int),ios::beg);
				fileExists.read(reinterpret_cast<char*>(&usaIndice),sizeof(usaIndice));
				fileExists.read(reinterpret_cast<char*>(&tipoIndice),sizeof(tipoIndice));
				if(usaIndice == 1){
					if(tipoIndice == 1){
						cargarIndices(filename);
					}else if(tipoIndice == 0){

					}
				}
				cout << "Archivo Abierto!"<<endl;
				fileExists.close();
			}else{
				cout << "El archivo no existe, se creara un archivo nuevo!"<<endl;
				cout << "Desea utilizar indices en este archivo? (1-Si/0-No): ";
				cin >> useIndex;
				if(useIndex == 1){
					cout << "Que tipo de indice le gustaria utilizar? (1-Lineal/0-Arbol B): ";
					cin >> indexType;
				}
				cout << "Cuantos campos desea crear: ";
				cin >> cantCampos;
				for(int i = 0; i < cantCampos; i++){
					Campo camp;
					cout << "Ingrese nombre del campo: ";
					cin >> camp.nombre;
					if(numPrimaryKeys==0){
						int llave;
						cout << "Es llave primaria(1-Si/0-No): ";
						cin >> llave;
						if(llave==0){
							camp.isKey = false;
						}else if(llave==1){
							camp.isKey = true;
						}
						if(!camp.isKey){
							cout << "Ingrese tipo del campo(1-Int/2-Texto): ";
							cin >> camp.tipo;
							if(camp.tipo == 2){
								cout << "Ingrese longitud del texto: ";
								cin >> camp.longitud;
							} else {
								camp.longitud = sizeof(int);
							}
						} else {
							camp.tipo = 2;
							cout << "Ingrese longitud de la llave: ";
							cin >> camp.longitud;
							numPrimaryKeys++;
						}
					}else{
						cout << "Ingrese tipo del campo(1-Int/2-Texto): ";
						cin >> camp.tipo;
						if(camp.tipo == 2){
							cout << "Ingrese longitud del texto: ";
							cin >> camp.longitud;
						} else {
							camp.longitud = sizeof(int);
						}
						camp.isKey = false;
					}
					campos.push_back(camp);
					cout << "Campo Creado!"<<endl;
				}

				/* Crea el encabezado del archivo */
				int position = 0; //primera posicion disponible en el availlist
				ofstream file(filename,ios::binary|ios::app);
				file.write(reinterpret_cast<const char*>(&cantCampos),sizeof(cantCampos)); //Guarda el numero de campos en el encabezado
				file.write(reinterpret_cast<const char*>(&useIndex),sizeof(useIndex)); //Guarda si el archivo utiliza indices o no
				file.write(reinterpret_cast<const char*>(&indexType),sizeof(indexType)); //Guarda que tipo de indice utiliza el archivo
				file.write(reinterpret_cast<const char*>(&position),sizeof(position));//guarda la primera posicion disponible para agregar en el availlist
				const char* pointer = reinterpret_cast<const char*>(&campos[0]);//Guarda la lista de campos con su respectiva informacion
				size_t bytes = campos.size() * sizeof(campos[0]);
				file.write(pointer, bytes);
				file.close();
				fileExists.close();
				if(useIndex == 1){
					if(indexType==1){
						ofstream indexFile("index_"+filename,ios::binary|ios::app);
						indexFile.close();
					}
				}
				campos.clear();
			}

		}

		if(opcion == 2){
			ifstream archivo(filename,ios::binary);
			if(archivo){
				ofstream file2(filename,ios::binary|ios_base::app);
				int useIndex,indexType;
				archivo.seekg(sizeof(int),ios::beg);
				archivo.read(reinterpret_cast<char*>(&useIndex),sizeof(useIndex));
				archivo.read(reinterpret_cast<char*>(&indexType),sizeof(indexType));
				vector<int> availList = cargarAvailList(filename);
				campos = cargarEstructura(filename);
				cout << campos.size() << endl << availList.size()<<endl;
				/*Revisar el availlist */
				if(availList.at(0) == 0){ //Si no hay posiciones que agregue al final
					info.clear();
					string text;
					int data;
					string key;
					int start;
					for(int i = 0; i < campos.size(); i++){
						Informacion temp;
						if(campos.at(i).tipo == 1){
							cout << campos.at(i).nombre << ": ";
							cin >> temp.value;
							temp.texto = "";
							info.push_back(temp);
						}
						if(campos.at(i).tipo == 2){
							cout << campos.at(i).nombre << ": ";
							cin >> temp.texto;
							if(campos.at(i).isKey){
								key = temp.texto;
							}
							temp.value = 0;
							info.push_back(temp);
						}
					}
					/*Escribe al final del archivo*/
					const char* puntero = reinterpret_cast<const char*>(&info[0]);
					size_t cantBytes = info.size() * sizeof(info[0]);
					file2.write(puntero,cantBytes);
					if(useIndex==1){
						if(indexType==1){
							start = ((int) file2.tellp()) - (campos.size()*sizeof(Informacion));//probablemente sumarle 1
							indice.insert(pair<string,PrimaryKey*>(key,new PrimaryKey(key,start)));
							string indexFile = "index_"+filename;
							ofstream index(indexFile,ios::binary|ios::trunc);
							index.close();
							ofstream indice_file(indexFile,ios::binary|ios::app);
							for (map<string,PrimaryKey*>::iterator it=indice.begin(); it!=indice.end(); it++){
								string key = it->first;
								int offset = it->second->getOffset();
								indice_file.write(reinterpret_cast<char*>(&key),sizeof(key));
								indice_file.write(reinterpret_cast<char*>(&offset),sizeof(offset));
							}

						}else if(indexType == 0){
							btree.insertar(new PrimaryKey(key,start));
						}
					}
					cout << "Registro Agregado"<<endl;
					info.clear();
					file2.close();
					archivo.close();
					
				}else{ // Si hay posiciones disponibles,escribe en esa posicion especifica
					info.clear();
					string text;
					int data;
					string key;
					int start;
					for(int i = 0; i < campos.size(); i++){
						Informacion temp;
						if(campos.at(i).tipo == 1){
							cout << campos.at(i).nombre << ": ";
							cin >> temp.value;
							temp.texto = "";
							info.push_back(temp);
						}
						if(campos.at(i).tipo == 2){
							cout << campos.at(i).nombre << ": ";
							cin >> temp.texto;
							if(campos.at(i).isKey){
								key = temp.texto;
							}
							temp.value = 0;
							info.push_back(temp);
						}
					}
					int rrn = availList.at(0);
					int next = availList.at(1);

					int offset = inicioRegistros(filename);
					offset += (rrn-1)*campos.size()*sizeof(Informacion);
					ofstream file2(filename,ios::binary| ios::in|ios::out);

					file2.seekp(sizeof(int)+sizeof(int)+sizeof(int),ios::beg);
					file2.write(reinterpret_cast<char*>(&next),sizeof(next));

					file2.seekp(offset,ios::beg);
					const char* puntero = reinterpret_cast<const char*>(&info[0]);
					size_t cantBytes = info.size() * sizeof(info[0]);

					file2.write(puntero,cantBytes);

					start = ((int)file2.tellp())-(campos.size()*sizeof(Informacion)); //posiblemente sumarle 1
					indice.insert(pair<string,PrimaryKey*>(key,new PrimaryKey(key,start)));

					file2.close();
					archivo.close();

					if(useIndex==1){
						if(indexType==1){
							start = ((int) file2.tellp()) - (campos.size()*sizeof(Informacion));//probablemente sumarle 1
							indice.insert(pair<string,PrimaryKey*>(key,new PrimaryKey(key,start)));
							string indexFile = "index_"+filename;
							ofstream index(indexFile,ios::binary|ios::trunc);
							index.close();
							ofstream indice_file(indexFile,ios::binary|ios::app);
							for (map<string,PrimaryKey*>::iterator it=indice.begin(); it!=indice.end(); it++){
								string key = it->first;
								int offset = it->second->getOffset();
								indice_file.write(reinterpret_cast<char*>(&key),sizeof(key));
								indice_file.write(reinterpret_cast<char*>(&offset),sizeof(offset));
							}

						}else if(indexType == 0){
							btree.insertar(new PrimaryKey(key,start));
						}
					}

					cout << "Registro Agregado!!"<<endl<<endl;
					availList.erase(availList.begin()); //borra el primer elemento, ya se uso esa posicion;
				}

			}else{
				cout << "No ha abierto un archivo valido" << endl;
			}
		}

		if(opcion == 3){
			ifstream fileIsOpen(filename,ios::binary);
			if(fileIsOpen){
				imprimir(filename);
			}else{
				cout << "No ha abierto un archivo valido" << endl;
			}
		}

		if(opcion == 4){
			ifstream file(filename,ios::binary);
			if(file){
				int useIndex,indexType;
				file.seekg(sizeof(int),ios::beg);
				file.read(reinterpret_cast<char*>(&useIndex),sizeof(useIndex));
				file.read(reinterpret_cast<char*>(&indexType),sizeof(indexType));
				if(useIndex == 0){
					cargarIndices(filename);
				}
				campos = cargarEstructura(filename);
				string llave;
				cout << "Ingrese la llave primaria del registro que quiere modificar: ";
				cin >> llave;
				PrimaryKey* key = indice.find(llave)->second;
				int offset = key->getOffset();
				campos = cargarEstructura(filename);
				int control = 0;
				string llave1,llave2;
				ifstream in(filename,ios::binary);
				in.seekg(offset,ios::beg);
				Informacion data;
				vector<Informacion> info;
				while(true){
					if(control < campos.size()){
						in.read(reinterpret_cast<char*>(&data),sizeof(data));
						cout << campos.at(control).nombre<<": ";
						if(campos.at(control).isKey){
							llave1 = data.texto;
						}
						if(campos.at(control).tipo==1){
							cin >> data.value;
						}else{
							cin >> data.texto;
							if(campos.at(control).isKey){
								llave2 = data.texto;
							}
						}
						info.push_back(data);
					} else if(control == campos.size()){
						break;
					}
					control++;
				}
				indice.erase(llave1);
				indice.insert(pair<string,PrimaryKey*>(llave2, new PrimaryKey(llave2,offset)));
				ofstream out(filename,ios::binary|ios::in|ios::out);
				out.seekp(offset,ios::beg);
				out.write(reinterpret_cast<char*>(&info[0]),info.size()*sizeof(info[0]));
				out.close();
				in.close();
				if(useIndex==1){
					if(indexType==1){
						string indexFile = "index_"+filename;
						ofstream index(indexFile,ios::binary|ios::trunc);
						index.close();
						ofstream indice_file(indexFile,ios::binary|ios::app);
						for (map<string,PrimaryKey*>::iterator it=indice.begin(); it!=indice.end(); it++){
							string key = it->first;
							int offset = it->second->getOffset();
							indice_file.write(reinterpret_cast<char*>(&key),sizeof(key));
							indice_file.write(reinterpret_cast<char*>(&offset),sizeof(offset));
						}

					}else if(indexType == 0){

					}
				}
				cout << "Registro Modificado"<<endl;

			}else{
				cout << "No ha abierto un archivo valido"<<endl;
			}
			file.close();
	
		}

		if(opcion == 5){
			ifstream file(filename,ios::binary);
			if(file){
				int useIndex,indexType;
				file.seekg(sizeof(int),ios::beg);
				file.read(reinterpret_cast<char*>(&useIndex),sizeof(useIndex));
				file.read(reinterpret_cast<char*>(&indexType),sizeof(indexType));

				campos = cargarEstructura(filename);
				if(useIndex == 0){
					cargarIndices(filename);
				}
				string llave;
				cout << "Ingrese la llave primaria del registro que quiere borrar: ";
				cin >> llave;

				PrimaryKey* key = indice.find(llave)->second;
				int original_offset;
				int offset = key->getOffset();
				char indicador = '&';

				original_offset = offset;

				int begin = sizeof(int) + sizeof(int) + sizeof (int) + sizeof(int) + (campos.size()*sizeof(Campo));
				ofstream out(filename, ios::binary|ios::in|ios::out);
				out.seekp(offset,ios::beg);
				out.write(reinterpret_cast<char*>(&indicador),sizeof(indicador));

				int first_avail;
				ifstream in(filename,ios::binary|ios::in);
				in.seekg(sizeof(int)+sizeof(int)+sizeof(int),ios::beg);
				in.read(reinterpret_cast<char*>(&first_avail),sizeof(first_avail));

				offset = offset-begin;

				int rrn = offset/(campos.size()*sizeof(Informacion)) + 1;
				cout << rrn<<endl;
				out.write(reinterpret_cast<char*>(&first_avail),sizeof(first_avail));
				out.seekp(sizeof(int)+sizeof(int)+sizeof(int),ios::beg);
				out.write(reinterpret_cast<char*>(&rrn),sizeof(rrn));
				out.close();
				cout << "Registro borrado"<<endl;
				indice.erase(llave);
				if(useIndex==1){
					if(indexType==1){
						string indexFile = "index_"+filename;
						ofstream index(indexFile,ios::binary|ios::trunc);
						index.close();
						ofstream indice_file(indexFile,ios::binary|ios::app);
						for (map<string,PrimaryKey*>::iterator it=indice.begin(); it!=indice.end(); it++){
							string key = it->first;
							int offset = it->second->getOffset();
							indice_file.write(reinterpret_cast<char*>(&key),sizeof(key));
							indice_file.write(reinterpret_cast<char*>(&offset),sizeof(offset));
						}

					}else if(indexType == 0){
						//btree.borrar(new PrimaryKey(key,original_offset));
					}
				}

			}else{
				cout << "No ha abierto un archivo valido"<<endl;
			}
			file.close();
		}

		if(opcion == 6){
			ifstream file(filename,ios::binary);
			if(file){
				string filename2 = "nuevo_"+filename;
				ofstream file2(filename2,ios::binary|ios_base::app);

				campos = cargarEstructura(filename); 
				Informacion data;

				int numero_Campos = campos.size();
				int useIndex,indexType,avail;

				file.read(reinterpret_cast<char*>(&numero_Campos),sizeof(numero_Campos));
				file.read(reinterpret_cast<char*>(&useIndex),sizeof(useIndex));
				file.read(reinterpret_cast<char*>(&indexType),sizeof(indexType));
				file.read(reinterpret_cast<char*>(&avail),sizeof(avail));
				file.read(reinterpret_cast<char*>(&campos[0]),sizeof(Campo)*campos.size());
				avail = 0;

			/*Reescribimos el header*/
				file2.write(reinterpret_cast<char*>(&numero_Campos),sizeof(numero_Campos));
				file2.write(reinterpret_cast<char*>(&useIndex),sizeof(useIndex));
				file2.write(reinterpret_cast<char*>(&indexType),sizeof(indexType));
				file2.write(reinterpret_cast<char*>(&avail),sizeof(avail));
				file2.write(reinterpret_cast<char*>(&campos[0]),sizeof(Campo)*campos.size());

				int control = 1;
				int numReg = 1;
				char c;
				int start;

				int offset = inicioRegistros(filename);
				file.seekg(offset,ios::beg);

				while(true){
					if(control <= campos.size()){
						start = file.tellg();
						if(file.read(reinterpret_cast<char*>(&c),sizeof(c))){
							if(c != '&'){
								file.seekg(start,ios::beg);
								if(file.read(reinterpret_cast<char*>(&data),sizeof(data))){
									file2.write(reinterpret_cast<char*>(&data),sizeof(data));
									control++;
								} else {
									break;
								}
							}else{
								int control2 = 1;
								file.seekg(start,ios::beg);
								while(control2 <= campos.size()){
									if(file.read(reinterpret_cast<char*>(&data),sizeof(data))){
										control2++;
									}
								}
								numReg++;
								control = 1;
							}
						}
					} else if(control > campos.size()){
						control = 1;
						numReg++;
					}
					if(file.eof()){
						break;
					}
				}
				file.close();
				file2.close();

				char original[filename.size()+1];
				char temp[filename2.size()+1];
				strcpy(original, filename.c_str());
				strcpy(temp,filename2.c_str());

				remove(original);
				rename(temp,original);

			}else{
				cout << "No ha abierto un archivo valido"<<endl;
			}
			file.close();

		}

		if(opcion == 7){
			string llave;
			ifstream file(filename,ios::binary);
			if(file){
				cout << "Ingrese la llave primaria del registro que quiere buscar: ";
				cin >> llave;

				PrimaryKey* key = indice.find(llave)->second;
				int offset = key->getOffset();

				file.seekg(offset,ios::beg);
				vector<Informacion> info;
				Informacion data;
				campos = cargarEstructura(filename);
				int control = 0;
				while(true){
					if(control < campos.size()){
						file.read(reinterpret_cast<char*>(&data),sizeof(data));
						cout << campos.at(control).nombre << ": ";
						if(campos.at(control).tipo == 1){
							cout << data.value;
						}else{
							cout << data.texto;
						}
						control++;
						cout << endl;
					}else{
						break;
					}
				}

			}else{
				cout << "No ha abierto un archivo valido"<<endl;
			}
			file.close();

		}

		if(opcion == 8){
			btree.recorrer();
		}
		if(opcion == 9){
			cout << "Ingrese el nombre del archivo de texto: ";
			cin >> filename;
			text2bin(filename);
			cout << "Terminado"<<endl;
		}
		if(opcion == 10){
			continuar = false;
		}
	}
	return 0;
}

int menu(){
	int opcion;
	cout << "1 - Abrir Archivo" << endl;
	cout << "2 - Agregar Registros" << endl;
	cout << "3 - Listar" << endl;
	cout << "4 - Modificar Registro"<<endl;
	cout << "5 - Borrar Registro"<<endl;
	cout << "6 - Compactar"<<endl;
	cout << "7 - Buscar"<<endl;
	cout << "8 - Cruzar Archivo" << endl;
	cout << "9 - Convertir un archivo de texto a binario"<<endl;
	cout << "10 - Salir"<<endl;
	cout << "Ingrese opcion: ";
	cin >> opcion;
	return opcion;
}

int menuIndices(){
	int opcion;
	cout << "1 - Cruzar Sin Indices"<<endl;
	cout << "2 - Cruzar Con Indice Lineal"<<endl;
	cout << "3 - Cruzar usando Arbol B"<<endl;
	cout << "Ingrese Opcion: ";
	cin >> opcion;
	return opcion;
}

void cargarIndices(string filename){
	indice.clear();
	cout << endl;
	string indexFileName = "index_"+filename;
	ifstream file(filename,ios::binary);
	int useIndex,indexType;
	file.seekg(sizeof(int),ios::beg);
	file.read(reinterpret_cast<char*>(useIndex),sizeof(useIndex));
	file.read(reinterpret_cast<char*>(indexType),sizeof(indexType));
	ifstream fileExists(indexFileName,ios::binary);
	int offset = inicioRegistros(filename);
	file.seekg(offset,ios::beg);
	vector<Informacion> info;
	vector<Campo> campos = cargarEstructura(filename); 
	Informacion info1;
	int control = 1;
	int start = -1;
	string llave;
	while(true){
		if(control <= campos.size()){
			if(control == 1){
				start = file.tellg();
			}
			if(file.read(reinterpret_cast<char*>(&info1),sizeof(info1))){
				info.push_back(info1);
				if(campos.at(control-1).isKey){
					indice.insert(pair<string,PrimaryKey*>(info.at(control-1).texto,new PrimaryKey(info.at(control-1).texto,start)));
				}
				control++;
			} else {
				break;
			}
		} else if(control > campos.size()){
			info.clear();
			control = 1;
		}
		if(file.eof()){
			break;
		}
	}
	file.close();
	if(useIndex == 1){
		if(indexType == 1){
			if(fileExists){
				ofstream borrar(indexFileName,ios::binary|ios::trunc);
				borrar.close();
				ofstream file2(indexFileName,ios::binary|ios::app);
				for (map<string,PrimaryKey*>::iterator it=indice.begin(); it!=indice.end(); it++){
					string key = it->first;
					int offset = it->second->getOffset();
					file2.write(reinterpret_cast<char*>(&key),sizeof(key));
					file2.write(reinterpret_cast<char*>(&offset),sizeof(offset));
				}
				file2.close();
			}else{
				ofstream file2(indexFileName,ios::binary|ios_base::app);
				for (map<string,PrimaryKey*>::iterator it=indice.begin(); it!=indice.end(); it++){
					string key = it->first;
					int offset = it->second->getOffset();
					file2.write(reinterpret_cast<char*>(&key),sizeof(key));
					file2.write(reinterpret_cast<char*>(&offset),sizeof(offset));
				}
				file2.close();
			}
		}
	}
}

void imprimir(string filename){
	cout << endl;
	ifstream file(filename,ios::binary);
	int offset = inicioRegistros(filename);
	file.seekg(offset,ios::beg);
	vector<Informacion> info;
	vector<Campo> campos = cargarEstructura(filename); 
	Informacion info1;
	int control = 1;
	int numReg = 1;
	char c;
	int start;
	while(true){
		if(control <= campos.size()){
			start = file.tellg();
			if(file.read(reinterpret_cast<char*>(&c),sizeof(c))){
				if(c != '&'){
					file.seekg(start,ios::beg);
					if(file.read(reinterpret_cast<char*>(&info1),sizeof(info1))){
						info.push_back(info1);
						control++;
					} else {
						break;
					}
				}else{
					int control2 = 1;
					file.seekg(start,ios::beg);
					while(control2 <= campos.size()){
						if(file.read(reinterpret_cast<char*>(&info1),sizeof(info1))){
							info.push_back(info1);
							control2++;
						}
					}
					info.clear();
					numReg++;
					control = 1;
				}
			}
		} else if(control > campos.size()){
			cout << "Registro #"<<numReg<<endl;
			for(int i = 0; i < campos.size(); i++){
				cout << campos.at(i).nombre<<": ";
				if(campos.at(i).tipo == 1){
					cout << info.at(i).value;
				}else if(campos.at(i).tipo == 2){
					cout << info.at(i).texto;
				}
				cout << endl;
			}
			cout << endl;
			info.clear();
			control = 1;
			numReg++;
		}
		if(file.eof()){
			break;
		}
	}
	file.close();

	/*for (map<string,PrimaryKey*>::iterator it=indice.begin(); it!=indice.end(); it++)
	    cout << it->first << " => " << it->second->getOffset() << '\n';*/
}

vector<Campo> cargarEstructura(string filename){
	vector<Campo> campos;
	ifstream file(filename,ios::binary|ios::in);
	Campo camp;
	int cantCampos = 0;
	int position;
	int offset = 0;
	file.seekg(0,ios::beg);
	file.read(reinterpret_cast<char*>(&cantCampos),sizeof(cantCampos)); //lee el numero de campos
	offset = sizeof(int) + sizeof(int) + sizeof(int) + sizeof(int);
	file.seekg(offset,ios::beg);
	int max = sizeof(int) + sizeof(int)+ sizeof(int) + (cantCampos * sizeof(Campo));

	while(offset < max){
		file.read(reinterpret_cast<char*>(&camp), sizeof(camp));
		campos.push_back(camp);
		offset += sizeof(Campo);
	}
	return campos;
	file.close();
}


int inicioRegistros(string filename){
	ifstream file(filename,ios::binary|ios::in);
	int cantCampos = 0;
	file.read(reinterpret_cast<char*>(&cantCampos),sizeof(cantCampos));
	int offset = sizeof(int) + sizeof(int) + sizeof(int) + sizeof(int) + (cantCampos*sizeof(Campo));
	return offset;
}

vector<int> cargarAvailList(string filename){
	int first_avail=0, offset, cantCampos;
	int readPosition = sizeof(int) + sizeof(int) +  sizeof(int);
	vector<int> posiciones;
	ifstream file(filename,ios::binary|ios::in);
	file.read(reinterpret_cast<char*>(&cantCampos),sizeof(cantCampos));
	file.seekg(readPosition,ios::beg);
	file.read(reinterpret_cast<char*>(&first_avail),sizeof(first_avail));
	//cout << first_avail << endl;
	posiciones.push_back(first_avail);
	offset = inicioRegistros(filename);
	//cout << offset << endl;
	file.seekg(offset,ios::beg);

	offset += (first_avail-1)*cantCampos*sizeof(Informacion);
	file.seekg(offset+1,ios::beg);
	int next;

	if(first_avail == 0){
		file.close();
		return posiciones;
	} else {
		while(true){
			file.read(reinterpret_cast<char*>(&next),sizeof(next));
			//cout << next << endl;
			offset = inicioRegistros(filename);
			offset += (next-1)*cantCampos*sizeof(Informacion);
			file.seekg(offset+1,ios::beg);
			posiciones.push_back(next);
			if(next == 0){
				break;
			}
		}
		file.close();
		return posiciones;
	}
}

void text2bin(string filename){
	ifstream in(filename);
	if(in){
		int useIndex = 0;
		int indexType = 0;
		cout << "Desea utilizar indices en este archivo? (1-Si/0-No): ";
		cin >> useIndex;
		if(useIndex == 1){
			cout << "Que tipo de indice le gustaria utilizar? (1-Lineal/0-Arbol B): ";
			cin >> indexType;
		}
		filename.erase(filename.end()-4,filename.end());
		string outputFile = filename+".bin";
		ofstream out(outputFile,ios::binary|ios::app);
		int control = 0;
		string linea,token;
		string delimitador = ",";
		size_t pos = 0;
		vector<string> splitString;
		vector<Campo> estructura;
		while(in >> linea){
			if(control == 0){
				while ((pos = linea.find(delimitador)) != std::string::npos) {
				    token = linea.substr(0, pos);
				    splitString.push_back(token);
				    linea.erase(0, pos + delimitador.length());
				}
				splitString.push_back(linea);
				int fieldCount = splitString.size();
				int available = 0;
				out.write(reinterpret_cast<char*>(&fieldCount),sizeof(fieldCount));
				out.write(reinterpret_cast<char*>(&useIndex),sizeof(useIndex));
				out.write(reinterpret_cast<char*>(&indexType),sizeof(indexType));
				out.write(reinterpret_cast<char*>(&available),sizeof(available));
				for(int i = 0; i < splitString.size(); i++){
					Campo campo;
					campo.nombre = splitString.at(i);
					if(i == 0){
						campo.tipo = 2;
						campo.isKey = true;
						campo.longitud = 10;
					}else{
						campo.tipo = 2;
						campo.isKey = false;
						campo.longitud = 15;
					}
					out.write(reinterpret_cast<char*>(&campo),sizeof(campo));
				}
				control++;
				splitString.clear();
			}else{
				while ((pos = linea.find(delimitador)) != std::string::npos) {
				    token = linea.substr(0, pos);
				    splitString.push_back(token);
				    linea.erase(0, pos + delimitador.length());
				}
				splitString.push_back(linea);
				for(int i = 0; i < splitString.size(); i++){
					Informacion data;
					data.value = 0;
					data.texto = splitString.at(i);
					out.write(reinterpret_cast<char*>(&data),sizeof(data));
				}
				splitString.clear();
				control++;
			}
		}
		in.close();
		out.close();

	}else{
		cout << "El archivo no existe"<<endl;
	}
}
