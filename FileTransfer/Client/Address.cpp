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
* \brief The constructor for the Address class. Takes in
*	five parameters.
* \param a - unsigned char -
* \param b - unsigned char -
* \param c - unsigned char - 
* \param d - unsigned char -
* \param port - unsigned short - 
* \return None
*/
Address::Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port)
{
	this->ip = (a << 24) | (b << 16) | (c << 8) | d;
	this->port = port;
}


/**
* \brief The constructor for the Address class. Takes in
*	two parameters.
* \param address - unsigned int -
* \param port - unsigned short -
* \return None
*/
Address::Address(unsigned int address, unsigned short port)
{
	this->ip = address;
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
* \brief
* \param None
* \return None
*/
unsigned int Address::GetAddress(void) const { 
	return ip;
}

/**
* \brief
* \param None
* \return None
*/
unsigned char Address::GetA(void) const {
	return (unsigned char)(ip >> 24);
}

/**
* \brief
* \param None
* \return None
*/
unsigned char Address::GetB(void) const {
	return (unsigned char)(ip >> 16);
}

/**
* \brief
* \param None
* \return None
*/
unsigned char Address::GetC(void) const {
	return (unsigned char)(ip >> 8);
}

/**
* \brief
* \param None
* \return None
*/
unsigned char Address::GetD(void) const {
	return (unsigned char)(ip);
}

/**
* \brief
* \param None
* \return None
*/
unsigned short Address::GetPort(void) const {
	return port;
}



//=====================
// OVERLOADED OPERATORS
//=====================
/**
* \brief
* \param None
* \return None
*/
bool Address::operator == (const Address & other) const
{
	return ip == other.ip && port == other.port;
}


/**
* \brief
* \param None
* \return None
*/
bool Address::operator != (const Address & other) const
{
	return !(*this == other);
}


/**
* \brief
* \param None
* \return None
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