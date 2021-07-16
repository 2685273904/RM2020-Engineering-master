#ifndef __GAS_8_H
#define __GAS_8_H

#include "user_common.h"


#define GAS8_SEND_ID    0x500

#define GAS_POS_1      0x01
#define GAS_POS_2      0x02
#define GAS_POS_3      0x04
#define GAS_POS_4      0x08
#define GAS_POS_5      0x10
#define GAS_POS_6      0x20
#define GAS_POS_7      0x40
#define GAS_POS_8      0x80
#define GAS_POS_ALL    0xFF

typedef struct{
  union {
		uint8_t dataPack;
		struct{
			bool GAS_1:1;
			bool GAS_2:1;
			bool GAS_3:1;
			bool GAS_4:1;
			bool GAS_5:1;
			bool GAS_6:1;
			bool GAS_7:1;
			bool GAS_8:1;
		};
	}data;
  
}GAS8_t;

extern GAS8_t GAS8s[8];

//void GAS8_Init(void);


void GAS8_setBits(uint8_t gasID, uint8_t pos);
void GAS8_resetBits(uint8_t gasID, uint8_t pos);
void GAS8_toggleBits(uint8_t gasID, uint8_t pos);
void GAS8_writeBits(uint8_t gasID, uint8_t pos, bool status);

void GAS8_writeData(uint8_t gasID, uint8_t data);

#endif /* __GAS_8_H */
