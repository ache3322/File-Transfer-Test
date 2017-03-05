/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILENAME		: CRC.h
* PROGRAMMER	: Austin Che
* DATE			: 
* DESCRIPTION	: Header file contains the CRC class definition.
* CREDIT		:
*/
#ifndef __CRC_H__
#define __CRC_H__


//=============================
// CRC CONSTANTS
//=============================
#define CRC32_POLYNOMIAL     0xEDB88320L
#define CRC32_ARRAY			256



//=============================
// CRC CLASS
//=============================
/*!
* \class CRC
* \brief
*/
class CRC
{
private:

	unsigned long CRCTable[CRC32_ARRAY];	//!< Tabled used for CRC calculations. BuildCRCTable() is called first
											//!< to initialize the CRC correctly

public:

	/*-Constructors-*/
	//------------------
	CRC(void);

	/*-Deconstructors-*/
	//------------------
	~CRC(void);

	/*-CRC Methods-*/
	//------------------
	void BuildCRCTable(void);
	unsigned long CalculateBufferCRC(unsigned long long count, unsigned long crc, void *buffer);
};

#endif //!__CRC_H__