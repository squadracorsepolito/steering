#include "ts.h"

#define TS_MIN(a, b) ((a > b) ? b : a)

static int32_t FT5336_Probe();

TS_Ctx_t Ts_Ctx;

/**
 * @brief  Initializes and configures the touch screen functionalities and
 *         configures all necessary hardware resources (GPIOs, I2C, clocks..).
 * @param  TS_Init  TS Init structure
 * @retval BSP status
 */
int32_t BSP_TS_Init(TS_Init_t *TS_Init)
{
  int32_t ret = BSP_ERROR_NONE;

  if ((TS_Init->Width == 0U) || (TS_Init->Width > TS_MAX_WIDTH) || (TS_Init->Height == 0U) ||
      (TS_Init->Height > TS_MAX_HEIGHT) || (TS_Init->Accuracy > TS_MIN((TS_Init->Width), (TS_Init->Height))))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (FT5336_Probe() != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_NO_INIT;
    }
    else
    {
      FT5336_CapabilitiesTypeDef Capabilities;
      uint32_t i;
      /* Store parameters on TS context */
      Ts_Ctx.Width = TS_Init->Width;
      Ts_Ctx.Height = TS_Init->Height;
      Ts_Ctx.Orientation = TS_Init->Orientation;
      Ts_Ctx.Accuracy = TS_Init->Accuracy;
      /* Get capabilities to retrieve maximum values of X and Y */
      if (FT5336_GetCapabilities(&Capabilities) < 0)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        /* Store maximum X and Y on context */
        Ts_Ctx.MaxX = Capabilities.MaxXl;
        Ts_Ctx.MaxY = Capabilities.MaxYl;
        /* Initialize previous position in order to always detect first touch */
        for (i = 0; i < TS_TOUCH_NBR; i++)
        {
          Ts_Ctx.PreviousX[i] = TS_Init->Width + TS_Init->Accuracy + 1U;
          Ts_Ctx.PreviousY[i] = TS_Init->Height + TS_Init->Accuracy + 1U;
        }
      }
    }
  }

  return ret;
}

/**
 * @brief  Get Touch Screen instance capabilities
 * @param  Capabilities pointer to Touch Screen capabilities
 * @retval BSP status
 */
int32_t BSP_TS_GetCapabilities(FT5336_CapabilitiesTypeDef *Capabilities)
{
  int32_t ret = BSP_ERROR_NONE;

  FT5336_GetCapabilities(Capabilities);

  return ret;
}

/**
 * @brief  Returns positions of a single touch screen.
 * @param  TS_State  Pointer to touch screen current state structure
 * @retval BSP status
 */
