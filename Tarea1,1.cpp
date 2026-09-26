/* CODIGO TAREA 1 */

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
#include <csignal>
#include <sys/resource.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

    int static tam_min= 100;
    int static tam_max= 5000;   
    int static tam_mens= 256;

 enum class Est{
    lista, pendiente, ejecutando, completado, finalizado, abortado
 };

 string est_cam(Est estado){

    switch(estado){

        case Est::lista:
        return "Lista";
       
        case Est::pendiente:
        return "Pendiente";
       
        case Est::ejecutando:
        return "Ejecutando";
       
        case Est::completado:
        return "Completado";
       
        case Est::finalizado:
        return "Finalizado";
       
        case Est::abortado:
        return "Abortado";
       
        default:
        return "Desconocido";
    

    }

 }

    struct act{
        string id;
        string nom;
        int tiem=0;
        
        vector<string>dep;
        vector<string>dep2;
        int des_res=0;
        
        Est est=Est::pendiente;
        
        pid_t pid=-1;
        vector<int> ent;
        vector<int> sal;

 };

    unordered_map<string, act> Act;
    unordered_map<pid_t, string> pid_id;
    unordered_set<int> ids;


    int pend=0;
    int cor=0;
    float prob_fallo=0.0;
    int g_sigpipe[2];
    volatile sig_atomic_t sigpipe_recb = 0;

    
 string trim(const string& s) {

    size_t in = s.find_first_not_of(" \t\n\r");
    if (in == string::npos) return "";

    size_t fin = s.find_last_not_of(" \t\n\r");
    return s.substr(in, fin - in + 1);
    
}
 
vector<string>split (const string& s, char delimiter) {
    
    vector<string> tokens;
    string token;       

    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter))
    {

        tokens.push_back(trim(token));

    }
    return tokens;
}

    bool plan(string &ruta, mt19937 &rng) {
        int cont=0;
        string lin;
        uniform_int_distribution<int> distribucion(tam_min, tam_max);
        ifstream archivo(ruta);

        if (!archivo.is_open()){
        
        cout<<"no se pudo abrir el archivo"<<endl; 
        return false;
    
        }

        while(getline(archivo,lin)){

            cont++;
            string l= trim(lin);
            
            if(l.empty()) continue;

            vector<string>sep=split(l, ':');

            if(sep.size()<3) {
                
                cout<<"formato incorrecto en la linea: "<<cont<<endl; 
                return false;
            }


        act Nact;
        Nact.id= sep[0];
        Nact.nom= sep[1];
        
        if(sep[2].empty()){
        
            Nact.tiem= distribucion(rng);
        
        }
        
         else{ 
         
         Nact.tiem= stoi(sep[2]);

         }

        if(sep.size()>=4 && !sep[3].empty()){

            for(auto &dep_str: split(sep[3],',')){
                Nact.dep.push_back(dep_str);
            }   

        }
        
        if(Act.count(Nact.id)) {
        
        cout <<"error en la Actividad "<<Nact.id<<" ya que existe"<<endl; 
        return false;
    
    }
        Act[Nact.id]= Nact;
    }

    for( auto &par:Act){
        
        string id= par.first;
        act &act_a= par.second;
        act_a.des_res= act_a.dep.size();

        for(const string &dep_id :act_a.dep){
        
        if(!Act.count(dep_id)){

            cout<<"error en la actividad en el"<<id << "depende de una act inexistente:"<<dep_id<<endl;
            return false;

        }
        
        Act[dep_id].dep2.push_back(id);
          
    }

    if (act_a.dep.size()==0) {
    
    act_a.est= Est::lista;
    }

        }
            return true;

        }

    bool val(){
        
        unordered_map<string, int> res; 
        queue<string> q;
        size_t vis=0;

        for( const auto &par:Act){
                 
            res[par.first]=par.second.des_res;
    
        }
        for ( auto &par:res){

            if(par.second==0){
            
            q.push(par.first);
        
        }
        }  

        while(!q.empty()){

            string id=q.front();
             q.pop();
            vis++;
            
            for(auto &dep:Act[id].dep2){
            
                res[dep]--;

                if(res[dep]==0) {

                q.push(dep);

                }
            
            }

        }
        
        return vis==Act.size();

    }


void chao_mundo(string id_raiz){

    queue<string> n;

    for(const string &hijo: Act[id_raiz].dep2){

        n.push(hijo);

    } 

    while(!n.empty()){

      string id=n.front();
      n.pop();

      if(Act[id].est != Est::abortado){
      
      Act[id].est= Est::abortado;
      cout<<" aborto en cascada por falta de cosas: "<<Act[id].nom<<endl;
    
      for( const string &dep: Act[id].dep2){

        n.push(dep);

      }
    }
      }

}



//señales 

   void manejador(int signo){

        if(SIGINT == signo){
            
            char c= 'I';
            sigpipe_recb=1;

        if (write(g_sigpipe[1], &c, 1) == -1){

        } 

    }
}

