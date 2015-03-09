#include <iostream>
#include <stack>
#include <vector>
#include <iomanip>
#include <fstream>
#include <string>
#include <map>
#include "primaryKey.h"

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

int main(int argc, char*argv[]){

	int cantCampos;
	string filename;
	vector<Campo> campos;
	vector<Informacion> info;
	vector<Registro> registros;
	bool continuar = true;
	while(continuar){
		int opcion = menu();

		if(opcion == 1){
			cout << "Esta opcion crea un nuevo archivo!"<<endl;
			cout << "Ingrese el nombre del nuevo archivo: ";
			cin >> filename;
			cout << "Cuantos campos desea crear: ";
			cin >> cantCampos;
			for(int i = 0; i < cantCampos; i++){
				Campo camp;
				cout << "Ingrese nombre del campo: ";
				cin >> camp.nombre;
				cout << "Es llave primaria(1-Si/0-No): ";
				cin >> camp.isKey;
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
				}
				campos.push_back(camp);
				cout << "Campo Creado!"<<endl;
			}

			/* Crea el encabezado del archivo */
			int position = 0; //primera posicion disponible en el availlist
			ofstream file(filename,ios::binary|ios::app);
			file.write(reinterpret_cast<const char*>(&cantCampos),sizeof(cantCampos)); //Guarda el numero de campos en el encabezado
			file.write(reinterpret_cast<const char*>(&position),sizeof(position));//guarda la primera posicion disponible para agregar en el availlist
			const char* pointer = reinterpret_cast<const char*>(&campos[0]);//Guarda la lista de campos con su respectiva informacion
			size_t bytes = campos.size() * sizeof(campos[0]);
			file.write(pointer, bytes);
			file.close();
			campos.clear();
			cout << endl;
			cout << "Estructura Guardada!!"<<endl;
		}

		if(opcion == 2){
			cout << "Ingrese el nombre del archivo donde quiere agregar un registro: ";
			cin >> filename;
			ofstream file2(filename,ios::binary|ios_base::app);
			vector<int> availList = cargarAvailList(filename);
			campos = cargarEstructura(filename);
			/*Revisar el availlist */
			if(availList.at(0) == 0){ //Si no hay posiciones que agregue al final
				info.clear();
				string text;
				int data;
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
						if(temp.texto.size() < campos.at(i).longitud){
							for(int j = temp.texto.size(); j < campos.at(i).longitud; j++){
								temp.texto += '-';
							}
						}
						temp.value = 0;
						info.push_back(temp);
					}
				}
				/*Escribe al final del archivo*/
				const char* puntero = reinterpret_cast<const char*>(&info[0]);
				size_t cantBytes = info.size() * sizeof(info[0]);
				file2.write(puntero,cantBytes);
				cout << "Registro Agregado"<<endl;
				info.clear();
				file2.close();
				
			}else{ // Si hay posiciones disponibles,escribe en esa posicion especifica
				info.clear();
				string text;
				int data;
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
						if(temp.texto.size() < campos.at(i).longitud){
							for(int j = temp.texto.size(); j < campos.at(i).longitud; j++){
								temp.texto += '-';
							}
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

				file2.seekp(sizeof(int),ios::beg);
				file2.write(reinterpret_cast<char*>(&next),sizeof(next));

				file2.seekp(offset,ios::beg);
				const char* puntero = reinterpret_cast<const char*>(&info[0]);
				size_t cantBytes = info.size() * sizeof(info[0]);
				file2.write(puntero,cantBytes);
				file2.close();
				cout << "Registro Agregado!!"<<endl<<endl;
				availList.erase(availList.begin()); //borra el primer elemento, ya se uso esa posicion;
			}
		}

		if(opcion == 3){
			cout << "Ingrese el nombre del archivo que quiere listar: ";
			cin >> filename;
			imprimir(filename);
		}

		if(opcion == 4){
			cout << "Ingrese el nombre del archivo donde quiere modificar un registro: ";
			cin >> filename;
			imprimir(filename);
			campos = cargarEstructura(filename);
			int rrn;
			int control = 0;
			string llave1,llave2;
			cout << "Ingrese el numero de registro que quiere modificar: ";
			cin >> rrn;
			int offset = inicioRegistros(filename);
			offset += (rrn-1)*campos.size()*sizeof(Informacion);
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
			ofstream out(filename,ios::binary|ios::in|ios::out);
			out.seekp(offset,ios::beg);
			out.write(reinterpret_cast<char*>(&info[0]),info.size()*sizeof(info[0]));
			out.close();
			in.close();
			cout << "Registro Modificado"<<endl;
		}

		if(opcion == 5){
			cout << "Ingrese nombre del archivo donde quiere borrar un registro: ";
			cin >> filename;
			campos = cargarEstructura(filename);
			int rrn;
			char indicador = '&';
			imprimir(filename);
			cout << "Ingrese el numero de registro que quiere borrar: ";
			cin >> rrn;
			int offset = inicioRegistros(filename);
			offset += (rrn-1)*campos.size()*sizeof(Informacion);

			ofstream out(filename, ios::binary|ios::in|ios::out);
			out.seekp(offset,ios::beg);
			out.write(reinterpret_cast<char*>(&indicador),sizeof(indicador));

			int first_avail;
			ifstream in(filename,ios::binary|ios::in);
			in.seekg(sizeof(int),ios::beg);
			in.read(reinterpret_cast<char*>(&first_avail),sizeof(first_avail));
			cout << first_avail << endl;

			out.write(reinterpret_cast<char*>(&first_avail),sizeof(first_avail));
			out.seekp(sizeof(int),ios::beg);
			out.write(reinterpret_cast<char*>(&rrn),sizeof(rrn));
			out.close();
			cout << "Registro borrado"<<endl;
		}

		if(opcion == 6){

			/*Cargamos registros no marcados a memoria*/
			int begin,end,size;
			int rrn = inicioRegistros(filename);
			int offset = 0;

							/*Leer del Archivo*/
			ifstream file(filename,ios::binary |ios::in |ios::out);
			begin = file.tellg();
			file.seekg(0,ios::end);
			end = file.tellg();
			size = end-begin;
			file.seekg(0,ios::beg);

			int numData = size/sizeof(Informacion);


			vector<Informacion> infoRegistro;
			Informacion data;
			int control = 1;

			while(true){
				cout << rrn << endl;
				if(rrn<numData){
					offset += rrn*sizeof(Informacion);
					char c;
					file.seekg(offset,ios::beg);
					file.get(c);
					if(c != '&'){
						if(file.read(reinterpret_cast<char*>(&data),sizeof(data))) {
							infoRegistro.push_back(data);
						}
						rrn++;
					}else{
						rrn = rrn+campos.size();
					}
				}else{
					break;
				}
			}
			file.close();
			cout << infoRegistro.size() << endl;

			/* Escribir Archivo Nuevo */
			ofstream file2(filename,ios::binary);
			const char* puntero = reinterpret_cast<const char*>(&infoRegistro[0]);
			size_t cantBytes = infoRegistro.size() * sizeof(infoRegistro[0]);
			file2.write(puntero,cantBytes);
			file2.close();
			cout << "NUEVO ARCHIVO CREADO"<<endl<<endl;


		}

		if(opcion == 7){
			if(registros.size() == 0){
				cout << "No hay registros, cargue un archivo de registros!!"<<endl;
			}else{
				string texto;
				int valor,tipo;
				cout << "Ingrese el tipo por el que le gustaria buscar(1-Valor/2-Texto): ";
				cin >> tipo;
				if(tipo == 1){

					cout << "Ingrese un valor para buscar en los registros: ";
					cin >> valor;

					int control = 0;
					for(int i = 0; i < registros.size(); i++){
						cout << endl;
						for(int j = 0; j < registros.at(i).informacion.size(); j++){
							if(registros.at(i).informacion.at(j).value == valor){
								while(control < campos.size()){
									cout << registros.at(i).estructura.at(control).nombre << ": ";
									if(registros.at(i).estructura.at(control).tipo == 1){
										cout << registros.at(i).informacion.at(control).value;
									}
									if(registros.at(i).estructura.at(control).tipo == 2){
										cout << registros.at(i).informacion.at(control).texto;
									}
									control++;
									cout << endl;
								}
							}
						}
					}

				}else if(tipo == 2){

					cout << "Ingrese texto para buscar en los registros: ";
					cin >> texto;

					int control = 0;
					cout << endl;
					for(int i = 0; i < registros.size(); i++){
						for(int j = 0; j < registros.at(i).informacion.size(); j++){
							if(registros.at(i).informacion.at(j).texto == texto){
								while(control < campos.size()){
									cout << registros.at(i).estructura.at(control).nombre << ": ";
									if(registros.at(i).estructura.at(control).tipo == 1){
										cout << registros.at(i).informacion.at(control).value;
									}
									if(registros.at(i).estructura.at(control).tipo == 2){
										cout << registros.at(i).informacion.at(control).texto;
									}
									control++;
									cout << endl;
								}
							}
						}
					}
					cout << endl;
				}
			}
		}

		if(opcion == 8){
			cout << "Ingrese el nombre del archivo que quiere usar indice lineal\n(este archivo usara para todas las operaciones): ";
			cin >> filename;
			cargarIndices(filename);
			campos = cargarEstructura(filename);
			bool seguir = true;
			while(seguir){
				int option = menuIndices();
				if(option == 1){
					vector<int> availList = cargarAvailList(filename);
					string key;
					int start;
					ofstream file(filename,ios::binary| ios_base::app);
					ofstream file2(filename,ios::binary| ios::in|ios::out);

					/*Revisar el availlist */
					if(availList.at(0) == 0){ //Si no hay posiciones
						info.clear();
						string text;
						int data;
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
								if(temp.texto.size() < campos.at(i).longitud){
									for(int j = temp.texto.size(); j < campos.at(i).longitud; j++){
										temp.texto += '-';
									}
								}
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
						file.write(puntero,cantBytes);
						start = ((int)file.tellp()) - (campos.size()*sizeof(Informacion));//probablemente sumarle 1
						indice.insert(pair<string,PrimaryKey*>(key,new PrimaryKey(key,start)));
						info.clear();
						
					}else{ // Si hay posiciones disponibles
						info.clear();
						string text;
						int data;
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
								if(temp.texto.size() < campos.at(i).longitud){
									for(int j = temp.texto.size(); j < campos.at(i).longitud; j++){
										temp.texto += '-';
									}
								}
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
						file2.seekp(sizeof(int),ios::beg);
						file2.write(reinterpret_cast<char*>(&next),sizeof(next));
						file2.seekp(offset,ios::beg);
						const char* puntero = reinterpret_cast<const char*>(&info[0]);
						size_t cantBytes = info.size() * sizeof(info[0]);
						file2.write(puntero,cantBytes);
						start = ((int)file2.tellp())-(campos.size()*sizeof(Informacion)); //posiblemente sumarle 1
						indice.insert(pair<string,PrimaryKey*>(key,new PrimaryKey(key,start)));
						availList.erase(availList.begin());
					}
					file.close();
					file2.close();

				}
				if(option == 2){
					int rrn;
					int control = 0;
					string llave1,llave2;
					imprimir(filename);
					cout << "Ingrese el numero de registro que quiere modificar: ";
					cin >> rrn;
					int offset = inicioRegistros(filename);
					offset += (rrn-1)*campos.size()*sizeof(Informacion);
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
					ofstream out(filename,ios::binary|ios::in|ios::out);
					out.seekp(offset,ios::beg);
					indice.insert(pair<string,PrimaryKey*>(llave2,new PrimaryKey(llave2,offset)));
					out.write(reinterpret_cast<char*>(&info[0]),info.size()*sizeof(info[0]));
					out.close();
					in.close();
				}
				if(option == 3){
					int rrn;
					char indicador = '&';
					imprimir(filename);
					cout << "Ingrese el numero de registro que quiere borrar: ";
					cin >> rrn;
					int offset = inicioRegistros(filename);
					offset += (rrn-1)*campos.size()*sizeof(Informacion);

					ifstream in(filename,ios::binary|ios::in);
					in.seekg(offset,ios::beg);
					int control = 0;
					string key;
					Informacion data;
					while(true){
						if(control < campos.size()){
							in.read(reinterpret_cast<char*>(&data),sizeof(data));
							if(campos.at(control).isKey){
								key = data.texto;
							}
						}else if(control == campos.size()){
							break;
						}
						control++;
					}

					ofstream out(filename, ios::binary|ios::in|ios::out);
					out.seekp(offset,ios::beg);
					out.write(reinterpret_cast<char*>(&indicador),sizeof(indicador));
					int first_avail;
					in.seekg(sizeof(int),ios::beg);
					in.read(reinterpret_cast<char*>(&first_avail),sizeof(first_avail));
					cout << first_avail << endl;
					out.write(reinterpret_cast<char*>(&first_avail),sizeof(first_avail));
					out.seekp(sizeof(int),ios::beg);
					out.write(reinterpret_cast<char*>(&rrn),sizeof(rrn));
					out.close();
					in.close();
					indice.erase(key);
					cout << "Registro borrado"<<endl;
				}
				if(option == 4){
					string llave;
					cout << "Ingrese la llave para buscar: ";
					cin >> llave;
					PrimaryKey* key = indice.find(llave)->second;
					int offset = key->getOffset();

					ifstream file(filename,ios::binary);
					file.seekg(offset,ios::beg);
					int control = 1;
					Informacion data;
					cout << endl;
					while(true){
						if(control < campos.size()){
							file.read(reinterpret_cast<char*>(&data),sizeof(data));
							cout << campos.at(control-1).nombre << ": ";
							if(campos.at(control-1).tipo == 1){
								cout << data.value;
							} else {
								cout << data.texto;
							}
						} else if(control == campos.size()) {
							file.read(reinterpret_cast<char*>(&data),sizeof(data));
							cout << campos.at(control-1).nombre << ": ";
							if(campos.at(control-1).tipo == 1){
								cout << data.value;
							} else {
								cout << data.texto;
							}
							cout << endl;
							break;
						}
						control++;
						cout << endl;
					}
					cout << endl;
				}
				if(option == 5){
					seguir = false;
				}
			}

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

		filename.clear();
	}
	return 0;
}

