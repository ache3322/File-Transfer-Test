/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILENAME		: Server.h
* PROGRAMMER	: Austin Che
* DATE			: 2017/02/28
* DESCRIPTION	: Header file contains the Server class definition.
*/
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "..\common\ReliableConnection.h"
#include "..\common\FlowControl.h"
#include "..\crc\CRC.h"
using namespace std;


#ifndef __SERVER_H__
#define __SERVER_H__


//=============================
// SERVER CONSTANTS
//=============================
#define kDefaultPort		13336
#define kServerTimeOut		10.0f


//=============================
// SERVER STRUCTS
//=============================
struct P
{
	unsigned char data[kPacketSize];
};


//=============================
// SERVER CLASS
//=============================
/*!
* \class Server
* \brief The Server class encapsulates the Window Socket operations. The Server
*	class adds a layer of abstraction to the socket capabilities.
* \details
*/
class Server
{
	//=============================
	// Private Data Members
	//=============================
private:

	Address address;					//!< The address (IPv4 and port) for the server
	ReliableConnection connection;		//!< The connection for listening for incoming data

	string serverIP;					//!< The IPv4 address of the server
	int serverPort;						//!< The port of the server


	//=============================
	// Public Attributes
	//=============================
public:

	/*-Constructors-*/
	//------------------
	Server(void);
	Server(int port);
	Server(string ipAddress, int port);

	/*-Deconstructors-*/
	//------------------
	~Server(void);

	/*-Initialization-*/
	//------------------
	bool Initialization(void);

	/*-Server-Entry-*/
	//------------------
	void Run(void);
	char* RebuildFile(vector<P>& package, long long& fileSize, unsigned long& crc);
	void CRCTest(char* buffer, const long long bufferSize, unsigned long expected = 0);

	/*-Statistics-*/
	//------------------
	void ShowStats(void);
};

#endif // !__SERVER_H__