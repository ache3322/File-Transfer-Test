/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILENAME		: Socket.h
* PROGRAMMER	: Austin Che
* DATE			: 2017/02/28
* DESCRIPTION	: CPP file contains the Socket class implementation.
* CREDIT		: https://github.com/ThisIsRobokitty/netgame/blob/master/03%20-%20Reliability%20and%20Flow%20Control/Net.h
*		Credit to ThisIsRoboKitty for providing the source code on Gaffer on Games implementation of UDP.
*		The source code is modified and used for experimental/educational purposes.
*/
#include "Socket.h"


//=============================
// CONSTRUCTOR / DESTRUCTOR
//=============================
/**
* \brief The constructor for the Socket class.
* \param None
* \return None
*/
Socket::Socket()
{
	socket = INVALID_SOCKET;
	address = { 0 };
}


/**
* \brief The deconstructor for the Socket class.
* \param None
* \return None
*/
Socket::~Socket()
{
}



//---------------------------------
//=======================
// WINSOCK INITIALIZATION
//=======================
/**
* \brief Opening the WinSock.
* \param None
* \return None
*/
bool InitializeSocket(void)
{
	int iResult = 0;
	WSADATA wsaData;

	iResult = WSAStartup(0x202, &wsaData);
	if (iResult != 0)
	{
		// Error occurred whilst initializing winsock
		printf("Error: %d. Cannot initialize WinSock\n", WSAGetLastError());
		return false;
	}

	return true;
}


/**
* \brief Closing the WinSock.
* \details Cleaning up any Window Socket resources (including the .dll resources).
* \param None
* \return int : A WSA code signifying if WSA was cleaned.
*/
int ShutdownSocket(void)
{
	return WSACleanup();
}



//---------------------------------
//=======================
// SOCKET OPERATIONS
//=======================
/**
* \brief Open the socket.
* \param port - unsigned int - The port to listen on
* \return bool : true if socket opened successfully; false if failed socket initialization.
*/
bool Socket::Open(unsigned int port)
{
	// Create the socket - UDP mode
	socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (socket < 0)
	{
		// Failed to create the socket
		printf("Failed to create the socket\n");
		socket = INVALID_SOCKET;
		return false;
	}

	// Bind the socket

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if (::bind(socket, (const sockaddr*)&address, sizeof(sockaddr_in)) < 0)
	{
		printf("failed to bind socket\n");
		Close();
		return false;
	}

	// Set the socket to non-blocking
	DWORD mode = 1;
	if (ioctlsocket(socket, FIONBIO, &mode) != 0)
	{
		printf("Failed to set non-blocking socket\n");
		// Close the socket
		return false;
	}

	return true;
}


/**
* \brief Closing the socket.
* \param None
* \return None
*/
void Socket::Close(void)
{
	if (socket != INVALID_SOCKET)
	{
		closesocket(socket);
		socket = INVALID_SOCKET;
	}
}


/**
* \brief
* \param
* \return bool :
*/
bool Socket::Send(const Address& destination, const void* data, int size)
{
	bool isSameSize = false;
	int sentBytes = 0;

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(destination.GetAddress());
	addr.sin_port = htons(destination.GetPort());

	sentBytes = sendto(socket, (const char *)data, size, 0, (sockaddr *)&addr, sizeof(sockaddr_in));

	// Check if sent bytes are equal to the size of the data
	isSameSize = (sentBytes == size);

	return isSameSize;
}


/**
* \brief
* \param
* \return bool :
*/
int Socket::Recv(Address& sender, void* data, int size)
{
	int recvBytes = 0;

	SOCKADDR_IN from;
	int length = sizeof(from);

	recvBytes = recvfrom(socket, (char *)data, size, 0, (sockaddr *)&from, &length);
	
	if (recvBytes > 0)
	{
		unsigned int address = ntohl(from.sin_addr.s_addr);
		unsigned short port = ntohs(from.sin_port);
		sender = Address(address, port);
	}
	else
	{
		recvBytes = 0;
	}

	return recvBytes;
}