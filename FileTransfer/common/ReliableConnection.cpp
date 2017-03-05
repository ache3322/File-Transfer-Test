/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILENAME		: ReliableConnection.cpp
* PROGRAMMER	: Austin Che
* DATE			: 2017/02/28
* DESCRIPTION	: CPP file contains the ReliableConnection implementation.
* CREDIT		: https://github.com/ThisIsRobokitty/netgame/blob/master/03%20-%20Reliability%20and%20Flow%20Control/Net.h
*		Credit to ThisIsRoboKitty for providing the source code on Gaffer on Games implementation of UDP.
*		The source code is modified and used for experimental/educational purposes.
*/
#include "ReliableConnection.h"



//==========================
// CONSTRUCTOR / DESTRUCTOR
//==========================
ReliableConnection::ReliableConnection(unsigned int protocolId, float timeout, unsigned int max_sequence)
	: Connection(protocolId, timeout), reliabilitySystem(max_sequence)
{
	ClearData();
}


ReliableConnection::~ReliableConnection()
{
	if (IsRunning())
	{
		Stop();
	}
}



//---------------------------------
//================================
// OVERRIDEN "CONNECTION" METHODS
//================================
bool ReliableConnection::SendPacket(const unsigned char data[], int size)
{
#ifdef NET_UNIT_TEST
	if (reliabilitySystem.GetLocalSequence() & packet_loss_mask)
	{
		reliabilitySystem.PacketSent(size);
		return true;
	}
#endif
	// Packet = header (12) + size (max. 256)
	const int kHeader = 12;
	unsigned char * packet = new unsigned char[kHeader + size];
	unsigned int seq = reliabilitySystem.GetLocalSequence();
	unsigned int ack = reliabilitySystem.GetRemoteSequence();
	unsigned int ack_bits = reliabilitySystem.GenerateAckBits();

	WriteHeader(packet, seq, ack, ack_bits);
	memcpy(packet + kHeader, data, size);

	if (!Connection::SendPacket(packet, size + kHeader)) 
	{
		return false;
	}
	reliabilitySystem.PacketSent(size);

	delete[] packet;
	return true;
}


int ReliableConnection::ReceivePacket(unsigned char data[], int size)
{
	const int kHeader = 12;
	if (size <= kHeader)
	{
		return false;
	}

	unsigned char * packet = new unsigned char[kHeader + size];
	int received_bytes = Connection::ReceivePacket(packet, size + kHeader);
	if (received_bytes == 0)
	{
		delete[] packet;
		return 0;
	}
	if (received_bytes <= kHeader)
	{
		delete[] packet;
		return received_bytes - kHeader;
	}

	unsigned int packet_sequence = 0;
	unsigned int packet_ack = 0;
	unsigned int packet_ack_bits = 0;

	ReadHeader(packet, packet_sequence, packet_ack, packet_ack_bits);
	reliabilitySystem.PacketReceived(packet_sequence, received_bytes - kHeader);
	reliabilitySystem.ProcessAck(packet_ack, packet_ack_bits);
	memcpy(data, packet + kHeader, received_bytes - kHeader);

	delete[] packet;
	// Return the received bytes (minus the header)
	return received_bytes - kHeader;
}


void ReliableConnection::Update(float deltaTime)
{
	Connection::Update(deltaTime);
	reliabilitySystem.Update(deltaTime);
}



//---------------------------------
//================================
// ACCESSORS
//================================
int ReliableConnection::GetHeaderSize(void) const
{
	return Connection::GetHeaderSize() + reliabilitySystem.GetHeaderSize();
}

ReliabilitySystem& ReliableConnection::GetReliabilitySystem(void)
{
	return reliabilitySystem;
}



//=================================
//---------------------------------
//=======================
// PROTECTED METHODS
//=======================
void ReliableConnection::WriteInteger(unsigned char * data, unsigned int value)
{
	data[0] = (unsigned char)(value >> 24);
	data[1] = (unsigned char)((value >> 16) & 0xFF);
	data[2] = (unsigned char)((value >> 8) & 0xFF);
	data[3] = (unsigned char)(value & 0xFF);
}


void ReliableConnection::WriteHeader(unsigned char * header, unsigned int sequence, unsigned int ack, unsigned int ack_bits)
{
	WriteInteger(header, sequence);
	WriteInteger(header + 4, ack);
	WriteInteger(header + 8, ack_bits);
}


void ReliableConnection::ReadInteger(const unsigned char * data, unsigned int & value)
{
	value = (
		((unsigned int)data[0] << 24) 
		| ((unsigned int)data[1] << 16) 
		| ((unsigned int)data[2] << 8) 
		| ((unsigned int)data[3]));
}


void ReliableConnection::ReadHeader(const unsigned char * header, unsigned int & sequence, unsigned int & ack, unsigned int & ack_bits)
{
	ReadInteger(header, sequence);
	ReadInteger(header + 4, ack);
	ReadInteger(header + 8, ack_bits);
}


void ReliableConnection::OnStop(void)
{
	ClearData();
}


void ReliableConnection::OnDisconnect(void)
{
	ClearData();
}