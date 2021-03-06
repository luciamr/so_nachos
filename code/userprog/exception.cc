// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "memoriavirtual.h"
#include "openfile.h"
#include "utility.h"
#include "thread.h"

#define FILENAME_MAX_LENGTH 128
#define ARGS_MAX
#define BUFFER_MAX_LENGTH 256
#define MIN(x,y) (((x)<(y))?(x):(y))

char **SaveArgs(int address);

//Es necesario aumentar el Program Counter al finalizar la instrucción.
//De lo contrario entra en un loop y ejecuta la misma sentencia una y otra vez.
void UpdateProgramCounter()
{
	int pc;

	pc = machine->ReadRegister(PCReg);
	machine->WriteRegister(PrevPCReg, pc);
	pc = machine->ReadRegister(NextPCReg);
	machine->WriteRegister(PCReg, pc);
	pc += 4;
	machine->WriteRegister(NextPCReg, pc);
}

void ProcessCreator(int arg)
{
	currentThread->space->InitRegisters();
	currentThread->space->RestoreState();

	machine->Run(); //lleva al user program
	ASSERT(false); //nunca retorna de machine->Run()
}

void HandlerHalt()
{
	DEBUG('c', "Shutdown, initiated by user program.\n");
   	interrupt->Halt();
}

void HandlerCreate()
{
	char fileName[FILENAME_MAX_LENGTH];
	int initialSize = 0;
	int fileNameAddress = machine->ReadRegister(4);
	ReadStringFromUser(fileNameAddress, fileName, FILENAME_MAX_LENGTH - 1);

	if (fileSystem->Create(fileName, initialSize)) {
		DEBUG('c', "File %s has been successfully created.\n", fileName);
	}
	else {
		DEBUG('c', "Error, failed to create file %s.\n", fileName);
	}
}

void HandlerRead() {

	int bufferAddress = machine->ReadRegister(4);
	int size = machine->ReadRegister(5);
	OpenFileId fileId = machine->ReadRegister(6);
	int bufferSize = MIN(size, BUFFER_MAX_LENGTH - 1);
	char *buffer = (char *)malloc(sizeof(char *) * (bufferSize + 1)); //+ 1 para '\0'
	int bytesRead;

	switch (fileId) {
		case ConsoleOutput:
			bytesRead = 0;
			break;
		case ConsoleInput:
			for(bytesRead = 0; bytesRead < bufferSize; bytesRead++)
				buffer[bytesRead] = synchConsole->GetChar();
			buffer[bytesRead] = '\0';
			break;
		default:
			OpenFile *file = currentThread->getFilesTable()->Get(fileId);
			bytesRead = file->Read(buffer, size);
			break;
	}

	DEBUG('c', "Read: %s\n", buffer);
	WriteBufferToUser(buffer, bufferAddress, bytesRead);
	machine->WriteRegister(2, bytesRead); //escribe la cantidad de caracteres leídos (return de Read)
}

void HandlerWrite() {

	int bufferAddress = machine->ReadRegister(4);
	int size = machine->ReadRegister(5);
	OpenFileId fileId = machine->ReadRegister(6);
	int bufferSize = MIN(size, BUFFER_MAX_LENGTH - 1);
	char *buffer = (char *)malloc(sizeof(char *) * (bufferSize + 1)); //+ 1 para '\0'
	//char buffer[BUFFER_MAX_LENGTH];
	int bytesWritten;

	ReadBufferFromUser(bufferAddress, buffer, size);
	DEBUG('c', "Write: %s\n", buffer);

	switch (fileId) {
		case ConsoleOutput:
			for(bytesWritten = 0; bytesWritten < bufferSize && buffer[bytesWritten] != '\0'; bytesWritten++)
				synchConsole->PutChar(buffer[bytesWritten]);
			break;
		case ConsoleInput:
			DEBUG('c', "Unable to write to console input.\n");
			break;
		default:
			OpenFile *file = currentThread->getFilesTable()->Get(fileId);
			if (file == NULL) {
				DEBUG('c', "The file could NOT be found, unable to write to file: OpenFileId -> %d.\n", fileId);
				return;
			}
			file->Write(buffer, bufferSize);
			break;
	}
}