int32_t BSP_TS_GetState(TS_State_t *TS_State)
{
  int32_t ret = BSP_ERROR_NONE;
  uint32_t x_oriented, y_oriented;
  uint32_t x_diff, y_diff;

  FT5336_StateTypeDef state;

  /* Get each touch coordinates */
  if (FT5336_GetState(&state) < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  } /* Check and update the number of touches active detected */
  else if (state.TouchDetected != 0U)
  {
    x_oriented = state.TouchX;
    y_oriented = state.TouchY;

    if ((Ts_Ctx.Orientation & TS_SWAP_XY) == TS_SWAP_XY)
    {
      x_oriented = state.TouchY;
      y_oriented = state.TouchX;
    }

    if ((Ts_Ctx.Orientation & TS_SWAP_X) == TS_SWAP_X)
    {
      x_oriented = Ts_Ctx.MaxX - state.TouchX - 1UL;
    }

    if ((Ts_Ctx.Orientation & TS_SWAP_Y) == TS_SWAP_Y)
    {
      y_oriented = Ts_Ctx.MaxY - state.TouchY;
    }

    /* Apply boundary */
    TS_State->TouchX = (x_oriented * Ts_Ctx.Width) / Ts_Ctx.MaxX;
    TS_State->TouchY = (y_oriented * Ts_Ctx.Height) / Ts_Ctx.MaxY;
    /* Store Current TS state */
    TS_State->TouchDetected = state.TouchDetected;

    /* Check accuracy */
    x_diff = (TS_State->TouchX > Ts_Ctx.PreviousX[0]) ? (TS_State->TouchX - Ts_Ctx.PreviousX[0])
                                                      : (Ts_Ctx.PreviousX[0] - TS_State->TouchX);

    y_diff = (TS_State->TouchY > Ts_Ctx.PreviousY[0]) ? (TS_State->TouchY - Ts_Ctx.PreviousY[0])
                                                      : (Ts_Ctx.PreviousY[0] - TS_State->TouchY);

    if ((x_diff > Ts_Ctx.Accuracy) || (y_diff > Ts_Ctx.Accuracy))
    {
      /* New touch detected */
      Ts_Ctx.PreviousX[0] = TS_State->TouchX;
      Ts_Ctx.PreviousY[0] = TS_State->TouchY;
    }
    else
    {
      TS_State->TouchX = Ts_Ctx.PreviousX[0];
      TS_State->TouchY = Ts_Ctx.PreviousY[0];
    }
  }
  else
  {
    TS_State->TouchDetected = 0U;
    TS_State->TouchX = Ts_Ctx.PreviousX[0];
    TS_State->TouchY = Ts_Ctx.PreviousY[0];
  }

  return ret;
}

#if (USE_TS_MULTI_TOUCH > 0)
/**
 * @brief  Returns positions of multi touch screen.
 * @param  TS_State  Pointer to touch screen current state structure
 * @retval BSP status
 */
int32_t BSP_TS_Get_MultiTouchState(FT5336_MultiTouch_StateTypeDef *TS_State)
{
  int32_t ret = BSP_ERROR_NONE;
  uint32_t index;
  uint32_t x_oriented, y_oriented;
  uint32_t x_diff, y_diff;

  FT5336_MultiTouch_StateTypeDef state;

  /* Get each touch coordinates */
  if (FT5336_GetMultiTouchState(&state) < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    /* Check and update the number of touches active detected */
    if (state.TouchDetected != 0U)
    {
      for (index = 0; index < state.TouchDetected; index++)
      {
        x_oriented = state.TouchX[index];
        y_oriented = state.TouchY[index];

        if ((Ts_Ctx.Orientation & TS_SWAP_XY) == TS_SWAP_XY)
        {
          x_oriented = state.TouchY[index];
          y_oriented = state.TouchX[index];
        }

        if ((Ts_Ctx.Orientation & TS_SWAP_X) == TS_SWAP_X)
        {
          x_oriented = Ts_Ctx.MaxX - state.TouchX[index] - 1UL;
        }

        if ((Ts_Ctx.Orientation & TS_SWAP_Y) == TS_SWAP_Y)
        {
          y_oriented = Ts_Ctx.MaxY - state.TouchY[index];
        }

        /* Apply boundary */
        TS_State->TouchX[index] = (x_oriented * Ts_Ctx.Width) / Ts_Ctx.MaxX;
        TS_State->TouchY[index] = (y_oriented * Ts_Ctx.Height) / Ts_Ctx.MaxY;
        /* Store Current TS state */
        TS_State->TouchDetected = state.TouchDetected;

        /* Check accuracy */
        x_diff = (TS_State->TouchX[index] > Ts_Ctx.PreviousX[index])
                     ? (TS_State->TouchX[index] - Ts_Ctx.PreviousX[index])
                     : (Ts_Ctx.PreviousX[index] - TS_State->TouchX[index]);

        y_diff = (TS_State->TouchY[index] > Ts_Ctx.PreviousY[index])
                     ? (TS_State->TouchY[index] - Ts_Ctx.PreviousY[index])
                     : (Ts_Ctx.PreviousY[index] - TS_State->TouchY[index]);

        if ((x_diff > Ts_Ctx.Accuracy) || (y_diff > Ts_Ctx.Accuracy))
        {
          /* New touch detected */
          Ts_Ctx.PreviousX[index] = TS_State->TouchX[index];
          Ts_Ctx.PreviousY[index] = TS_State->TouchY[index];
        }
        else
        {
          TS_State->TouchX[index] = Ts_Ctx.PreviousX[index];
          TS_State->TouchY[index] = Ts_Ctx.PreviousY[index];
        }
      }
    }
    else
    {
      TS_State->TouchDetected = 0U;
      for (index = 0; index < TS_TOUCH_NBR; index++)
      {
        TS_State->TouchX[index] = Ts_Ctx.PreviousX[index];
        TS_State->TouchY[index] = Ts_Ctx.PreviousY[index];
      }
    }
  }

  return ret;
}
#endif /* USE_TS_MULTI_TOUCH == 1 */

