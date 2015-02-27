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
	char nombre[20];
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
int inicioRegistros(string);

vector<Campo> cargarEstructura(string);
map<string,PrimaryKey*> indice;

int main(int argc, char*argv[]){

	int cantCampos;
	string filename;
	vector<Campo> campos;
	vector<Informacion> info;
	vector<Registro> registros;

	stack<int> availList;

	bool continuar = true;
	cout << "Ingrese el nombre del archivo: ";
	cin >> filename;
	filename += ".bin";
	while(continuar){
		int opcion = menu();

		if(opcion == 1){
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
			ofstream file(filename,ios::binary|ios::app);
			file.write(reinterpret_cast<const char*>(&cantCampos),sizeof(cantCampos)); //Guarda el numero de campos en el encabezado
			const char* pointer = reinterpret_cast<const char*>(&campos[0]);//Guarda la lista de campos con su respectiva informacion
			size_t bytes = campos.size() * sizeof(campos[0]);
			file.write(pointer, bytes);
			file.close();

			cout << endl;
			cout << "Estructura Guardada!!"<<endl;
		}

		if(opcion == 2){
			/*Revisar el availlist */
			if(availList.size() == 0){ //Si el availist esta vacio
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
				ofstream file2(filename,ios::binary| ios_base::app);
				const char* puntero = reinterpret_cast<const char*>(&info[0]);
				size_t cantBytes = info.size() * sizeof(info[0]);
				file2.write(puntero,cantBytes);
				cout << "Registro Agregado"<<endl;
				info.clear();
				file2.close();
				
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
						temp.value = 0;
						info.push_back(temp);
					}
				}
				int rrn = availList.top();
				int offset = inicioRegistros(filename);
				offset += (rrn-1)*campos.size()*sizeof(Informacion);
				ofstream file2(filename,ios::binary| ios::in|ios::out);
				file2.seekp(offset,ios::beg);
				const char* puntero = reinterpret_cast<const char*>(&info[0]);
				size_t cantBytes = info.size() * sizeof(info[0]);
				file2.write(puntero,cantBytes);
				file2.close();
				cout << "Registro Agregado!!"<<endl<<endl;
				availList.pop();
			}
		}

		if(opcion == 3){
			imprimir(filename);
		}

		if(opcion == 4){
			cout << "Ingrese el nombre del archivo que quiere abrir: ";
			cin >> filename;
			campos = cargarEstructura(filename);
			cargarIndices(filename);
			cout << "Archivo abierto"<<endl<<endl;
		}

		if(opcion == 5){
			int rrn = 0;
			int offset = inicioRegistros(filename);
			imprimir(filename);
			cout << "Ingrese el numero de registro que quiere modificar: ";
			cin >> rrn;
			offset += (rrn-1)*campos.size()*sizeof(Informacion);

				/*Leer del Archivo*/
			ifstream file(filename,ios::binary |ios::in |ios::out);
			file.seekg(offset,ios::beg);
			vector<Informacion> infoRegistro;
			Informacion data;
			int control = 1;
			while(true){
				if(control < campos.size()){
					if(file.read(reinterpret_cast<char*>(&data),sizeof(data))){
						infoRegistro.push_back(data);
						control++;
					} 
				} else if(control == campos.size()){
					break;
				}
			}
			file.close();

				/*Modificar Datos*/
			for(int i = 0; i < campos.size(); i++){
				for(int j = 0; j < infoRegistro.size(); j++){
					cout << campos.at(i).nombre << ": ";
					if(campos.at(i).tipo == 1){
						cin >> infoRegistro.at(j).value;
					}
					if(campos.at(i).tipo == 2){
						cin >> infoRegistro.at(j).texto;
					}
				}
				cout << endl;
			}

				/*Escribir al Archivo*/
			ofstream file2(filename,ios::binary| ios::in|ios::out);
			file2.seekp(offset,ios::beg);
			const char* puntero = reinterpret_cast<const char*>(&infoRegistro[0]);
			size_t cantBytes = infoRegistro.size() * sizeof(infoRegistro[0]);
			file2.write(puntero,cantBytes);
			file2.close();
			cout << "Registro modificado!!"<<endl<<endl;
		}

		if(opcion == 6){
			int rrn;
			imprimir(filename);
			cout << "Ingrese el numero de registro que quiere borrar: ";
			cin >> rrn;
			int offset = inicioRegistros(filename);
			offset += (rrn-1)*campos.size()*sizeof(Informacion);
			ofstream out(filename, ios::in|ios::binary);
			out.seekp(offset,ios::beg);
			out.put('&');
			out.close();
			availList.push(rrn);
			cout << "Registro borrado"<<endl;
		}

		if(opcion == 7){

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

		if(opcion == 8){
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

		if(opcion == 9){
			bool seguir = true;
			while(seguir){
				int option = menuIndices();
				if(option == 1){

					string key;
					int start;
					ofstream file(filename,ios::binary| ios_base::app);
					ofstream file2(filename,ios::binary| ios::in|ios::out);

					/*Revisar el availlist */
					if(availList.size() == 0){ //Si el availist esta vacio
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
						int rrn = availList.top();
						int offset = inicioRegistros(filename);
						offset += (rrn-1)*campos.size()*sizeof(Informacion);
						file2.seekp(offset,ios::beg);
						const char* puntero = reinterpret_cast<const char*>(&info[0]);
						size_t cantBytes = info.size() * sizeof(info[0]);
						file2.write(puntero,cantBytes);
						availList.pop();
						start = ((int)file2.tellp())-(campos.size()*sizeof(Informacion)); //posiblemente sumarle 1
						indice.insert(pair<string,PrimaryKey*>(key,new PrimaryKey(key,start)));
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
					imprimir(filename);
					cout << "Ingrese el numero de registro que quiere borrar: ";
					cin >> rrn;
					int offset = inicioRegistros(filename);
					offset += (rrn-1)*campos.size()*sizeof(Informacion);
					ofstream out(filename, ios::in|ios::binary);
					out.seekp(offset,ios::beg);

					ifstream in(filename,ios::binary);
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
					out.put('&');
					out.close();
					in.close();
					availList.push(rrn);
					indice.erase(key);
					cout << "Registro borrado"<<endl;
					imprimir(filename);
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

		if(opcion == 10){
			continuar = false;
		}
	}
	return 0;
}

int menu(){
	int opcion;
	cout << "1 - Crear Estructura de Archivo" << endl;
	cout << "2 - Agregar Registros" << endl;
	cout << "3 - Listar" << endl;
	cout << "4 - Abrir un Archivo" << endl;
	cout << "5 - Modificar Registro"<<endl;
	cout << "6 - Borrar Registro"<<endl;
	cout << "7 - Compactar"<<endl;
	cout << "8 - Buscar"<<endl;
	cout << "9 - Usar Indice Lineal" << endl;
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
	while(true){
		if(control <= campos.size()){
			if(file.read(reinterpret_cast<char*>(&info1),sizeof(info1))){
				info.push_back(info1);
				control++;
			} else {
				break;
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
	}
	file.close();
	for (map<string,PrimaryKey*>::iterator it=indice.begin(); it!=indice.end(); it++)
	    cout << it->first << " => " << it->second->getOffset() << '\n';
}

vector<Campo> cargarEstructura(string filename){
	vector<Campo> campos;
	ifstream file(filename,ios::binary|ios::in);
	Campo camp;
	int cantCampos = 0;
	int offset = 0;

	file.read(reinterpret_cast<char*>(&cantCampos),sizeof(int));
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
	int offset = sizeof(int)+(cantCampos*sizeof(Campo));
	return offset;
}
