/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILENAME		: Address.h
* PROGRAMMER	: Austin Che
* DATE			: 2017/02/28
* DESCRIPTION	: CPP file contains the Address class implementation.
* CREDIT		: https://github.com/ThisIsRobokitty/netgame/blob/master/03%20-%20Reliability%20and%20Flow%20Control/Net.h
*		Credit to ThisIsRoboKitty for providing the source code on Gaffer on Games implementation of UDP.
*		The source code is modified and used for experimental/educational purposes.
*/
#ifndef __ADDRESS_H__
#define __ADDRESS_H__

#include <stdio.h>
#include <string>



//=============================
// CLASS
//=============================
/*!
* \class Address
* \brief The class encapsulates a IPv4 address and a port.
* \details Numerous methods allow the storage of an address. This includes 
*	retrieval of the 4 octets in a IPv4 address.
*/
class Address
{
private:

	unsigned int ip;			//!< IPv4 address
	unsigned short port;		//!< Port number


public:

	/*-Constructors-*/
	//------------------
	Address(void);
	Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port);
	Address(unsigned int address, unsigned short port);
	Address(const char* address, unsigned short port);

	/*-Destructors-*/
	//------------------
	~Address(void);

	/*-Accessors-*/
	//------------------
	unsigned int GetAddress(void) const;
	unsigned char GetA(void) const;
	unsigned char GetB(void) const;
	unsigned char GetC(void) const;
	unsigned char GetD(void) const;
	unsigned short GetPort(void) const;

	/*-Validation-*/
	//------------------
	static bool ValidateIP(std::string& ipAddress);
	static bool ValidateIP(char* ipAddress);

	/*-Overloaded operators-*/
	//------------------
	bool operator == (const Address & other) const;
	bool operator != (const Address & other) const;
	bool operator < (const Address & other) const;
};

#endif 