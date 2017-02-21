// synchconsole.h

#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#include "console.h"
#include "synch.h"


class SynchConsole {
  public:
    SynchConsole(const char *in, const char *out);
    ~SynchConsole();

    char GetChar();
    void PutChar(const char c);
    void ReadAvail(); //Invocado por el manejador de interrupciones para señalar que está en condiciones de leer
    void WriteDone(); //Invocado por el manejador de interrupciones para señalar que terminó de escribir

  private:
    Console *console;
    Lock *getLock; //Solamente un pedido de lectura puede ser enviado a la vez
	Lock *putLock; //Solamente un pedido de escritura puede ser enviado a la vez
    Semaphore *readAvailSemaphore;
    Semaphore *writeDoneSemaphore;
};

#endif // SYNCHCONSOLE_H
