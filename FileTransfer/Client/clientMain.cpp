/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILE			: clientMain.cpp
* PROGRAMMER	: Austin Che
* DATE			: 2017/02/28
* DESCRIPTION	: This is the main-entry point for the Client application.
*/
#include <stdio.h>
#include <stdlib.h>



int main(int argc, char* argv[])
{
	// NOTE: Argument count can be 3 or 4 (if port specificed)
	int argCount = argc;

	if (argCount == 4)
	{
		// Once the required arguments are given, instantiate a Client object
		// The Client object will take in the ip, port, and file
	}
	else
	{
		printf("Usage : Client.exe <ip_address> <port> <file>\n");
		printf("  ip_address - IPv4 address to the host server\n");
		printf("  port - The port number\n");
		printf("  file - The file to be transferred to the server\n");
	}

#ifdef _WIN32
	system("pause");
	getchar();
#endif
	return 0;
}