void HandlerOpen()
{
	char fileName[FILENAME_MAX_LENGTH];
	int fileNameAddress = machine->ReadRegister(4);
	OpenFile *file;
	OpenFileId fileId;

	ReadStringFromUser(fileNameAddress, fileName, FILENAME_MAX_LENGTH - 1);

	file = fileSystem->Open(fileName);

	if (file != NULL) {
		fileId = currentThread->getFilesTable()->Insert(file);
		DEBUG('c', "File %s has been successfully opened.\n", fileName);
	}
	else {
		DEBUG('c', "Error, failed to open file %s.\n", fileName);
	}

	machine->WriteRegister(2, fileId); //escribe la id del archivo (return de Open)
}

void HandlerClose()
{
	int fileId = machine->ReadRegister(4);

	currentThread->getFilesTable()->Delete(fileId);
	DEBUG('c', "File has been successfully closed: OpenFileId -> %d.\n", fileId);
}

void HandlerExit()
{

	int value = machine->ReadRegister(4);
	int spaceId = processesTable->GetSpaceId(currentThread);

	if (value == 0)
		DEBUG('c', "The user program has exited normally.\n");
	else
		DEBUG('c', "The user program has NOT exited normally.\n");

	//If the spaceId was found, save the exit value (needed in case of join)
	if (spaceId != -1)
		processesTable->SetExitValue(spaceId, value);

	currentThread->Finish();
}

void HandlerJoin()
{
	SpaceId spaceId = machine->ReadRegister(4);
	bool zombieThread = processesTable->Zombie(spaceId);
	Thread *thread;
	int threadExitValue;

	if (zombieThread) {
		DEBUG('c', "The process is a zombie or could NOT be found, unable to join: SpaceId -> %d.\n", spaceId);
		return;
	}

	thread = processesTable->GetProcess(spaceId);

	if (thread == NULL) {
			DEBUG('c', "The process could NOT be found, unable to join: SpaceId -> %d.\n", spaceId);
			return;

	}

	thread->Join();

	threadExitValue = processesTable->GetExitValue(spaceId);
	machine->WriteRegister(2, threadExitValue);
}

void HandlerExec()
{
	char fileName[FILENAME_MAX_LENGTH];
	char **args;
	int fileNameAddress = machine->ReadRegister(4);
    int argsAddress = machine->ReadRegister(5);

	Thread *newThread;
	SpaceId spaceId;
	OpenFile *file;
	AddrSpace *space;

	ReadStringFromUser(fileNameAddress, fileName, FILENAME_MAX_LENGTH - 1);
	args = SaveArgs(argsAddress);
	if (args == NULL) {
		DEBUG('c', "Unable to read args for %s, unable to Exec.\n", fileName);
		return;
	}

	file = fileSystem->Open(fileName);
	if (file == NULL) {
		DEBUG('c', "The file %s could NOT be found, unable to Exec.\n", fileName);
		return;
	}

    space = new AddrSpace(file);
    newThread = new Thread(fileName, true);
    newThread->space = space;
    spaceId = processesTable->Insert(newThread);
    machine->WriteRegister(2, spaceId);
	newThread->Fork((VoidFunctionPtr)ProcessCreator, (void *)args);

    delete file;
}

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------
void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    //printf("Exception Handler: %d\n", type);

    if (which == SyscallException) {
    	switch (type) {
    		case SC_Halt:
    			HandlerHalt();
    		   	break;
    		case SC_Create:
    			HandlerCreate();
    			UpdateProgramCounter();
    			break;
    		case SC_Read:
    			HandlerRead();
    			UpdateProgramCounter();
    			break;
    		case SC_Write:
    			HandlerWrite();
    			UpdateProgramCounter();
    			break;
    		case SC_Open:
    			HandlerOpen();
    			UpdateProgramCounter();
    			break;
    		case SC_Close:
    			HandlerClose();
    			UpdateProgramCounter();
    			break;
    		case SC_Exit:
    			HandlerExit();
    			UpdateProgramCounter();
    			break;
    		case SC_Join:
    			HandlerJoin();
    			UpdateProgramCounter();
    			break;
    		case SC_Exec:
    			HandlerExec();
    			UpdateProgramCounter();
    			break;
    		default:
    			printf("Unknown syscall code %d\n", type);
    			ASSERT(false);
    	}
    }
    else {
    	printf("Unexpected user mode exception %d %d\n", which, type);
    	ASSERT(false);
    }

}
