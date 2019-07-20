#include <gui/screen1_screen/Screen1View.hpp>
#include "hw.h"


Screen1View::Screen1View()
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::handleTickEvent()
{
  usb_hid_joy_msg_t joy_msg;

  usbHidJoyCurrentRead(&joy_msg);


  if (joy_msg.buttons & (1<<JOY_BTN_START))
  {
    application().gotoScreen2ScreenSlideTransitionEast();
  }
}
