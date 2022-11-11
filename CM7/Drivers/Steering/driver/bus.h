#ifndef BUS_H
#define BUS_H

#include "driver_conf.h"
#include "errno.h"

/* Definition for I2C4 clock resources */
#define BUS_I2C4                              I2C4
#define BUS_I2C4_CLK_ENABLE()                  __HAL_RCC_I2C4_CLK_ENABLE()
#define BUS_I2C4_CLK_DISABLE()                 __HAL_RCC_I2C4_CLK_DISABLE()
#define BUS_I2C4_SCL_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOD_CLK_ENABLE()
#define BUS_I2C4_SCL_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOD_CLK_DISABLE()
#define BUS_I2C4_SDA_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOD_CLK_ENABLE()
#define BUS_I2C4_SDA_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOD_CLK_DISABLE()

#define BUS_I2C4_FORCE_RESET()                 __HAL_RCC_I2C4_FORCE_RESET()
#define BUS_I2C4_RELEASE_RESET()               __HAL_RCC_I2C4_RELEASE_RESET()

/* Definition for I2C4 Pins */
#define BUS_I2C4_SCL_PIN                       GPIO_PIN_12
#define BUS_I2C4_SDA_PIN                       GPIO_PIN_13
#define BUS_I2C4_SCL_GPIO_PORT                 GPIOD
#define BUS_I2C4_SDA_GPIO_PORT                 GPIOD
#define BUS_I2C4_SCL_AF                        GPIO_AF4_I2C4
#define BUS_I2C4_SDA_AF                        GPIO_AF4_I2C4

#ifndef BUS_I2C4_FREQUENCY
#define BUS_I2C4_FREQUENCY  100000U /* Frequency of I2Cn = 100 KHz*/
#endif

extern I2C_HandleTypeDef hbus_i2c4;

int32_t BSP_I2C4_Init(void);
int32_t BSP_I2C4_DeInit(void);
int32_t BSP_I2C4_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData,
		uint16_t Length);
int32_t BSP_I2C4_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData,
		uint16_t Length);
int32_t BSP_I2C4_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData,
		uint16_t Length);
int32_t BSP_I2C4_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData,
		uint16_t Length);
int32_t BSP_I2C4_Recv(uint16_t DevAddr, uint16_t Reg, uint16_t MemAddSize,
		uint8_t *pData, uint16_t Length);
int32_t BSP_I2C4_Send(uint16_t DevAddr, uint16_t Reg, uint16_t MemAddSize,
		uint8_t *pData, uint16_t Length);
int32_t BSP_I2C4_IsReady(uint16_t DevAddr, uint32_t Trials);
int32_t BSP_GetTick(void);

#endif /* BUS_H */
