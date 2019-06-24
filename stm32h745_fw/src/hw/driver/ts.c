/*
 * ts.c
 *
 *  Created on: Feb 13, 2018
 *      Author: opus
 */


#include "ts.h"
#include "ft5336.h"
#include "i2c.h"


#define TS_I2C_ADDRESS                   (((uint16_t)0x70)>>1)

#define TS_INT_PIN                        ((uint32_t)GPIO_PIN_2)
#define TS_INT_GPIO_PORT                  ((GPIO_TypeDef*)GPIOG)
#define TS_INT_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOG_CLK_ENABLE()
#define TS_INT_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOG_CLK_DISABLE()
#define TS_INT_EXTI_IRQn                  EXTI2_IRQn



static TS_DrvTypeDef *tsDriver;
static uint16_t tsXBoundary, tsYBoundary;
static uint8_t  tsOrientation;
static uint8_t  I2cAddress;

/* Table for touchscreen event information display on LCD : table indexed on enum @ref TS_TouchEventTypeDef information */
char * ts_event_string_tab[TOUCH_EVENT_NB_MAX] = { "None",
                                                   "Press down",
                                                   "Lift up",
                                                   "Contact"
                                                  };

/* Table for touchscreen gesture Id information display on LCD : table indexed on enum @ref TS_GestureIdTypeDef information */
char * ts_gesture_id_string_tab[GEST_ID_NB_MAX] = { "None",
                                                    "Move Up",
                                                    "Move Right",
                                                    "Move Down",
                                                    "Move Left",
                                                    "Zoom In",
                                                    "Zoom Out"
                                                  };

/**
  * @}
  */

/** @defgroup STM32H745I_DISCOVERY_TS_Private_Function_Prototypes  Private Function Prototypes
  * @{
  */
/**
  * @}
  */

/** @defgroup STM32H745I_DISCOVERY_TS_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief  Initializes and configures the touch screen functionalities and
  *         configures all necessary hardware resources (GPIOs, I2C, clocks..).
  * @param  ts_SizeX: Maximum X size of the TS area on LCD
  * @param  ts_SizeY: Maximum Y size of the TS area on LCD
  * @retval TS_OK if all initializations are OK. Other value if error.
  */
uint8_t tsInit(uint16_t ts_SizeX, uint16_t ts_SizeY)
{
  uint8_t status = TS_OK;
  tsXBoundary = ts_SizeX;
  tsYBoundary = ts_SizeY;

  /* Read ID and verify if the touch screen driver is ready */
  ft5336_ts_drv.Init(TS_I2C_ADDRESS);
  if(ft5336_ts_drv.ReadID(TS_I2C_ADDRESS) == FT5336_ID_VALUE)
  {
    /* Initialize the TS driver structure */
    tsDriver = &ft5336_ts_drv;
    I2cAddress = TS_I2C_ADDRESS;
    tsOrientation = TS_SWAP_XY;

    /* Initialize the TS driver */
    tsDriver->Start(I2cAddress);
  }
  else
  {
    status = TS_DEVICE_NOT_FOUND;
  }

  return status;
}

/**
  * @brief  DeInitializes the TouchScreen.
  * @retval TS state
  */
uint8_t tsDeInit(void)
{
  /* Actually ts_driver does not provide a DeInit function */
  return TS_OK;
}

/**
  * @brief  Configures and enables the touch screen interrupts.
  * @retval TS_OK if all initializations are OK. Other value if error.
  */
uint8_t tsITConfig(void)
{
  GPIO_InitTypeDef gpio_init_structure;

  /* Configure Interrupt mode for SD detection pin */
  gpio_init_structure.Pin = TS_INT_PIN;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_structure.Mode = GPIO_MODE_IT_RISING;
  HAL_GPIO_Init(TS_INT_GPIO_PORT, &gpio_init_structure);

  /* Enable and set Touch screen EXTI Interrupt to the lowest priority */
  HAL_NVIC_SetPriority((IRQn_Type)(TS_INT_EXTI_IRQn), 0x0F, 0x00);
  HAL_NVIC_EnableIRQ((IRQn_Type)(TS_INT_EXTI_IRQn));

  /* Enable the TS ITs */
  tsDriver->EnableIT(I2cAddress);

  return TS_OK;
}

/**
  * @brief  Gets the touch screen interrupt status.
  * @retval TS_OK if all initializations are OK. Other value if error.
  */
uint8_t tsITGetStatus(void)
{
  /* Return the TS IT status */
  return (tsDriver->GetITStatus(I2cAddress));
}

/**
  * @brief  Returns status and positions of the touch screen.
  * @param  TS_State: Pointer to touch screen current state structure
  * @retval TS_OK if all initializations are OK. Other value if error.
  */
