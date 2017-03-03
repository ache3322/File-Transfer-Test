/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILE			: clientMain.cpp
* PROGRAMMER	: Austin Che
* DATE			: 2017/02/28
* DESCRIPTION	: This is the main-entry point for the Client application.
*/
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include "..\common\ReliableConnection.h"
#include "..\common\FlowControl.h"
using namespace std;


// CONSTANTS
const int kServerPort = 30008;
const int kClientPort = 30009;
const int kProtocolId = 0x11223344;

struct S {
	char a[256 + 1];
};

void ReadBytes(string fileName);


int main(int argc, char* argv[])
{
	// NOTE: Argument count can be 3 or 4 (if port specificed)
	//	int argCount = argc;
	//
	//	if (argCount == 4)
	//	{
	//		// Once the required arguments are given, instantiate a Client object
	//		// The Client object will take in the ip, port, and file
	//	}
	//	else
	//	{
	//		printf("Usage : Client.exe <ip_address> <port> <file>\n");
	//		printf("  ip_address - IPv4 address to the host server\n");
	//		printf("  port - The port number\n");
	//		printf("  file - The file to be transferred to the server\n");
	//	}
	//
	//#ifdef _WIN32
	//	system("pause");
	//	getchar();
	//#endif
	//	return 0;

	ReadBytes("test");
	return 0;

// parse command line

	enum Mode
	{
		Client,
		Server
	};

	Mode mode = Server;
	Address address;

	if (argc >= 2)
	{
		int a, b, c, d;
		if (sscanf_s(argv[1], "%d.%d.%d.%d", &a, &b, &c, &d))
		{
			mode = Client;
			// Here is where the Client is connecting to the "Server Port"
			address = Address(a, b, c, d, kServerPort);
		}
	}

	// initialize

	if (!InitializeSocket())
	{
		printf("failed to initialize sockets\n");
		return 1;
	}

	ReliableConnection connection(kProtocolId, kTimeOut);

	// This port is the current application
	int port = 0;
	if (mode == Server)
	{
		port = kServerPort;
	}
	else
	{
		port = kClientPort;
	}

	if (!connection.Start(port))
	{
		printf("could not start connection on port %d\n", port);
		return 1;
	}

	if (mode == Client) {
		connection.Connect(address);
	}
	else {
		connection.Listen();
	}

	bool connected = false;
	float sendAccumulator = 0.0f;
	float statsAccumulator = 0.0f;

	FlowControl flowControl;

	while (true)
	{
		// update flow control

		if (connection.IsConnected())
		{
			flowControl.Update(kDeltaTime, connection.GetReliabilitySystem().GetRoundTripTime() * 1000.0f);
		}

		const float sendRate = flowControl.GetSendRate();

		// detect changes in connection state

		if (mode == Server && connected && !connection.IsConnected())
		{
			flowControl.Reset();
			printf("reset flow control\n");
			connected = false;
		}

		if (!connected && connection.IsConnected())
		{
			printf("client connected to server\n");
			connected = true;
		}

		if (!connected && connection.ConnectFailed())
		{
			printf("connection failed\n");
			break;
		}

		// send and receive packets

		sendAccumulator += kDeltaTime;

		//
		// sendRate = 1 / 30
		// Delta = 1 / 30
		// 
		while (sendAccumulator > 1.0f / sendRate)
		{
			unsigned char packet[kPacketSize];
			memset(packet, 0, sizeof(packet));
			connection.SendPacket(packet, sizeof(packet));
			sendAccumulator -= 1.0f / sendRate;
		}

		while (true)
		{
			unsigned char packet[256];
			int bytes_read = connection.ReceivePacket(packet, sizeof(packet));
			if (bytes_read == 0)
			{
				break;
			}
		}

		// show packets that were acked this frame

#ifdef SHOW_ACKS
		unsigned int * acks = NULL;
		int ack_count = 0;
		connection.GetReliabilitySystem().GetAcks(&acks, ack_count);
		if (ack_count > 0)
		{
			printf("acks: %d", acks[0]);
			for (int i = 1; i < ack_count; ++i)
				printf(",%d", acks[i]);
			printf("\n");
		}
#endif

		// update connection

		connection.Update(kDeltaTime);

		// show connection stats

		statsAccumulator += kDeltaTime;

		while (statsAccumulator >= 0.25f && connection.IsConnected())
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

			statsAccumulator -= 0.25f;
		}

		FlowControl::wait_seconds(kDeltaTime);
	}

	ShutdownSocket();

	return 0;
}



void ReadBytes(string fileName)
{
	std::ifstream is("test.txt", std::ifstream::binary);

	int bufferLength = 0;
	char* buffer = NULL;


	if (is) {
		// get length of file:
		is.seekg(0, is.end);
		bufferLength = is.tellg();
		is.seekg(0, is.beg);

		buffer = new char[bufferLength];
		// read data as a block:
		is.read(buffer, bufferLength);

		is.close();
	}

	struct S lol = { 0 };
	vector<S> package;


	int tmpCount = 0;
	int remainder = bufferLength % 256;


	for (int i = 0; i < bufferLength; ++i)
	{
		if (i % 256 == 0 && i != 0)
		{
			package.push_back(lol);
			tmpCount = 0;
			memset(lol.a, 0, 256 + 1);
		}

		lol.a[tmpCount] = buffer[i];
		++tmpCount;
	}


	for each (auto var in package)
	{
		printf("%s\n", var.a);
	}
}