/*
 * main.cpp
 *
 *  Created on: 2019. 6. 14.
 *      Author: HanCheol Cho
 */




#include "main.h"



#define configAP_TASK_PRIORITY                 ( tskIDLE_PRIORITY + 3 )
#define configAP_TASK_STK_SIZE                 ( 4096 / 4 )


extern int touchgfxMain(void);

static void apTask(void* params);


int main(void)
{
  hwInit();
  apInit();


  xTaskCreate(apTask, "apTask",
              configAP_TASK_STK_SIZE,
              NULL,
              configAP_TASK_PRIORITY,
              NULL);

  touchgfxMain();

  vTaskStartScheduler();

  apMain();

  return 0;
}

void apTask(void* params)
{
  apMain();
}
