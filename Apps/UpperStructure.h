#ifndef __UPPERSTRUCTURE_H
#define __UPPERSTRUCTURE_H
/*--------------------- I N C L U D E - F I L E S ----------------------------*/
#include "user_common.h"


/*-------------------------G L O B A L - T Y P E S----------------------------*/
typedef struct{
  uint8_t isOpen;
  uint8_t openReq;
  uint16_t nowtep;
}UpperStructrue_t;


extern UpperStructrue_t UpperStructrue;
/*----------------------G L O B A L - D E F I N E S---------------------------*/
void UpperStructure_Init(void);
void UpperStructure_processing(void);
uint8_t UpperStructure_close(void);
uint8_t UpperStructure_open(int32_t pos);
void UpperStructure_M3508executeAngle(void);
/*----------------------------------FILE OF END-------------------------------*/

#endif /* __UPPERSTRUCTURE_H */
