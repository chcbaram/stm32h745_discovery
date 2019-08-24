#include <gui/screen1_screen/Screen1View.hpp>
#include "hw.h"


extern uint32_t frame_buf[640*480] __attribute__((section(".sdram_buf")));
extern uint8_t _frame_buf[480 * 272 * 3] __attribute__((section(".sdram_buf")));

Screen1View::Screen1View()
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();

    touchgfx::HAL::getInstance()->setFrameBufferStartAddress((uint16_t*)_frame_buf, 24, false, true);
    //touchgfx::HAL::getInstance()->flushFrameBuffer();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::handleTickEvent()
{
  static uint8_t cnt = 0;

  usb_hid_joy_msg_t joy_msg;

  usbHidJoyCurrentRead(&joy_msg);


  if (joy_msg.buttons & (1<<JOY_BTN_START))
  {
    application().gotoScreen2ScreenSlideTransitionEast();
  }


  //void blitCopy(const uint16_t* sourceData, const Rect& source, const Rect& blitRect, uint8_t alpha, bool hasTransparentPixels);


  //virtual void blitCopy(const uint16_t* pSrc, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t srcWidth, uint8_t alpha, bool hasTransparentPixels);


  //if (cnt%5 == 0)
  //{
    //touchgfx::HAL::getInstance()->blitCopy((const uint16_t *)_frame_buf, (uint16_t)0, (uint16_t)0, (uint16_t)150, (uint16_t)272, (uint16_t)480, (uint8_t)255, false);
    //touchgfx::HAL::getInstance()->blitCopyARGB8888((const uint16_t *)frame_buf, (uint16_t)0, (uint16_t)0, (uint16_t)100, (uint16_t)100, (uint16_t)180, (uint8_t)255);
    //touchgfx::HAL::getInstance()->flushFrameBuffer();
    //touchgfx::HAL::getInstance()->setFrameBufferStartAddress()
  //}


  cnt++;

}
