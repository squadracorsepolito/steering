/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_RTD_Pin GPIO_PIN_6
#define LED_RTD_GPIO_Port GPIOB
#define OSC32_IN_Pin GPIO_PIN_14
#define OSC32_IN_GPIO_Port GPIOC
#define BUZZER_EXTERNAL_Pin GPIO_PIN_7
#define BUZZER_EXTERNAL_GPIO_Port GPIOB
#define LCD_DISPLAY_MODE_Pin GPIO_PIN_7
#define LCD_DISPLAY_MODE_GPIO_Port GPIOD
#define SPARE_CTRL_CMD_2_Pin GPIO_PIN_8
#define SPARE_CTRL_CMD_2_GPIO_Port GPIOB
#define A_Pin GPIO_PIN_13
#define A_GPIO_Port GPIOC
#define PWM_RAD_FAN_Pin GPIO_PIN_8
#define PWM_RAD_FAN_GPIO_Port GPIOC
#define PWM_PUMP_Pin GPIO_PIN_9
#define PWM_PUMP_GPIO_Port GPIOC
#define TO_MCU_BP_SPEED_Pin GPIO_PIN_8
#define TO_MCU_BP_SPEED_GPIO_Port GPIOA
#define PWM_BP_FAN_Pin GPIO_PIN_7
#define PWM_BP_FAN_GPIO_Port GPIOC
#define BUTTON_RTD_Pin GPIO_PIN_13
#define BUTTON_RTD_GPIO_Port GPIOI
#define LCD_INT_Pin GPIO_PIN_2
#define LCD_INT_GPIO_Port GPIOG
#define LCD_INT_EXTI_IRQn EXTI2_IRQn
#define OSC_IN_Pin GPIO_PIN_0
#define OSC_IN_GPIO_Port GPIOH
#define LCD_BL_CTRL_Pin GPIO_PIN_0
#define LCD_BL_CTRL_GPIO_Port GPIOK
#define NTC_1_Pin GPIO_PIN_0
#define NTC_1_GPIO_Port GPIOC
#define NTC_2_Pin GPIO_PIN_1
#define NTC_2_GPIO_Port GPIOC
#define TO_MCU_RAD_SPEED_Pin GPIO_PIN_0
#define TO_MCU_RAD_SPEED_GPIO_Port GPIOA
#define VCP_TX_Pin GPIO_PIN_10
#define VCP_TX_GPIO_Port GPIOB
#define VCP_RX_Pin GPIO_PIN_11
#define VCP_RX_GPIO_Port GPIOB
#define VOLTAGE_SENSE_3V3_Pin GPIO_PIN_6
#define VOLTAGE_SENSE_3V3_GPIO_Port GPIOA
#define MANETTINO_1_Pin GPIO_PIN_0
#define MANETTINO_1_GPIO_Port GPIOA
#define MANETTINO_2_Pin GPIO_PIN_1
#define MANETTINO_2_GPIO_Port GPIOA
#define VOLTAGE_SENS_5V_Pin GPIO_PIN_4
#define VOLTAGE_SENS_5V_GPIO_Port GPIOC
#define VOLTAGE_SENSE_24V_Pin GPIO_PIN_1
#define VOLTAGE_SENSE_24V_GPIO_Port GPIOB
#define LED_BMS_Pin GPIO_PIN_2
#define LED_BMS_GPIO_Port GPIOJ
#define LCD_RESET_Pin GPIO_PIN_12
#define LCD_RESET_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
