//memoriavirtual.h
//Permite copiar datos desde el núcleo al espacio de memoria virtual del usuario y viceversa.


#ifndef USERPROG_MEMORIAVIRTUAL_H_
#define USERPROG_MEMORIAVIRTUAL_H_

void ReadStringFromUser(int usrAddress, char *outString, unsigned maxByteCount);

void ReadBufferFromUser(int usrAddress, char *outBuffer, unsigned byteCount);

void WriteStringToUser(const char *string, int usrAddress);

void WriteBufferToUser(const char *buffer, int usrAddress, unsigned byteCount);


#endif /* USERPROG_MEMORIAVIRTUAL_H_ */
