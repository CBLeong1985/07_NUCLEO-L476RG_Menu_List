/*******************************************************************************
 * Filename:            matrix_button.c
 * Revised:             Date: 2020.04.16
 * Revision:            V001
 * Description:         Matrix button function
*******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "matrix_button.h"
#include "gpio.h"
#include "software_timer.h"

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
    uint8_t debounceTimerId[NUM_OF_MATRIX_BUTTON_ROW];
    sMATRIX_BUTTON_PIN sMatrixButtonColumnPin[NUM_OF_MATRIX_BUTTON_COLUMN];
    sMATRIX_BUTTON_PIN sMatrixButtonRowPin[NUM_OF_MATRIX_BUTTON_ROW];
    MATRIX_BUTTON_CALLBACK matrixButtonCallback;
    uint32_t buttonPattern;
}
sMATRIX_BUTTON_PRO;
static sMATRIX_BUTTON_PRO sMatrixButtonPro;

/*******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************/
void CheckPressedButton(uint8_t row);

/*******************************************************************************
 * @fn      CheckPressedButton
 * @brief   Check pressed button
 * @paramz  row
 * @return  None
 ******************************************************************************/
void CheckPressedButton(uint8_t row)
{
	uint8_t i = 0;

	// Set all column to high level
	for(i = 0; i < NUM_OF_MATRIX_BUTTON_COLUMN; i++)
	{
		HAL_GPIO_WritePin(sMatrixButtonPro.sMatrixButtonColumnPin[i].gpio, sMatrixButtonPro.sMatrixButtonColumnPin[i].pin, GPIO_PIN_SET);
	}
	// Waiting row pin go back to high level
	while(HAL_GPIO_ReadPin(sMatrixButtonPro.sMatrixButtonRowPin[row].gpio, sMatrixButtonPro.sMatrixButtonRowPin[row].pin) == GPIO_PIN_RESET)
	{
	}

	for(i = 0; i < NUM_OF_MATRIX_BUTTON_COLUMN; i++)
	{
		HAL_GPIO_WritePin(sMatrixButtonPro.sMatrixButtonColumnPin[i].gpio, sMatrixButtonPro.sMatrixButtonColumnPin[i].pin, GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(sMatrixButtonPro.sMatrixButtonRowPin[row].gpio, sMatrixButtonPro.sMatrixButtonRowPin[row].pin) == GPIO_PIN_RESET)
		{
			sMatrixButtonPro.buttonPattern |= (0x01 << ((NUM_OF_MATRIX_BUTTON_COLUMN * row) + i));
		}
		HAL_GPIO_WritePin(sMatrixButtonPro.sMatrixButtonColumnPin[i].gpio, sMatrixButtonPro.sMatrixButtonColumnPin[i].pin, GPIO_PIN_SET);
		// Waiting row pin go back to high level
		while(HAL_GPIO_ReadPin(sMatrixButtonPro.sMatrixButtonRowPin[row].gpio, sMatrixButtonPro.sMatrixButtonRowPin[row].pin) == GPIO_PIN_RESET)
		{
		}
	}

	// Set all column to low level
	for(i = 0; i < NUM_OF_MATRIX_BUTTON_COLUMN; i++)
	{
		HAL_GPIO_WritePin(sMatrixButtonPro.sMatrixButtonColumnPin[i].gpio, sMatrixButtonPro.sMatrixButtonColumnPin[i].pin, GPIO_PIN_RESET);
	}

	// Clear interrupt flag to avoid go in external interrupt again
	for(i = 0; i < NUM_OF_MATRIX_BUTTON_ROW; i++)
	{
		__HAL_GPIO_EXTI_CLEAR_IT(sMatrixButtonPro.sMatrixButtonRowPin[i].pin);
	}
}

/*******************************************************************************
 * CALLBACK FUNCTIONS
 ******************************************************************************/
void DebounceTimerCallback(uint8_t softwareTimerId);

/*******************************************************************************
 * @fn      DebounceTimerCallback
 * @brief   Debounce timer callback
 * @paramz  softwareTimerId
 * @return  None
 ******************************************************************************/
