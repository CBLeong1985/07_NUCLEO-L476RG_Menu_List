/**
  ******************************************************************************
  * File Name          : RTC.c
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

/* Includes ------------------------------------------------------------------*/
#include "rtc.h"

/* USER CODE BEGIN 0 */

static void RtcConfig(void);

/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{
  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /** Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
	if(HAL_RTCEx_BKUPRead(&hrtc, RTC_SETTING_BACKUP_ADDR) != RTC_SETTING_BACKUP)
	{
		// Fiist time power on, set the RTC time and config
		RtcConfig();
	}
	return;
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date 
  */
  sTime.Hours = 23;
  sTime.Minutes = 30;
  sTime.Seconds = 30;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_TUESDAY;
  sDate.Month = RTC_MONTH_MAY;
  sDate.Date = 26;
  sDate.Year = 0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();

    /* RTC interrupt Init */
    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();

    /* RTC interrupt Deinit */
    HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
/*******************************************************************************
 * @fn      RtcConfig
 * @brief   Initial the RTC configuration, set 2001-01-01 00:00:00
 * @paramz  None
 * @return  None
 ******************************************************************************/
static void RtcConfig(void)
{
	RTC_DateTypeDef sData;
	RTC_TimeTypeDef sTime;

	// Configure the Date
	// Set Date: Monday January 1st 2001
	sData.Year = 0x01;
	sData.Month = RTC_MONTH_JANUARY;
	sData.Date = 0x01;
	sData.WeekDay = RTC_WEEKDAY_MONDAY;

	// Configure the Time
	// Set Time: 00:00:00
	sTime.Hours = 0x00;
	sTime.Minutes = 0x00;
	sTime.Seconds = 0x00;
	sTime.TimeFormat = RTC_HOURFORMAT12_AM;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;

	RtcSetDateTime(&sData, &sTime);

	// Writes a data in a RTC Backup data Register0
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_SETTING_BACKUP_ADDR, RTC_SETTING_BACKUP);
}

/*******************************************************************************
 * @fn      RtcSetDateTime
 * @brief   Set RTC date & time
 * @paramz  sDate
 * 			sTime
 * @return  None
 ******************************************************************************/
void RtcSetDateTime(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime)
{
	sTime->DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime->StoreOperation = RTC_STOREOPERATION_RESET;
	if(sTime->Hours <= 11)
	{
		sTime->TimeFormat = RTC_HOURFORMAT12_AM;
	}
	else if(sTime->Hours <= 23)
	{
		sTime->TimeFormat = RTC_HOURFORMAT12_PM;
	}
	// Set the RTC current Time
	if(HAL_RTC_SetTime(&hrtc, sTime, FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}
	// Set the RTC current Date
	if(HAL_RTC_SetDate(&hrtc, sDate, FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}
}

/*******************************************************************************
 * @fn      RtcSetUnixTime
 * @brief   Set RTC date & time from unix time
 * @paramz  unixTime
 * @return  None
 ******************************************************************************/
void RtcSetUnixTime(time_t* unixTime)
{
	struct tm *psTime;
	RTC_DateTypeDef sDate;
	RTC_TimeTypeDef sTime;

	// Unix time to normai time
	psTime = localtime(unixTime);

	// Can't set unix time before year of 2000
	if(psTime->tm_year < 100)
	{
		Error_Handler();
	}
	// Can't set unix time after year of 2100
	if(psTime->tm_year >= 200)
	{
		Error_Handler();
	}
	sDate.Year = psTime->tm_year + 1900 - 2000;
	sDate.Month = psTime->tm_mon + 1;
	sDate.Date = psTime->tm_mday;
	if(psTime->tm_wday == 0)
	{
		sDate.WeekDay = RTC_WEEKDAY_SUNDAY;
	}
	else
	{
		sDate.WeekDay = psTime->tm_wday;
	}
	sTime.Hours = psTime->tm_hour;
	sTime.Minutes = psTime->tm_min;
	sTime.Seconds = psTime->tm_sec;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;

	RtcSetDateTime(&sDate, &sTime);
}

/*******************************************************************************
 * @fn      RtcGetDateTime
 * @brief   Get RTC date & time
 * @paramz  sDate(format is FORMAT_BIN)
 * 			sTime(format is FORMAT_BIN)
 * @return  None
 ******************************************************************************/
void RtcGetDateTime(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime)
{
	// Get the RTC current Time
	if(HAL_RTC_GetTime(&hrtc, sTime, FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}
	// Get the RTC current Date
	if(HAL_RTC_GetDate(&hrtc, sDate, FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}
}

/*******************************************************************************
 * @fn      RtcGetUnixTime
 * @brief   Get RTC unix time
 * @paramz  None
 * @return  None
 ******************************************************************************/
void RTCGetUnixTime(time_t *unixTime)
{
	RTC_DateTypeDef sDate;
	RTC_TimeTypeDef sTime;
	struct tm s_Time;

	RtcGetDateTime(&sDate, &sTime);
	s_Time.tm_year = 2000 + sDate.Year - 1900;
	s_Time.tm_mon = sDate.Month - 1;
	s_Time.tm_mday = sDate.Date;
	s_Time.tm_hour = sTime.Hours;
	s_Time.tm_min = sTime.Minutes;
	s_Time.tm_sec = sTime.Seconds;
	s_Time.tm_isdst = 0;
	// Normal time to unix time
	*unixTime = mktime(&s_Time);
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
