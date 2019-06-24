/*
 * i2c.c
 *
 *  Created on: 2019. 3. 21.
 *      Author: HanCheol Cho
 */




#include "i2c.h"
#include "cmdif.h"
#include "delay.h"

#if HW_USE_CMDIF_I2C == 1
static void i2cCmdifInit(void);
static void i2cCmdif(void);
#endif



static uint32_t i2c_timeout[I2C_MAX_CH];
static uint32_t i2c_errcount[I2C_MAX_CH];
static uint32_t i2c_freq[I2C_MAX_CH];


static bool is_init[I2C_MAX_CH];


static I2C_HandleTypeDef hi2c4;

typedef struct
{
  I2C_HandleTypeDef *p_hi2c;

  GPIO_TypeDef *scl_port;
  int           scl_pin;

  GPIO_TypeDef *sda_port;
  int           sda_pin;
} i2c_tbl_t;

static i2c_tbl_t i2c_tbl[I2C_MAX_CH] =
    {
        { &hi2c4, GPIOD, GPIO_PIN_12,  GPIOD, GPIO_PIN_13},
    };


bool i2cInit(void)
{
  uint32_t i;


  for (i=0; i<I2C_MAX_CH; i++)
  {
    i2c_timeout[i] = 10;
    i2c_errcount[i] = 0;
    is_init[i] = false;
  }

#if HW_USE_CMDIF_I2C == 1
  i2cCmdifInit();
#endif

  return true;
}





bool i2cBegin(uint8_t ch, uint32_t freq_khz)
{
  bool ret = false;

  I2C_HandleTypeDef *p_handle = i2c_tbl[ch].p_hi2c;

  if (ch >= I2C_MAX_CH)
  {
    return false;
  }


  switch(ch)
  {
    case _DEF_I2C1:
    case _DEF_I2C2:
      i2c_freq[ch] = freq_khz;

      p_handle->Instance             = I2C4;
      p_handle->Init.Timing          = ((uint32_t)0x40912732);
      p_handle->Init.OwnAddress1     = 0x00;
      p_handle->Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
      p_handle->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
      p_handle->Init.OwnAddress2     = 0xFF;
      p_handle->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
      p_handle->Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

      HAL_I2C_DeInit(p_handle);
      if(HAL_I2C_Init(p_handle) != HAL_OK)
      {
      }

      /* Enable the Analog I2C Filter */
      HAL_I2CEx_ConfigAnalogFilter(p_handle,I2C_ANALOGFILTER_ENABLE);

      /* Configure Digital filter */
      HAL_I2CEx_ConfigDigitalFilter(p_handle, 0);

      ret = true;
      is_init[ch] = true;
      break;
  }

  return ret;
}

