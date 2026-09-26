# Tarea 1 - Planificacion Dieciochera - Sistema Operativo
Integrantes: Jasson Valverde - Carlos Araya
 
 Sección: 2

## Introducción: 
El señor Loyola quiere celebrar las Fiestas Patrias durante toda la semana, tratando la ramada como un evento no caótico sino como un sistema de forma estructurada de actividades con dependencias entre sí (por ejemplo, no se puede asar la carne sin antes prender el carbón y comprar la carne). Para esta tarea consiste en construir e implementar un **Simulador y Planificador de Actividades** que reciba un plan de actividades (plan.txt) en forma de DAG y las ejecute considerando lo siguiente:
 
* **Orden de dependencias**: entre las actividades (ya sea una actividad que no puede empezar, hasta que todas las actividades que necesita hayan terminado de manera eficiente).
*  **Límite de concurrencia (K = numero actividades)**: de modo que nunca haya más de K actividades ejecutandose al mismo tiempo.
* **Tolerancia a fallos**: En caso de que una actividad falla, solo se aborta la rama del plan que se dependían de ellas, sin detener el resto de la celebración.
*  **Señal de interrupción (Ctrl + C)**: Que simula la llegada de la autoridad ("la Seremi") y obliga a abortar toda actividad en curso.


## Explicacion del codigo:
Para la implementación del código, se nos exige usar el lenguaje de programación C / C++ , para esta tarea decidimos usar C++ por la facilidad de implementación, ademas que recalco que cada actividades se simula como un proceso de manera independiente (creado con *fork()*), que se comunica con el proceso principal mediante *pipes* con el propósito de avisar cuándo termina y si tuvo éxito o no.
Por otro lado el uso de hilos/hebras se encuentra prohibido para esta Tarea, por lo que toda la concurrencia del programa se resuelve exclusivamente con procesos, señales y tuberías.

* Compilacion:
```bash
g++ -Wall -Wextra -std=c++17 -lpthread -o planificador "Tarea1,1.cpp"
```
La flag *-lpthread* va solamente por la exigencia de la tarea que pide el comando exacto para realizar la compilación, ya que se prohíbe cualquier uso de hilo en ningún lado, toda la concurrencia es con procesos.

/*agregar explicacion de compilacion*/
* Como ejecutarlo::

```bash
./ planificador plan.txt K [prob_fallo]
```
* plan.txt --> Es un archivo con las actividades
* K --> Indica el numero máximo de actividades que se ejecuta en paralelo
* prob_fallo --> Es la probabilidad (entre 0 - 1) de que cada actividad falle, de lo contrario es 0

/*agregar explicacion de ejecucion*/

### Funciones Implementadas:

- *Parseo archivo plan.txt (fila entre 121 - 210)*

En la función *plan()* procede a leer el archivo linea por linea, además que separa cada linea con un ":" mediante la funcion auxiliar *split()* , que tiene el objetivo de validar el formato del archivo antes de armar cada actividad correspondiente, de lo contrario se retorna un false y no permitira su ejecucion: 
 ```cpp
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

```
El código funciona de tal manera que resuelve primero revisando el archivo si se encuentra vacío, en caso de ser así se genera un intervalo de tiempo de forma aleatoria entre 100 a 5000 ms usando una distribución de manera uniforme. En otro caso de que contenga un valor, ese valor se convierte de texto a numero enteros de forma directa mediante la función *stoi*. Luego de resolver el tiempo, se revisa si existe algún campo con las dependencias. Se separa por comas y cada ID que resulta se almacena en un vector *dep* de esa actividad, Por último, luego de que se leyó todas las actividades del archivo, se valida de que cada actividad no cuente con las mismas ID (para evitar inconsistencia) y que cada dependencia sea correspondiente a una actividad que sea parte del plan existente. En caso de que alguna de estas condiciones falle, el programa corta la ejecución retornando un mensaje de error.   

- *Modelado del DAG (fila entre 67 - 82)*
  
Para cada actividad es una estructura (struct) denominado *act*, con una lista de dependencia (*dep*) y, al revés, quien depende de ella es (*dep2*):
```cpp
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
```
En el vector *dep2* que se arma al final de la función *plan()*, que recorre las dependencia que fueron declaradas y que son guardadas en el enlace inverso de la actividad de la que se depende, de tal manera que cuando una actividad finaliza, pueda encontrar a quien pueda avisarle sin la necesidad de recorrer todo el grafo nuevamente. 

Para corroborar que el plan no tenga ciclos, mediante la función *val()* realiza un método de ordenamiento topológico tipo Kahn, que consiste en encolar las actividades sin dependencias pendientes, la va sacando de la cola y el contador de sus dependiente van disminuyendo, y al final se compara cuanta actividades se visitó con el total. En caso de que no calcen, hay un ciclo y el programa se detiene y retorna con un mensaje de error. 

- *Creacion de Procesos (fila entre 419 - 484)*

Por cada actividad lista y que no se llegue al límite K, se crean un pipe y se hace un *fork()*:

``` cpp
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
```
El código procede a sacar el primer ID de la cola y se crea un pipe con *pipe (p_cm)* con el propósito de que puedan comunicarse con el hijo que viene. la función *fork()*  duplica el proceso actual de la memoria con sus descriptores abiertos que están incluidos en ello y retorna 2 veces, el PID del hijo en el padre , 0 en el hijo, lo que permite poder distinguir en qué rama está cada uno.

* Nodo Hijo: Cierra el extremo del pipe que no usa (*p_cm[0]*), luego simula el trabajo con un *usleep()*, y se decide con una probabilidad en caso de que si falla o no. Con esa probabilidad se calcula con un generador propio con *getpid()* , de ser necesario porque el nodo hijo hereda el estado del generador del nodo padre al realizar un *fork()* y sin propagar todos los nodos hijos que generan la misma secuencia de forma aleatoria. Una vez que termina, el pipe escribe un pequeño mensaje y saliendo con (exit(0) o exit(1)), el código que el nodo padre usara después mediante *waitpid()* para poder saber el resultado real.
* Nodo Padre: 
























































