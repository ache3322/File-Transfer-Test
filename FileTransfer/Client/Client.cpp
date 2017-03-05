/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILENAME		: Client.cpp
* PROGRAMMER	: Austin Che
* DATE			: 2017/02/28
* DESCRIPTION	: CPP file contains the Client class implementation.
*/
#include "Client.h"



//=============================
// CONSTRUCTOR / DESTRUCTOR
//=============================
/**
* \brief The constructor for the Client class.
* \param None
* \return None
*/
Client::Client()
	: connection(kProtocolId, kTimeOut)
{
	clientIP = "127.0.0.1";
	clientPort = 13337;
}


/**
* \brief The constructor for the Client class.
* \details The constructor takes in two arguments (ip and port).
* \param ipAddress - string - The IPv4 of the client
* \param port - int - The client's port number
* \return None
*/
Client::Client(string ipAddress, int port)
	: connection(kProtocolId, kTimeOut)
{
	clientIP = ipAddress;
	clientPort = port;
}


/**
* \brief The destructor for the Client class.
* \param None
* \return None
*/
Client::~Client()
{
}



//---------------------------------
//=======================
// INITIALIZATION
//=======================
/**
* \brief Initializes the client.
* \details The initialization process must initialize the Window Sockets and Sockets.
*	This allows the client to attempt to connect to the server.
* \param targetPort - int - Where the client will be attempting to connect to
* \return bool : true if initialization was a success; false if initialization failed.
*/
bool Client::Initialization(int targetPort)
{
	// Initialize the Window sockets
	if (!InitializeSocket())
	{
		printf(" >> Failed to initialize sockets\n");
		return false;
	}

	// Ensure the IP address is valid...
	if (!Address::ValidateIP(clientIP))
	{
		printf(" >> Invalid IP address\n");
		return false;
	}

	// Specify who the client will connect to (using the target port)
	address = Address(clientIP.c_str(), targetPort);

	// Attempt to start a connection on the client port
	if (!connection.Start(clientPort))
	{
		printf(">> Could not start connection on port %d\n", clientPort);
		return false;
	}

	// If connection was success - start the real connection with
	// the Address that was recovered
	connection.Connect(address);

	printf(" Client IP : %s\n", clientIP.c_str());
	printf(" Client port : %d\n", clientPort);
	printf(" Server port : %d\n", targetPort);
	printf("\n");

	return true;
}



//---------------------------------
//=======================
// CLIENT RUN
//=======================
/**
* \brief The entry point for performing sending and receiving
* \details This method is where the client operates until connection has timed-out.
*	Various activities that the server must perform is flow control, connection, sending and
*	receiving, and connection time.
* \param package - vector<P>& - Contains data that will be sent to the receiver
* \return None
*/
void Client::Run(vector<P>& package)
{
	bool connected = false;
	float sendAccumulator = 0.0f;
	float statsAccumulator = 0.0f;

	float rtt = 0.0f;
	float sent_bandwidth = 0.0f;

	FlowControl flowControl;

	bool isDone = false;
	while (!isDone)
	{
		// update flow control
		if (connection.IsConnected())
		{
			flowControl.Update(kDeltaTime, connection.GetReliabilitySystem().GetRoundTripTime() * 1000.0f);
		}

		const float sendRate = flowControl.GetSendRate();

		//
		// Detect changes in connection state
		//
		if (!connected && connection.IsConnected())
		{
			printf(" >> Client connected to server\n");
			connected = true;
		}
		// Client disconnection
		if (connection.IsDisconnected())
		{
			printf(" >> Client has disconnected\n");
			break;
		}
		// Client connection failed
		if (!connected && connection.ConnectFailed())
		{
			printf(" >> Connection failed\n");
			break;
		}

		// send and receive packets
		sendAccumulator += kDeltaTime;

		//
		// SENDING DATA
		//
		while (sendAccumulator > 1.0f / sendRate)
		{
			if (connected == true)
			{
				// Send the data to the Server
				//  1. The 1st element of package is the file size
				//  2. The 2nd element of package is the remaining bytes
				//  3. The 3rd element of package is the CRC checksum
				//  4. Other elemens of package is the file contents
				for (size_t i = 0; i < package.size(); ++i)
				{
					int length = sizeof(package[i].pack);
					connection.SendPacket(package[i].pack, length);

					rtt += connection.GetReliabilitySystem().GetRoundTripTime();
					sent_bandwidth += connection.GetReliabilitySystem().GetSentBandwidth();
				}

				sendAccumulator -= 1.0f / sendRate;
				isDone = true;
			}
			else
			{
				/* SENDING ACKs to the Server */
				unsigned char packet[kPacketSize];
				memset(packet, 'f', sizeof(packet));
				connection.SendPacket(packet, sizeof(packet));

				sendAccumulator -= 1.0f / sendRate;
			}
		}

		//
		// RECEIVING DATA
		//
		while (true)
		{
			unsigned char packet[kPacketSize];
			int bytes_read = connection.ReceivePacket(packet, sizeof(packet));
			if (bytes_read == 0)
			{
				break;
			}
		}

		// Update connection - ensures Client is active
		connection.Update(kDeltaTime);

		// show connection stats
		statsAccumulator += kDeltaTime;

		while (statsAccumulator >= 0.25f && connection.IsConnected())
		{
			//ShowStats();
			statsAccumulator -= 0.25f;
		}

		FlowControl::wait_seconds(kDeltaTime);
	}


	printf("rtt %.1fms, sent bandwidth = %.1fkbps\n", rtt * 1000.0f, sent_bandwidth);
}


