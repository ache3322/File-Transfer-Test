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
// TYPEDEF DEFINITIONS
//=============================
typedef unsigned long long long64_t;


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
	ReliableConnection connection;		//!< The connection to the server (receiver)
	CRC crc;							//!< CRC object contains implementation to perform CRC (Cylic Redundancy Check)

	string targetIP;					//!< The IPv4 address of the client
	int clientPort;						//!< The port of the client

	string fileName;					//!< The filename that is being sent
	long64_t fileSize;					//!< The size of the file (bytes)


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
	void DisplayResults(int milli, float rtt, float sentBandwidth);

	/*-Accessors-*/
	//------------------
	string GetFileName(void) const;
	long64_t GetFileSize(void) const;

	/*-Mutators-*/
	//------------------
	void SetFileName(string& name);
	void SetFileSize(long64_t size);
};

#endif // !__CLIENT_H__