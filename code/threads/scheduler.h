// scheduler.h 
//	Data structures for the thread dispatcher and scheduler.
//	Primarily, the list of threads that are ready to run.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "copyright.h"
#include "list.h"
#include "thread.h"

#define NUM_COLAS_PRIORIDAD (MAX_PRIORIDAD - MIN_PRIORIDAD + 1)

// The following class defines the scheduler/dispatcher abstraction
// the data structures and operations needed to keep track of which
// thread is running, and which threads are ready but not running.

class Scheduler {
  public:
    Scheduler();	// Initialize list of ready threads
    ~Scheduler();	// De-allocate ready list

    void ReadyToRun(Thread* thread);	// Thread can be dispatched.
    Thread* FindNextToRun();			// Dequeue first thread on the ready list, if any, and return thread.
    void Run(Thread* nextThread);		// Cause nextThread to start running
    void Print();						// Print contents of ready list
    
  private:
    //Plancha 2 - Ej 4
    //En vez de tener un solo puntero, paso a tener un arreglo de punteros, uno por cada cola
    List<Thread*> *readyList[NUM_COLAS_PRIORIDAD]; // Queue of threads that are ready to run, but not running
};

#endif // SCHEDULER_H
