#include <iostream>
#include <stack>
#include <vector>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;

struct Campo{
	char nombre[20];
	int tipo;
	int longitud;
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
void imprimirRegistros(vector<Registro>);
void salvarArchivo(vector<Campo>,vector<Registro>);

/* Estos dos metodos, cargan el contenido de los archivos 
a memoria con el fin de poder visualizar el contenido de 
los archivos */
vector<Campo> cargarEstructura();
vector<Registro> cargarRegistros(vector<Campo>);

int main(int argc, char*argv[]){

	int cantCampos;
	vector<Campo> campos;
	vector<Informacion> info;
	vector<Registro> registros;

	stack<int> availList;

	bool continuar = true;
	while(continuar){
		int opcion = menu();

		if(opcion == 1){
			cout << "Cuantos campos desea crear: ";
			cin >> cantCampos;
			for(int i = 0; i < cantCampos; i++){
				Campo camp;
				cout << "Ingrese nombre del campo: ";
				cin >> camp.nombre;
				cout << "Ingrese tipo del campo(1-Int/2-Texto): ";
				cin >> camp.tipo;
				if(camp.tipo == 2){
					cout << "Ingrese longitud del texto: ";
					cin >> camp.longitud;
				} else {
					camp.longitud = sizeof(int);
				}
				campos.push_back(camp);
				cout << "Campo Creado!"<<endl;
			}

			ofstream file("estructura.bin",ios::binary);
			const char* pointer = reinterpret_cast<const char*>(&campos[0]);
			size_t bytes = campos.size() * sizeof(campos[0]);
			file.write(pointer, bytes);
			cout << endl;
			cout << "Estructura Guardada!!"<<endl;
		}

		if(opcion == 2){
			if(availList.size() == 0){
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
				ofstream file2("registros.bin",ios::binary| ios_base::app);
				const char* puntero = reinterpret_cast<const char*>(&info[0]);
				size_t cantBytes = info.size() * sizeof(info[0]);
				file2.write(puntero,cantBytes);
				cout << "Registro Agregado"<<endl;
				info.clear();
				file2.close();
			}else{
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
				int offset = (rrn-1)*campos.size()*sizeof(Informacion);
				ofstream file2("registros.bin",ios::binary| ios::in|ios::out);
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
			imprimirRegistros(registros);
		}

		if(opcion == 4){
			salvarArchivo(campos,registros);
		}

		if(opcion == 5){
			campos = cargarEstructura();
			registros = cargarRegistros(campos);
		}

		if(opcion == 6){
			if(!(registros.size()==0)){
				int rrn = 0;
				int offset = 0;
				imprimirRegistros(registros);
				cout << "Ingrese el numero de registro que quiere modificar: ";
				cin >> rrn;
				offset = (rrn-1)*campos.size()*sizeof(Informacion);

				/*Leer del Archivo*/
				ifstream file("registros.bin",ios::binary |ios::in |ios::out);
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
				ofstream file2("registros.bin",ios::binary| ios::in|ios::out);
				file2.seekp(offset,ios::beg);
				const char* puntero = reinterpret_cast<const char*>(&infoRegistro[0]);
				size_t cantBytes = infoRegistro.size() * sizeof(infoRegistro[0]);
				file2.write(puntero,cantBytes);
				file2.close();
				cout << "Registro modificado!!"<<endl<<endl;
			}
		}

		if(opcion == 7){
			int rrn,offset;
			imprimirRegistros(registros);
			cout << "Ingrese el numero de registro que quiere borrar: ";
			cin >> rrn;
			offset = (rrn-1)*campos.size()*sizeof(Informacion);
			ofstream out("registros.bin", ios::in|ios::binary);
			out.seekp(offset,ios::beg);
			out.put('&');
			out.close();
			availList.push(rrn);
			cout << "Registro borrado"<<endl;
		}

		if(opcion == 8){

			/*Cargamos registros no marcados a memoria*/
			int begin,end,size;
			int rrn = 0;
			int offset = 0;

							/*Leer del Archivo*/
			ifstream file("registros.bin",ios::binary |ios::in |ios::out);
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
					offset = rrn*sizeof(Informacion);
					char c;
					file.seekg(offset,ios::beg);
					file.get(c);
					if(c != '&'){
						if(file.read(reinterpret_cast<char*>(&data),sizeof(data))) {
							infoRegistro.push_back(data);
							cout << "Hola"<<endl;
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
			ofstream file2("registros2.bin",ios::binary);
			const char* puntero = reinterpret_cast<const char*>(&infoRegistro[0]);
			size_t cantBytes = infoRegistro.size() * sizeof(infoRegistro[0]);
			file2.write(puntero,cantBytes);
			file2.close();
			cout << "NUEVO ARCHIVO CREADO"<<endl<<endl;


		}

		if(opcion == 9){

			


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
	cout << "4 - Guardar" << endl;
	cout << "5 - Cargar" << endl;
	cout << "6 - Modificar Registro"<<endl;
	cout << "7 - Borrar Registro"<<endl;
	cout << "8 - Buscar"<<endl;
	cout << "9 - Compactar"<<endl;
	cout << "10 - Salir" << endl;
	cout << "Ingrese opcion: ";
	cin >> opcion;
	return opcion;
}

void imprimirRegistros(vector<Registro> registros){
	cout << endl;
	for(int i = 0; i < registros.size(); i++){
		cout << "Registro #"<<i+1<<endl;
		for(int j = 0; j < registros.at(i).estructura.size(); j++){
			cout << registros.at(i).estructura.at(j).nombre<<": ";
			if(registros.at(i).estructura.at(j).tipo == 1){
				cout << registros.at(i).informacion.at(j).value << endl;
			}
			if(registros.at(i).estructura.at(j).tipo == 2){
				cout << registros.at(i).informacion.at(j).texto << endl;
			}
		}
		cout << endl;
	}
}

void salvarArchivo(vector<Campo> campos, vector<Registro> registros){
	ofstream file("estructura.bin",ios::binary);
	//ofstream file2("registros.bin",ios::binary);
	const char* pointer = reinterpret_cast<const char*>(&campos[0]);
	size_t bytes = campos.size() * sizeof(campos[0]);
	file.write(pointer, bytes);
	cout << endl;
	cout << "Estructura Guardada!!"<<endl;

	/*
	for(int i = 0; i < registros.size(); i++){
		const char* puntero = reinterpret_cast<const char*>(&registros.at(i).informacion[0]);
		size_t cantBytes = registros.at(i).informacion.size() * sizeof(registros.at(i).informacion[0]);
		file2.write(puntero,cantBytes);
	}
	*/

	file.close();
	//file2.close();
	cout << endl;
	//cout << "Registros Guardados!"<<endl;
	//cout << endl;
}

vector<Campo> cargarEstructura(){
	vector<Campo> campos;
	ifstream file("estructura.bin",ios::binary);
	Campo camp;
	while(true){
		if(file.read(reinterpret_cast<char*>(&camp), sizeof(camp))){
			campos.push_back(camp);
		} else {
			break;
		}
	}
	file.close();
	cout << "Estructura Cargada!"<<endl;
	cout << campos.size()<<endl; 
	return campos;
}

vector<Registro> cargarRegistros(vector<Campo> campos){
	ifstream file2("registros.bin",ios::binary);
	vector<Registro> registros;
	vector<Informacion> info;
	Informacion info1;
	int control = 1;
	while(true){
		if(control <= campos.size()){
			if(file2.read(reinterpret_cast<char*>(&info1),sizeof(info1))){
				info.push_back(info1);
				control++;
			} else {
				break;
			}
		} else if(control > campos.size()){
			Registro record;
			record.estructura = campos;
			record.informacion = info;
			registros.push_back(record);
			info.clear();
			control = 1;
		}
	}
	file2.close();
	cout << "Registros Cargados!" << endl;
	cout << registros.size() << endl;
	return registros;

}