void DebounceTimerCallback(uint8_t softwareTimerId)
{
    uint8_t i = 0;

    for(i = 0; i < NUM_OF_MATRIX_BUTTON_ROW; i++)
    {
        // Found the matrix button row
        if(sMatrixButtonPro.debounceTimerId[i] == softwareTimerId)
        {
    		if(HAL_GPIO_ReadPin(sMatrixButtonPro.sMatrixButtonRowPin[i].gpio, sMatrixButtonPro.sMatrixButtonRowPin[i].pin) == GPIO_PIN_RESET)
    		{
                CheckPressedButton(i);
                if(sMatrixButtonPro.matrixButtonCallback)
                {
                    sMatrixButtonPro.matrixButtonCallback();
                }
    		}
            break;
        }
    }
}

/*******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************/
static void MatrixButtonInitialize(MATRIX_BUTTON_CALLBACK matrixButtonCallback, ...);
static uint32_t MatrixButtonGetPressedButton(void);

/*******************************************************************************
 * @fn      MatrixButtonInitialize
 * @brief   Matrix button initialize
 * @param   matrixButtonCallback
 * 			MATRIX_BUTTON_COLUMN_1_GPIO_Port
 * 			MATRIX_BUTTON_COLUMN_1_Pin
 * 			MATRIX_BUTTON_COLUMN_2_GPIO_Port
 * 			MATRIX_BUTTON_COLUMN_2_Pin
 * 			*
 * 			*
 * 			*
 * 			MATRIX_BUTTON_ROW_1_GPIO_Port
 * 			MATRIX_BUTTON_ROW_1_Pin
 * 			MATRIX_BUTTON_ROW_2_GPIO_Port
 * 			MATRIX_BUTTON_ROW_2_Pin
 * 			*
 * 			*
 * 			*
 * @return  None
 ******************************************************************************/
static void MatrixButtonInitialize(MATRIX_BUTTON_CALLBACK matrixButtonCallback, ...)
{
    uint8_t i = 0;
    va_list argumentPointer;

    sMatrixButtonPro.buttonPattern = 0;

    va_start(argumentPointer, matrixButtonCallback);
    for(i = 0; i < NUM_OF_MATRIX_BUTTON_COLUMN; i++)
    {
        sMatrixButtonPro.sMatrixButtonColumnPin[i].gpio = va_arg(argumentPointer, GPIO_TypeDef*);
        sMatrixButtonPro.sMatrixButtonColumnPin[i].pin = va_arg(argumentPointer, uint32_t);
    }

    for(i = 0; i < NUM_OF_MATRIX_BUTTON_ROW; i++)
    {
        sMatrixButtonPro.sMatrixButtonRowPin[i].gpio = va_arg(argumentPointer, GPIO_TypeDef*);
        sMatrixButtonPro.sMatrixButtonRowPin[i].pin = va_arg(argumentPointer, uint32_t);
        sMatrixButtonPro.debounceTimerId[i] = sSoftwareTimer.Initialize(NULL, DebounceTimerCallback, NULL, TIMER_ONCE_TYPE);
    }
    sMatrixButtonPro.matrixButtonCallback = matrixButtonCallback;

    va_end(argumentPointer);
}

/*******************************************************************************
 * @fn      MatrixButtonGetPressedButton
 * @brief   Matrix button get press button
 * @param   None
 * @return  buttonPattern
 ******************************************************************************/
//0b00000000
//  *    ***
//  *    ***
//  *    ***
//  *    **1st button
//  *    *2nd button
//  *    3rd button
//  *
//  8th button
static uint32_t MatrixButtonGetPressedButton(void)
{
	uint32_t buttonPattern = sMatrixButtonPro.buttonPattern;

	sMatrixButtonPro.buttonPattern = 0;
	return buttonPattern;
}

/*******************************************************************************
 * @fn      MatrixButtonStartDebounce
 * @brief   Matrix button start debounce
 * @param   row
 * @return  None
 ******************************************************************************/
static void MatrixButtonStartDebounce(uint8_t row)
{
	sSoftwareTimer.Start(sMatrixButtonPro.debounceTimerId[row], DEBOUNCE_DELAY);
}

// MAtrix button function structure
sMATRIX_BUTTON sMatrixButton =
{
	MatrixButtonInitialize,
	MatrixButtonGetPressedButton,
	MatrixButtonStartDebounce,
};
