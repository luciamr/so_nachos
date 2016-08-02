// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create several threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustrate the inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.
//
// Parts from Copyright (c) 2007-2009 Universidad de Las Palmas de Gran Canaria
//

#include "copyright.h"
#include "system.h"
#include "synch.h"

//Plancha 1 - 15
#ifdef SEMAPHORE_TEST
    static Semaphore *semaphore;
#endif

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 10 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"name" points to a string with a thread name, just for
//      debugging purposes.
//----------------------------------------------------------------------

void
SimpleThread(void* name)
{
    // Reinterpret arg "name" as a string
    char* threadName = (char*)name;
    
    //Plancha 1 - 15/16
    #ifdef SEMAPHORE_TEST
        semaphore->P();
        DEBUG('s', "Thread \"%s\" -> Semaphore \"%s\" -> P()\n", threadName, semaphore->getName());
    #endif
    
    // If the lines dealing with interrupts are commented,
    // the code will behave incorrectly, because
    // printf execution may cause race conditions.
    for (int num = 0; num < 10; num++) {
        IntStatus oldLevel = interrupt->SetLevel(IntOff); //orig comentado
	    printf("*** thread %s looped %d times\n", threadName, num);
	    interrupt->SetLevel(oldLevel); //orig comentado
        currentThread->Yield();
    }
    IntStatus oldLevel = interrupt->SetLevel(IntOff); //orig comentado
    printf(">>> Thread %s has finished\n", threadName);
    interrupt->SetLevel(oldLevel); //orign comentado
    
    //Plancha 1 - 15/16
    #ifdef SEMAPHORE_TEST
        semaphore->V();
        DEBUG('s', "Thread \"%s\" -> Semaphore \"%s\" -> V()\n", threadName, semaphore->getName());
    #endif
}

//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between several threads, by launching
//	ten threads which call SimpleThread, and finally calling 
//	SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest()
{
    DEBUG('t', "Entering SimpleTest");
    
    /* original
    char *threadname = new char[128];
    strcpy(threadname,"Hilo 1");
    Thread* newThread = new Thread (threadname);
    newThread->Fork (SimpleThread, (void*)threadname);
    */

    //Plancha 1 - 15
    #ifdef SEMAPHORE_TEST
        semaphore = new Semaphore("threadtest semaphore", 3);
    #endif
    
    //Plancha 1 - 14
    char *threadname1 = new char[128];
    strcpy(threadname1,"Hilo 1");
    Thread* newThread1 = new Thread (threadname1);
    newThread1->Fork (SimpleThread, (void*)threadname1);
    char *threadname2 = new char[128];
    strcpy(threadname2,"Hilo 2");
    Thread* newThread2 = new Thread (threadname2);
    newThread2->Fork (SimpleThread, (void*)threadname2);
    char *threadname3 = new char[128];
    strcpy(threadname3,"Hilo 3");
    Thread* newThread3 = new Thread (threadname3);
    newThread3->Fork (SimpleThread, (void*)threadname3);
    char *threadname4 = new char[128];
    strcpy(threadname4,"Hilo 4");
    Thread* newThread4 = new Thread (threadname4);
    newThread4->Fork (SimpleThread, (void*)threadname4);
    
    SimpleThread((void*)"Hilo 0");
}

