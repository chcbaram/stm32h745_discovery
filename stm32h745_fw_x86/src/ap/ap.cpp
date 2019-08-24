/*
 * ap.cpp
 *
 *  Created on: 2019. 6. 14.
 *      Author: HanCheol Cho
 */




#include "ap.h"


extern "C" void fake86Main(void);


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
  usb_hid_joy_msg_t joy_msg;



  usbHidStart();

  fake86Main();

  while(1)
  {
    cmdifMain();

    if (millis()-pre_time >= 100)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);

      usbHidJoyCurrentRead(&joy_msg);

      //printf("%d %d 0x%08X \r\n", joy_msg.l_x, joy_msg.l_y, joy_msg.buttons);
    }
  }
}
