/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILENAME		: Socket.h
* PROGRAMMER	: Austin Che
* DATE			: 2017/02/28
* DESCRIPTION	: Header file contains the Socket class definition.
* CREDIT		: 
*/
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <string>
#include "Address.h"
#pragma comment(lib, "ws2_32.lib")
using namespace std;



#ifndef __SOCKET_H__
#define __SOCKET_H__


/*!
* \class Socket
* \brief
*/
class Socket
{
	//=============================
	// Private Data Members
	//=============================
private:

	SOCKET socket;					//!< The WinSock socket
	SOCKADDR_IN address;			//!< The socket address information


	//=============================
	// Public Attributes
	//=============================
public:

	/*-Constructors-*/
	//------------------
	Socket(void);

	/*-Deconstructors-*/
	//------------------
	~Socket(void);


	/*-Accessors-*/
	//------------------


	/*-Mutators-*/
	//------------------


	/*-Socket Methods-*/
	//------------------
	bool Open(unsigned int port);				// For opening the WinSock
	void Close(void);							// For closing the WinSock

	bool Send(const Address& dest, const void* data, int size);		// Sending the packet to the receiver
	int Recv(Address& sender, void* data, int size);				// Receiving the packets

	int SendBytes(int messageSize);
	int RecvBytes(int& messageSize);

	string GenerateError(int errorCode);
};


/*-Socket Initialization-*/
//--------------------------
bool InitializeSocket(void);
int ShutdownSocket(void);

#endif