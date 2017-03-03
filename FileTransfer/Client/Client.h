/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILENAME		: Client.h
* PROGRAMMER	: Austin Che
* DATE			: 2017/02/28
* DESCRIPTION	: Header file contains the Client class definition.
*/
#include <iostream>
#include <string>
using namespace std;


#ifndef __CLIENT_H__
#define __CLIENT_H__


/*!
* \class Client
* \brief The Client class encapsulates the Window Socket operations. The Client
*	class adds a layer of abstraction to the socket capabilities.
* \details 
*/
class Client
{
	//=============================
	// Private Data Members
	//=============================
private:

	char* endpointIP;				//!< The IPv4 address



	//=============================
	// Public Attributes
	//=============================
public:

	/*-Constructors-*/
	//------------------
	Client(void);
	Client(string ipAddress);

	/*-Deconstructors-*/
	//------------------
	~Client(void);


	/*-Accessors-*/
	//------------------
	string GetEndPointIP(void) const;


	/*-Mutators-*/
	//------------------


	/*-General Methods-*/
	//------------------
	int InitializeWinSock(void);
	int ConnectToServer(void);
	int DisconnectFromServer(void);

	int SendBytes(int messageSize);
	int RecvBytes(int& messageSize);

	int SendString(string& message);
	int RecvString(string& message);

	string GenerateError(int errorCode);
};

#endif // !__CLIENT_H__