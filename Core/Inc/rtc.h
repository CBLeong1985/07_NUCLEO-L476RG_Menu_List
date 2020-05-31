/**
  ******************************************************************************
  * File Name          : RTC.h
  * Description        : This file provides code for the configuration
  *                      of the RTC instances.
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __rtc_H
#define __rtc_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "common.h"

/* USER CODE END Includes */

extern RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN Private defines */
#define RTC_SETTING_BACKUP_ADDR		RTC_BKP_DR0
#define RTC_SETTING_BACKUP			0x55555555

/* USER CODE END Private defines */

void MX_RTC_Init(void);

/* USER CODE BEGIN Prototypes */
/*******************************************************************************
 * @fn      RtcSetDateTime
 * @brief   Set RTC date & time
 * @paramz  None
 * @return  None
 ******************************************************************************/
void RtcSetDateTime(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime);

/*******************************************************************************
 * @fn      RtcSetUnixTime
 * @brief   Set RTC date & time from unix time
 * @paramz  unixTime
 * @return  None
 ******************************************************************************/
void RtcSetUnixTime(time_t* unixTime);

/*******************************************************************************
 * @fn      RtcGetDateTime
 * @brief   Get RTC date & time
 * @paramz  sDate(format is FORMAT_BIN)
 * 			sTime(format is FORMAT_BIN)
 * @return  None
 ******************************************************************************/
void RtcGetDateTime(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime);

/*******************************************************************************
 * @fn      RtcGetUnixTime
 * @brief   Get RTC unix time
 * @paramz  None
 * @return  None
 ******************************************************************************/
void RTCGetUnixTime(time_t *unixTime);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ rtc_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
