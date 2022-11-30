// IO Expander

#ifndef FT5336_H
#define FT5336_H

#include "ft5336_conf.h"
#include "ft5336_reg.h"
#include <stddef.h>

typedef enum
{
  FT5336_OK = 0,
  FT5336_ERROR = -1,
} FT5336_StatusTypeDef;

/* Max detectable simultaneous touches */
#define FT5336_MAX_NB_TOUCH 5U

/* Touch FT5336 IDs */
#define FT5336_ID 0x51U

/* Possible values of FT5336_DEV_MODE_REG */
#define FT5336_DEV_MODE_WORKING 0x00U
#define FT5336_DEV_MODE_FACTORY 0x04U

/* Possible values of FT5336_GEST_ID_REG */
#define FT5336_GEST_ID_NO_GESTURE 0x00U
#define FT5336_GEST_ID_MOVE_UP 0x10U
#define FT5336_GEST_ID_MOVE_RIGHT 0x14U
#define FT5336_GEST_ID_MOVE_DOWN 0x18U
#define FT5336_GEST_ID_MOVE_LEFT 0x1CU
#define FT5336_GEST_ID_ZOOM_IN 0x48U
#define FT5336_GEST_ID_ZOOM_OUT 0x49U

/* Values Pn_XH and Pn_YH related */
#define FT5336_TOUCH_EVT_FLAG_PRESS_DOWN 0x00U
#define FT5336_TOUCH_EVT_FLAG_LIFT_UP 0x01U
#define FT5336_TOUCH_EVT_FLAG_CONTACT 0x02U
#define FT5336_TOUCH_EVT_FLAG_NO_EVENT 0x03U

/* Possible values of FT5336_GMODE_REG */
#define FT5336_G_MODE_INTERRUPT_POLLING 0x00U
#define FT5336_G_MODE_INTERRUPT_TRIGGER 0x01U

typedef struct
{
  uint32_t Radian;
  uint32_t OffsetLeftRight;
  uint32_t OffsetUpDown;
  uint32_t DistanceLeftRight;
  uint32_t DistanceUpDown;
  uint32_t DistanceZoom;
} FT5336_Gesture_InitTypeDef;

typedef struct
{
  uint32_t TouchDetected;
  uint32_t TouchX;
  uint32_t TouchY;
} FT5336_StateTypeDef;

typedef struct
{
  uint32_t TouchDetected;
  uint32_t TouchX[FT5336_MAX_NB_TOUCH];
  uint32_t TouchY[FT5336_MAX_NB_TOUCH];
  uint32_t TouchWeight[FT5336_MAX_NB_TOUCH];
  uint32_t TouchEvent[FT5336_MAX_NB_TOUCH];
  uint32_t TouchArea[FT5336_MAX_NB_TOUCH];
} FT5336_MultiTouch_StateTypeDef;

typedef struct
{
  uint8_t MultiTouch;
  uint8_t Gesture;
  uint8_t MaxTouch;
  uint32_t MaxXl;
  uint32_t MaxYl;
} FT5336_CapabilitiesTypeDef;

FT5336_StatusTypeDef FT5336_Init();
FT5336_StatusTypeDef FT5336_GestureConfig(FT5336_Gesture_InitTypeDef *GestureInit);
FT5336_StatusTypeDef FT5336_ReadID(uint32_t *Id);
FT5336_StatusTypeDef FT5336_GetState(FT5336_StateTypeDef *State);
FT5336_StatusTypeDef FT5336_GetMultiTouchState(FT5336_MultiTouch_StateTypeDef *State);
FT5336_StatusTypeDef FT5336_GetGesture(uint8_t *GestureId);
FT5336_StatusTypeDef FT5336_EnableIT();
FT5336_StatusTypeDef FT5336_DisableIT();
FT5336_StatusTypeDef FT5336_ITStatus();
FT5336_StatusTypeDef FT5336_ClearIT();
FT5336_StatusTypeDef FT5336_GetCapabilities(FT5336_CapabilitiesTypeDef *Capabilities);

#endif /* FT5336_H */
