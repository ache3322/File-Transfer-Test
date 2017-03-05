/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILENAME		: Address.cpp
* PROGRAMMER	: Austin Che
* DATE			: 2017/02/28
* DESCRIPTION	: CPP file contains the Address class implementation.
* CREDIT		: https://github.com/ThisIsRobokitty/netgame/blob/master/03%20-%20Reliability%20and%20Flow%20Control/Net.h
*		Credit to ThisIsRoboKitty for providing the source code on Gaffer on Games implementation of UDP.
*		The source code is modified and used for experimental/educational purposes.
*/
#include "Address.h"


//=====================
// CONSTRUCTORS
//=====================
/**
* \brief The constructor for the Address class.
* \param None
* \return None
*/
Address::Address()
{
	ip = 0;
	port = 0;
}


/**
* \brief The constructor for the Address class. Takes in 5 parameters.
* \param a - unsigned char - The first IPv4 octet
* \param b - unsigned char - The second IPv4 octet
* \param c - unsigned char - The third IPv4 octet
* \param d - unsigned char - The forth IPv4 octet
* \param port - unsigned short - The port
* \return None
*/
Address::Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port)
{
	this->ip = (a << 24) | (b << 16) | (c << 8) | d;
	this->port = port;
}


/**
* \brief The constructor for the Address class. Takes in 2 parameters.
* \param address - unsigned int - An unsigned int version of the IPv4 address
* \param port - unsigned short - The port
* \return None
*/
Address::Address(unsigned int address, unsigned short port)
{
	this->ip = address;
	this->port = port;
}


/**
* \brief The constructor for the Address class. Takes in 2 parameters.
* \param address - const char* - A human-readable IPv4 address
* \param port - unsigned short - The port
* \return None
*/
Address::Address(const char* address, unsigned short port)
{
	int a, b, c, d;
	sscanf_s(address, "%d.%d.%d.%d", &a, &b, &c, &d);

	this->ip = (a << 24) | (b << 16) | (c << 8) | d;
	this->port = port;
}



//=====================
// DESTRUCTORS
//=====================
/**
* \brief The destructor for the Address class.
* \param None
* \return None
*/
Address::~Address(void)
{
}



//=====================
// ACCESSORS
//=====================
/**
* \brief Get the IPv4 address
* \param None
* \return unsigned int : The IPv4 address
*/
unsigned int Address::GetAddress(void) const { 
	return ip;
}

/**
* \brief Get the class A of the IPv4 address
* \param None
* \return unsigned char : The class A octet
*/
unsigned char Address::GetA(void) const {
	return (unsigned char)(ip >> 24);
}

/**
* \brief Get the class B of the IPv4 address
* \param None
* \return unsigned char : The class B octet
*/
unsigned char Address::GetB(void) const {
	return (unsigned char)(ip >> 16);
}

/**
* \brief Get the class C of the IPv4 address
* \param None
* \return unsigned char : The class C octet
*/
unsigned char Address::GetC(void) const {
	return (unsigned char)(ip >> 8);
}

/**
* \brief Get the class D of the IPv4 address
* \param None
* \return unsigned char : The class D octet
*/
unsigned char Address::GetD(void) const {
	return (unsigned char)(ip);
}

/**
* \brief Get the port
* \param None
* \return unsigned short : The port
*/
unsigned short Address::GetPort(void) const {
	return port;
}



//======================
// VALIDATION
//======================
bool Address::ValidateIP(std::string& ipAddress)
{
	bool retBool = false;
	int a = 0;
	int b = 0;
	int c = 0;
	int d = 0;
	int count = 0;

	count = sscanf_s(ipAddress.c_str(), "%d.%d.%d.%d", &a, &b, &c, &d);
	if (count == 4) {
		retBool = true;
	}
	return retBool;
}

bool Address::ValidateIP(char* ipAddress)
{
	bool retBool = false;
	int a = 0;
	int b = 0;
	int c = 0;
	int d = 0;
	int count = 0;

	count = sscanf_s(ipAddress, "%d.%d.%d.%d", &a, &b, &c, &d);
	if (count == 4) {
		retBool = true;
	}
	return retBool;
}



//======================
// OVERLOADED OPERATORS
//======================
/**
* \brief Overloaded equality operator
* \param other - const Address& - The object being compared
* \return bool : true if equal; false if not equal.
*/
bool Address::operator == (const Address & other) const
{
	return ip == other.ip && port == other.port;
}


/**
* \brief Overloaded not-equal operator
* \param other - const Address& - The object being compared
* \return bool : true if equal; false if not equal.
*/
bool Address::operator != (const Address & other) const
{
	return !(*this == other);
}


/**
* \brief Overloaded less-than operator
* \param other - const Address& - The object being compared
* \return bool : true if left-operand less than; false if right-operand greater than.
*/
bool Address::operator < (const Address & other) const
{
	if (ip < other.ip) {
		return true;
	}
	if (ip > other.ip) {
		return false;
	}

	return port < other.port;
}