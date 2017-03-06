/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILE			: clientMain.cpp
* PROGRAMMER	: Austin Che
* DATE			: 2017/02/28
* DESCRIPTION	: This is the main-entry point for the Client application.
*		The client application takes in 5 command-line arguments.
*
*		1st argument : The server's IPv4
*		2nd argument : The client's port
*		3rd argument : The server port (who the client will connect to)
*		4th argument : The file to be sent to the server
*/
#include <stdio.h>
#include "Client.h"
using namespace std;



int main(int argc, char* argv[])
{
	if (argc >= 5)
	{
		int clientPort = atoi(argv[2]);
		int serverPort = atoi(argv[3]);

		if (clientPort != serverPort)
		{

			Client client(argv[1], clientPort);

			// Pass in the Server port to connect to...
			// Therefore the Client connects to the Server
			if (!client.Initialization(serverPort))
			{
				printf(" >> Failed initialization");
				return -1;
			}

			vector<P> package;
			// Read the file and perform the CRC
			if (client.ReadFile(argv[4], package))
			{
				// argv[4] contains the file
				client.Run(package);
				ShutdownSocket();
			}
			else
			{
				printf(" >> Error. File does not exist\n");
			}
		}
		else
		{
			printf(" >> Error. Cannot have same port as server\n");
		}
	}
	else
	{
		printf("Usage : Client.exe ip client_port server_port file\n");
		printf("  ip - The IPv4 address of the server\n");
		printf("  client_port - The port for the client\n");
		printf("  server_port - The port for connecting to the server\n");
		printf("  file - The file being transferred\n");
	}


	printf("Closing the application\n");
	return 0;
}