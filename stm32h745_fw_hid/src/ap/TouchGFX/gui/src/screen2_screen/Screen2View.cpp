#include <gui/screen2_screen/Screen2View.hpp>
#include "hw.h"

Screen2View::Screen2View()
{

}

void Screen2View::setupScreen()
{
    Screen2ViewBase::setupScreen();
}

void Screen2View::tearDownScreen()
{
    Screen2ViewBase::tearDownScreen();
}

void Screen2View::handleTickEvent()
{
  static uint32_t pre_time;
  usb_hid_joy_msg_t joy_msg;

  usbHidJoyCurrentRead(&joy_msg);


  Screen2View::button1.moveTo(joy_msg.l_x/2 +  66, -joy_msg.l_y/2 + 106);
  Screen2View::button2.moveTo(joy_msg.r_x/2 + 288, -joy_msg.r_y/2 + 106);

  //printf("btn : 0x%X\r\n", joy_msg.buttons);

  if (joy_msg.buttons & (1<<JOY_BTN_BACK))
  {
    application().gotoScreen1ScreenSlideTransitionWest();
  }


  if (joy_msg.buttons & (1<<JOY_BTN_R_LEFT))
  {
    if (Screen2View::boxWithBorder1.isVisible() == false)
    {
      Screen2View::boxWithBorder1.setVisible(true);
      Screen2View::boxWithBorder1.invalidate();
    }
  }
  else
  {
    if (Screen2View::boxWithBorder1.isVisible() == true)
    {
      Screen2View::boxWithBorder1.setVisible(false);
      Screen2View::boxWithBorder1.invalidate();
    }
  }
  if (joy_msg.buttons & (1<<JOY_BTN_R_RIGHT))
  {
    if (Screen2View::boxWithBorder1_1.isVisible() == false)
    {
      Screen2View::boxWithBorder1_1.setVisible(true);
      Screen2View::boxWithBorder1_1.invalidate();
    }
  }
  else
  {
    if (Screen2View::boxWithBorder1_1.isVisible() == true)
    {
      Screen2View::boxWithBorder1_1.setVisible(false);
      Screen2View::boxWithBorder1_1.invalidate();
    }
  }



  if (usbHidJoyIsConnected() == true)
  {
    Screen2View::radioButton1.setSelected(true);
  }
  else
  {
    Screen2View::radioButton1.setSelected(false);
  }
  pre_time = millis();
}
