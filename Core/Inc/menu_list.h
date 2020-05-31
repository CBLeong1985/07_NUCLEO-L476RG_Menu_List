/*******************************************************************************
 * Filename:			menu_list.h
 * Revised:				Date: 2020.05.23
 * Revision:			V001
 * Description:		    Menu list function
*******************************************************************************/

#ifndef _MENU_LIST_H_
#define _MENU_LIST_H_

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

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
#define TITLE_MAX_LENGTH				17
#define NUM_OF_MENU_LIST				50
#define INDICATE_CURRENT_MENU_SYMBOL	'~'
#define HIDDEN_SYMBOL					'*'
#define ALPHABET_BUTTON_DELAY			1500
#define CURSOR_ON_TYPE					CURSOR_ON
#define CURSOR_OFF_TYPE					CURSOR_OFF

/*******************************************************************************
 * STRUCTURE
 ******************************************************************************/
// Define menu list function structure
typedef struct _sMENU_LIST
{
	void (*Initialize)(void);
	void (*ButtonPressed)(uint32_t pressedButton);
	void (*UpdateDateTime)(void);
}
sMENU_LIST;

/*******************************************************************************
 * PUBLIC VARIABLES
 ******************************************************************************/
extern sMENU_LIST sMenuList;

/*******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _MENU_LIST_H_ */
