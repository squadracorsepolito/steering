#include "ft5336.h"

static FT5336_StatusTypeDef FT5336_DetectTouch();

/**
 * @brief  Get FT5336 sensor capabilities
 * @param  Capabilities pointer to FT5336 sensor capabilities
 * @retval Component status
 */
FT5336_StatusTypeDef FT5336_GetCapabilities(FT5336_CapabilitiesTypeDef *Capabilities)
{

  /* Store component's capabilities */
  Capabilities->MultiTouch = 1;
  Capabilities->Gesture = 1;
  Capabilities->MaxTouch = FT5336_MAX_NB_TOUCH;
  Capabilities->MaxXl = FT5336_MAX_X_LENGTH;
  Capabilities->MaxYl = FT5336_MAX_Y_LENGTH;

  return FT5336_OK;
}

/**
 * @brief  Initialize the FT5336 communication bus
 *         from MCU to FT5336 : ie I2C channel initialization (if required).
 * @retval FT5336_OK
 */
FT5336_StatusTypeDef FT5336_Init()
{
  return FT5336_DisableIT();
  ;
}

/**
 * @brief  Configure the FT5336 gesture
 * @param  GestureInit Gesture init structure
 * @retval FT5336_OK
 */
FT5336_StatusTypeDef FT5336_GestureConfig(FT5336_Gesture_InitTypeDef *GestureInit)
{
  int32_t ret;

  ret = ft5336_radian_value((uint8_t)GestureInit->Radian);
  ret += ft5336_offset_left_right((uint8_t)GestureInit->OffsetLeftRight);
  ret += ft5336_offset_up_down((uint8_t)GestureInit->OffsetUpDown);
  ret += ft5336_disatnce_left_right((uint8_t)GestureInit->DistanceLeftRight);
  ret += ft5336_distance_up_down((uint8_t)GestureInit->DistanceUpDown);
  ret += ft5336_distance_zoom((uint8_t)GestureInit->DistanceZoom);

  if (ret != FT5336_OK)
  {
    ret = FT5336_ERROR;
  }

  return ret;
}

/**
 * @brief  Read the FT5336 device ID, pre initialize I2C in case of need to be
 *         able to read the FT5336 device ID, and verify this is a FT5336.
 * @param The Device ID (two bytes).
 * @retval FT5336_OK
 */
FT5336_StatusTypeDef FT5336_ReadID(uint32_t *Id)
{
  return ft5336_chip_id((uint8_t *)Id);
}

/**
 * @brief  Get the touch screen X and Y positions values
 * @param  State: Single Touch stucture pointer
 * @retval FT5336_OK.
 */
FT5336_StatusTypeDef FT5336_GetState(FT5336_StateTypeDef *State)
{
  int32_t ret = FT5336_OK;
  uint8_t data[4];

  State->TouchDetected = (uint32_t)FT5336_DetectTouch();
  if (ft5336_read_reg(FT5336_P1_XH_REG, data, (uint16_t)sizeof(data)) != FT5336_OK)
  {
    ret = FT5336_ERROR;
  }
  else
  {
    /* Send back first ready X position to caller */
    State->TouchX =
        (((uint32_t)data[0] & FT5336_P1_XH_TP_BIT_MASK) << 8) | ((uint32_t)data[1] & FT5336_P1_XL_TP_BIT_MASK);
    /* Send back first ready Y position to caller */
    State->TouchY =
        (((uint32_t)data[2] & FT5336_P1_YH_TP_BIT_MASK) << 8) | ((uint32_t)data[3] & FT5336_P1_YL_TP_BIT_MASK);
  }

  return ret;
}

/**
 * @brief  Get the touch screen Xn and Yn positions values in multi-touch mode
 * @param  State Multi Touch structure pointer
 * @retval FT5336_OK.
 */
