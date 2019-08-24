
#include "STM32H750TouchController.hpp"

extern "C"
{
//#include "stm32h750b_discovery_ts.h"
#include "ts.h"

    uint32_t LCD_GetXSize();
    uint32_t LCD_GetYSize();
}

using namespace touchgfx;

void STM32H750TouchController::init()
{
  tsInit(LCD_GetXSize(), LCD_GetYSize());
}

bool STM32H750TouchController::sampleTouch(int32_t& x, int32_t& y)
{
  TS_StateTypeDef state = { 0 };
  tsGetState(&state);
  if (state.touchDetected)
  {
      x = state.touchX[0];
      y = state.touchY[0];

      printf(" %d %d \r\n", x, y);
      return true;
  }

  return false;
}
