/*******************************************************************************
 * Filename:			software_timer.c
 * Revised:				Date: 2020.02.19
 * Revision:			V001
 * Description:		    Software timer function
*******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "software_timer.h"
#include "gpio.h"

/*******************************************************************************
 * PUBLIC VARIABLES
 ******************************************************************************/

/*******************************************************************************
 * LOCAL VARIBLES
 ******************************************************************************/

/*******************************************************************************
 * STRUCTURE
 ******************************************************************************/
// Define software timer property structure
typedef struct
{
    uint8_t usedTimer;
    eTIMER_TYPE eTimerType[NUM_OF_SOFTWARE_TIMER];
    volatile uint32_t period[NUM_OF_SOFTWARE_TIMER];
    volatile uint32_t countdown[NUM_OF_SOFTWARE_TIMER];
    SOFTWARE_TIMER_CALLBACK softwareTimerStartCallback[NUM_OF_SOFTWARE_TIMER];
    SOFTWARE_TIMER_CALLBACK softwareTimerCallback[NUM_OF_SOFTWARE_TIMER];
    SOFTWARE_TIMER_CALLBACK softwareTimerStopCallback[NUM_OF_SOFTWARE_TIMER];
}
sSOFTWARE_TIMER_PRO;
static sSOFTWARE_TIMER_PRO sSoftwareTimerPro;

/*******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************/
static bool SoftwareTimerEnable(void);
static bool SoftwareTimerDisable(void);
static uint8_t SoftwareTimerInitialize(SOFTWARE_TIMER_CALLBACK softwareTimerStartCallback, SOFTWARE_TIMER_CALLBACK softwareTimerCallback, SOFTWARE_TIMER_CALLBACK softwareTimerStopCallback, eTIMER_TYPE eTimerType);
static void SoftwareTimerStart(uint8_t softwareTimerId, uint32_t countdown);
static void SoftwareTimerStop(uint8_t softwareTimerId);

/*******************************************************************************
 * @fn      SoftwareTimerEnable
 * @brief   Software timer enable
 * @param	None
 * @return	true
 *			false
 ******************************************************************************/
static bool SoftwareTimerEnable(void)
{
	__HAL_TIM_CLEAR_IT(&SOFTWARE_TIMER_HANDLE, TIM_IT_UPDATE);
	if(HAL_TIM_Base_Start_IT(&SOFTWARE_TIMER_HANDLE) != HAL_OK)
	{
    	for(;;)
    	{
    	}
	}
	return true;
}

/*******************************************************************************
 * @fn      SoftwareTimerDisable
 * @brief   Software timer disable
 * @param	None
 * @return	true
 *			false
 ******************************************************************************/
static bool SoftwareTimerDisable(void)
{
	if(HAL_TIM_Base_Stop_IT(&SOFTWARE_TIMER_HANDLE) != HAL_OK)
	{
    	for(;;)
    	{
    	}
	}
	return true;
}

/*******************************************************************************
 * @fn      SoftwareTimerInitialize
 * @brief   Software timer initialize
 * @param   softwareTimerStartCallback
 *			softwareTimerCallback
 *			softwareTimerStopCallback
 *          eTimerType
 * @return  Software timer ID
 ******************************************************************************/
static uint8_t SoftwareTimerInitialize(SOFTWARE_TIMER_CALLBACK softwareTimerStartCallback, SOFTWARE_TIMER_CALLBACK softwareTimerCallback, SOFTWARE_TIMER_CALLBACK softwareTimerStopCallback, eTIMER_TYPE eTimerType)
{
    if(sSoftwareTimerPro.usedTimer == NUM_OF_SOFTWARE_TIMER)
    {
    	// Increase "NUM_OF_SOFTWARE_TIMER"
    	for(;;)
    	{
    	}
    }
    sSoftwareTimerPro.countdown[sSoftwareTimerPro.usedTimer] = 0;
    sSoftwareTimerPro.eTimerType[sSoftwareTimerPro.usedTimer] = eTimerType;
    sSoftwareTimerPro.softwareTimerStartCallback[sSoftwareTimerPro.usedTimer] = softwareTimerStartCallback;
    sSoftwareTimerPro.softwareTimerCallback[sSoftwareTimerPro.usedTimer] = softwareTimerCallback;
    sSoftwareTimerPro.softwareTimerStopCallback[sSoftwareTimerPro.usedTimer] = softwareTimerStopCallback;
    sSoftwareTimerPro.usedTimer++;
    return sSoftwareTimerPro.usedTimer - 1;
}

/*******************************************************************************
 * @fn      SoftwareTimerStart
 * @brief   Software timer start
 * @param   softwareTimerId
 *          period
 * @return  None
 ******************************************************************************/
static void SoftwareTimerStart(uint8_t softwareTimerId, uint32_t period)
{
	if(period > 0)
	{
		if(sSoftwareTimerPro.softwareTimerStartCallback[softwareTimerId])
		{
			sSoftwareTimerPro.softwareTimerStartCallback[softwareTimerId](softwareTimerId);
		}
		sSoftwareTimerPro.period[softwareTimerId] = period;
		sSoftwareTimerPro.countdown[softwareTimerId] = period;
	}
}

/*******************************************************************************
 * @fn      SoftwareTimerStop
 * @brief   Software timer stop
 * @param   softwareTimerId
 * @return  None
 ******************************************************************************/
static void SoftwareTimerStop(uint8_t softwareTimerId)
{
    sSoftwareTimerPro.period[softwareTimerId] = 0;
    sSoftwareTimerPro.countdown[softwareTimerId] = 0;
	if(sSoftwareTimerPro.softwareTimerStopCallback[softwareTimerId])
	{
		sSoftwareTimerPro.softwareTimerStopCallback[softwareTimerId](softwareTimerId);
	}
}

// Software timer function structure
sSOFTWARE_TIMER sSoftwareTimer =
{
	SoftwareTimerEnable,
	SoftwareTimerDisable,
	SoftwareTimerInitialize,
	SoftwareTimerStart,
	SoftwareTimerStop,
};

/*******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/
//// 1. Check timer is 1ms interval
//bool toggle = false;

/*******************************************************************************
 * INTERRUPT CALLBACK
 ******************************************************************************/
/*******************************************************************************
 * @fn      SoftwareTimerInterruptCallback
 * @brief   Software timer interrupt callback
 * @param	None
 * @return	None
 ******************************************************************************/
void SoftwareTimerInterruptCallback(void)
{
//	// 1. Check timer is 1ms interval
//	toggle = !toggle;
    uint8_t i = 0;

    for(i = 0; i < NUM_OF_SOFTWARE_TIMER; i++)
    {
        // Decrease countdown
        if(sSoftwareTimerPro.countdown[i] > 0)
        {
        	sSoftwareTimerPro.countdown[i]--;
            // Timeout
            if(sSoftwareTimerPro.countdown[i] == 0)
            {
                // Callback
        		if(sSoftwareTimerPro.softwareTimerCallback[i])
        		{
                	sSoftwareTimerPro.softwareTimerCallback[i](i);
        		}
                // Periodic timer
                if(sSoftwareTimerPro.eTimerType[i] == TIMER_PERIODIC_TYPE)
                {
                	sSoftwareTimerPro.countdown[i] = sSoftwareTimerPro.period[i];
                }
            }
        }
    }
}