/**
* \brief Read data from a file.
* \details This method reads binary data from a file. This data is stored
*	to a local buffer. A CRC is performed on this buffer. Finally, the data
*	is split into equal packet sizes - this allows the data to be sent through
*	the UDP connection.
* \param fileName - string - The file being read
* \param package - vector<P>& - A container holding the split file data
* \return bool : true if file can be opened; false if file could not be opened
*/
bool Client::ReadFile(string fileName, vector<P>& package)
{
	bool retBool = false;
	ifstream is(fileName.c_str(), ifstream::binary);

	if (is) 
	{
		char* buffer = NULL;
		int bufferLength = 0;

		// get length of file:
		is.seekg(0, is.end);
		bufferLength = (int)is.tellg();
		is.seekg(0, is.beg);

		buffer = new char[bufferLength + 1];
		memset(buffer, 0, bufferLength + 1);

		// read data as a block:
		is.read(buffer, bufferLength);
		is.close();

		//--------------------------
		// Perform CRC test
		printf(">> File size: %d\n", bufferLength);

		printf(">> Performing CRC test on %s\n", fileName.c_str());
		unsigned long checkSum = CRCTest(buffer, bufferLength);

		//------------------------------------
		// Package the file contents...

		int arrIndex = 0;
		struct P data = { 0 };
		memset(data.pack, 0, kPacketSize);
		
		// 1. Store the entire length (bufferLength) of the file
		sprintf_s((char *)data.pack, kPacketSize, "%ld", bufferLength);
		package.push_back(data);

		// 2. Get the remainder of any left-over file bytes
		int remainder = bufferLength % (kPacketSize - 1);
		sprintf_s((char *)data.pack, kPacketSize, "%d", remainder);
		package.push_back(data);

		// 3. Store the Client's CRC checksum
		sprintf_s((char *)data.pack, kPacketSize, "%ld", checkSum);
		package.push_back(data);

		// 3. Package the "actual" file contents
		memset(data.pack, 0, kPacketSize);
		for (int i = 0; i < bufferLength; ++i)
		{
			if ( (i % (kPacketSize - 1) == 0) && i != 0 )
			{
				// Note. A complete packet contains 255 bytes (chars)
				package.push_back(data);
				arrIndex = 0;
				memset(data.pack, 0, kPacketSize);
			}

			data.pack[arrIndex] = buffer[i];
			++arrIndex;
		}

		// Push the remaining bytes
		package.push_back(data);


		delete[] buffer;
		retBool = true;
	}

	return retBool;
}


/**
* \brief Performs the CRC checksum test.
* \details The CRC is applied to a buffer. This buffer contains ASCII data.
* \param buffer - char* - Contains the received data. The CRC is performed on this buffer
* \param bufferSize - const long long - The total size of the buffer
* \return unsigned long : The CRC checksum.
*/
unsigned long Client::CRCTest(char* buffer, const long long bufferSize)
{
	CRC crc;
	unsigned long checkSum = 0L;

	// Applying the CRC - checksum!
	crc.BuildCRCTable();
	checkSum = crc.CalculateBufferCRC(bufferSize, checkSum, buffer);
	checkSum = ~checkSum;	// Bit-invert

	printf(" >> CRC32 is %08lX\n", checkSum);
	printf("\n");

	return checkSum;
}



//---------------------------------
//=======================
// STATISTICS
//=======================
/**
* \brief Show connection and reliability system statistics.
* \details The statistics include the RTT, Sent Packets, Acked Packets, Lost Packets,
*	Sent Bandwidth, and Acked Bandwidth.<br/>
*	These statistics account for the packets that were sent to the receiver. As well as
*	the packets that were retrieved. For every packet sent or retrieved - the statistics
*	will vary.
* \param None
* \return None
*/
void Client::ShowStats(void)
{
	float rtt = connection.GetReliabilitySystem().GetRoundTripTime();

	unsigned int sent_packets = connection.GetReliabilitySystem().GetSentPackets();
	unsigned int acked_packets = connection.GetReliabilitySystem().GetAckedPackets();
	unsigned int lost_packets = connection.GetReliabilitySystem().GetLostPackets();

	float sent_bandwidth = connection.GetReliabilitySystem().GetSentBandwidth();
	float acked_bandwidth = connection.GetReliabilitySystem().GetAckedBandwidth();

	printf("rtt %.1fms, sent %d, acked %d, lost %d (%.1f%%), sent bandwidth = %.1fkbps, acked bandwidth = %.1fkbps\n",
		rtt * 1000.0f, sent_packets, acked_packets, lost_packets,
		sent_packets > 0.0f ? (float)lost_packets / (float)sent_packets * 100.0f : 0.0f,
		sent_bandwidth, acked_bandwidth);
}