void instalar(){

    if(pipe(g_sigpipe)==-1){

        perror("pipe");
        exit(1);

    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler= manejador;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=0;
    sigaction(SIGINT, &sa, nullptr);

}





int main( int argc, char *argv[])
{
//txt

    if (argc < 3) {
        
        cerr << "Uso: " << argv[0] << " plan.txt K [prob_fallo]" << endl;
        return 1;   

    }
    string ruta= argv[1];
    int K= atoi(argv[2]);

    if (K<=0){

        cout << "error K debe ser positivo"<<endl;
        return 1;
    }

    if (K>= FD_SETSIZE){

        cout << "error K es muy grande para el select" <<endl;
        return 1;
    } 
    
    if (argc >= 4) {
        prob_fallo = stof(argv[3]);

        if (prob_fallo < 0 || prob_fallo > 1) {
            cout << "prob_fallo fue invalido" << endl;
            return 1;
        }
    } 
    
    instalar();
    random_device rd;
    mt19937 rng(rd());

    if(!plan(ruta,rng)){

        cout<< "leyo mal el plan"<<endl;

        return 1;
    }

    if(!val()){

        cout<<"el plan tiene un ciclo"<<endl;
        return 1;

    }

    cout<< "Inicia la fonda; K="<<K<<endl;

    queue<string> ok;

    for (const auto &par:Act){

        if(par.second.des_res==0){

            ok.push(par.first);

        }
    }

    unordered_map <int,string> map_fds;
    fd_set read_fds;

    while(!ok.empty() || cor>0){

        if(sigpipe_recb){

            cout<<"SEREMI: chao"<<endl;
            
            for(pid_t p: ids){
                kill(p,SIGTERM);
            }
            
            usleep(50000);
            for(pid_t p: ids){

                
                kill(p,SIGKILL);
                waitpid(p,NULL,0);

            }

        cout<<"se aborto todo"<<endl;
        break; 

    }

    while(!ok.empty() && cor<K){

    string id_up= ok.front();
    ok.pop();
    
    int p_cm[2];

    if(pipe(p_cm)==-1){

        perror("pipe");
        exit(1);

    }

    pid_t pid=fork();
    
    if(pid<0){
        perror("hijo mal creado o error en fork");
        exit(1);
       
    }

    else if(pid==0){

        close(p_cm[0]);
        usleep(Act[id_up].tiem*1000);
        
        mt19937 local_rng(getpid());
        uniform_real_distribution<float> prob_dist(0.0, 1.0);

        if(prob_dist(local_rng) < prob_fallo){

            string msj= "fallo "+id_up;
            write(p_cm[1], msj.c_str(), msj.size()+1);
            close(p_cm[1]);
            exit(1);
        
        }


        else {
        string msj="fin"+id_up;
        
        write(p_cm[1], msj.c_str(), msj.size()+1);
        close(p_cm[1]);
        exit(0);
    }

    }
    else{

        close(p_cm[1]);

        Act[id_up].pid=pid;
        Act[id_up].est=Est::ejecutando;

        ids.insert(pid);
        pid_id[pid]=id_up;
        map_fds[p_cm[0]]=id_up;
        cor++;
        cout<<"ejecutando"<<Act[id_up].nom << "con pid: "<< pid<<endl;

    }
    

    }

    FD_ZERO(&read_fds);
    FD_SET(g_sigpipe[0], &read_fds);
    int max_fd= g_sigpipe[0];
    
    for(const auto &par: map_fds){

        FD_SET(par.first, &read_fds);
        
        if(par.first>max_fd){

            max_fd= par.first;

        }

    }

    if(select(max_fd+1, &read_fds, NULL, NULL, NULL)== -1){

        if(errno==EINTR) continue;
    
    }

    vector<int> borrar;

    for(const auto &par: map_fds){

        int fhijo= par.first;
        
        if(FD_ISSET(fhijo, &read_fds)){

            char buffer[tam_mens]= {0};
            read(fhijo, buffer, tam_mens);

            string act_id= par.second;
            pid_t pidH=Act[act_id].pid; 

            int sta;
            waitpid(pidH, &sta, 0);

            if(WIFEXITED(sta) && WEXITSTATUS(sta)==0){

                Act[act_id].est=Est::finalizado;
                cout<< "termino de actividad: "<<Act[act_id].nom << "con mensaje " <<buffer<<endl;
            
                for(const string &dep: Act[act_id].dep2){

                    Act[dep].des_res--;

                    if(Act[dep].des_res==0 && Act[dep].est!=Est::abortado){

                        Act[dep].est=Est::lista;
                        ok.push(dep);

                    }

                }


            }
            else{

                Act[act_id].est=Est::abortado;
                cout<<"fallo la actividad: "<<Act[act_id].nom << "con mensaje "<<buffer<<endl;
                chao_mundo(act_id);
            }
        
        ids.erase(pidH);
        cor--;
        close(fhijo);
        borrar.push_back(fhijo);
        
    }

    }


    for(int fd: borrar){

        map_fds.erase(fd);

    }
    }
    if(!sigpipe_recb){

        cout<<" se completo la fonda"<<endl;
    }



  return 0;
}
