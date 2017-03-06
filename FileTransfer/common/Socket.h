/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILENAME		: Socket.h
* PROGRAMMER	: Austin Che
* DATE			: 2017/02/28
* DESCRIPTION	: Header file contains the Socket class definition.
* CREDIT		: https://github.com/ThisIsRobokitty/netgame/blob/master/03%20-%20Reliability%20and%20Flow%20Control/Net.h
*		Credit to ThisIsRoboKitty for providing the source code on Gaffer on Games implementation of UDP.
*		The source code is modified and used for experimental/educational purposes.
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


//=============================
// CONSTANTS
//=============================
#define kMinimumPort		1024


//=============================
// CLASS
//=============================
/*!
* \class Socket
* \brief The Socket class encapsulates the functionalities of a Windows Socket.
* \details
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

	/*-Socket Methods-*/
	//------------------
	bool Open(unsigned int port);				// For opening the WinSock
	void Close(void);							// For closing the WinSock

	bool Send(const Address& dest, const void* data, int size);		// Sending the packet to the receiver
	int Recv(Address& sender, void* data, int size);				// Receiving the packets
};


/*-Socket Initialization-*/
//--------------------------
bool InitializeSocket(void);
int ShutdownSocket(void);

#endif