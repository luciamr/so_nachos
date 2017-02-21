// synchconsole.cc

#include "synchconsole.h"

//----------------------------------------------------------------------
// SynchConsole::SynchConsole
//	Inicializa la interfaz de acceso sincronizado a la consola.
//	"in" -- archivo de UNIX que simula el teclado (NULL -> usa stdin)
//	"out" -- archivo de UNIX que simula la salida (NULL -> usa stdout)
//----------------------------------------------------------------------

SynchConsole::SynchConsole(const char *in, const char *out)
{
	//Console recibe los handler de interrupciones para poder manejar las callback
	console = new Console(in, out, ReadAvail, WriteDone, 0);
	getLock = new Lock("synchconsole_getLock");
    putLock = new Lock("synchconsole_putLock");
    readAvailSemaphore = new Semaphore("synchconsole_readAvailSemaphore", 0);
    writeDoneSemaphore = new Semaphore("synchconsole_writeDoneSemaphore", 0);
}

//----------------------------------------------------------------------
// SynchConsole::~SynchConsole
// 	Desalloca las estructuras utilizadas.
//----------------------------------------------------------------------

SynchConsole::~SynchConsole()
{
    delete console;
    delete getLock;
    delete putLock;
    delete readAvailSemaphore;
    delete writeDoneSemaphore;
}

//----------------------------------------------------------------------
// SynchConsole::GetChar
//  Espera que haya un caracter disponible, para luego poder leerlo y
//  devolverlo.
//----------------------------------------------------------------------

char
SynchConsole::GetChar()
{
	char c;

    getLock->Acquire();
    readAvailSemaphore->P(); //espera que el caracter esté disponible
    c = console->GetChar();
    getLock->Release();
}

//----------------------------------------------------------------------
// SynchConsole::PutChar
// 	Escribe un caracter y espera hasta que se complete la escritura.
//----------------------------------------------------------------------

void
SynchConsole::PutChar(char c)
{
    putLock->Acquire();
    console->PutChar(c);
    writeDoneSemaphore->P(); //espera que se termine de escribir
    putLock->Release();
}

//----------------------------------------------------------------------
// SynchConsole::ReadAvail
// 	Manejador de interrupciones. Despierta al thread que está esperando
//  un caracter para leer.
//----------------------------------------------------------------------

void
SynchConsole::ReadAvail()
{
    readAvailSemaphore->V();
}

//----------------------------------------------------------------------
// SynchConsole::WriteDone
// 	Manejador de interrupciones. Despierta al thread que está esperando
//  que se complete la escritura.
//----------------------------------------------------------------------

void
SynchConsole::WriteDone()
{
    writeDoneSemaphore->V();
}
