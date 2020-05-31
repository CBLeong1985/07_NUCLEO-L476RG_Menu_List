/*******************************************************************************
 * Filename:			software_timer.h
 * Revised:				Date: 2020.02.19
 * Revision:			V001
 * Description:		    Software timer function
*******************************************************************************/

#ifndef _SOFTWARE_TIMER_H_
#define _SOFTWARE_TIMER_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "common.h"

/*******************************************************************************
 * EXTERNAL VARIABLES
 ******************************************************************************/
extern TIM_HandleTypeDef htim6;

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
#define SOFTWARE_TIMER_HANDLE	htim6
#define NUM_OF_SOFTWARE_TIMER	8

/*******************************************************************************
 * ENUMERATE
 ******************************************************************************/
// Timer type define
typedef enum
{
	TIMER_ONCE_TYPE			= 0,
	TIMER_PERIODIC_TYPE,
}
eTIMER_TYPE;

/*******************************************************************************
 * STRUCTURE
 ******************************************************************************/
// Software timer callback function.
typedef void (*SOFTWARE_TIMER_CALLBACK)(uint8_t softwareTimerId);

// Define software timer function structure
typedef struct _sSOFTWARE_TIMER
{
	bool (*Enable)(void);
	bool (*Disable)(void);
	uint8_t (*Initialize)(SOFTWARE_TIMER_CALLBACK softwareTimerStartCallback, SOFTWARE_TIMER_CALLBACK softwareTimerCallback, SOFTWARE_TIMER_CALLBACK softwareTimerStopCallback, eTIMER_TYPE eTimerType);
	void (*Start)(uint8_t softwareTimerId, uint32_t period);
	void (*Stop)(uint8_t softwareTimerId);
}
sSOFTWARE_TIMER;

/*******************************************************************************
 * PUBLIC VARIABLES
 ******************************************************************************/
extern sSOFTWARE_TIMER sSoftwareTimer;

/*******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 * INTERRUPT CALLBACK
 ******************************************************************************/
/*******************************************************************************
 * @fn      SoftwareTimerInterruptCallback
 * @brief   Software timer interrupt callback
 * @param	None
 * @return	None
 ******************************************************************************/
void SoftwareTimerInterruptCallback(void);

#ifdef __cplusplus
}
#endif

#endif /* _SOFTWARE_TIMER_H_ */
