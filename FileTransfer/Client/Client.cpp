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
	targetIP = "127.0.0.1";
	clientPort = 13337;
	fileName = "";
	fileSize = 0;
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
	targetIP = ipAddress;
	clientPort = port;
	fileName = "";
	fileSize = 0;
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
	if (!Address::ValidateIP(targetIP))
	{
		printf(" >> Invalid IP address\n");
		return false;
	}

	// Specify who the client will connect to (using the target port)
	address = Address(targetIP.c_str(), targetPort);

	// Attempt to start a connection on the client port
	if (!connection.Start(clientPort))
	{
		printf(">> Could not start connection on port %d\n", clientPort);
		return false;
	}

	// If connection was success - start the real connection with
	// the Address that was recovered
	connection.Connect(address);

	printf(" Server IP : %s\n", targetIP.c_str());
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

	// ackBuffer will check for ACKs
	char ackBuffer[kPacketSize] = { 0 };
	memset(ackBuffer, 'a', kPacketSize);

	u_int64 packageIndex = 0;

	bool sendDisconnect = false;
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
				if (sendDisconnect == false)
				{
					// Start the timer
					flowControl.StartTimer();

					// Send the data to the Server
					//  1. The 1st element of package is the file size
					//  2. The 2nd element of package is the remaining bytes
					//  3. The 3rd element of package is the CRC checksum
					//  4. Other elements of package is the file contents
					//for (size_t i = 0; i < package.size(); ++i)
					//{
						int length = sizeof(package[packageIndex].pack);
						connection.SendPacket(package[packageIndex].pack, length);
					//}

					// End the timer
					flowControl.EndTimer();
					if (packageIndex == (package.size() - 1))
					{
						sendDisconnect = true;
					}
				}
				else
				{
					unsigned char disACK[kPacketSize];
					memset(disACK, 'd', kPacketSize);
					connection.SendPacket(disACK, kPacketSize);
				}

				sendAccumulator -= 1.0f / sendRate;
			}
			else
			{
				/* SENDING ACKs to the Server */
				unsigned char packet[kPacketSize];
				memset(packet, 'a', kPacketSize);
				connection.SendPacket(packet, kPacketSize);

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
			if (bytes_read > 0)
			{
				if (connected == true)
				{
					if (memcmp(packet, ackBuffer, kPacketSize) == 0)
					{
						if (sendDisconnect == false)
						{
							// Increase what will be sent next...
							++packageIndex;
							printf("index : %lld\n", packageIndex);
						}
					}
				}
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

	printf(">> Outside the send/receiving loop\n");
	//----------------------------------------------

	// Displaying final results
	int milli = flowControl.GetDeltaTime();
	printf(">> The std::vector size: %lld\n", package.size());
	DisplayResults(milli, rtt, sent_bandwidth);
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
		long64_t bufferLength = 0;

		// get length of file:
		is.seekg(0, is.end);
		bufferLength = (long64_t)is.tellg();
		is.seekg(0, is.beg);

		buffer = new char[bufferLength + 1];
		memset(buffer, 0, bufferLength + 1);

		// read data as a block:
		is.read(buffer, bufferLength);
		is.close();

		//--------------------------

		SetFileName(fileName);
		SetFileSize(bufferLength);

		//--------------------------
		// Perform CRC test
		printf(">> File size: %lld bytes\n", bufferLength);
		printf(">> Performing CRC test on %s\n", fileName.c_str());
		unsigned long checkSum = CRCTest(buffer, bufferLength);

		//------------------------------------
		// Package the file contents...

		int arrIndex = 0;
		struct P data = { 0 };
		memset(data.pack, 0, kPacketSize);
		
		// 1. Store the entire length (bufferLength) of the file
		sprintf_s((char *)data.pack, kPacketSize, "%lld", bufferLength);
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
	unsigned long checkSum = 0L;

	// Applying the CRC - checksum!
	crc.BuildCRCTable();
	checkSum = crc.CalculateBufferCRC(bufferSize, checkSum, buffer);
	checkSum = ~checkSum;	// Bit-invert
	crc.SetCheckSum(checkSum);

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


/**
* \brief Show the data results.
* \param milli - int - The elapsed milliseconds for how long it took to send the data
* \param rtt - float - The Round Transmission Time that it takes for the data to be sent and ACKed
* \param sentBandwidth - float - The bandwidth speed for sending data
* \return None
*/
void Client::DisplayResults(int milli, float rtt, float sentBandwidth)
{
	float sec = (float)(milli / 1000.0f);

	// Calculate the speed (Bytes per second)
	//  1. Speed = bytes sent / milliseconds?
	//  2. Convert the milli to seconds? by dividing by 60s
	float speed = GetFileSize() / sec;

	// Calculate the speed (bits per second)
	//  1. Convert bytes to bits by multiplying by 8 bits
	float speedBits = (GetFileSize() * 8) / sec;

	printf("\n");
	printf("---------------------------------\n");
	printf("File size: %lld bytes (%.3f kB)\n", GetFileSize(), (float)(GetFileSize() / 1000.0f));
	printf("Total send time: %d ms (%.2f s)\n", milli, sec);
	printf("Approx. speed: %.1f B/s (%.1f kB/s) (%.1f bps)\n", speed, speed / 1000.0f, speedBits);
	printf("rtt %.1fms, sent bandwidth = %.1fkbps\n", rtt * 1000.0f, sentBandwidth);

	// Displaying the CRC results
	printf("\n");
	printf(">> CRC32 test on file %s\n", GetFileName().c_str());
	printf(" >> CRC32 is %08lX\n", crc.GetCheckSum());
	printf("---------------------------------\n");
	printf("\n");
}



//---------------------------------
//=======================
// ACCESSORS
//=======================
/**
* \brief Get the file name.
* \param None
* \return string : The file name.
*/
string Client::GetFileName(void) const
{
	return fileName;
}

/**
* \brief Get the file size.
* \param None
* \return long4_t : The file size.
*/
long64_t Client::GetFileSize(void) const
{
	return fileSize;
}



//---------------------------------
//=======================
// MUTATORS
//=======================
/**
* \brief Set the file name.
* \param None
* \return None
*/
void Client::SetFileName(string& name)
{
	this->fileName = name;
}

/**
* \brief Set the file size.
* \param None
* \return None
*/
void Client::SetFileSize(long64_t size)
{
	this->fileSize = size;
}