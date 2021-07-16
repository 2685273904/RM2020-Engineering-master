#ifndef __MUTUALLY_CONTROL_H
#define __MUTUALLY_CONTROL_H

#include "user_common.h"

#include "GAS_8.h"


typedef struct{
	uint8_t close;
	uint8_t open;
	uint8_t timeCounter;
}Mutually_t;

extern Mutually_t Mutually;

void Mutually_start(void);
void Mutually_close(void);

#endif
