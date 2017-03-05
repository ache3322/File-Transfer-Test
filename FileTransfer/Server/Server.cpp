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
	float sendAccumulator = 0.0f;
	float statsAccumulator = 0.0f;

	FlowControl flowControl;
	vector<P> package;

	while (true)
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
			memset(packet, 'f', sizeof(packet));
			connection.SendPacket(packet, sizeof(packet));
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
					char checker[kPacketSize] = { 0 };
					memset(checker, 'f', sizeof(checker));

					// Check if entire packet buffer is all ACKs
					// else, we know it is the data
					if (memcmp(packet, checker, kPacketSize) != 0)
					{
						struct P tmp = { 0 };
						memcpy(tmp.data, packet, bytes_read);
						printf("Bytes read: %s | Bytes decimal: %d\n", packet, bytes_read);

						package.push_back(tmp);
						hasConnected = true;
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
		long long fileSize = 0;
		unsigned long crcCheckSum = 0L;

		reconstruct = RebuildFile(package, fileSize, crcCheckSum);
		CRCTest(reconstruct, fileSize, crcCheckSum);

		delete[] reconstruct;
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
* \param[out] fileSize - long long& - The file size
* \param[out] crc - unsigned long& - The expected CRC value
* \return char* : The reconstructed file.
*/
char* Server::RebuildFile(vector<P>& package, long long& fileSize, unsigned long& crc)
{
	// Rebuilding the entire file... using the package
	int indexCount = 0;
	int packageSize = package.size();

	// 1. The 1st element is file size
	long long sizeOfFile = atoll((char *)package[0].data);
	fileSize = sizeOfFile;

	// 2. The 2nd element is the remaining bytes
	int leftOver = atoi((char *)package[1].data);

	// 3 The 3rd element is the CRC checksum
	crc = atol((char*)package[2].data);

	// Allocate enough space
	char* output = new char[(size_t)sizeOfFile + 1];
	memset(output, 0, (size_t)sizeOfFile + 1);

	// Ensure the package elements size is greater than 4
	// to proceed to the for-loop
	if (packageSize > 4)
	{
		// The rest of the index is the file contents
		// NOTE: The size of these packets will always be 256
		for (int i = 3; i < packageSize - 1; ++i)
		{
			for (int j = 0; j < 255; ++j)
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
void Server::CRCTest(char* buffer, const long long bufferSize, unsigned long expected)
{
	CRC crc;
	unsigned long checkSum = 0L;

	// Applying the CRCd
	crc.BuildCRCTable();
	checkSum = crc.CalculateBufferCRC(bufferSize, checkSum, buffer);
	// Bit-invert
	checkSum = ~checkSum;

	printf("\n");
	printf(">> Actual CRC32 is %08lX\n", checkSum);
	printf(">> Expected CRC32 is %08lX\n", expected);

	if (checkSum == expected)
	{
		printf(" >> PASSED\n");
	}
	else
	{
		printf(" >> FAILED\n");
	}
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