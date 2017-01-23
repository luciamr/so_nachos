// synch.cc 
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks 
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(const char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List<Thread*>;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    
    while (value == 0) { 			// semaphore not available
	queue->Append(currentThread);		// so go to sleep
	currentThread->Sleep();
    } 
    value--; 					// semaphore available, 
						// consume its value
    
    interrupt->SetLevel(oldLevel);		// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
	scheduler->ReadyToRun(thread);
    value++;
    interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!

// Plancha 2 - Ejercicio 1
Lock::Lock(const char* debugName) {
	name = debugName;
	semaphore = new Semaphore(debugName, 1);
}

Lock::~Lock() {}

void Lock::Acquire() {
	ASSERT(!this->isHeldByCurrentThread());
	semaphore->P();
	thread = currentThread;
}

void Lock::Release() {
	ASSERT(this->isHeldByCurrentThread());
	semaphore->V();
	thread = NULL;
}

bool Lock::isHeldByCurrentThread() {
	return (currentThread == thread);
}

Condition::Condition(const char* debugName, Lock* conditionLock) {
	name = debugName;
	lock = conditionLock;
	sem = new Semaphore("sem " + debugName, 0);
	count = 0;
	countLock = new Lock("lock " + debugName);
	handshake = new Semaphore("handshake " + debugName, 0);
}

Condition::~Condition() { }
void Condition::Wait() {
	//ASSERT(false);
	countLock->Acquire();
	count++;
	countLock->Release();
	lock->Release();
	sem->P();
	handshake->V();
	lock->Acquire();
}

void Condition::Signal() {
	countLock->Acquire();
	if (count > 0) {
		count--;
		sem->V();
		handshake->P();
	}
	countLock->Release();
}

void Condition::Broadcast() {
	countLock->Acquire();
	for(int i = 0; i < count; i++) {
		sem->V();
	}
	while(count > 0) {
		count--;
		handshake->P();
	}
	countLock->Release();
}

Puerto::Puerto(const char* debugName) {
	name = debugName;
	//buffer = new std::queue<int>();
	sem = new Lock(debugName);
	vacio = new Condition(debugName, sem);
	lleno = new Condition(debugName, sem);
}

Puerto::~Puerto() { }

void Puerto::Send(int mensaje) {
	sem->Acquire();
	lleno->Wait();
	buffer = mensaje;
	//buffer->push(mensaje);
	vacio->Signal();
	sem->Release();
}

void Puerto::Receive(int* mensaje) {
	sem->Acquire();
	//while(buffer->empty()) {
	//	vacio->Wait();
	//}
	vacio->Wait();
	//*mensaje = buffer->front();
	//buffer->pop();
	*mensaje = buffer;
	lleno->Signal();
	sem->Release();
}

