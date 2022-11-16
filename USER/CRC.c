#include "string.h"	 
#include "math.h"
#include "stdarg.h"	
#include "stdio.h"
#include "sys.h"
#include "CRC.h"

unsigned int CRC32_table[256] = { 0 };

void init_CRC32_table()
{
	int i;
	int j;
	unsigned int crc;
	for (i = 0; i != 256; i++)
	{
		 crc = i;
		for (j = 0; j != 8; j++)
		{
			if (crc & 1)
				crc = (crc >> 1) ^ 0xEDB88320;
			else
				crc >>= 1;
		}
		CRC32_table[i] = crc;
	}
}



unsigned int GetCRC32(unsigned int crc, unsigned char* data, unsigned int size)
{
	int i ;
	for ( i = 0; i < size; i++)
	{
		crc = ((crc >> 8) & 0xFFFFFF) ^ CRC32_table[(crc ^ data[i]) & 0xFF];
	}
	return crc;
}






