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

- *Creación de Procesos (fila entre 419 - 484)*
  
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
* Nodo Padre: También cierra su extremo que no usa (*p_cm[1]*), guarda el PID y el FD de lectura (usando *ids* y *map_fds* de forma respectiva, para usarlo más adelante en la función *select()* y con el manejo de Ctrl + C), además que durante la ejecución, retorna un mensaje como “*Est : :ejecutando*”, y suma uno a *cor*. Por ende no se bloquea esperando a este nodo hijo, continúa de forma inmediata con la siguiente actividad lista mientras sigan dentro de los límites de K actividades, luego recién se entera de que finalizó cuando la función *select()* avisa.  

- *Concurrencia sin busy-waiting ni condiciones de carrera (fila entre 486-506)*

Como habíamos dicho que K es el número máximo de actividades , por ende se debe cumplir con la condición de *cor*<K dentro del ciclo *while*, evitando que se quede mas de K procesos vivos al mismo tiempo, el resto debe esperar en la cola *ok*. 

En lugar de andar preguntando en un loop si algún nodo hijo terminó, se arma un *fd_set* con todos los pipes que se encuentren activos y se bloquean la función *select()* hasta que llegue algo:
  

``` cpp
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
```
Con esto el proceso principal evita consumir recursos de la CPU cuando no hay novedades (sin busy-waiting). Además que no hay condiciones de carrera porque no hay memoria compartida entre los proceso ya que cada nodo hijo tiene su “copia” de todo (especialmente lo que hereda del *fork()* ), y la única comunicación es mediante pipes, ya que el kernel se sincroniza solo. El nodo padre es el único que modifica el estado (*Act*,*cor*,*ok*), siempre de forma secuencial dentro de su propio loop.

- *Pasos de mensajes (pipes) (fila entre 516 - 526) *

Para cada actividad está compuesto por un pipe, que fue creado justo antes del *fork()*. El nodo hijo escribe el mensaje corto (*”fin<id>”* o *”fallo <id>”*) antes de morir. El nodo padre, cuando la función *select()* le avisa que ese fd tiene datos, procede a leer el mensaje y utiliza un *waitpid()* para obtener el código de la salida real del proceso:

```cpp
            char buffer[tam_mens]= {0};
            read(fhijo, buffer, tam_mens);

            string act_id= par.second;
            pid_t pidH=Act[act_id].pid; 

            int sta;
            waitpid(pidH, &sta, 0);

            if(WIFEXITED(sta) && WEXITSTATUS(sta)==0){
```
El código de salida (*WEXITSTATUS(sta))*, no el texto del mensaje leído del pipe, es lo que realmente decide si la actividad se marca como *Est: : finalizado* o *Est : : abortado*. 

- *Aislamiento de errores (fila entre 257 - 285)*
  
En caso de que una actividad falle, retorna un mensaje (*”Est : : abortado”*) y llama a la función *chao_mundo()*, que tiene el objetivo de recorrer con BFS (Breadth-First Search) en todas las actividades que dependen de ella ya sea de forma directa o indirecta y las marca abortadas también:
```cpp
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
```
Por ende el resto de la ejecución sigue avanzando, el programa nunca se cierra por un fallo de una sola actividad, solo se corta la rama afectada para abortar la ejecución. 

- *Carga de estres* (pendiente)

## Justificación decisiones tomadas:
- Implementar *select()* en lugar de hilos o *poll()*: El uso de hilos no está permitido por el enunciado, en cambio al implementar la función *select()* permite esperar a varios pipes a la vez sin sondeo activo. Además que se optó por validar *K* con respecto a *fd_setsize* en lugar de migrar a *poll()* ´porque para la tarea *K* siempre estará muy por debajo de 1024.
- Un pipe por cada actividad : Es mejor un pipe por cada actividad ya que queda directo al momento de asociar cada descriptor de lectura con sus respectivos ID de la actividad (el mapa *map_fds* hace justo eso), sin la necesidad de armar un protocolo de mensajes con encabezados para poder diferenciar de quien viene cada aviso del pipes.
- Mensaje de texto simples por cada pipes: Ya que el resultado real (éxito / fallo) ya se obtiene del código de salida mediante *waitpid()*, que es confiable. En cambio el mensaje por el pipe solamente es para avisar “ya termine”, y no se considera una fuente de verdad del resultado.
- Aborto en cascada con BFS: Al usar la lista inversa *dep2* que ya está montada para parsear, la función *chao_mundo()* solo visita a las actividades que realmente están afectadas por el fallo, sin la necesidad de recorrer las ramas que no tiene nada de relevancia
- Pipes extra para SIGINT (*g_sigpipe*): Dentro de un manejador de señales no es seguro llamar a funciones complejas, así que el manejador solo prende una bandera (flag) y escribe un byte en un pipe adicional. Ese pipe utiliza el mismo *fd_set* de las actividades, así que la función *select()* reacciona ante un Ctrl + C sin la necesidad de explorar adicionalmente. En caso de que se detecte la bandera (flag), se envía a *SIGTERM* a todos los procesos vivos y, después de un periodo corto, el *SIGKILL* a los procesos que siguen sin responder , ya que esto evita dejar algún nodo hijo huérfano. 














































