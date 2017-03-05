/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILE			: main.cpp
* PROGRAMMER	: Austin Che
* DATE			: 2017/02/28
* DESCRIPTION	: This is the main-entry point for the Server application. The server
*			application will ask for a port. This port allows the server to listen for
*			any client connections. Once the server accepts the client, it may begin
*			to retrieve the data.
*/
#include <stdio.h>
#include "Server.h"
using namespace std;



int main(int argc, char* argv[])
{
	if (argc >= 2)
	{
		int port = atoi(argv[1]);
		Server server(port);

		// Pass in the Server port to connect to...
		// Therefore the Client connects to the Server
		if (!server.Initialization())
		{
			printf(" >> Failed initialization");
			return 0;
		}

		server.Run();
		ShutdownSocket();
	}
	else
	{
		printf("Usage : Server.exe port\n");
		printf("  port - The port of the server\n");
	}

	return 0;
}