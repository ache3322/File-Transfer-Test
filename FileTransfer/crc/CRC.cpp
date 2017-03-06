/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILENAME		: CRC.cpp
* PROGRAMMER	: Austin Che
* DATE			: 2017/02/28
* DESCRIPTION	: CPP file contains the CRC class implementation.
* CREDIT		:
*/
#include "CRC.h"



//=============================
// CONSTRUCTOR / DESTRUCTOR
//=============================
CRC::CRC()
{
}


CRC::~CRC()
{
}



//---------------------------------
//=======================
// CRC METHODS
//=======================
void CRC::BuildCRCTable(void)
{
	int i;
	int j;
	unsigned long crc;

	for (i = 0; i <= 255; i++) {
		crc = i;
		for (j = 8; j > 0; j--) {
			if (crc & 0x01)
				crc = (crc >> 0x01) ^ CRC32_POLYNOMIAL;
			else
				crc >>= 1;
		}
		CRCTable[i] = crc;
	}	/* end for */
}	/* end BuildCRCTable */


unsigned long CRC::CalculateBufferCRC(unsigned long long count, unsigned long crc, void *buffer)
{
	unsigned char *p;
	unsigned long temp1;
	unsigned long temp2;

	p = (unsigned char *)buffer;
	while (count-- != 0) {
		temp1 = (crc >> 8) & 0x00FFFFFFL;
		temp2 = CRCTable[((int)crc ^ *p++) & 0xff];
		crc = temp1 ^ temp2;
	}	/* end while */
	return crc;
}	/* end CalculateBufferCRC */



//---------------------------------
//=======================
// ACCESSORS
//=======================
unsigned long CRC::GetCheckSum(void) const
{
	return checkSum;
}



//---------------------------------
//=======================
// ACCESSORS
//=======================
void CRC::SetCheckSum(unsigned long sum)
{
	this->checkSum = sum;
}