#if (USE_TS_GESTURE == 1)
/**
 * @brief  Update gesture Id following a touch detected.
 * @param  GestureConfig Pointer to gesture configuration structure
 * @retval BSP status
 */
int32_t BSP_TS_GestureConfig(FT5336_Gesture_InitTypeDef *GestureConfig)
{
  int32_t ret = BSP_ERROR_NONE;

  if (FT5336_GestureConfig(GestureConfig) < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }

  return ret;
}

/**
 * @brief  Update gesture Id following a touch detected.
 * @param  GestureId  Pointer to gesture ID
 * @retval BSP status
 */
int32_t BSP_TS_GetGestureId(uint32_t *GestureId)
{
  int32_t ret = BSP_ERROR_NONE;
  uint8_t tmp = 0;

  if (FT5336_GetGesture(&tmp) < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    /* Remap gesture Id to a TS_Gesture_Id_t value */
    switch (tmp)
    {
    case FT5336_GEST_ID_NO_GESTURE:
      *GestureId = GESTURE_ID_NO_GESTURE;
      break;
    case FT5336_GEST_ID_MOVE_UP:
      *GestureId = GESTURE_ID_MOVE_UP;
      break;
    case FT5336_GEST_ID_MOVE_RIGHT:
      *GestureId = GESTURE_ID_MOVE_RIGHT;
      break;
    case FT5336_GEST_ID_MOVE_DOWN:
      *GestureId = GESTURE_ID_MOVE_DOWN;
      break;
    case FT5336_GEST_ID_MOVE_LEFT:
      *GestureId = GESTURE_ID_MOVE_LEFT;
      break;
    case FT5336_GEST_ID_ZOOM_IN:
      *GestureId = GESTURE_ID_ZOOM_IN;
      break;
    case FT5336_GEST_ID_ZOOM_OUT:
      *GestureId = GESTURE_ID_ZOOM_OUT;
      break;
    default:
      *GestureId = GESTURE_ID_NO_GESTURE;
      break;
    }
  }

  return ret;
}
#endif /* USE_TS_GESTURE == 1 */

/**
 * @brief  Set TS orientation
 * @param  Orientation Orientation to be set
 * @retval BSP status
 */
int32_t BSP_TS_Set_Orientation(uint32_t Orientation)
{
  Ts_Ctx.Orientation = Orientation;
  return BSP_ERROR_NONE;
}

/**
 * @brief  Get TS orientation
 * @param  Orientation Current Orientation to be returned
 * @retval BSP status
 */
int32_t BSP_TS_Get_Orientation(uint32_t *Orientation)
{
  *Orientation = Ts_Ctx.Orientation;
  return BSP_ERROR_NONE;
}

/**
 * @}
 */
/** @defgroup TS_Private_Functions Private Functions
 * @{
 */

/**
 * @brief  Register Bus IOs if component ID is OK
 * @retval BSP status
 */
static int32_t FT5336_Probe()
{
  int32_t ret = BSP_ERROR_NONE;
  uint32_t ft5336_id = 0;

  if (FT5336_ReadID(&ft5336_id) != FT5336_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else if (ft5336_id != FT5336_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    FT5336_Init();
  }

  return ret;
}
