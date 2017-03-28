//memoriavirtual.c
//Permite copiar datos desde el núcleo al espacio de memoria virtual del usuario y viceversa.

#include "memoriavirtual.h"
#include "../threads/system.h"

#define errorDebug DEBUG('m', "Error while executing %s\n", __func__)

void ReadStringFromUser(int usrAddress, char *outString, unsigned maxByteCount) {

	int intValue = -1;
	unsigned i;

	for(i = 0; i < maxByteCount && machine->ReadMem(usrAddress + i, 1, &intValue) && intValue != 0; i++)
		outString[i] = (char)intValue;

	if (intValue == 0)
		outString[i] = 0;
	else
		errorDebug;

}

void ReadBufferFromUser(int usrAddress, char *outBuffer, unsigned byteCount) {
	int intValue;
	unsigned i;

	for(i = 0; i < byteCount && machine->ReadMem(usrAddress + i, 1, &intValue); i++)
		outBuffer[i] = (char)intValue;

	if (i != byteCount)
		errorDebug;
}

void WriteStringToUser(const char *string, int usrAddress) {

	unsigned i;

	for(i = 0; machine->WriteMem(usrAddress + i, 1, string[i]) && string[i] != 0; i++);

	if (string[i] != 0)
		errorDebug;
}

void WriteBufferToUser(const char *buffer, int usrAddress, unsigned byteCount) {
	unsigned i;

	for(i = 0; i < byteCount && machine->WriteMem(usrAddress + i, 1, buffer[i]); i++);

	if (i != byteCount)
		errorDebug;
}
