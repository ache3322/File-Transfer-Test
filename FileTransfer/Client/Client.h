/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILENAME		: Client.h
* PROGRAMMER	: Austin Che
* DATE			: 2017/02/28
* DESCRIPTION	: Header file contains the Client class definition.
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


#ifndef __CLIENT_H__
#define __CLIENT_H__


//=============================
// STRUCT DEFINITIONS
//=============================
struct P 
{
	unsigned char pack[kPacketSize];
};


//=============================
// CLIENT CLASS
//=============================
/*!
* \class Client
* \brief The Client class encapsulates the networking portion of application.
*	This class allows for specific functionality - sending file data.
* \details The Client relies on numerous implementations of more general UDP
*	connections. Instances like the usage of the ReliableConnection class ensures
*	data being sent and received is reliably consistent.<br/>
*	Additional method functionalities include the capability to perform a CRC on data.
*/
class Client
{
	//=============================
	// Private Data Members
	//=============================
private:

	Address address;					//!< The address (IPv4 and port) for the client
	ReliableConnection connection;		//!<

	string clientIP;					//!< The IPv4 address of the client
	int clientPort;						//!< The port of the client


	//=============================
	// Public Attributes
	//=============================
public:

	/*-Constructors-*/
	//------------------
	Client(void);
	Client(string ipAddress, int port);

	/*-Destructors-*/
	//------------------
	~Client(void);

	/*-Initialization-*/
	//------------------
	bool Initialization(int targetPort);

	/*-Client-Entry-*/
	//------------------
	void Run(vector<P>& package);

	bool ReadFile(string fileName, vector<P>& package);
	unsigned long CRCTest(char* buffer, const long long bufferSize);

	/*-Statistics-*/
	//------------------
	void ShowStats(void);
};

#endif // !__CLIENT_H__