FT5336_StatusTypeDef FT5336_GetMultiTouchState(FT5336_MultiTouch_StateTypeDef *State)
{
  int32_t ret = FT5336_OK;
  uint8_t data[30];
  uint32_t i;

  State->TouchDetected = (uint32_t)FT5336_DetectTouch();

  if (ft5336_read_reg(FT5336_P1_XH_REG, data, (uint16_t)sizeof(data)) != FT5336_OK)
  {
    ret = FT5336_ERROR;
  }
  else
  {
    for (i = 0; i < FT5336_MAX_NB_TOUCH; i++)
    {
      /* Send back first ready X position to caller */
      State->TouchX[i] = (((uint32_t)data[i * 6U] & FT5336_P1_XH_TP_BIT_MASK) << 8U) |
                         ((uint32_t)data[(i * 6U) + 1U] & FT5336_P1_XL_TP_BIT_MASK);
      /* Send back first ready Y position to caller */
      State->TouchY[i] = (((uint32_t)data[(i * 6U) + 2U] & FT5336_P1_YH_TP_BIT_MASK) << 8U) |
                         ((uint32_t)data[(i * 6U) + 3U] & FT5336_P1_YL_TP_BIT_MASK);
      /* Send back first ready Event to caller */
      State->TouchEvent[i] = (((uint32_t)data[i * 6U] & FT5336_P1_XH_EF_BIT_MASK) >> FT5336_P1_XH_EF_BIT_POSITION);
      /* Send back first ready Weight to caller */
      State->TouchWeight[i] = ((uint32_t)data[(i * 6U) + 4U] & FT5336_P1_WEIGHT_BIT_MASK);
      /* Send back first ready Area to caller */
      State->TouchArea[i] = ((uint32_t)data[(i * 6U) + 5U] & FT5336_P1_MISC_BIT_MASK) >> FT5336_P1_MISC_BIT_POSITION;
    }
  }

  return ret;
}

/**
 * @brief  Get Gesture ID
 * @param  GestureId: gesture ID
 * @retval Gesture ID.
 */
FT5336_StatusTypeDef FT5336_GetGesture(uint8_t *GestureId)
{
  return ft5336_gest_id(GestureId);
}

/**
 * @brief  Configure the FT5336 device to generate IT on given INT pin
 *         connected to MCU as EXTI.
 * @retval None
 */
FT5336_StatusTypeDef FT5336_EnableIT()
{
  return ft5336_g_mode(FT5336_G_MODE_INTERRUPT_TRIGGER);
}

/**
 * @brief  Configure the FT5336 device to stop generating IT on the given INT pin
 *         connected to MCU as EXTI.
 * @retval None
 */
FT5336_StatusTypeDef FT5336_DisableIT()
{
  return ft5336_g_mode(FT5336_G_MODE_INTERRUPT_POLLING);
}

/**
 * @brief  Get IT status from FT5336 interrupt status registers
 *         Should be called Following an EXTI coming to the MCU to know the detailed
 *         reason of the interrupt.
 *         @note : This feature is not applicable to FT5336.
 * @retval TS interrupts status : always return 0 here
 */
FT5336_StatusTypeDef FT5336_ITStatus()
{
  /* Always return FT5336_OK as feature not applicable to FT5336 */
  return FT5336_OK;
}

/**
 * @brief  Clear IT status in FT5336 interrupt status clear registers
 *         Should be called Following an EXTI coming to the MCU.
 *         @note : This feature is not applicable to FT5336.
 * @retval None
 */
FT5336_StatusTypeDef FT5336_ClearIT()
{
  /* Always return FT5336_OK as feature not applicable to FT5336 */
  return FT5336_OK;
}

/**
 * @brief  Return if there is touches detected or not.
 *         Try to detect new touches and forget the old ones (reset internal global
 *         variables).
 * @retval Number of active touches detected (can be 0, 1 or 2) or FT5336_ERROR
 *         in case of error
 */
static FT5336_StatusTypeDef FT5336_DetectTouch()
{
  int32_t ret;
  uint8_t nb_touch;

  /* Read register FT5336_TD_STAT_REG to check number of touches detection */
  if (ft5336_td_status(&nb_touch) != FT5336_OK)
  {
    ret = FT5336_ERROR;
  }
  else
  {
    if (nb_touch > FT5336_MAX_NB_TOUCH)
    {
      /* If invalid number of touch detected, set it to zero */
      ret = 0;
    }
    else
    {
      ret = (int32_t)nb_touch;
    }
  }
  return ret;
}