void i2cReset(uint8_t ch)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  i2c_tbl_t *p_pin = &i2c_tbl[ch];


  GPIO_InitStruct.Pin       = p_pin->scl_pin;
  GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(p_pin->scl_port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin       = p_pin->sda_pin;
  GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  HAL_GPIO_Init(p_pin->sda_port, &GPIO_InitStruct);

  /*
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
  for (uint8_t i=0; i<9; i++)
  {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
  }
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
  */

  HAL_GPIO_WritePin(p_pin->scl_port, p_pin->scl_pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(p_pin->sda_port, p_pin->sda_pin, GPIO_PIN_SET);
  delayUs(5);

  for (int i = 0; i < 9; i++)
  {

    HAL_GPIO_WritePin(p_pin->scl_port, p_pin->scl_pin, GPIO_PIN_RESET);
    delayUs(5);
    HAL_GPIO_WritePin(p_pin->scl_port, p_pin->scl_pin, GPIO_PIN_SET);
    delayUs(5);
  }

  HAL_GPIO_WritePin(p_pin->scl_port, p_pin->scl_pin, GPIO_PIN_RESET);
  delayUs(5);
  HAL_GPIO_WritePin(p_pin->sda_port, p_pin->sda_pin, GPIO_PIN_RESET);
  delayUs(5);

  HAL_GPIO_WritePin(p_pin->scl_port, p_pin->scl_pin, GPIO_PIN_SET);
  delayUs(5);
  HAL_GPIO_WritePin(p_pin->sda_port, p_pin->sda_pin, GPIO_PIN_SET);
}

bool i2cRecovery(uint8_t ch)
{
  bool ret;

  i2cReset(ch);

  ret = i2cBegin(ch, i2c_freq[ch]);

  return ret;
}

bool i2cReadByte (uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data, uint32_t timeout)
{
  return i2cReadBytes(ch, dev_addr, reg_addr, p_data, 1, timeout);
}

bool i2cReadBytes(uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data, uint32_t length, uint32_t timeout)
{
  bool ret;
  HAL_StatusTypeDef i2c_ret;
  I2C_HandleTypeDef *p_handle = i2c_tbl[ch].p_hi2c;

  if (ch >= I2C_MAX_CH)
  {
    return false;
  }

  i2c_ret = HAL_I2C_Mem_Read(p_handle, (uint16_t)(dev_addr << 1), reg_addr, I2C_MEMADD_SIZE_8BIT, p_data, length, timeout);

  if( i2c_ret == HAL_OK )
  {
    ret = true;
  }
  else
  {
    ret = false;
  }

  return ret;
}

bool i2cReadData(uint8_t ch, uint16_t dev_addr, uint8_t *p_data, uint32_t length, uint32_t timeout)
{
  bool ret;
  HAL_StatusTypeDef i2c_ret;
  I2C_HandleTypeDef *p_handle = i2c_tbl[ch].p_hi2c;

  if (ch >= I2C_MAX_CH)
  {
    return false;
  }

  i2c_ret = HAL_I2C_Master_Receive(p_handle, (uint16_t)(dev_addr << 1), p_data, length, timeout);

  if( i2c_ret == HAL_OK )
  {
    ret = true;
  }
  else
  {
    ret = false;
  }

  return ret;
}

bool i2cWriteByte (uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t data, uint32_t timeout)
{
  return i2cWriteBytes(ch, dev_addr, reg_addr, &data, 1, timeout);
}

bool i2cWriteBytes(uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data, uint32_t length, uint32_t timeout)
{
  bool ret;
  HAL_StatusTypeDef i2c_ret;
  I2C_HandleTypeDef *p_handle = i2c_tbl[ch].p_hi2c;

  if (ch >= I2C_MAX_CH)
  {
    return false;
  }

  i2c_ret = HAL_I2C_Mem_Write(p_handle, (uint16_t)(dev_addr << 1), reg_addr, I2C_MEMADD_SIZE_8BIT, p_data, length, timeout);

  if(i2c_ret == HAL_OK)
  {
    ret = true;
  }
  else
  {
    ret = false;
  }

  return ret;
}

bool i2cWriteData(uint8_t ch, uint16_t dev_addr, uint8_t *p_data, uint32_t length, uint32_t timeout)
{
  bool ret;
  HAL_StatusTypeDef i2c_ret;
  I2C_HandleTypeDef *p_handle = i2c_tbl[ch].p_hi2c;

  if (ch >= I2C_MAX_CH)
  {
    return false;
  }


  i2c_ret = HAL_I2C_Master_Transmit(p_handle, (uint16_t)(dev_addr << 1), p_data, length, timeout);

  if(i2c_ret == HAL_OK)
  {
    ret = true;
  }
  else
  {
    ret = false;
  }

  return ret;
}

void i2cSetTimeout(uint8_t ch, uint32_t timeout)
{
  i2c_timeout[ch] = timeout;
}

uint32_t i2cGetTimeout(uint8_t ch)
{
  return i2c_timeout[ch];
}

void i2cClearErrCount(uint8_t ch)
{
  i2c_errcount[ch] = 0;
}

uint32_t i2cGetErrCount(uint8_t ch)
{
  return i2c_errcount[ch];
}







#if HW_USE_CMDIF_I2C == 1
void i2cCmdifInit(void)
{
  cmdifAdd("i2c", i2cCmdif);
}


void i2cCmdif(void)
{
  bool ret = true;
  bool i2c_ret;

  uint8_t print_ch;
  uint8_t ch;
  uint16_t dev_addr;
  uint16_t reg_addr;
  uint16_t length;

  uint32_t i;
  uint8_t i2c_data[128];


  if (cmdifGetParamCnt() == 2)
  {
    print_ch = (uint16_t) cmdifGetParam(1);

    print_ch = constrain(print_ch, 1, I2C_MAX_CH);
    print_ch -= 1;

    if(cmdifHasString("begin", 0) == true)
    {
      i2c_ret = i2cBegin(print_ch, 400);
      if (i2c_ret == true)
      {
        cmdifPrintf("I2C CH%d Begin OK\n", print_ch + 1);
      }
      else
      {
        cmdifPrintf("I2C CH%d Begin Fail\n", print_ch + 1);
      }
    }
  }
  else if (cmdifGetParamCnt() == 5)
  {
    print_ch = (uint16_t) cmdifGetParam(1);
    print_ch = constrain(print_ch, 1, I2C_MAX_CH);

    dev_addr = (uint16_t) cmdifGetParam(2);
    reg_addr = (uint16_t) cmdifGetParam(3);
    length   = (uint16_t) cmdifGetParam(4);
    ch       = print_ch - 1;

    if(cmdifHasString("read", 0) == true)
    {
      for (i=0; i<length; i++)
      {
        i2c_ret = i2cReadByte(ch, dev_addr, reg_addr+i, i2c_data, 100);

        if (i2c_ret == true)
        {
          cmdifPrintf("%d I2C - 0x%02X : 0x%02X\n", print_ch, reg_addr+i, i2c_data[0]);
        }
        else
        {
          cmdifPrintf("%d I2C - Fail \n", print_ch);
          break;
        }
      }
    }
    else
    {
      ret = false;
    }
  }
  else
  {
    ret = false;
  }

  if (ret == false)
  {
    cmdifPrintf( "i2c begin channel[1~%d]\n", I2C_MAX_CH);
    cmdifPrintf( "i2c read channel dev_addr reg_addr length\n");
  }
}




void I2C4_EV_IRQHandler(void)
{
  HAL_I2C_EV_IRQHandler(&hi2c4);
}

void I2C1_ER_IRQHandler(void)
{
  HAL_I2C_ER_IRQHandler(&hi2c4);
}




void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
  GPIO_InitTypeDef  gpio_init_structure;
  RCC_PeriphCLKInitTypeDef  RCC_PeriphClkInit;

  /* Configure the I2C clock source */
  RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C123;
  RCC_PeriphClkInit.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
  HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);

  RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C4;
  RCC_PeriphClkInit.I2c4ClockSelection = RCC_I2C4CLKSOURCE_D3PCLK1;
  HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);


  /* set STOPWUCK in RCC_CFGR */
  //__HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_HSI);

  __HAL_RCC_I2C4_CLK_ENABLE();

  /*** Configure the GPIOs ***/
  /* Enable GPIO clock */
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /* Configure I2C Tx as alternate function */
  gpio_init_structure.Pin = GPIO_PIN_12;
  gpio_init_structure.Mode = GPIO_MODE_AF_OD;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_structure.Alternate = GPIO_AF4_I2C4;
  HAL_GPIO_Init(GPIOD, &gpio_init_structure);

  /* Configure I2C Rx as alternate function */
  gpio_init_structure.Pin = GPIO_PIN_13;
  HAL_GPIO_Init(GPIOD, &gpio_init_structure);

  /*** Configure the I2C peripheral ***/
  /* Enable I2C clock */
  //__HAL_RCC_I2C4_CLK_ENABLE();

  /* Force the I2C peripheral clock reset */
  //__HAL_RCC_I2C4_FORCE_RESET();

  /* Release the I2C peripheral clock reset */
  //__HAL_RCC_I2C4_RELEASE_RESET();

  /* Enable and set I2Cx Interrupt to a lower priority */
  HAL_NVIC_SetPriority(I2C4_EV_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(I2C4_EV_IRQn);

  /* Enable and set I2Cx Interrupt to a lower priority */
  HAL_NVIC_SetPriority(I2C4_ER_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(I2C4_ER_IRQn);
}

/**
  * @brief I2C MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @param hi2c: I2C handle pointer
  * @retval None
  */
void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{

  /*##-1- Reset peripherals ##################################################*/
  __HAL_RCC_I2C4_FORCE_RESET();
  __HAL_RCC_I2C4_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure I2C Tx as alternate function  */
  HAL_GPIO_DeInit(GPIOD, GPIO_PIN_12);
  /* Configure I2C Rx as alternate function  */
  HAL_GPIO_DeInit(GPIOD, GPIO_PIN_13);

    /*##-3- Disable the NVIC for I2C ##########################################*/
  HAL_NVIC_DisableIRQ(I2C4_ER_IRQn);
  HAL_NVIC_DisableIRQ(I2C4_EV_IRQn);
}

#endif
