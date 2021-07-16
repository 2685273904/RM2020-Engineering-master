#ifndef __MAGAZINE_CONTROL_H
#define __MAGAZINE_CONTROL_H

#include "user_common.h"

#include "GAS_8.h"


typedef struct{
	uint8_t close;
	uint8_t open;
	uint8_t timeCounter;
	uint8_t nextReq;
}Magazine_t;

extern Magazine_t Magazine;

void Magazine_Open(void);
void Magazine_Close(void);
void Magazine_processing(void);

#endif
