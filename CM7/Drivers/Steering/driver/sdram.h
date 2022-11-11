#ifndef SDRAM_H
#define SDRAM_H

#include "driver_conf.h"
#include "errno.h"
#include "mt48lc4m32b2/mt48lc4m32b2.h"

#define SDRAM_DEVICE_ADDR                  0xD0000000U
#define SDRAM_DEVICE_SIZE                  0x200000U

extern SDRAM_HandleTypeDef hsdram1;

int32_t BSP_SDRAM_Init();
int32_t BSP_SDRAM_DeInit();
int32_t BSP_SDRAM_SendCmd(FMC_SDRAM_CommandTypeDef *SdramCmd);

#endif /*SDRAM_H */
