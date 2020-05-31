/*******************************************************************************
 * Filename:			matrix_button.h
 * Revised:				Date: 2020.04.16
 * Revision:			V001
 * Description:		    Matrix button function
*******************************************************************************/

#ifndef _MATRIX_BUTTON_H_
#define _MATRIX_BUTTON_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "common.h"

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
#define DEBOUNCE_DELAY						50
#define NUM_OF_MATRIX_BUTTON_ROW			4
#define NUM_OF_MATRIX_BUTTON_COLUMN			4

/*******************************************************************************
 * STRUCTURE
 ******************************************************************************/
// Define matrix button structure
typedef struct
{
	GPIO_TypeDef* gpio;
	uint32_t pin;
}
sMATRIX_BUTTON_PIN;

// Matrix button callback function.
typedef void (*MATRIX_BUTTON_CALLBACK)(void);

// Define matrix button function structure
typedef struct _sMATRIX_BUTTON
{
	void (*Initialize)(MATRIX_BUTTON_CALLBACK matrixButtonCallback, ...);
	uint32_t (*GetPressedButton)(void);
	void (*StartDebounce)(uint8_t row);
}
sMATRIX_BUTTON;

/*******************************************************************************
 * PUBLIC VARIABLES
 ******************************************************************************/
extern sMATRIX_BUTTON sMatrixButton;

/*******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _MATRIX_BUTTON_H_ */
