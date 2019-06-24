/*
 * ap.cpp
 *
 *  Created on: 2019. 6. 14.
 *      Author: HanCheol Cho
 */




#include "ap.h"



void apInit(void)
{
  delay(100);
  ledOn(_DEF_LED3);

  uartOpen(_DEF_UART1, 57600);
  cmdifOpen(_DEF_UART1, 57600);
}

void apMain(void)
{
  uint32_t pre_time;

  while(1)
  {
    cmdifMain();

    if (millis()-pre_time >= 100)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
    }
  }
}
