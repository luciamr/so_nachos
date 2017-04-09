/* halt.c
 *	Simple program to test whether running a user program works.
 *	
 *	Just do a "syscall" that shuts down the OS.
 *
 * 	NOTE: for some reason, user programs with global data structures 
 *	sometimes haven't worked in the Nachos environment.  So be careful
 *	out there!  One option is to allocate data structures as 
 * 	automatics within a procedure, but if you do this, you have to
 *	be careful to allocate a big enough stack to hold the automatics!
 */

#include "syscall.h"


int
main()
{
	//Original
    Create("test.txt");
    OpenFileId o = Open("test.txt");  
    Write("Hello world\n",12,o);
    //Close(o);

    //For testing read
    OpenFileId r = Open("read.txt");
    char buffer[200];
    int bytesRead;
    bytesRead = Read(buffer, 10, r);
    Write(buffer, 10, o);
    Close(o);
    Close(r);
    /* not reached */
}
