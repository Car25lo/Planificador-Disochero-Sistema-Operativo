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
   /agregar comando de compilacion/
