/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILENAME		: ReliableConnection.h
* PROGRAMMER	: Austin Che
* DATE			: 2017/02/28
* DESCRIPTION	: Header file contains the ReliableConnection class definition.
* CREDIT		: https://github.com/ThisIsRobokitty/netgame/blob/master/03%20-%20Reliability%20and%20Flow%20Control/Net.h
*		Credit to ThisIsRoboKitty for providing the source code on Gaffer on Games implementation of UDP.
*		The source code is modified and used for experimental/educational purposes.
*/
#include "Connection.h"
#include "ReliabilitySystem.h"

#ifndef __RELIABLE_CONNECTION_H__
#define __RELIABLE_CONNECTION_H__


//=============================
// CONSTANTS
//=============================
#define kPacketSize		256
#define kProtocolId		0x11223344



/**
* \class ReliableConnection
* \brief
*/
class ReliableConnection : public Connection
{
private:

	void ClearData(void)
	{
		reliabilitySystem.Reset();
	}

#ifdef NET_UNIT_TEST
	unsigned int packet_loss_mask;			// mask sequence number, if non-zero, drop packet - for unit test only
#endif

	ReliabilitySystem reliabilitySystem;	//!< reliability system: manages sequence numbers and acks, tracks network stats etc.

public:

	/*-Constructors-*/
	//------------------
	ReliableConnection(unsigned int protocolId, float timeout, unsigned int max_sequence = 0xFFFFFFFF);

	/*-Destructors-*/
	//------------------
	~ReliableConnection(void);

	/*-Overriden "Connection" methods-*/
	//----------------------------------
	bool SendPacket(const unsigned char data[], int size);

	int ReceivePacket(unsigned char data[], int size);

	void Update(float deltaTime);

	/*-Accessors-*/
	//------------------
	int GetHeaderSize(void) const;
	ReliabilitySystem& GetReliabilitySystem(void);

	// unit test controls

#ifdef NET_UNIT_TEST
	void SetPacketLossMask(unsigned int mask)
	{
		packet_loss_mask = mask;
	}
#endif

protected:

	void WriteInteger(unsigned char * data, unsigned int value);

	void WriteHeader(unsigned char * header, unsigned int sequence, unsigned int ack, unsigned int ack_bits);

	void ReadInteger(const unsigned char * data, unsigned int & value);

	void ReadHeader(const unsigned char * header, unsigned int & sequence, unsigned int & ack, unsigned int & ack_bits);

	virtual void OnStop(void);

	virtual void OnDisconnect(void);

};

#endif