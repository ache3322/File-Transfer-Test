/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILENAME		: ReliabilitySystem.h
* PROGRAMMER	: Austin Che
* DATE			: 2017/02/28
* DESCRIPTION	:
* CREDIT		: https://github.com/ThisIsRobokitty/netgame/blob/master/03%20-%20Reliability%20and%20Flow%20Control/Net.h
*		Credit to ThisIsRoboKitty for providing the source code on Gaffer on Games implementation of UDP.
*		The source code is modified and used for experimental/educational purposes.
*/
#ifndef __RELIABILITY_SYSTEM_H__
#define __RELIABILITY_SYSTEM_H__

#include <stdio.h>
#include <assert.h>
#include <vector>
#include "PacketQueue.h"


/*!
* \class ReliabilitySystem
* \brief
*/
class ReliabilitySystem
{
private:

	unsigned int max_sequence;			//!< maximum sequence value before wrap around (used to test sequence wrap at low # values)
	unsigned int local_sequence;		//!< local sequence number for most recently sent packet
	unsigned int remote_sequence;		//!< remote sequence number for most recently received packet

	unsigned int sent_packets;			//!< total number of packets sent
	unsigned int recv_packets;			//!< total number of packets received
	unsigned int lost_packets;			//!< total number of packets lost
	unsigned int acked_packets;			//!< total number of packets acked

	float sent_bandwidth;				//!< approximate sent bandwidth over the last second
	float acked_bandwidth;				//!< approximate acked bandwidth over the last second
	float rtt;							//!< estimated round trip time
	float rtt_maximum;					//!< maximum expected round trip time (hard coded to one second for the moment)

	std::vector<unsigned int> acks;		//!< acked packets from last set of packet receives. cleared each update!

	PacketQueue sentQueue;				//!< sent packets used to calculate sent bandwidth (kept until rtt_maximum)
	PacketQueue pendingAckQueue;		//!< sent packets which have not been acked yet (kept until rtt_maximum * 2 )
	PacketQueue receivedQueue;			//!< received packets for determining acks to send (kept up to most recent recv sequence - 32)
	PacketQueue ackedQueue;				//!< acked packets (kept until rtt_maximum * 2)

protected:

	/*-Protected methods-*/
	//---------------------
	void AdvanceQueueTime(float deltaTime);
	void UpdateQueues(void);
	void UpdateStats(void);

public:

	/*-Constructors-*/
	//------------------
	ReliabilitySystem(unsigned int max_sequence = 0xFFFFFFFF);

	/*-Packet Related-*/
	//------------------
	void Reset(void);

	void PacketSent(int size);

	void PacketReceived(unsigned int sequence, int size);

	unsigned int GenerateAckBits(void);

	void ProcessAck(unsigned int ack, unsigned int ack_bits);

	void Update(float deltaTime);

	void Validate(void);


	/*-Utility Functions-*/
	//---------------------
	static bool sequence_more_recent(unsigned int s1, unsigned int s2, unsigned int max_sequence);

	static int bit_index_for_sequence(unsigned int sequence, unsigned int ack, unsigned int max_sequence);

	static unsigned int generate_ack_bits(unsigned int ack, const PacketQueue & received_queue, unsigned int max_sequence);

	static void process_ack(unsigned int ack, unsigned int ack_bits,
		PacketQueue & pending_ack_queue, PacketQueue & acked_queue,
		std::vector<unsigned int> & acks, unsigned int & acked_packets,
		float & rtt, unsigned int max_sequence);


	/*-Accessors-*/
	//---------------------
	unsigned int GetLocalSequence(void) const;

	unsigned int GetRemoteSequence(void) const;

	unsigned int GetMaxSequence(void) const;

	void GetAcks(unsigned int ** acks, int & count);

	unsigned int GetSentPackets(void) const;

	unsigned int GetReceivedPackets(void) const;

	unsigned int GetLostPackets(void) const;

	unsigned int GetAckedPackets(void) const;

	float GetSentBandwidth(void) const;

	float GetAckedBandwidth(void) const;

	float GetRoundTripTime(void) const;

	int GetHeaderSize(void) const;

};

#endif