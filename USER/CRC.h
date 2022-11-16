#ifndef __CRC_H
#define __CRC_H
#include "sys.h"  

void init_CRC32_table(void);
unsigned int GetCRC32(unsigned int crc, unsigned char* data, unsigned int size);

#endif  
