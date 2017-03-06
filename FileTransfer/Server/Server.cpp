/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILENAME		: Server.cpp
* PROGRAMMER	: Austin Che
* DATE			: 2017/02/28
* DESCRIPTION	: CPP file contains the Server class implementation.
*/
#include "Server.h"



//=============================
// CONSTRUCTOR / DESTRUCTOR
//=============================
/**
* \brief The default constructor for the server class.
* \details This constructor uses an initialization list to
*		instantiate a ReliableConnection object. This connection
*		object dictates the connection timeout.
* \param None
* \return None
*/
Server::Server()
	: connection(kProtocolId, kServerTimeOut)
{
	serverIP = "127.0.0.1";
	serverPort = kDefaultPort;
}


/**
* \brief A constructor that takes in a port.
* \param port - int - The port that the server will be listening on
* \return None
*/
Server::Server(int port)
	: connection(kProtocolId, kServerTimeOut)
{
	serverIP = "NA";

	if (port > 1024)
	{
		serverPort = port;
	}
	else
	{
		printf(" >> Port invalid. Defaulting port to %d\n", kDefaultPort);
		serverPort = kDefaultPort;
	}
}


/**
* \brief A constructor that takes in an IPv4 and a port.
* \param ipAddress - string - The IPv4 of the server
* \param port - int - The port that the server will be listening on
* \return None
*/
Server::Server(string ipAddress, int port)
	: connection(kProtocolId, kServerTimeOut)
{
	serverIP = ipAddress;
	serverPort = port;
}


/**
* \brief The deconstructor for the server class.
* \param None
* \return None
*/
Server::~Server()
{
}



//---------------------------------
//=======================
// INITIALIZATION
//=======================
/**
* \brief Initializes the server.
* \details The initialization process must initialize the Window Sockets and Sockets.
*	Then the connection is established by binding the port. This ensures the server will
*	be listening for connections on the binded port.
* \param None
* \return bool : true if initialization was a success; false if initialization failed.
*/
bool Server::Initialization(void)
{
	// Initialize the Window sockets
	if (!InitializeSocket())
	{
		printf(" >> Failed to initialize sockets\n");
		return false;
	}

	// Populate the Address with the corresponding address
	address = Address(serverIP.c_str(), serverPort);

	// Attempt to start a connection
	if ( !connection.Start(serverPort) )
	{
		printf(">> Could not start connection on port %d\n", serverPort);
		return false;
	}

	// If connection was success - start listening for incoming clients
	connection.Listen();

	return true;
}



