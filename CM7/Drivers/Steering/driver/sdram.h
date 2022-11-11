/**
  ******************************************************************************
  * @file    sdram.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the sdram.c driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SDRAM_H
#define SDRAM_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "driver_conf.h"
#include "errno.h"
#include "mt48lc4m32b2/mt48lc4m32b2.h"
/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32H745I_DISCO
  * @{
  */

/** @addtogroup SDRAM
  * @{
  */

/** @defgroup SDRAM_Exported_Types Exported Types
  * @{
  */
#if (USE_HAL_SDRAM_REGISTER_CALLBACKS == 1)
typedef struct
{
  void (* pMspInitCb)(SDRAM_HandleTypeDef *);
  void (* pMspDeInitCb)(SDRAM_HandleTypeDef *);
}BSP_SDRAM_Cb_t;
#endif /* (USE_HAL_SDRAM_REGISTER_CALLBACKS == 1) */
/**
  * @}
  */

/** @defgroup SDRAM_Exported_Constants Exported Constants
  * @{
  */
#define SDRAM_INSTANCES_NBR                1U
#define SDRAM_DEVICE_ADDR                  0xD0000000U
#define SDRAM_DEVICE_SIZE                  0x200000U

/* DMA definitions for SDRAM DMA transfer */
#define SDRAM_MDMAx_CLK_ENABLE             __HAL_RCC_MDMA_CLK_ENABLE
#define SDRAM_MDMAx_CLK_DISABLE            __HAL_RCC_MDMA_CLK_DISABLE
#define SDRAM_MDMAx_CHANNEL                MDMA_Channel0
#define SDRAM_MDMAx_IRQn                   MDMA_IRQn
#define SDRAM_MDMA_IRQHandler              MDMA_IRQHandler
/**
  * @}
  */

/** @defgroup SDRAM_Exported_Macro Exported Macro
  * @{
  */
/**
  * @}
  */
/** @addtogroup SDRAM_Exported_Variables
  * @{
  */
extern SDRAM_HandleTypeDef hsdram[SDRAM_INSTANCES_NBR];
/**
  * @}
  */

/** @addtogroup SDRAM_Exported_Functions
  * @{
  */
int32_t BSP_SDRAM_Init(uint32_t Instance);
int32_t BSP_SDRAM_DeInit(uint32_t Instance);
#if (USE_HAL_SDRAM_REGISTER_CALLBACKS == 1)
int32_t BSP_SDRAM_RegisterDefaultMspCallbacks (uint32_t Instance);
int32_t BSP_SDRAM_RegisterMspCallbacks (uint32_t Instance, BSP_SDRAM_Cb_t *CallBacks);
#endif /* (USE_HAL_SDRAM_REGISTER_CALLBACKS == 1)  */
int32_t BSP_SDRAM_SendCmd(uint32_t Instance, FMC_SDRAM_CommandTypeDef *SdramCmd);

/* These functions can be modified in case the current settings need to be
changed for specific application needs */
HAL_StatusTypeDef MX_SDRAM_BANK2_Init(SDRAM_HandleTypeDef *hsdram, uint32_t RowBitsNumber, uint32_t MemoryDataWidth);
void BSP_SDRAM_IRQHandler(uint32_t Instance);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /*SDRAM_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
