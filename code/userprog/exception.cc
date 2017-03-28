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

#define FILENAME_MAX_LENGTH 128
#define BUFFER_MAX_LENGTH 256

void HandlerHalt()
{
	DEBUG('a', "Shutdown, initiated by user program.\n");
   	interrupt->Halt();
}

void HandlerCreate()
{
	char fileName[FILENAME_MAX_LENGTH];
	int initialSize = 0;
	int fileNameAddress = machine->ReadRegister(4);
	ReadStringFromUser(fileNameAddress, fileName, FILENAME_MAX_LENGTH - 1);

	if (fileSystem->Create(fileName, initialSize)) {
		DEBUG('a', "File %s has been successfully created.\n");
	}
	else {
		DEBUG('a', "Error, failed to create file %s.\n", fileName);
	}
}

void HandlerRead() {

	int bufferAddress = machine->ReadRegister(4);
	int size = machine->ReadRegister(5);
	OpenFileId fileId = machine->ReadRegister(6);
	char buffer[BUFFER_MAX_LENGTH];
	int bytesRead;

	switch (fileId) {
		case ConsoleOutput:
			bytesRead = 0;
			break;
		case ConsoleInput:
			for(bytesRead = 0; bytesRead < size && bytesRead < BUFFER_MAX_LENGTH; bytesRead++)
				buffer[bytesRead] = synchConsole->GetChar();
			break;
		default:
			//completar, por ahora sólo a consola
			//OpenFile *file; // = currentThread-> (fileId);
			//bytesRead = file->Read(buffer, size);
			break;
	}

	WriteBufferToUser(buffer, bufferAddress, bytesRead);
	machine->WriteRegister(2, bytesRead); //escribe la cantidad de caracteres leídos (return de Read)
}

void HandlerWrite() {

	int bufferAddress = machine->ReadRegister(4);
	int size = machine->ReadRegister(5);
	OpenFileId fileId = machine->ReadRegister(6);
	char buffer[BUFFER_MAX_LENGTH];
	int bytesWritten;

	ReadBufferFromUser(bufferAddress, buffer, size);

	switch (fileId) {
		case ConsoleOutput:
			for(bytesWritten = 0; bytesWritten < size && bytesWritten < BUFFER_MAX_LENGTH; bytesWritten++)
			{		DEBUG('a', "HandlerWrite %d %c\n", bytesWritten, buffer[bytesWritten]);

				synchConsole->PutChar(buffer[bytesWritten]);}
			break;
		case ConsoleInput:
			bytesWritten = 0;
			break;
		default:
			//completar, por ahora sólo a consola
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
		//fileId = currentThread->
		DEBUG('a', "File %s has been successfully opened.\n", fileName);
	}
	else {
		DEBUG('a', "Error, failed to open file %s.\n", fileName);
	}

	machine->WriteRegister(2, fileId); //escribe la id del archivo (return de Open)
}

void HandlerClose()
{
	int fileId = machine->ReadRegister(4);

/*	if (fileSystem->Close(fileId)) {
		DEBUG('a', "File has been successfully closed.\n");
	}
	else {
		DEBUG('a', "Error, failed to close file.\n");
	}*/
}

void HandlerExit()
{

}

void HandlerJoin()
{

}

void HandlerExec()
{

}

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    printf("Exception Handler: %d\n", type);

    if (which == SyscallException) {
    	switch (type) {
    		case SC_Halt:
    			HandlerHalt();
    		   	break;
    		case SC_Create:
    			HandlerCreate();
    			break;
    		case SC_Read:
    			HandlerRead();
    			break;
    		case SC_Write:
    			HandlerWrite();
    			break;
    		case SC_Open:
    			HandlerOpen();
    			break;
    		case SC_Close:
    			HandlerClose();
    			break;
    		case SC_Exit:
    			HandlerExit();
    			break;
    		case SC_Join:
    			HandlerJoin();
    			break;
    		case SC_Exec:
    			HandlerExec();
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
