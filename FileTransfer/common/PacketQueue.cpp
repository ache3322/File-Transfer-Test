/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILENAME		: PacketQueue.cpp
* PROGRAMMER	: Austin Che
* DATE			: 2017/02/28
* DESCRIPTION	: CPP file contains the PacketQueue implementation.
* CREDIT		: https://github.com/ThisIsRobokitty/netgame/blob/master/03%20-%20Reliability%20and%20Flow%20Control/Net.h
*		Credit to ThisIsRoboKitty for providing the source code on Gaffer on Games implementation of UDP.
*		The source code is modified and used for experimental/educational purposes.
*/
#include "PacketQueue.h"




//=============================
// PACKETQUEUE METHODS
//=============================
/**
* \brief
* \details
* \param
* \return
*/
bool PacketQueue::Exists(unsigned int sequence)
{
	for (iterator itor = begin(); itor != end(); ++itor) 
	{
		if (itor->sequence == sequence) {
			return true;
		}
	}

	return false;
}


/**
* \brief
* \details
* \param
* \return
*/
void PacketQueue::InsertSorted(const PacketData & p, unsigned int max_sequence)
{
	if (empty())
	{
		push_back(p);
	}
	else
	{
		if (!sequence_more_recent(p.sequence, front().sequence, max_sequence))
		{
			push_front(p);
		}
		else if (sequence_more_recent(p.sequence, back().sequence, max_sequence))
		{
			push_back(p);
		}
		else
		{
			for (PacketQueue::iterator itor = begin(); itor != end(); itor++)
			{
				if (sequence_more_recent(itor->sequence, p.sequence, max_sequence))
				{
					insert(itor, p);
					break;
				}
			}
		}
	}
}


/**
* \brief
* \details
* \param
* \return
*/
void PacketQueue::VerifySorted(unsigned int max_sequence)
{
	PacketQueue::iterator prev = end();
	for (PacketQueue::iterator itor = begin(); itor != end(); itor++)
	{
		if (prev != end())
		{
			prev = itor;
		}
	}
}