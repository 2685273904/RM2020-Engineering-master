#ifndef __SYSTEMINIT_TASKS_H
#define __SYSTEMINIT_TASKS_H

#include "user_common.h"

#define EVENT_BIT_CHASSIS_INIT_FINISH           (1<<0)
#define EVENT_BIT_UPPERSTRUCTURE_INIT_FINISH    (1<<1)
extern EventGroupHandle_t xSystemInitEventGroup;

void vTaskSystemInit(void *pvParameters);


#endif /* __SYSTEMINIT_TASKS_H */
