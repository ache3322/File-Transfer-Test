/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILENAME		: Connection.h
* PROGRAMMER	: Austin Che
* DATE			: 2017/02/28
* DESCRIPTION	: Header file containing the class defintion for Connection.
* CREDIT		: https://github.com/ThisIsRobokitty/netgame/blob/master/03%20-%20Reliability%20and%20Flow%20Control/Net.h
*		Credit to ThisIsRoboKitty for providing the source code on Gaffer on Games implementation of UDP.
*		The source code is modified and used for experimental/educational purposes.
*/
#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <stdio.h>
#include "Socket.h"


//=============================
// ENUM
//=============================
enum Mode
{
	None,
	c_Client,
	c_Server
};


//=============================
// CLASS
//=============================
/*!
* \class Connection
* \brief
*/
class Connection
{
private:

	enum State
	{
		Disconnected,
		Listening,
		Connecting,
		ConnectFail,
		Connected
	};

	unsigned int protocolId;
	float timeout;
	float timeoutAccumulator;

	bool running;
	Address address;
	Mode mode;
	State state;
	Socket socket;


	/*-Private methods-*/
	//------------------
	void clearData(void);


protected:

	virtual void OnStart(void) {}
	virtual void OnStop(void) {}
	virtual void OnConnect(void) {}
	virtual void OnDisconnect(void) {}


public:
	
	/*-Constructors-*/
	//------------------
	Connection(unsigned int protocolId, float timeout);

	/*-Destructors-*/
	//------------------
	virtual ~Connection(void);

	/*-Methods-*/
	//------------------
	bool Start(int port);
	void Stop(void);
	void Listen(void);
	void Connect(const Address & address);
	virtual void Update(float deltaTime);

	/*-Sending/Receiving-*/
	//------------------
	virtual bool SendPacket(const unsigned char data[], int size);
	virtual int ReceivePacket(unsigned char data[], int size);

	/*-Accessors-*/
	//------------------
	bool IsRunning(void) const;
	bool IsConnecting(void) const;
	bool IsConnected(void) const;
	bool IsListening(void) const;
	bool IsDisconnected(void) const;
	bool ConnectFailed(void) const;
	Mode GetMode(void) const;
	int GetHeaderSize(void) const;
};

#endif // !__CONNECTION_H__