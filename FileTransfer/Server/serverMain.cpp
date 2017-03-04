/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILE			: main.cpp
* PROGRAMMER	: Austin Che
* DATE			: 2017/02/28
* DESCRIPTION	: This is the main-entry point for the Server application.
*/
#include <stdio.h>
#include <stdlib.h>
#include "..\common\ReliableConnection.h"
#include "..\common\FlowControl.h"
using namespace std;


// CONSTANTS
const int kServerPort = 30008;
const int kClientPort = 30009;
const int kProtocolId = 0x11223344;

struct S {
	unsigned char a[256];
};

int main(int argc, char* argv[])
{
	vector<S> package;

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

	const int port = mode == Server ? kServerPort : kClientPort;

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
			flowControl.Update(kDeltaTime, connection.GetReliabilitySystem().GetRoundTripTime() * 1000.0f);

		const float sendRate = flowControl.GetSendRate();

		// detect changes in connection state

		if (mode == Server && connected && !connection.IsConnected())
		{
			flowControl.Reset();
			printf("reset flow control\n");
			connected = false;
			break;
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

		// sendRate = 1 / 30
		// Delta = 1 / 30
		// 
		while (sendAccumulator > 1.0f / sendRate)
		{
			// Server sending back ACKs to the client
			unsigned char packet[kPacketSize];
			memset(packet, 'f', sizeof(packet));
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
			if (bytes_read > 0)
			{
				if (connected == true)
				{
					char checker[256] = { 0 };
					memset(checker, 'f', sizeof(checker));
					
					if (memcmp((packet), checker, 256) != 0)
					{
						struct S wol = { 0 };
						memcpy(wol.a, packet, 256);
						printf("Bytes read: %s | Bytes decimal: %d\n", packet, bytes_read);

						package.push_back(wol);
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

	int length = 0;
	for each(auto var in package)
	{
		length += sizeof(var.a);
	}

	printf("Buffer Length: %d\n", length);

	char* finalResult = new char[length];
	memset(finalResult, 0, length);

	for each (auto var in package)
	{
		strcat(finalResult, reinterpret_cast<char *>(var.a));
	}
	
	delete[] finalResult;

	return 0;
}