//---------------------------------
//=======================
// SERVER - RUN
//=======================
/**
* \brief The entry point for performing sending and receiving
* \details This method is where the server operates until connection has timed-out.
*	Various activities that the server must perform is flow control, connection, sending and
*	receiving, and connection time.
* \param None
* \return None
*/
void Server::Run(void)
{
	bool connected = false;
	bool hasConnected = false;
	bool isTiming = false;
	float sendAccumulator = 0.0f;
	float statsAccumulator = 0.0f;

	// ackBuffer will check for ACKs
	char ackBuffer[kPacketSize] = { 0 };
	memset(ackBuffer, 'a', kPacketSize);
	// Used for checking if disconnect flag has been received
	char disACK[kPacketSize] = { 0 };
	memset(disACK, 'd', kPacketSize);

	int numOfDisACK = 0;

	FlowControl flowControl;
	vector<P> package;

	bool isDone = false;
	while (!isDone)
	{
		// update flow control

		if (connection.IsConnected())
		{
			flowControl.Update(kDeltaTime, connection.GetReliabilitySystem().GetRoundTripTime() * 1000.0f);
		}

		const float sendRate = flowControl.GetSendRate();

		// detect changes in connection state

		if (connected && !connection.IsConnected())
		{
			flowControl.Reset();
			printf(">> reset flow control\n");
			connected = false;
			break;
		}

		if (!connected && connection.IsConnected())
		{
			printf(">> client connected to server\n");
			connected = true;
		}
		// Server has disconnected
		if (connection.IsDisconnected())
		{
			printf(" >> server timeout. shutting down server\n");
			break;
		}


		sendAccumulator += kDeltaTime;
		//
		// SENDING packets 
		// 
		while (sendAccumulator > 1.0f / sendRate)
		{
			// Server sending back ACKs to the client
			unsigned char packet[kPacketSize];
			memset(packet, 'a', kPacketSize);
			connection.SendPacket(packet, kPacketSize);
			sendAccumulator -= 1.0f / sendRate;
		}

		//
		// RECEIVING packets
		//
		while (true)
		{
			unsigned char packet[kPacketSize];
			int bytes_read = connection.ReceivePacket(packet, kPacketSize);
			if (bytes_read == 0)
			{
				break;
			}
			if (bytes_read > 0)
			{
				if (connected == true)
				{
					// Check if entire packet buffer is all ACKs
					// else, we know it is the actual data
					if (memcmp(packet, ackBuffer, kPacketSize) != 0)
					{
						if (memcmp(packet, disACK, kPacketSize) == 0)
						{
							++numOfDisACK;
							if (numOfDisACK > 32)
							{
								printf(">> Recieved disconnection ACK\n");
								isDone = true;
							}
							if (isTiming)
							{
								printf(">> Stopped the timer. Waiting for disconnection...\n");
								// Stop the timer
								flowControl.EndTimer();
								isTiming = false;
							}
						}
						else
						{
							if (!isTiming)
							{
								printf(">> Starting the timer. Receiving data. It may take a while...\n");
								// Start the timer when we 1st receive the data
								flowControl.StartTimer();
								isTiming = true;
							}

							struct P tmp = { 0 };
							memcpy(tmp.data, packet, bytes_read);
							package.push_back(tmp);
							hasConnected = true;
						}
					}
				}
			}
		}

		// update connection
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

	// Ensure that the server has retrieved data
	if (hasConnected)
	{
		//-----------------------------
		// Rebuilding the entire file... through the package
		char* reconstruct = NULL;
		unsigned long expectedCRC = 0L;

		reconstruct = RebuildFile(package, expectedCRC);
		CRCTest(reconstruct, fileSize);

		delete[] reconstruct;
		//-----------------------------
		printf("\n");
		printf(">> The std::vector actual size : %lld\n", package.size());
		int milliseconds = flowControl.GetDeltaTime();
		DisplayResults(milliseconds, expectedCRC);

		//-----------------------------
	}
	else
	{
		printf(" >> Server did receive any data\n");
	}
}


/**
* \brief Rebuilds the file from the packets
* \details This method rebuilds the entire file by combining all the bytes (inside each packet
*	which is stored inside a std::vector). The std::vector contains all the packets - including
*	information about the file size, remaining bytes, and CRC checksum. A buffer is allocated enough
*	memory to hold the reconstructed file.
* \param package - vector<P>& - Contains all the data for reconstructing the file
* \param[out] crc - unsigned long& - The expected CRC value
* \return char* : The reconstructed file.
*/
char* Server::RebuildFile(vector<P>& package, unsigned long& crc)
{
	// Rebuilding the entire file... using the package
	int indexCount = 0;
	int packageSize = package.size();

	// 1. The 1st element is file size
	long64_t sizeOfFile = atoll((char *)package[0].data);
	SetFileSize(sizeOfFile);

	// 2. The 2nd element is the remaining bytes
	int leftOver = atoi((char *)package[1].data);

	// 3 The 3rd element is the CRC checksum
	crc = atoll((char*)package[2].data);

	// Allocate enough space
	char* output = new char[(size_t)sizeOfFile + 1];
	memset(output, 0, (size_t)sizeOfFile + 1);

	// Ensure the package elements size is greater than 4
	// to proceed to the for-loop
	if (packageSize > 4)
	{
		// The rest of the index is the file contents
		// NOTE: The size of these packets will always be kPacketSize
		for (int i = 3; i < packageSize - 1; ++i)
		{
			for (int j = 0; j < (kPacketSize - 1); ++j)
			{
				output[indexCount] = package[i].data[j];
				++indexCount;
			}
		}
	}

	int lastIndex = package.size() - 1;
	// The remaining file contents
	for (int j = 0; j < leftOver; ++j)
	{
		output[indexCount] = package[lastIndex].data[j];
		++indexCount;
	}

	return output;
}


/**
* \brief Performs the CRC checksum test.
* \details The CRC is applied to the transferred data. The checksum is then performed. This
*	method will display the actual checksum and the expected checksum.
* \param buffer - char* - Contains the received data. The CRC is performed on this buffer
* \param bufferSize - const long long - The total size of the buffer
* \param expected - unsigned long - The expected CRC value
* \return None
*/
void Server::CRCTest(char* buffer, const long long bufferSize)
{
	unsigned long checkSum = 0L;

	// Applying the CRCd
	crc.BuildCRCTable();
	checkSum = crc.CalculateBufferCRC(bufferSize, checkSum, buffer);
	// Bit-invert
	checkSum = ~checkSum;
	crc.SetCheckSum(checkSum);
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
void Server::ShowStats(void)
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
* \param milli - int - The elapsed milliseconds for how long it took to receive the data
* \param expectedCRC - unsigned long - The expected CRC checksum to compare against the actual checksum
* \return None
*/
void Server::DisplayResults(int milli, unsigned long expectedCRC)
{
	unsigned long actualCRC = crc.GetCheckSum();

	float sec = (float)milli / 1000.0f;
	// Calculate the speed (Bytes per second)
	float speedBytes = GetFileSize() / sec;
	// Calculate the speed (bits per second)
	float speedBits = (GetFileSize() * 8) / sec;

	printf("\n");
	printf("---------------------------------\n");
	// Displaying the transmission speed
	printf("File size: %lld bytes (%.3f kB)\n", GetFileSize(), (float)(GetFileSize() / 1000.0f));
	printf("Total receive time: %d ms (%.1f s)\n", milli, sec);
	printf("Approx. transfer speed: %.1f B/s (%.1f kB/s) (%.1f bps) (%.3f kbps)\n", 
		speedBytes, speedBytes / 1000.0f, speedBits, speedBits / 1000.0f);

	// Displaying the CRC results
	printf("\n");
	printf(">> Actual CRC32 is %08lX\n", actualCRC);
	printf(">> Expected CRC32 is %08lX\n", expectedCRC);

	if (actualCRC == expectedCRC) {
		printf(" >> PASSED\n");
	}
	else {
		printf(" >> FAILED\n");
	}

	printf("---------------------------------\n");
	printf("\n");
}



//---------------------------------
//=======================
// ACCESSORS
//=======================
/**
* \brief Get the file size.
* \param None
* \return long4_t : The file size.
*/
long64_t Server::GetFileSize(void) const
{
	return fileSize;
}



//---------------------------------
//=======================
// MUTATORS
//=======================
/**
* \brief Set the file size.
* \param None
* \return None
*/
void Server::SetFileSize(long64_t size)
{
	this->fileSize = size;
}