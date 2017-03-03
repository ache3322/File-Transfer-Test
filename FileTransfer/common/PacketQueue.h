/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILENAME		: PacketQueue.h
* PROGRAMMER	: Austin Che
* DATE			: 2017/02/28
* DESCRIPTION	: Header file contains the PacketQueue class definition.
* CREDIT		:
*/
#ifndef __PACKET_QUEUE_H__
#define __PACKET_QUEUE_H__

#include <list>


// Struct definition
struct PacketData
{
	unsigned int sequence;			// packet sequence number
	float time;					    // time offset since packet was sent or received (depending on context)
	int size;						// packet size in bytes
};


/*
* METHOD
* DETAILS
* PARAM
* RETURNS
*/
inline bool sequence_more_recent(unsigned int s1, unsigned int s2, unsigned int max_sequence)
{
	return (s1 > s2) && (s1 - s2 <= max_sequence / 2) || (s2 > s1) && (s2 - s1 > max_sequence / 2);
}



/*!
* \class PacketQueue
* \brief Helps sorting, verifying, and storing information about the sequence numbers.
* \details The PacketQueue class inherits from the STL list. There is added functionality
*		that this class contains for organizing PacketData.
*/
class PacketQueue : public std::list<PacketData>
{
	//=============================
	// Public Attributes
	//=============================
public:

	/*-PacketQueue methods-*/
	//------------------
	bool Exists(unsigned int sequence);
	void InsertSorted(const PacketData & p, unsigned int max_sequence);
	void VerifySorted(unsigned int max_sequence);
};

#endif