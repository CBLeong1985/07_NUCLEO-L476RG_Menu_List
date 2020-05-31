/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32l4xx_hal.h"

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
#define TIMER_PRESCALER 7999
#define TIMER_COUNTER 9
#define LCD_TIMER_PRESCALER 1
#define LCD_TIMER_COUNTER 3
#define LCD_DB4_Pin GPIO_PIN_0
#define LCD_DB4_GPIO_Port GPIOH
#define LCD_DB5_Pin GPIO_PIN_1
#define LCD_DB5_GPIO_Port GPIOH
#define LCD_RS_Pin GPIO_PIN_0
#define LCD_RS_GPIO_Port GPIOC
#define LCD_RW_Pin GPIO_PIN_1
#define LCD_RW_GPIO_Port GPIOC
#define LCD_DB6_Pin GPIO_PIN_2
#define LCD_DB6_GPIO_Port GPIOC
#define LCD_DB7_Pin GPIO_PIN_3
#define LCD_DB7_GPIO_Port GPIOC
#define LCD_DB2_Pin GPIO_PIN_0
#define LCD_DB2_GPIO_Port GPIOA
#define LCD_DB1_Pin GPIO_PIN_1
#define LCD_DB1_GPIO_Port GPIOA
#define LCD_DB0_Pin GPIO_PIN_4
#define LCD_DB0_GPIO_Port GPIOA
#define MATRIX_BUTTON_COLUMN_4_Pin GPIO_PIN_5
#define MATRIX_BUTTON_COLUMN_4_GPIO_Port GPIOA
#define MATRIX_BUTTON_COLUMN_3_Pin GPIO_PIN_6
#define MATRIX_BUTTON_COLUMN_3_GPIO_Port GPIOA
#define MATRIX_BUTTON_COLUMN_2_Pin GPIO_PIN_7
#define MATRIX_BUTTON_COLUMN_2_GPIO_Port GPIOA
#define LCD_E_Pin GPIO_PIN_0
#define LCD_E_GPIO_Port GPIOB
#define MATRIX_BUTTON_ROW_1_Pin GPIO_PIN_10
#define MATRIX_BUTTON_ROW_1_GPIO_Port GPIOB
#define MATRIX_BUTTON_ROW_1_EXTI_IRQn EXTI15_10_IRQn
#define MATRIX_BUTTON_ROW_4_Pin GPIO_PIN_7
#define MATRIX_BUTTON_ROW_4_GPIO_Port GPIOC
#define MATRIX_BUTTON_ROW_4_EXTI_IRQn EXTI9_5_IRQn
#define MATRIX_BUTTON_ROW_2_Pin GPIO_PIN_8
#define MATRIX_BUTTON_ROW_2_GPIO_Port GPIOA
#define MATRIX_BUTTON_ROW_2_EXTI_IRQn EXTI9_5_IRQn
#define MATRIX_BUTTON_ROW_3_Pin GPIO_PIN_9
#define MATRIX_BUTTON_ROW_3_GPIO_Port GPIOA
#define MATRIX_BUTTON_ROW_3_EXTI_IRQn EXTI9_5_IRQn
#define MATRIX_BUTTON_COLUMN_1_Pin GPIO_PIN_6
#define MATRIX_BUTTON_COLUMN_1_GPIO_Port GPIOB
#define LCD_DB3_Pin GPIO_PIN_7
#define LCD_DB3_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