uint8_t tsGetState(TS_StateTypeDef *TS_State)
{
  static uint32_t _x[TS_MAX_NB_TOUCH] = {0, 0};
  static uint32_t _y[TS_MAX_NB_TOUCH] = {0, 0};
  uint8_t ts_status = TS_OK;
  uint16_t x[TS_MAX_NB_TOUCH];
  uint16_t y[TS_MAX_NB_TOUCH];
  uint16_t brute_x[TS_MAX_NB_TOUCH];
  uint16_t brute_y[TS_MAX_NB_TOUCH];
  uint16_t x_diff;
  uint16_t y_diff;
  uint32_t index;
#if (TS_MULTI_TOUCH_SUPPORTED == 1)
  uint32_t weight = 0;
  uint32_t area = 0;
  uint32_t event = 0;
#endif /* TS_MULTI_TOUCH_SUPPORTED == 1 */

  /* Check and update the number of touches active detected */
  TS_State->touchDetected = tsDriver->DetectTouch(I2cAddress);

  if(TS_State->touchDetected)
  {
    for(index=0; index < TS_State->touchDetected; index++)
    {
      /* Get each touch coordinates */
      tsDriver->GetXY(I2cAddress, &(brute_x[index]), &(brute_y[index]));

      if(tsOrientation == TS_SWAP_NONE)
      {
        x[index] = brute_x[index];
        y[index] = brute_y[index];
      }

      if(tsOrientation & TS_SWAP_X)
      {
        x[index] = 4096 - brute_x[index];
      }

      if(tsOrientation & TS_SWAP_Y)
      {
        y[index] = 4096 - brute_y[index];
      }

      if(tsOrientation & TS_SWAP_XY)
      {
        y[index] = brute_x[index];
        x[index] = brute_y[index];
      }

      x_diff = x[index] > _x[index]? (x[index] - _x[index]): (_x[index] - x[index]);
      y_diff = y[index] > _y[index]? (y[index] - _y[index]): (_y[index] - y[index]);

      if ((x_diff + y_diff) > 5)
      {
        _x[index] = x[index];
        _y[index] = y[index];
      }

      if(I2cAddress == FT5336_I2C_SLAVE_ADDRESS)
      {
        TS_State->touchX[index] = x[index];
        TS_State->touchY[index] = y[index];
      }
      else
      {
        /* 2^12 = 4096 : indexes are expressed on a dynamic of 4096 */
        TS_State->touchX[index] = (tsXBoundary * _x[index]) >> 12;
        TS_State->touchY[index] = (tsYBoundary * _y[index]) >> 12;
      }

#if (TS_MULTI_TOUCH_SUPPORTED == 1)

      /* Get touch info related to the current touch */
      ft5336_TS_GetTouchInfo(I2cAddress, index, &weight, &area, &event);

      /* Update TS_State structure */
      TS_State->touchWeight[index] = weight;
      TS_State->touchArea[index]   = area;

      /* Remap touch event */
      switch(event)
      {
        case FT5336_TOUCH_EVT_FLAG_PRESS_DOWN :
          TS_State->touchEventId[index] = TOUCH_EVENT_PRESS_DOWN;
          break;
        case FT5336_TOUCH_EVT_FLAG_LIFT_UP :
          TS_State->touchEventId[index] = TOUCH_EVENT_LIFT_UP;
          break;
        case FT5336_TOUCH_EVT_FLAG_CONTACT :
          TS_State->touchEventId[index] = TOUCH_EVENT_CONTACT;
          break;
        case FT5336_TOUCH_EVT_FLAG_NO_EVENT :
          TS_State->touchEventId[index] = TOUCH_EVENT_NO_EVT;
          break;
        default :
          ts_status = TS_ERROR;
          break;
      } /* of switch(event) */

#endif /* TS_MULTI_TOUCH_SUPPORTED == 1 */

    } /* of for(index=0; index < TS_State->touchDetected; index++) */

#if (TS_MULTI_TOUCH_SUPPORTED == 1)
    /* Get gesture Id */
    ts_status = tsGet_GestureId(TS_State);
#endif /* TS_MULTI_TOUCH_SUPPORTED == 1 */

  } /* end of if(TS_State->touchDetected != 0) */

  return (ts_status);
}

#if (TS_MULTI_TOUCH_SUPPORTED == 1)
/**
  * @brief  Update gesture Id following a touch detected.
  * @param  TS_State: Pointer to touch screen current state structure
  * @retval TS_OK if all initializations are OK. Other value if error.
  */
