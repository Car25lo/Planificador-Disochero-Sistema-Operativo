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

- *Parseo archivo plan.txt (fila entre 134 - 147)*

 La función *plan()* procede a leer el archivo linea por linea, además que separa cada linea con un ":" mediante la funcion auxiliar *split()* , que tiene el objetivo de validar el formato del archivo antes de armar cada actividad correspondiente, de lo contrario se retorna un false y no permitira su ejecucion: 
 ```cpp
while(getline(archivo,lin)){

    string l= trim(lin);

    if(l.empty()) continue;
 
    vector<string>sep=split(l, ':');

    if(sep.size()<3) {

        cout<<"formato incorrecto en la linea: "<<cont<<endl;

        return false;
    }
    ...
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


























