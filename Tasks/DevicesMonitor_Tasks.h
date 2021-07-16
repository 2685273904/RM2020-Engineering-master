#ifndef __DEVICESMONITOR_TASKS_H
#define __DEVICESMONITOR_TASKS_H


#include "DevicesMonitor.h"

#include "user_common.h"



extern TaskHandle_t xHandleDevicesMonitor;

void vTaskDevicesMonitor(void *pvParameters);


#endif /* __DEVICESMONITOR_TASKS_H */