int menu(){
	int opcion;
	cout << "1 - Crear Estructura de Archivo" << endl;
	cout << "2 - Agregar Registros" << endl;
	cout << "3 - Listar" << endl;
	cout << "4 - Modificar Registro"<<endl;
	cout << "5 - Borrar Registro"<<endl;
	cout << "6 - Compactar"<<endl;
	cout << "7 - Buscar"<<endl;
	cout << "8 - Usar Indice Lineal" << endl;
	cout << "9 - Convertir un archivo de texto a binario"<<endl;
	cout << "10 - Salir"<<endl;
	cout << "Ingrese opcion: ";
	cin >> opcion;
	return opcion;
}

int menuIndices(){
	int opcion;
	cout << "1 - Agregar registro usando Indice Lineal"<<endl;
	cout << "2 - Modificar registro usando Indice Lineal"<<endl;
	cout << "3 - Eliminar Registro usando Indice Lineal"<<endl;
	cout << "4 - Buscar usando indice Lineal"<<endl;
	cout << "5 - Desactivar indice lineal"<<endl;
	cout << "Ingrese opcion: ";
	cin >> opcion;
	return opcion;
}

void cargarIndices(string filename){
	cout << endl;
	ifstream file(filename,ios::binary);

	int offset = inicioRegistros(filename);
	file.seekg(offset,ios::beg);

	vector<Informacion> info;
	vector<Campo> campos = cargarEstructura(filename); 
	Informacion info1;
	int control = 1;
	int start = -1;
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
	}
	file.close();
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

	file.read(reinterpret_cast<char*>(&cantCampos),sizeof(int)); //lee el numero de campos
	offset += sizeof(int);
	file.seekg(offset,ios::beg);
	file.read(reinterpret_cast<char*>(&position),sizeof(int));//lee la primera posicion disponible para agregar al availlist;
	//availList.push(position);
	offset += sizeof(int);
	file.seekg(offset,ios::beg);

	int max = sizeof(int) + (cantCampos * sizeof(Campo));

	while(offset < max){
		file.read(reinterpret_cast<char*>(&camp), sizeof(camp));
		campos.push_back(camp);
		offset += sizeof(Campo);
	}
	return campos;
}


int inicioRegistros(string filename){
	ifstream file(filename,ios::binary|ios::in);
	int cantCampos = 0;
	file.read(reinterpret_cast<char*>(&cantCampos),sizeof(cantCampos));
	int offset = sizeof(int) + sizeof(int) + (cantCampos*sizeof(Campo));
	return offset;
}

vector<int> cargarAvailList(string filename){
	int first_avail=0, offset, cantCampos;
	vector<int> posiciones;
	ifstream file(filename,ios::binary|ios::in);
	file.read(reinterpret_cast<char*>(&cantCampos),sizeof(cantCampos));
	file.seekg(sizeof(int),ios::beg);
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
}
