/*
 * hw.c
 *
 *  Created on: 2019. 6. 14.
 *      Author: HanCheol Cho
 */




#include "hw.h"





void hwInit(void)
{
  bspInit();

  microsInit();
  millisInit();
  delayInit();
  cmdifInit();

  ledInit();
  sdramInit();
  ltdcInit();
  uartInit();
  i2cInit();
  uartOpen(_DEF_UART1, 57600);
  usbHidInit();
}
