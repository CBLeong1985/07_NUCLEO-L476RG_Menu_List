/*******************************************************************************
 * Filename:			lcd.h
 * Revised:				Date: 2020.04.27
 * Revision:			V001
 * Description:		    1602 LCD function
*******************************************************************************/

#ifndef _LCD_H_
#define _LCD_H_

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
extern TIM_HandleTypeDef htim7;

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
#define LCD_TIMER_HANDLE		htim7
#define LCD_MAX_LINE			2
#define LCD_MAX_LENGTH			40
#define LCD_MAX_DISPLAY_LENGTH	16
#define BUSY_FLAG_DELAY			10

/*******************************************************************************
 * ENUMERATE
 ******************************************************************************/
// Lcd attribute define
typedef enum
{
    _4_BIT_BUS = 0,
    _8_BIT_BUS,
    _1_LINES,
    _2_LINES,
    NORMAL_FONT,
    TALL_FONT,
    DISPLAY_OFF,
    DISPLAY_ON,
    CURSOR_OFF,
    CURSOR_ON,
    CURSOR_BLINK_OFF,
    CURSOR_BLINK_ON,
    CURSOR_MOVE_LEFT,
    CURSOR_MOVE_RIGHT,
    NO_SHIFT_DISPLAY,
    SHIFT_DISPLAY,
}
eLCD_ATTRIBUTE;

// Lcd shift define
typedef enum
{
	SHIFT_CURSOR_LEFT = 0,
	SHIFT_CURSOR_RIGHT,
	SHIFT_DISPLAY_LEFT,
	SHIFT_DISPLAY_RIGHT,
}
eLCD_SHIFT;

// Lcd align define
typedef enum
{
	LCD_ALIGN_LEFT = 0,
	LCD_ALIGN_CENTER,
	LCD_ALIGN_RIGHT,
}
eLCD_ALIGN;

// Lcd logo define
typedef enum
{
    CELSIUS_DEGREE_LOGO = 0,
    BATTERY_LOGO,
    SIGNAL_LOGO,
    BULETOOTH_LOGO,
    SPEAKER_LOGO,
    CHARGING_LOGO,
    PLUG_LOGO,
    USB_LOGO,
}
eLCD_LOGO;

/*******************************************************************************
 * STRUCTURE
 ******************************************************************************/

// Define lcd function structure
typedef struct _sLCD
{
	bool (*Initialize)(void);
	bool (*SetAttribute)(uint8_t noOfAttribute, ...);
	bool (*ClearDisplay)(void);
	bool (*ReturnHome)(void);
	bool (*GoTo)(uint8_t line, uint8_t position);
	bool (*WriteString)(uint8_t line, uint8_t position, char* data, eLCD_ALIGN eLcdAlign);
	bool (*WriteCharacter)(uint8_t data);
	bool (*WriteCharacterTo)(uint8_t line, uint8_t position, uint8_t data);
	bool (*ShiftCursorDisplay)(eLCD_SHIFT eLcdShift);
}
sLCD;

/*******************************************************************************
 * PUBLIC VARIABLES
 ******************************************************************************/
extern sLCD sLcd;

/*******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 * INTERRUPT CALLBACK
 ******************************************************************************/
/*******************************************************************************
 * @fn      LcdTimerInterruptCallback
 * @brief   Lcd timer interrupt callback
 * @param	None
 * @return	None
 ******************************************************************************/
void LcdTimerInterruptCallback(void);

#ifdef __cplusplus
}
#endif

#endif /* _LCD_H_ */