uint8_t tsGet_GestureId(TS_StateTypeDef *TS_State)
{
  uint32_t gestureId = 0;
  uint8_t  ts_status = TS_OK;

  /* Get gesture Id */
  ft5336_TS_GetGestureID(I2cAddress, &gestureId);

  /* Remap gesture Id to a TS_GestureIdTypeDef value */
  switch(gestureId)
  {
    case FT5336_GEST_ID_NO_GESTURE :
      TS_State->gestureId = GEST_ID_NO_GESTURE;
      break;
    case FT5336_GEST_ID_MOVE_UP :
      TS_State->gestureId = GEST_ID_MOVE_UP;
      break;
    case FT5336_GEST_ID_MOVE_RIGHT :
      TS_State->gestureId = GEST_ID_MOVE_RIGHT;
      break;
    case FT5336_GEST_ID_MOVE_DOWN :
      TS_State->gestureId = GEST_ID_MOVE_DOWN;
      break;
    case FT5336_GEST_ID_MOVE_LEFT :
      TS_State->gestureId = GEST_ID_MOVE_LEFT;
      break;
    case FT5336_GEST_ID_ZOOM_IN :
      TS_State->gestureId = GEST_ID_ZOOM_IN;
      break;
    case FT5336_GEST_ID_ZOOM_OUT :
      TS_State->gestureId = GEST_ID_ZOOM_OUT;
      break;
    default :
      ts_status = TS_ERROR;
      break;
  } /* of switch(gestureId) */

  return(ts_status);
}
#endif /* TS_MULTI_TOUCH_SUPPORTED == 1 */

/**
  * @brief  Clears all touch screen interrupts.
  */
void tsITClear(void)
{
  /* Clear TS IT pending bits */
  tsDriver->ClearIT(I2cAddress);
}

/**
  * @brief  Function used to reset all touch data before a new acquisition
  *         of touch information.
  * @param  TS_State: Pointer to touch screen current state structure
  * @retval TS_OK if OK, TE_ERROR if problem found.
  */
uint8_t tsResetTouchData(TS_StateTypeDef *TS_State)
{
  uint8_t ts_status = TS_ERROR;
  uint32_t index;

  if (TS_State != (TS_StateTypeDef *)NULL)
  {
    TS_State->gestureId = GEST_ID_NO_GESTURE;
    TS_State->touchDetected = 0;

    for(index = 0; index < TS_MAX_NB_TOUCH; index++)
    {
      TS_State->touchX[index]       = 0;
      TS_State->touchY[index]       = 0;
      TS_State->touchArea[index]    = 0;
      TS_State->touchEventId[index] = TOUCH_EVENT_NO_EVT;
      TS_State->touchWeight[index]  = 0;
    }

    ts_status = TS_OK;

  } /* of if (TS_State != (TS_StateTypeDef *)NULL) */

  return (ts_status);
}


















/******************************** LINK TS (TouchScreen) ***********************/


/**
  * @brief  Initialize I2C communication
  *         channel from MCU to TouchScreen (TS).
  */
void TS_IO_Init(void)
{
  i2cBegin(_DEF_I2C1, 400);
}

/**
  * @brief  Writes single data with I2C communication
  *         channel from MCU to TouchScreen.
  * @param  Addr: I2C address
  * @param  Reg: Register address
  * @param  Value: Data to be written
  */
void TS_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value)
{
  bool ret;


  ret = i2cWriteByte(_DEF_I2C1, Addr, Reg, Value, 100);
  if (ret != true)
  {
    i2cRecovery(_DEF_I2C1);
  }
}

/**
  * @brief  Reads single data with I2C communication
  *         channel from TouchScreen.
  * @param  Addr: I2C address
  * @param  Reg: Register address
  * @retval Read data
  */
uint8_t TS_IO_Read(uint8_t Addr, uint8_t Reg)
{
  bool ret;
  uint8_t Value = 0;


  ret = i2cReadBytes(_DEF_I2C1, Addr, Reg, &Value, 1, 100);

  if (ret != true)
  {
    i2cRecovery(_DEF_I2C1);
  }

  return Value;
}

/**
  * @brief  Reads multiple data with I2C communication
  *         channel from TouchScreen.
  * @param  Addr: I2C address
  * @param  Reg: Register address
  * @param  Buffer: Pointer to data buffer
  * @param  Length: Length of the data
  * @retval Number of read data
  */
uint16_t TS_IO_ReadMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length)
{
  bool ret;
  uint16_t status = 0;

  ret = i2cReadBytes(_DEF_I2C1, Addr, Reg, Buffer, Length, 500);

  if (ret != true)
  {
    i2cRecovery(_DEF_I2C1);
    status = 1;
  }



  return status;
}

/**
  * @brief  Writes multiple data with I2C communication
  *         channel from MCU to TouchScreen.
  * @param  Addr: I2C address
  * @param  Reg: Register address
  * @param  Buffer: Pointer to data buffer
  * @param  Length: Length of the data
  */
void TS_IO_WriteMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length)
{
  bool ret;

  ret = i2cWriteBytes(_DEF_I2C1, Addr, Reg, Buffer, Length, 100);

  if (ret != true)
  {
    i2cRecovery(_DEF_I2C1);
  }
}

/**
  * @brief  Delay function used in TouchScreen low level driver.
  * @param  Delay: Delay in ms
  */
void TS_IO_Delay(uint32_t Delay)
{
  HAL_Delay(Delay);
}



