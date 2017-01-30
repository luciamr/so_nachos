// synch.h
//	NOTA: Éste es el único fichero fuente con los comentarios en español
//	2000 - José Miguel Santos Espino - ULPGC
//
//	Estructuras de datos para sincronizar hilos (threads)
//
//	Aquí se definen tres mecanismos de sincronización: semáforos
//	(semaphores), cerrojos (locks) y variables condición (condition var-
//	iables). Sólo están implementados los semáforos; de los cerrojos y
//	variables condición sólo se proporciona la interfaz. Precisamente el
//	primer trabajo incluye realizar esta implementación.
//
//	Todos los objetos de sincronización tienen un parámetro "name" en
//	el constructor; su única finalidad es facilitar la depuración del
//	programa.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// synch.h -- synchronization primitives.  

#ifndef SYNCH_H
#define SYNCH_H

#include "copyright.h"
#include "thread.h"
#include "list.h"

// La siguiente clase define un "semáforo" cuyo valor es un entero positivo.
// El semáforo ofrece sólo dos operaciones, P() y V():
//
//	P() -- espera a que value>0, luego decrementa value
//
//	V() -- incrementa value, despiera a un hilo en espera si lo hay
//
// Observen que esta interfaz NO permite leer directamente el valor del
// semáforo -- aunque hubieras podido leer el valor, no te sirve de nada,
// porque mientras tanto otro hilo puede haber modificado el semáforo,
// si t� has perdido la CPU durante un tiempo.

class Semaphore {
  public:
    // Constructor: da un valor inicial al semáforo
    Semaphore(const char* debugName, int initialValue);	// set initial value
    ~Semaphore();   					// destructor
    const char* getName() { return name;}			// para depuración

    // Las �nicas operaciones p�blicas sobre el semáforo
    // ambas deben ser *at�micas*
    void P();
    void V();
    
  private:
    const char* name;        		// para depuración
    int value;         		// valor del semáforo, siempre es >= 0
    List<Thread*> *queue;       // Cola con los hilos que esperan en P() porque el
                       		// valor es cero
};

// La siguiente clase define un "cerrojo" (Lock). Un cerrojo puede tener
// dos estados: libre y ocupado. Sólo se permiten dos operaciones sobre
// un cerrojo:
//
//	Acquire -- espera a que el cerrojo está libre y lo marca como ocupado
//
//	Release -- marca el cerrojo como libre, despertando a algún otro
//                 hilo que estuviera bloqueado en un Acquire
//
// Por conveniencia, nadie excepto el hilo que tiene adquirido el cerrojo
// puede liberarlo. No hay ninguna operación para leer el estado del cerrojo.


class Lock {
  public:
  // Constructor: inicia el cerrojo como libre
  Lock(const char* debugName);

  ~Lock();          // destructor
  const char* getName() { return name; }	// para depuración

  // Operaciones sobre el cerrojo. Ambas deben ser *atómicas*
  void Acquire(); 
  void Release();

  // devuelve 'true' si el hilo actual es quien posee el cerrojo.
  // útil para comprobaciones en el Release() y en las variables condición
  bool isHeldByCurrentThread();	

  private:
    const char* name;				// para depuración
    // añadir aquí otros campos que sean necesarios
    Semaphore *semaphore;	// se implementa el lock usando un semáforo como base
    Thread *thread; //

};

//  La siguiente clase define una "variable condición". Una variable condición
//  no tiene valor alguno. Se utiliza para encolar hilos que esperan (Wait) a
//  que otro hilo les avise (Signal). Las variables condición están vinculadas
//  a un cerrojo (Lock). 
//  Estas son las tres operaciones sobre una variable condición:
//
//     Wait()      -- libera el cerrojo y expulsa al hilo de la CPU.
//                    El hilo se espera hasta que alguien le hace un Signal()
//
//     Signal()    -- si hay alguien esperando en la variable, despierta a uno
//                    de los hilos. Si no hay nadie esperando, no ocurre nada.
//
//     Broadcast() -- despierta a todos los hilos que están esperando
//
//
//  Todas las operaciones sobre una variable condición deben ser realizadas
//  adquiriendo previamente el cerrojo. Esto significa que las operaciones
//  sobre variables condición han de ejecutarse en exclusi�n mutua.
//
//  Las variables condición de Nachos deberáan funcionar seg�n el estilo
//  "Mesa". Cuando un Signal() o Broadast() despierta a otro hilo,
//  �ste se coloca en la cola de preparados. El hilo despertado es responsable
//  de volver a adquirir el cerrojo. Esto lo deben implementar en el cuerpo de
//  la funci�n Wait().
//  En contraste, tambi�n existe otro estilo de variables condición, seg�n
//  el estilo "Hoare", seg�n el cual el hilo que hace el Signal() pierde
//  el control del cerrojo y entrega la CPU al hilo despertado, quien se
//  ejecuta de inmediato y cuando libera el cerrojo, devuelve el control
//  al hilo que efectu� el Signal().
//
//  El estilo "Mesa" es algo más fácil de implementar, pero no garantiza
//  que el hilo despertado recupere de inmediato el control del cerrojo.

class Condition {
 public:
    // Constructor: se le indica cuál es el cerrojo al que pertenece
    // la variable condición
    Condition(const char* debugName, Lock* conditionLock);	

    // libera el objeto
    ~Condition();	
    const char* getName() { return (name); }

    // Las tres operaciones sobre variables condición.
    // El hilo que invoque a cualquiera de estas operaciones debe tener
    // adquirido el cerrojo correspondiente; de lo contrario se debe
    // producir un error.
    void Wait(); 	
    void Signal();   
    void Broadcast();

  private:
    const char* name;
    Lock* lock;
    int waiters;
    Lock* waitersLock;
    Semaphore* sem;
    Semaphore* handshake;
};

/*

C�digo original del Nachos para las variables condición - NO USAR
  
class Condition {
  public:
    Condition(char* debugName);		// initialize condition to 
					// "no one waiting"
    ~Condition();			// deallocate the condition
    char* getName() { return (name); }
    
    void Wait(Lock *conditionLock); 	// these are the 3 operations on 
					// condition variables; releasing the 
					// lock and going to sleep are 
					// *atomic* in Wait()
    void Signal(Lock *conditionLock);   // conditionLock must be held by
    void Broadcast(Lock *conditionLock);// the currentThread for all of 
					// these operations

  private:
    char* name;
    // plus some other stuff you'll need to define
};

*/

class Puerto {
  public:
  // Constructor: inicia el puerto
  Puerto(const char* debugName);

  ~Puerto();          // destructor

  const char* getName() { return name; }	// para depuración

  // Operaciones sobre el puerto
  void Send(int mensaje);
  void Receive(int* mensaje);

  private:
    const char* name; // para depuración
    int buffer;
    bool full; //indica si hay información esperando ser leída
    Lock* lock;
    Condition* sendCondition;
    Condition* receiveCondition;
};

#endif // SYNCH_H
