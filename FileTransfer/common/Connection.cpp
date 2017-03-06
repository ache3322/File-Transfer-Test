/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILENAME		: Connection.cpp
* PROGRAMMER	: Austin Che
* DATE			: 2017/02/28
* DESCRIPTION	: CPP file containing the implementation for the Connection class.
* CREDIT		: https://github.com/ThisIsRobokitty/netgame/blob/master/03%20-%20Reliability%20and%20Flow%20Control/Net.h
*		Credit to ThisIsRoboKitty for providing the source code on Gaffer on Games implementation of UDP.
*		The source code is modified and used for experimental/educational purposes.
*/
#include <assert.h>
#include "Connection.h"



//==========================
// CONSTRUCTOR / DESTRUCTOR
//==========================
Connection::Connection(unsigned int protocolId, float timeout)
{
	this->protocolId = protocolId;
	this->timeout = timeout;
	mode = None;
	running = false;
	clearData();
}


Connection::~Connection()
{
	if (IsRunning())
	{
		Stop();
	}
}



//---------------------------------
//=======================
// METHODS
//=======================
bool Connection::Start(int port)
{
	printf(">> start connection on port %d\n", port);
	if (!socket.Open(port))
	{
		return false;
	}

	running = true;
	OnStart();
	return true;
}


void Connection::Stop(void)
{
	bool connected = IsConnected();
	clearData();
	socket.Close();
	running = false;
	if (connected) 
	{
		OnDisconnect();
	}
	OnStop();
}


void Connection::Listen(void)
{
	printf(">> server listening for connection\n");

	bool connected = IsConnected();
	clearData();
	if (connected) 
	{
		OnDisconnect();
	}

	mode = c_Server;
	state = Listening;
}


void Connection::Connect(const Address & address)
{
	printf(">> client connecting to %d.%d.%d.%d:%d\n",
		address.GetA(), address.GetB(), address.GetC(), address.GetD(), address.GetPort());

	bool connected = IsConnected();
	clearData();
	if (connected)
	{
		OnDisconnect();
	}

	mode = c_Client;
	state = Connecting;
	this->address = address;
}


void Connection::Update(float deltaTime)
{
	timeoutAccumulator += deltaTime;
	if (timeoutAccumulator > timeout)
	{
		if (state == Connecting)
		{
			printf(">> connect timed out\n");
			clearData();
			state = ConnectFail;
			OnDisconnect();
		}
		else if (state == Connected)
		{
			printf(">> connection timed out\n");
			clearData();
			if (state == Connecting)
			{
				state = ConnectFail;
			}
			OnDisconnect();
		}
		// Disconnect server if no client connects
		// Therefore the server has timed out
		else if (state == Listening)
		{
			clearData();
			OnDisconnect();
		}
	}
}



//---------------------------------
//=======================
// SENDING / RECEIVING
//=======================
bool Connection::SendPacket(const unsigned char data[], int size)
{
	if (address.GetAddress() == 0)
	{
		return false;
	}

	unsigned char * packet = new unsigned char[size + 4];
	packet[0] = (unsigned char)(protocolId >> 24);
	packet[1] = (unsigned char)((protocolId >> 16) & 0xFF);
	packet[2] = (unsigned char)((protocolId >> 8) & 0xFF);
	packet[3] = (unsigned char)((protocolId) & 0xFF);
	memcpy(&packet[4], data, size);

	bool res = socket.Send(address, packet, size + 4);

	delete[] packet;
	return res;
}


int Connection::ReceivePacket(unsigned char data[], int size)
{
	Address sender;
	unsigned char * packet = new unsigned char[size + 4];

	int bytes_read = socket.Recv(sender, packet, size + 4);
	if (bytes_read <= 4)
	{
		delete[] packet;
		return 0;
	}
	if (packet[0] != (unsigned char)(protocolId >> 24) ||
		packet[1] != (unsigned char)((protocolId >> 16) & 0xFF) ||
		packet[2] != (unsigned char)((protocolId >> 8) & 0xFF) ||
		packet[3] != (unsigned char)(protocolId & 0xFF))
	{
		delete[] packet;
		return 0;
	}
	if (mode == c_Server && !IsConnected())
	{
		printf(">> server accepts connection from client %d.%d.%d.%d:%d\n",
			sender.GetA(), sender.GetB(), sender.GetC(), sender.GetD(), sender.GetPort());
		state = Connected;
		address = sender;
		OnConnect();
	}
	if (sender == address)
	{
		if (mode == c_Client && state == Connecting)
		{
			printf(">> client completes connection with server\n");
			state = Connected;
			OnConnect();
		}

		// If a packet was received - then we know the connection is active
		// so reset the timeoutAccumulator to 0
		timeoutAccumulator = 0.0f;
		memcpy(data, &packet[4], bytes_read - 4);

		delete[] packet;
		return bytes_read - 4;
	}

	delete[] packet;
	return 0;
}



//---------------------------------
//=======================
// ACCESSORS
//=======================
bool Connection::IsRunning(void) const
{
	return running;
}

bool Connection::IsConnecting(void) const
{
	return state == Connecting;
}

bool Connection::IsConnected(void) const
{
	return state == Connected;
}

bool Connection::IsListening(void) const
{
	return state == Listening;
}

bool Connection::IsDisconnected(void) const
{
	return state == Disconnected;
}

bool Connection::ConnectFailed(void) const
{
	return state == ConnectFail;
}

Mode Connection::GetMode(void) const
{
	return mode;
}

int Connection::GetHeaderSize(void) const
{
	return 4;
}



//---------------------------------
//=======================
// PRIVATE METHODS
//=======================
void Connection::clearData(void)
{
	state = Disconnected;
	timeoutAccumulator = 0.0f;
	address = Address();
}