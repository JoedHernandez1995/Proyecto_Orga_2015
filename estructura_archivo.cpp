#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

struct Campo{
	char nombre[20];
	int tipo;
	int longitud;
};

struct Informacion{
	int value;
	string texto;
};

struct Registro{
	vector<Campo> estructura;
	vector<Informacion> informacion;
};

int menu();
void imprimirRegistros(vector<Registro>);

int main(int argc, char*argv[]){
	int cantCampos;
	vector<Campo> campos;
	vector<Informacion> info;
	vector<Registro> registros;

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
			cout << "Estructura Creada!"<<endl;
		}
		if(opcion == 2){
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
					cout << temp.texto.size() << endl;
					if(temp.texto.size() < campos.at(i).longitud){
						for(int j = temp.texto.size(); j < campos.at(i).longitud; j++){
							cout << j << endl;
							temp.texto += '-';
						}
					}
					temp.value = 0;
					info.push_back(temp);
				}
			}
			Registro registro;
			registro.estructura = campos;
			registro.informacion = info;
			registros.push_back(registro);
			cout << "Registro creado"<<endl;
			info.clear();
		}
		if(opcion == 3){
			imprimirRegistros(registros);
		}
		if(opcion == 4){
			ofstream file("estructura.bin",ios::binary);
			ofstream file2("registros.bin",ios::binary);
			const char* pointer = reinterpret_cast<const char*>(&campos[0]);
			size_t bytes = campos.size() * sizeof(campos[0]);
			file.write(pointer, bytes);
			cout << "Estructura Guardada!!"<<endl;
			for(int i = 0; i < registros.size(); i++){
				const char* puntero = reinterpret_cast<const char*>(&registros.at(i).informacion[0]);
				size_t cantBytes = registros.at(i).informacion.size() * sizeof(registros.at(i).informacion[0]);
				file2.write(puntero,cantBytes);
			}
			cout << "Registros Guardados!"<<endl;
		}
		if(opcion == 5){
			ifstream file("estructura.bin",ios::binary);
        	Campo camp;
        	while(true){
        		if(file.read(reinterpret_cast<char*>(&camp), sizeof(camp))){
        			campos.push_back(camp);
        		} else {
        			break;
        		}
        	}

        	ifstream file2("registros.bin",ios::binary);
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
		}
		
		if(opcion == 6){
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
	cout << "6 - Salir" << endl;
	cout << "Ingrese opcion: ";
	cin >> opcion;
	return opcion;
}

void imprimirRegistros(vector<Registro> registros){
	cout << endl;
	for(int i = 0; i < registros.size(); i++){
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