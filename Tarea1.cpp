//para almacenar y completar la tarea 
/*#include <iostream>
#include <string>
#include<fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include<unordered_set>
//librerias para manejo de procesos
#include <unistd.h>
#include<sys/resource.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <signal.h>*/
#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
 
#include <signal.h>
#include <sys/resource.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


using namespace std;
//limite de los mensajes 
static int tam_mens=256;
static int tam_min=100;
static int tam_max=5000;

//modelos de estados de las actividades
enum class Estados{
    lista,
    pendiente,
    ejecutando,
    completado,
    finalizado,
    abortado
};
string estadoToString(Estados estado) {
    switch (estado) {

        case Estados::lista:
            return "Lista";
        case Estados::pendiente:
            return "Pendiente";
        case Estados::ejecutando:
            return "Ejecutando";
        case Estados::completado:
            return "Completado";
        case Estados::finalizado:
            return "Finalizado";
        case Estados::abortado:
            return "Abortado";
        default:
            return "Desconocido";
    }
}
struct act{
    string id;
    string nombre;
  int tiempo=0;
  vector<string> dependencias;
  vector<string> dependientes;
  int dep_restantes=0;
  Estados estado=Estados::lista;
  int pid=-1;

  vector<int> entradas;
  vector<int> salidas;

};
//estados del planificador 
unordered_map<string, act> actividades;
unordered_map<pid_t, string> pid_id;

unordered_set<int> pids_ejecutando;
int pendientes=0;
int corriendo=0;
float prob_fallo=0.0;

int g_sigpipe[2];
volatile sig_atomic_t sigpipe_recibido = 0;

//utilidades strings

string trim(const string& s) {
size_t in= s.find_first_not_of(" \t\n\r");
if (in== string::npos) return "";
size_t fin = s.find_last_not_of(" \t\n\r");
return s.substr(in, fin - in + 1);

}
vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}
//perseo plan 
bool plan_paseo(string &ruta, mt19937 &rng) {
  
ifstream archivo(ruta);

if (!archivo.is_open()) {
    cerr << "Error al abrir el archivo: " << ruta << endl;
    return false;}

int cont=0;

    uniform_int_distribution<int> distribucion(tam_min, tam_max);
    string linea;

    while (getline(archivo, linea)) {
    cont++;
    string l=trim(linea);

        if (l.empty()) continue;

        vector<string> partes = split(l, ':');

        if (partes.size() < 3) {
            cerr << "Formato incorrecto en la línea: " << cont << endl;    
            return false;
        }
    
    act a; 
    a.id=partes[0];
    a.nombre=partes[1];


    if (partes[2].empty()) {
        a.tiempo = distribucion(rng);
    } else {
    a.tiempo = stoi(partes[2]);
    }
    if (partes.size() >= 4 && !partes[3].empty()) {

    for(auto &dep : split(partes[3], ',')){
    
            a.dependencias.push_back(dep);
        }
    }

    if (actividades.count(a.id)){
        cerr << "Error: Actividad con ID duplicado: " << a.id << endl;
        return false;
    }

    actividades[a.id] = a;
    }
        return true;
    }

int main(){
    cout << "Hola mundo" << endl;
    return 0;
}