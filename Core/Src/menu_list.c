/*******************************************************************************
 * Filename:			menu_list.c
 * Revised:				Date: 2020.05.23
 * Revision:			V001
 * Description:		    Menu List function
*******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "menu_list.h"
#include "lcd.h"
#include "software_timer.h"
#include "rtc.h"

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

/*******************************************************************************
 * PUBLIC VARIABLES
 ******************************************************************************/

/*******************************************************************************
 * LOCAL VARIBLES
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATE
 ******************************************************************************/
// Menu level define
typedef enum
{
    LEVEL1 = 0,
    LEVEL2,
    LEVEL3,
    LEVEL4,
    LEVEL5,
 }
eMENU_LEVEL;

// Menu type define
typedef enum
{
    TITLE = 0,
    NUMBER,
    ALPHABET,
    OPTION,
    INFO,
 }
eMENU_TYPE;

/*******************************************************************************
 * STRUCTURE
 ******************************************************************************/
// Define menu attribute structure
typedef union
{
    uint8_t attribute;
    struct
    {
		uint8_t eMenuType : 3;
		uint8_t isHidden : 1;
		uint8_t keyinMaxLength : 4;
    };
}
uMENU_ATTRIBUTE;

// Menu action function.
typedef void (*MENU_ACTION)(void);

// Define menu structure
struct sMENU
{
	eMENU_LEVEL eMenuLevel;
	uint8_t index;
	char title[TITLE_MAX_LENGTH];
	MENU_ACTION menuAction;
	uMENU_ATTRIBUTE uMenuAttribute;
	struct sMENU *pNext;
	struct sMENU *pPrevious;
	struct sMENU *pParent;
	struct sMENU *pChild;
};

// Define menu list property structure
typedef struct
{
	uint8_t usedMenu;
	uint8_t keyinCounter;
	uint8_t alphabetRepeatCounter;
	uint32_t previousPressedButton;
	uint8_t AlphabetButtonTimerId;
	struct sMENU *pCurrentMenu;
	struct sMENU *pOptionMenu;
	struct sMENU sMenu[NUM_OF_MENU_LIST];
	char password[7];
	bool leapYear;
	uint8_t lastDate;
	uint8_t dateTimeIndex;
	uint8_t passwordIndex;
	uint8_t firstMenuIndex;
	uint8_t yearIndex;
	uint8_t monthIndex;
	uint8_t dateIndex;
	uint8_t hourIndex;
	uint8_t minuteIndex;
	uint8_t secondIndex;
	uint8_t weekDayIndex;
	uint8_t oldPasswordIndex;
	uint8_t newPasswordIndex;
	uint8_t confirmPasswordIndex;
}
sMENU_PRO;
static sMENU_PRO sMenuPro;

/*******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************/
static uint8_t MenuListAddMenu(eMENU_LEVEL eMenuLevel, char* title, MENU_ACTION menuAction, eMENU_TYPE eMenuType, bool isHidden, uint8_t keyinMaxLength);
static void MenuListPrepareKeyin(void);
static bool MenuListCheckDataValid(uint32_t minimumData, uint32_t data, uint32_t maximumData, struct sMENU *nextCurrentMenu);
static void MenuListProcessData(void);
static void MenuListNavigationButton(uint32_t pressedButton);
static void MenuListNumberButton(uint32_t pressedButton);
static void MenuListAlphabetButton(uint32_t pressedButton);
static void MenuListOptionButton(uint32_t pressedButton);

/*******************************************************************************
 * @fn      MenuListAddMenu
 * @brief   Add menu to menu list
 * @param   eMenuLevel
 *			title
 *			menuAction
 *			eMenuType
 *			isHidden
 *			keyinMaxLength
 * @return  index
 ******************************************************************************/
static uint8_t MenuListAddMenu(eMENU_LEVEL eMenuLevel, char* title, MENU_ACTION menuAction, eMENU_TYPE eMenuType, bool isHidden, uint8_t keyinMaxLength)
{
	int8_t i = 0;
	int8_t levelDifferent = 0;
	struct sMENU *psMenu;

    if(sMenuPro.usedMenu == NUM_OF_MENU_LIST)
    {
    	// Increase "NUM_OF_MENU_LIST"
    	for(;;)
    	{
    	}
    }
	sMenuPro.sMenu[sMenuPro.usedMenu].eMenuLevel = eMenuLevel;
	sMenuPro.sMenu[sMenuPro.usedMenu].index = sMenuPro.usedMenu;
	sprintf(sMenuPro.sMenu[sMenuPro.usedMenu].title, "%s", title);
	sMenuPro.sMenu[sMenuPro.usedMenu].menuAction = menuAction;
	sMenuPro.sMenu[sMenuPro.usedMenu].uMenuAttribute.eMenuType = eMenuType;
	sMenuPro.sMenu[sMenuPro.usedMenu].uMenuAttribute.isHidden = isHidden;
	sMenuPro.sMenu[sMenuPro.usedMenu].uMenuAttribute.keyinMaxLength = keyinMaxLength;

	if(sMenuPro.usedMenu > 0)
	{
		levelDifferent = sMenuPro.sMenu[sMenuPro.usedMenu - 1].eMenuLevel - sMenuPro.sMenu[sMenuPro.usedMenu].eMenuLevel;
		switch(levelDifferent)
		{
			// Current menu is previous menu's child
			case -1:
				sMenuPro.sMenu[sMenuPro.usedMenu - 1].pChild = &sMenuPro.sMenu[sMenuPro.usedMenu];
				sMenuPro.sMenu[sMenuPro.usedMenu].pParent = &sMenuPro.sMenu[sMenuPro.usedMenu - 1];
				break;
			// Current menu is previous menu's next
			case 0:
				sMenuPro.sMenu[sMenuPro.usedMenu - 1].pNext = &sMenuPro.sMenu[sMenuPro.usedMenu];
				sMenuPro.sMenu[sMenuPro.usedMenu].pPrevious = &sMenuPro.sMenu[sMenuPro.usedMenu - 1];
				sMenuPro.sMenu[sMenuPro.usedMenu].pParent = sMenuPro.sMenu[sMenuPro.usedMenu - 1].pParent;
				break;
			default:
				if(levelDifferent < 0)
				{
					break;
				}
				psMenu = &sMenuPro.sMenu[sMenuPro.usedMenu - 1];
				for(i = 0; i < levelDifferent; i++)
				{
					psMenu = psMenu->pParent;
				}
				psMenu->pNext = &sMenuPro.sMenu[sMenuPro.usedMenu];
				sMenuPro.sMenu[sMenuPro.usedMenu].pPrevious = psMenu;
				sMenuPro.sMenu[sMenuPro.usedMenu].pParent = psMenu->pParent;
				break;
		}
	}
	sMenuPro.usedMenu++;
	return sMenuPro.usedMenu - 1;
}

/*******************************************************************************
 * @fn      MenuListPrepareKeyin
 * @brief   Prepare user key in data
 * @paramz  None
 * @return  None
 ******************************************************************************/
static void MenuListPrepareKeyin(void)
{
	sLcd.WriteString(1, 0, "                ", LCD_ALIGN_LEFT);
	sMenuPro.keyinCounter = 0;
	sMenuPro.alphabetRepeatCounter = 0;
	sMenuPro.previousPressedButton = 0;
	sLcd.GoTo(1, 0);
	sLcd.SetAttribute(1, CURSOR_ON_TYPE);
}

/*******************************************************************************
 * @fn      MenuListCheckDataValid
 * @brief   Check key in data valid or not
 * @paramz  minimumData
 * 			data
 * 			maximumData
 * 			nextCurrentMenu
 * @return  None
 ******************************************************************************/
static bool MenuListCheckDataValid(uint32_t minimumData, uint32_t data, uint32_t maximumData, struct sMENU *nextCurrentMenu)
{
	if((minimumData <= data) && (data <= maximumData))
	{
		sLcd.SetAttribute(1, CURSOR_OFF_TYPE);
		sMenuPro.pCurrentMenu = nextCurrentMenu;
		sMenuPro.pCurrentMenu->menuAction();
		return true;
	}
	else
	{
		return false;
	}
}

/*******************************************************************************
 * @fn      MenuListProcessData
 * @brief   Process user key in data
 * @paramz  None
 * @return  None
 ******************************************************************************/
static void MenuListProcessData(void)
{
	bool result = true;
	uint32_t data;
	static RTC_DateTypeDef sDate;
	static RTC_TimeTypeDef sTime;

	if(sMenuPro.pCurrentMenu->uMenuAttribute.eMenuType == NUMBER ||
	   sMenuPro.pCurrentMenu->uMenuAttribute.eMenuType == ALPHABET)
	{
		sMenuPro.pCurrentMenu->title[sMenuPro.keyinCounter] = 0;
	}
	if(sMenuPro.pCurrentMenu->index == sMenuPro.passwordIndex)
	{
		// Password correct
		if(strcmp(sMenuPro.password, sMenuPro.pCurrentMenu->title) == 0)
		{
			sLcd.SetAttribute(1, CURSOR_OFF_TYPE);
			sMenuPro.pCurrentMenu = &sMenuPro.sMenu[sMenuPro.firstMenuIndex];
			sMenuPro.pCurrentMenu->menuAction();
			return;
		}
		// Password wrong
		else
		{
			result = false;
		}
	}
	else if(sMenuPro.pCurrentMenu->index == sMenuPro.yearIndex)
	{
		if(sMenuPro.keyinCounter > 0)
		{
			sscanf(sMenuPro.pCurrentMenu->title, "%ld", &data);
			sDate.Year = data;
			if((sDate.Year % 4) != 0)
			{
				sMenuPro.leapYear = false;
			}
			else
			{
				sMenuPro.leapYear = true;
			}
			sLcd.SetAttribute(1, CURSOR_OFF_TYPE);
			sMenuPro.pCurrentMenu = sMenuPro.pCurrentMenu->pNext;
			sMenuPro.pCurrentMenu->menuAction();
			return;
		}
		else
		{
			result = false;
		}
	}
	else if(sMenuPro.pCurrentMenu->index == sMenuPro.monthIndex)
	{
		sscanf(sMenuPro.pCurrentMenu->title, "%ld", &data);
		sDate.Month = data;
		if(MenuListCheckDataValid(1, sDate.Month, 12, sMenuPro.pCurrentMenu->pNext))
		{
			switch(sDate.Month)
			{
				case 1:
				case 3:
				case 5:
				case 7:
				case 8:
				case 10:
				case 12:
					sMenuPro.lastDate = 31;
					break;
				case 4:
				case 6:
				case 9:
				case 11:
					sMenuPro.lastDate = 30;
					break;
				case 2:
					if(sMenuPro.leapYear == false)
					{
						sMenuPro.lastDate = 28;
					}
					else
					{
						sMenuPro.lastDate = 29;
					}
					break;
			}
			return;
		}
		else
		{
			result = false;
		}
	}
	else if(sMenuPro.pCurrentMenu->index == sMenuPro.dateIndex)
	{
		sscanf(sMenuPro.pCurrentMenu->title, "%ld", &data);
		sDate.Date = data;
		if(MenuListCheckDataValid(1, sDate.Date, sMenuPro.lastDate, sMenuPro.pCurrentMenu->pNext))
		{
			return;
		}
		else
		{
			result = false;
		}
	}
	else if(sMenuPro.pCurrentMenu->index == sMenuPro.hourIndex)
	{
		sscanf(sMenuPro.pCurrentMenu->title, "%ld", &data);
		sTime.Hours = data;
		if(MenuListCheckDataValid(0, sTime.Hours, 23, sMenuPro.pCurrentMenu->pNext))
		{
			return;
		}
		else
		{
			result = false;
		}
	}
	else if(sMenuPro.pCurrentMenu->index == sMenuPro.minuteIndex)
	{
		sscanf(sMenuPro.pCurrentMenu->title, "%ld", &data);
		sTime.Minutes = data;
		if(MenuListCheckDataValid(0, sTime.Minutes, 59, sMenuPro.pCurrentMenu->pNext))
		{
			return;
		}
		else
		{
			result = false;
		}
	}
	else if(sMenuPro.pCurrentMenu->index == sMenuPro.secondIndex)
	{
		sscanf(sMenuPro.pCurrentMenu->title, "%ld", &data);
		sTime.Seconds = data;
		if(MenuListCheckDataValid(0, sTime.Seconds, 59, sMenuPro.pCurrentMenu->pNext))
		{
			return;
		}
		else
		{
			result = false;
		}
	}
	else if(sMenuPro.pCurrentMenu->index == sMenuPro.weekDayIndex)
	{
		if(strcmp(sMenuPro.pCurrentMenu->title, "Monday") == 0)
		{
			sDate.WeekDay = RTC_WEEKDAY_MONDAY;
		}
		else if(strcmp(sMenuPro.pCurrentMenu->title, "Tuesday") == 0)
		{
			sDate.WeekDay = RTC_WEEKDAY_TUESDAY;
		}
		else if(strcmp(sMenuPro.pCurrentMenu->title, "Wednesday") == 0)
		{
			sDate.WeekDay = RTC_WEEKDAY_WEDNESDAY;
		}
		else if(strcmp(sMenuPro.pCurrentMenu->title, "Thursday") == 0)
		{
			sDate.WeekDay = RTC_WEEKDAY_THURSDAY;
		}
		else if(strcmp(sMenuPro.pCurrentMenu->title, "Friday") == 0)
		{
			sDate.WeekDay = RTC_WEEKDAY_FRIDAY;
		}
		else if(strcmp(sMenuPro.pCurrentMenu->title, "Saturday") == 0)
		{
			sDate.WeekDay = RTC_WEEKDAY_SATURDAY;
		}
		else if(strcmp(sMenuPro.pCurrentMenu->title, "Sunday") == 0)
		{
			sDate.WeekDay = RTC_WEEKDAY_SUNDAY;
		}
		RtcSetDateTime(&sDate, &sTime);
	}
	else if(sMenuPro.pCurrentMenu->index == sMenuPro.oldPasswordIndex)
	{
		if(strcmp(sMenuPro.password, sMenuPro.pCurrentMenu->title) == 0)
		{
			MenuListCheckDataValid(0, 0, 0, sMenuPro.pCurrentMenu->pNext);
			return;
		}
		else
		{
			result = false;
		}
	}
	else if(sMenuPro.pCurrentMenu->index == sMenuPro.newPasswordIndex)
	{
		if(sMenuPro.keyinCounter == 6)
		{
			MenuListCheckDataValid(0, 0, 0, sMenuPro.pCurrentMenu->pNext);
			return;
		}
		else
		{
			return;
		}
	}
	else if(sMenuPro.pCurrentMenu->index == sMenuPro.confirmPasswordIndex)
	{
		if(strcmp(sMenuPro.pCurrentMenu->title, sMenuPro.sMenu[sMenuPro.newPasswordIndex].title) == 0)
		{
			sprintf(sMenuPro.password, "%s", sMenuPro.pCurrentMenu->title);
		}
		else
		{
			result = false;
		}
	}

	if(result)
	{
		sLcd.SetAttribute(1, CURSOR_OFF_TYPE);
		sMenuPro.pCurrentMenu = sMenuPro.pCurrentMenu->pParent;
		sMenuPro.pCurrentMenu->menuAction();
	}
	else
	{
		MenuListPrepareKeyin();
	}
}

/*******************************************************************************
 * @fn      MenuListNavigationButton
 * @brief   Menu list navigation button
 * @paramz  pressedButton
 * @return  None
 ******************************************************************************/
static void MenuListNavigationButton(uint32_t pressedButton)
{
	struct sMENU *pNewMenu = NULL;

	switch(pressedButton)
	{
		// Up
		case 0x00000200:
			if(sMenuPro.pCurrentMenu->pPrevious != NULL)
			{
				pNewMenu = sMenuPro.pCurrentMenu->pPrevious;
			}
			break;
		// Down
		case 0x00002000:
			if(sMenuPro.pCurrentMenu->pNext != NULL)
			{
				pNewMenu = sMenuPro.pCurrentMenu->pNext;
			}
			break;
		// Exit / Left
		case 0x00001000:
			if(sMenuPro.pCurrentMenu->pParent != NULL)
			{
				pNewMenu = sMenuPro.pCurrentMenu->pParent;
			}
			break;
		// Enter / Right
		case 0x00004000:
			if(sMenuPro.pCurrentMenu->pChild != NULL)
			{
				pNewMenu = sMenuPro.pCurrentMenu->pChild;
			}
			break;
		default:
			break;
	}
	if(pNewMenu != NULL)
	{
		sMenuPro.pCurrentMenu = pNewMenu;
		sMenuPro.pCurrentMenu->menuAction();
	}
}

/*******************************************************************************
 * @fn      MenuListNumberButton
 * @brief   Menu list number button
 * @paramz  pressedButton
 * @return  None
 ******************************************************************************/
static void MenuListNumberButton(uint32_t pressedButton)
{
	char userKeyin = 0;

	switch(pressedButton)
	{
		// 1
		case 0x00000001:
			userKeyin = '1';
			break;
		// 2
		case 0x00000002:
			userKeyin = '2';
			break;
		// 3
		case 0x00000004:
			userKeyin = '3';
			break;
		// 4
		case 0x00000010:
			userKeyin = '4';
			break;
		// 5
		case 0x00000020:
			userKeyin = '5';
			break;
		// 6
		case 0x00000040:
			userKeyin = '6';
			break;
		// 7
		case 0x00000100:
			userKeyin = '7';
			break;
		// 8
		case 0x00000200:
			userKeyin = '8';
			break;
		// 9
		case 0x00000400:
			userKeyin = '9';
			break;
		// 0
		case 0x00002000:
			userKeyin = '0';
			break;
		// Exit / Left
		case 0x00001000:
			if(sMenuPro.keyinCounter > 0)
			{
				sLcd.ShiftCursorDisplay(SHIFT_CURSOR_LEFT);
				sLcd.WriteCharacter(' ');
				sLcd.ShiftCursorDisplay(SHIFT_CURSOR_LEFT);
				sMenuPro.keyinCounter--;
			}
			else
			{
				sLcd.SetAttribute(1, CURSOR_OFF_TYPE);
				sMenuPro.pCurrentMenu = sMenuPro.pCurrentMenu->pParent;
				sMenuPro.pCurrentMenu->menuAction();
				return;
			}
			break;
		// Enter / Right
		case 0x00004000:
			MenuListProcessData();
			return;
	}
	// Check maximum length
	if(sMenuPro.keyinCounter >= sMenuPro.pCurrentMenu->uMenuAttribute.keyinMaxLength)
	{
		return;
	}
	sMenuPro.pCurrentMenu->title[sMenuPro.keyinCounter] = userKeyin;
	if(userKeyin != 0)
	{
		if(!sMenuPro.pCurrentMenu->uMenuAttribute.isHidden)
		{
			sLcd.WriteCharacter(userKeyin);
		}
		else
		{
			sLcd.WriteCharacter(HIDDEN_SYMBOL);
		}
		sMenuPro.keyinCounter++;
	}
}

/*******************************************************************************
 * @fn      MenuListAlphabetButton
 * @brief   Menu list alphabet button
 * @paramz  pressedButton
 * @return  None
 ******************************************************************************/
static void MenuListAlphabetButton(uint32_t pressedButton)
{
	char userKeyin = 0;

	if(sMenuPro.previousPressedButton == pressedButton)
	{
		sMenuPro.alphabetRepeatCounter++;
	}
	else
	{
		sMenuPro.alphabetRepeatCounter = 0;
	}

	switch(pressedButton)
	{
		// 1
		case 0x00000001:
			sMenuPro.alphabetRepeatCounter %= 18;
			if(sMenuPro.alphabetRepeatCounter < 1)
			{
				userKeyin = '1';
			}
			else if(sMenuPro.alphabetRepeatCounter < 8)
			{
				userKeyin = '9' + sMenuPro.alphabetRepeatCounter;
			}
			else if(sMenuPro.alphabetRepeatCounter < 14)
			{
				userKeyin = 'S' + sMenuPro.alphabetRepeatCounter;
			}
			else if(sMenuPro.alphabetRepeatCounter < 18)
			{
				userKeyin = 'm' + sMenuPro.alphabetRepeatCounter;
			}
			break;
		// 2(abcABC2)
		case 0x00000002:
			sMenuPro.alphabetRepeatCounter %= 7;
			if(sMenuPro.alphabetRepeatCounter < 3)
			{
				userKeyin = 'a' + sMenuPro.alphabetRepeatCounter;
			}
			else if(sMenuPro.alphabetRepeatCounter < 6)
			{
				userKeyin = '>' + sMenuPro.alphabetRepeatCounter;
			}
			else
			{
				userKeyin = '2';
			}
			break;
		// 3(defDEF3)
		case 0x00000004:
			sMenuPro.alphabetRepeatCounter %= 7;
			if(sMenuPro.alphabetRepeatCounter < 3)
			{
				userKeyin = 'd' + sMenuPro.alphabetRepeatCounter;
			}
			else if(sMenuPro.alphabetRepeatCounter < 6)
			{
				userKeyin = 'A' + sMenuPro.alphabetRepeatCounter;
			}
			else
			{
				userKeyin = '3';
			}
			break;
		// 4(ghiGHI4)
		case 0x00000010:
			sMenuPro.alphabetRepeatCounter %= 7;
			if(sMenuPro.alphabetRepeatCounter < 3)
			{
				userKeyin = 'g' + sMenuPro.alphabetRepeatCounter;
			}
			else if(sMenuPro.alphabetRepeatCounter < 6)
			{
				userKeyin = 'D' + sMenuPro.alphabetRepeatCounter;
			}
			else
			{
				userKeyin = '4';
			}
			break;
		// 5(jklJKL5)
		case 0x00000020:
			sMenuPro.alphabetRepeatCounter %= 7;
			if(sMenuPro.alphabetRepeatCounter < 3)
			{
				userKeyin = 'j' + sMenuPro.alphabetRepeatCounter;
			}
			else if(sMenuPro.alphabetRepeatCounter < 6)
			{
				userKeyin = 'G' + sMenuPro.alphabetRepeatCounter;
			}
			else
			{
				userKeyin = '5';
			}
			break;
		// 6(mnoMNO6)
		case 0x00000040:
			sMenuPro.alphabetRepeatCounter %= 7;
			if(sMenuPro.alphabetRepeatCounter < 3)
			{
				userKeyin = 'm' + sMenuPro.alphabetRepeatCounter;
			}
			else if(sMenuPro.alphabetRepeatCounter < 6)
			{
				userKeyin = 'J' + sMenuPro.alphabetRepeatCounter;
			}
			else
			{
				userKeyin = '6';
			}
			break;
		// 7(pqrsPQRS7)
		case 0x00000100:
			sMenuPro.alphabetRepeatCounter %= 9;
			if(sMenuPro.alphabetRepeatCounter < 4)
			{
				userKeyin = 'p' + sMenuPro.alphabetRepeatCounter;
			}
			else if(sMenuPro.alphabetRepeatCounter < 8)
			{
				userKeyin = 'L' + sMenuPro.alphabetRepeatCounter;
			}
			else
			{
				userKeyin = '7';
			}
			break;
		// 8(tuvTUV8)
		case 0x00000200:
			sMenuPro.alphabetRepeatCounter %= 7;
			if(sMenuPro.alphabetRepeatCounter < 3)
			{
				userKeyin = 't' + sMenuPro.alphabetRepeatCounter;
			}
			else if(sMenuPro.alphabetRepeatCounter < 6)
			{
				userKeyin = 'Q' + sMenuPro.alphabetRepeatCounter;
			}
			else
			{
				userKeyin = '8';
			}
			break;
		// 9(wxyzWXYZ9)
		case 0x00000400:
			sMenuPro.alphabetRepeatCounter %= 9;
			if(sMenuPro.alphabetRepeatCounter < 4)
			{
				userKeyin = 'w' + sMenuPro.alphabetRepeatCounter;
			}
			else if(sMenuPro.alphabetRepeatCounter < 8)
			{
				userKeyin = 'S' + sMenuPro.alphabetRepeatCounter;
			}
			else
			{
				userKeyin = '9';
			}
			break;
		// 0
		case 0x00002000:
			sMenuPro.alphabetRepeatCounter %= 17;
			if(sMenuPro.alphabetRepeatCounter < 1)
			{
				userKeyin = '0';
			}
			else if(sMenuPro.alphabetRepeatCounter < 17)
			{
				userKeyin = 31 + sMenuPro.alphabetRepeatCounter;
			}
			break;
		// Exit / Left
		case 0x00001000:
			pressedButton = 0;
			if(sMenuPro.keyinCounter > 0)
			{
				if(sMenuPro.previousPressedButton == 0)
				{
					sLcd.ShiftCursorDisplay(SHIFT_CURSOR_LEFT);
				}
				sLcd.WriteCharacter(' ');
				sLcd.ShiftCursorDisplay(SHIFT_CURSOR_LEFT);
				sMenuPro.keyinCounter--;
			}
			else
			{
				sLcd.SetAttribute(1, CURSOR_OFF_TYPE);
				sMenuPro.pCurrentMenu = sMenuPro.pCurrentMenu->pParent;
				sMenuPro.pCurrentMenu->menuAction();
				return;
			}
			break;
		// Enter / Right
		case 0x00004000:
			MenuListProcessData();
			sSoftwareTimer.Stop(sMenuPro.AlphabetButtonTimerId);
			return;
	}
	// Check maximum length
	if(sMenuPro.keyinCounter >= sMenuPro.pCurrentMenu->uMenuAttribute.keyinMaxLength)
	{
		sSoftwareTimer.Stop(sMenuPro.AlphabetButtonTimerId);
		return;
	}
	if(sMenuPro.previousPressedButton == pressedButton && userKeyin != 0)
	{
		sMenuPro.keyinCounter--;
	}
	if(sMenuPro.previousPressedButton != pressedButton &&
			sMenuPro.previousPressedButton != 0 &&
			userKeyin != 0 &&
			sMenuPro.keyinCounter > 0)
	{
		sLcd.ShiftCursorDisplay(SHIFT_CURSOR_RIGHT);
	}
	sMenuPro.pCurrentMenu->title[sMenuPro.keyinCounter] = userKeyin;
	sMenuPro.previousPressedButton = pressedButton;

	if(userKeyin != 0)
	{
		sSoftwareTimer.Start(sMenuPro.AlphabetButtonTimerId, ALPHABET_BUTTON_DELAY);
		sLcd.WriteCharacter(userKeyin);
		sLcd.ShiftCursorDisplay(SHIFT_CURSOR_LEFT);
		sMenuPro.keyinCounter++;
	}
	else
	{
		sSoftwareTimer.Stop(sMenuPro.AlphabetButtonTimerId);
	}
}

/*******************************************************************************
 * @fn      MenuListOptionButton
 * @brief   Menu list option button
 * @paramz  pressedButton
 * @return  None
 ******************************************************************************/
static void MenuListOptionButton(uint32_t pressedButton)
{
	switch(pressedButton)
	{
		// Up
		case 0x00000200:
			if(sMenuPro.pOptionMenu->pPrevious != NULL)
			{
				sMenuPro.pOptionMenu = sMenuPro.pOptionMenu->pPrevious;
				sLcd.WriteString(1, 1, sMenuPro.pOptionMenu->title, LCD_ALIGN_LEFT);
			}
			break;
		// Down
		case 0x00002000:
			if(sMenuPro.pOptionMenu->pNext != NULL)
			{
				sMenuPro.pOptionMenu = sMenuPro.pOptionMenu->pNext;
				sLcd.WriteString(1, 1, sMenuPro.pOptionMenu->title, LCD_ALIGN_LEFT);
			}
			break;
		// Exit / Left
		case 0x00001000:
			if(sMenuPro.pCurrentMenu->pParent != NULL)
			{
				sMenuPro.pCurrentMenu = sMenuPro.pCurrentMenu->pParent;
				sMenuPro.pCurrentMenu->menuAction();
			}
			break;
		// Enter / Right
		case 0x00004000:
			sprintf(sMenuPro.pCurrentMenu->title, "%s", sMenuPro.pOptionMenu->title);
			MenuListProcessData();
			break;
		default:
			break;
	}
}

/*******************************************************************************
 * @fn      MenuListInfoButton
 * @brief   Menu list info button
 * @paramz  pressedButton
 * @return  None
 ******************************************************************************/
static void MenuListInfoButton(uint32_t pressedButton)
{
	// Exit / Left
	if(pressedButton == 0x00001000)
	{
		sMenuPro.pCurrentMenu = sMenuPro.pCurrentMenu->pParent;
		sMenuPro.pCurrentMenu->menuAction();
	}
}

/*******************************************************************************
 * CALLBACK FUNCTIONS
 ******************************************************************************/
static void AlphabetButtonCallback(uint8_t softwareTimerId);

/*******************************************************************************
 * @fn      AlphabetButtonCallback
 * @brief   Alphabet button callback
 * @paramz  softwareTimerId
 * @return  None
 ******************************************************************************/
static void AlphabetButtonCallback(uint8_t softwareTimerId)
{
	sMenuPro.alphabetRepeatCounter = 0;
	sMenuPro.previousPressedButton = 0;
	sLcd.ShiftCursorDisplay(SHIFT_CURSOR_RIGHT);
}

/*******************************************************************************
 * ACTION FUNCTIONS
 ******************************************************************************/
static void PasswordAction(void);
static void DateTimeAction(void);
static void TitleAction(void);
static void KeyinAction(void);
static void OptionAction(void);
static void InfoAction(void);

/*******************************************************************************
 * @fn      PasswordAction
 * @brief   Key in password
 * @paramz  None
 * @return  None
 ******************************************************************************/
static void PasswordAction(void)
{
	if(sMenuPro.pCurrentMenu->index == sMenuPro.passwordIndex)
	{
		sLcd.WriteString(0, 0, "Key in password", LCD_ALIGN_LEFT);
	}
	else if(sMenuPro.pCurrentMenu->index == sMenuPro.oldPasswordIndex)
	{
		sLcd.WriteString(0, 0, "Old Password", LCD_ALIGN_LEFT);
	}
	else if(sMenuPro.pCurrentMenu->index == sMenuPro.newPasswordIndex)
	{
		sLcd.WriteString(0, 0, "New Password", LCD_ALIGN_LEFT);
	}
	else if(sMenuPro.pCurrentMenu->index == sMenuPro.confirmPasswordIndex)
	{
		sLcd.WriteString(0, 0, "Confirm Password", LCD_ALIGN_LEFT);
	}
	MenuListPrepareKeyin();
}

/*******************************************************************************
 * @fn      DateTimeAction
 * @brief   Key in new date and time
 * @paramz  None
 * @return  None
 ******************************************************************************/
static void DateTimeAction(void)
{
	if(sMenuPro.pCurrentMenu->index == sMenuPro.yearIndex)
	{
		sLcd.WriteString(0, 0, "Year(0-99)", LCD_ALIGN_LEFT);
	}
	else if(sMenuPro.pCurrentMenu->index == sMenuPro.monthIndex)
	{
		sLcd.WriteString(0, 0, "Month(1-12)", LCD_ALIGN_LEFT);
	}
	else if(sMenuPro.pCurrentMenu->index == sMenuPro.dateIndex)
	{
		sLcd.WriteString(0, 0, "Date(1-31)", LCD_ALIGN_LEFT);
	}
	else if(sMenuPro.pCurrentMenu->index == sMenuPro.hourIndex)
	{
		sLcd.WriteString(0, 0, "Hour(0-23)", LCD_ALIGN_LEFT);
	}
	else if(sMenuPro.pCurrentMenu->index == sMenuPro.minuteIndex)
	{
		sLcd.WriteString(0, 0, "Minute(0-59)", LCD_ALIGN_LEFT);
	}
	else if(sMenuPro.pCurrentMenu->index == sMenuPro.secondIndex)
	{
		sLcd.WriteString(0, 0, "Second(0-59)", LCD_ALIGN_LEFT);
	}
	else if(sMenuPro.pCurrentMenu->index == sMenuPro.weekDayIndex)
	{
		sLcd.WriteString(0, 0, "Week Day", LCD_ALIGN_LEFT);
		OptionAction();
		return;
	}
	MenuListPrepareKeyin();
}

/*******************************************************************************
 * @fn      TitleAction
 * @brief   Show current menu to LCD
 * @paramz  None
 * @return  None
 ******************************************************************************/
static void TitleAction(void)
{
	char string[LCD_MAX_LENGTH + 1];

	sprintf(string, "%c%s", INDICATE_CURRENT_MENU_SYMBOL, sMenuPro.pCurrentMenu->title);
	sLcd.WriteString(0, 0, string, LCD_ALIGN_LEFT);
	if(sMenuPro.pCurrentMenu->pNext != NULL)
	{
		sLcd.WriteString(1, 1, sMenuPro.pCurrentMenu->pNext->title, LCD_ALIGN_LEFT);
	}
	else
	{
		sLcd.WriteString(1, 0, "                ", LCD_ALIGN_LEFT);
	}
}

/*******************************************************************************
 * @fn      KeyinAction
 * @brief   Show user previous key in value
 * @paramz  None
 * @return  None
 ******************************************************************************/
static void KeyinAction(void)
{
	uint8_t i = 0;

	// Clear second line
	MenuListPrepareKeyin();
	// Show user previous key in value
	for(i = 0; i < strlen(sMenuPro.pCurrentMenu->title); i++)
	{
		if(!sMenuPro.pCurrentMenu->uMenuAttribute.isHidden)
		{
			sLcd.WriteCharacter(sMenuPro.pCurrentMenu->title[i]);
		}
		else
		{
			sLcd.WriteCharacter(HIDDEN_SYMBOL);
		}
		sMenuPro.keyinCounter++;
	}
}

/*******************************************************************************
 * @fn      OptionAction
 * @brief   Show option list value
 * @paramz  None
 * @return  None
 ******************************************************************************/
static void OptionAction(void)
{
	struct sMENU *psMenu;

	if(sMenuPro.pCurrentMenu->title[0] == 0)
	{
		sLcd.WriteString(1, 1, sMenuPro.pCurrentMenu->pChild->title, LCD_ALIGN_LEFT);
		sMenuPro.pOptionMenu = sMenuPro.pCurrentMenu->pChild;
	}
	else
	{
		psMenu = sMenuPro.pCurrentMenu->pChild;
		for(;;)
		{
			if(strcmp(sMenuPro.pCurrentMenu->title, psMenu->title) == 0)
			{
				sLcd.WriteString(1, 1, psMenu->title, LCD_ALIGN_LEFT);
				sMenuPro.pOptionMenu = psMenu;
				break;
			}
			else
			{
				psMenu = psMenu->pNext;
			}
			if(psMenu == NULL)
			{
				break;
			}
		}
	}
}

/*******************************************************************************
 * @fn      InfoAction
 * @brief   Show read only value
 * @paramz  None
 * @return  None
 ******************************************************************************/
static void InfoAction(void)
{
	sLcd.WriteString(1, 1, sMenuPro.pCurrentMenu->title, LCD_ALIGN_LEFT);
}

/*******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/
static void MenuListInitialize(void);
static void MenuListButtonPressed(uint32_t pressedButton);
static void MenuListUpdateDateTime(void);

/*******************************************************************************
 * @fn      MenuListInitialize
 * @brief   Menu list initialize
 * @param   None
 * @return  None
 ******************************************************************************/
static void MenuListInitialize(void)
{
	sMenuPro.dateTimeIndex = MenuListAddMenu(LEVEL1, "", MenuListUpdateDateTime, TITLE, false, 0);
		sMenuPro.passwordIndex = MenuListAddMenu(LEVEL2, "", PasswordAction, NUMBER, true, 6);
		sMenuPro.firstMenuIndex = MenuListAddMenu(LEVEL2, "Setting", TitleAction, TITLE, false, 0);
			MenuListAddMenu(LEVEL3, "User Name", TitleAction, TITLE, false, 0);
				MenuListAddMenu(LEVEL4, "", KeyinAction, ALPHABET, false, 8);
			MenuListAddMenu(LEVEL3, "Serial Number", TitleAction, TITLE, false, 0);
				MenuListAddMenu(LEVEL4, "", KeyinAction, NUMBER, false, 8);
			MenuListAddMenu(LEVEL3, "Temperature", TitleAction, TITLE, false, 0);
				MenuListAddMenu(LEVEL4, "", OptionAction, OPTION, false, 0);
					MenuListAddMenu(LEVEL5, "Low", TitleAction, TITLE, false, 0);
					MenuListAddMenu(LEVEL5, "Medium", TitleAction, TITLE, false, 0);
					MenuListAddMenu(LEVEL5, "High", TitleAction, TITLE, false, 0);
			MenuListAddMenu(LEVEL3, "Backlight", TitleAction, TITLE, false, 0);
				MenuListAddMenu(LEVEL4, "", OptionAction, OPTION, false, 0);
					MenuListAddMenu(LEVEL5, "ON", TitleAction, TITLE, false, 0);
					MenuListAddMenu(LEVEL5, "OFF", TitleAction, TITLE, false, 0);
			MenuListAddMenu(LEVEL3, "Date Time", TitleAction, TITLE, false, 0);
				sMenuPro.yearIndex = MenuListAddMenu(LEVEL4, "", DateTimeAction, NUMBER, false, 2);
				sMenuPro.monthIndex = MenuListAddMenu(LEVEL4, "", DateTimeAction, NUMBER, false, 2);
				sMenuPro.dateIndex = MenuListAddMenu(LEVEL4, "", DateTimeAction, NUMBER, false, 2);
				sMenuPro.hourIndex = MenuListAddMenu(LEVEL4, "", DateTimeAction, NUMBER, false, 2);
				sMenuPro.minuteIndex = MenuListAddMenu(LEVEL4, "", DateTimeAction, NUMBER, false, 2);
				sMenuPro.secondIndex = MenuListAddMenu(LEVEL4, "", DateTimeAction, NUMBER, false, 2);
				sMenuPro.weekDayIndex = MenuListAddMenu(LEVEL4, "", DateTimeAction, OPTION, false, 0);
					MenuListAddMenu(LEVEL5, "Monday", TitleAction, TITLE, false, 0);
					MenuListAddMenu(LEVEL5, "Tuesday", TitleAction, TITLE, false, 0);
					MenuListAddMenu(LEVEL5, "Wednesday", TitleAction, TITLE, false, 0);
					MenuListAddMenu(LEVEL5, "Thursday", TitleAction, TITLE, false, 0);
					MenuListAddMenu(LEVEL5, "Friday", TitleAction, TITLE, false, 0);
					MenuListAddMenu(LEVEL5, "Saturday", TitleAction, TITLE, false, 0);
					MenuListAddMenu(LEVEL5, "Sunday", TitleAction, TITLE, false, 0);
			MenuListAddMenu(LEVEL3, "Change Password", TitleAction, TITLE, false, 0);
				sMenuPro.oldPasswordIndex = MenuListAddMenu(LEVEL4, "", PasswordAction, NUMBER, true, 6);
				sMenuPro.newPasswordIndex = MenuListAddMenu(LEVEL4, "", PasswordAction, NUMBER, true, 6);
				sMenuPro.confirmPasswordIndex = MenuListAddMenu(LEVEL4, "", PasswordAction, NUMBER, true, 6);
		MenuListAddMenu(LEVEL2, "Report", TitleAction, TITLE, false, 0);
			MenuListAddMenu(LEVEL3, "Daily Report", TitleAction, TITLE, false, 0);
			MenuListAddMenu(LEVEL3, "Weekly Report", TitleAction, TITLE, false, 0);
			MenuListAddMenu(LEVEL3, "Monthly Report", TitleAction, TITLE, false, 0);
		MenuListAddMenu(LEVEL2, "Info", TitleAction, TITLE, false, 0);
			MenuListAddMenu(LEVEL3, "Version", TitleAction, TITLE, false, 0);
				MenuListAddMenu(LEVEL4, "V1.0.0", InfoAction, INFO, false, 0);
			MenuListAddMenu(LEVEL3, "Last Update", TitleAction, TITLE, false, 0);
				MenuListAddMenu(LEVEL4, "20.05.25 23:00", InfoAction, INFO, false, 0);
    sMenuPro.pCurrentMenu = &sMenuPro.sMenu[0];
    sMenuPro.pCurrentMenu->menuAction();
    sMenuPro.AlphabetButtonTimerId = sSoftwareTimer.Initialize(NULL, AlphabetButtonCallback, NULL, TIMER_ONCE_TYPE);
    sprintf(sMenuPro.password, "123456");
}

/*******************************************************************************
 * @fn      MenuListButtonPressed
 * @brief   Menu list receive button pressed
 * @param   pressedButton
 * @return  None
 ******************************************************************************/
static void MenuListButtonPressed(uint32_t pressedButton)
{
	switch(sMenuPro.pCurrentMenu->uMenuAttribute.eMenuType)
	{
		case TITLE:
			MenuListNavigationButton(pressedButton);
			break;
		case NUMBER:
			MenuListNumberButton(pressedButton);
			break;
		case ALPHABET:
			MenuListAlphabetButton(pressedButton);
			break;
		case OPTION:
			MenuListOptionButton(pressedButton);
			break;
		case INFO:
			MenuListInfoButton(pressedButton);
			break;
		default:
			break;
	}
}

/*******************************************************************************
 * @fn      MenuListUpdateDateTime
 * @brief   Menu list update date and time
 * @param   None
 * @return  None
 ******************************************************************************/
static void MenuListUpdateDateTime(void)
{
	char dateTime[17];
	RTC_DateTypeDef sDate;
	RTC_TimeTypeDef sTime;

	if(sMenuPro.pCurrentMenu->index != sMenuPro.dateTimeIndex)
	{
		return;
	}
	RtcGetDateTime(&sDate, &sTime);
	sprintf(dateTime, "20%02d-%02d-%02d", sDate.Year, sDate.Month, sDate.Date);
	sLcd.WriteString(0, 0, dateTime, LCD_ALIGN_CENTER);
	switch(sDate.WeekDay)
	{
		case RTC_WEEKDAY_MONDAY:
			sprintf(dateTime, "MON ");
			break;
		case RTC_WEEKDAY_TUESDAY:
			sprintf(dateTime, "TUE ");
			break;
		case RTC_WEEKDAY_WEDNESDAY:
			sprintf(dateTime, "WED ");
			break;
		case RTC_WEEKDAY_THURSDAY:
			sprintf(dateTime, "THU ");
			break;
		case RTC_WEEKDAY_FRIDAY:
			sprintf(dateTime, "FRI ");
			break;
		case RTC_WEEKDAY_SATURDAY:
			sprintf(dateTime, "SAT ");
			break;
		case RTC_WEEKDAY_SUNDAY:
			sprintf(dateTime, "SUN ");
			break;
		default:
			sprintf(dateTime, "    ");
			break;
	}
	sprintf(dateTime + 4, "%02d:%02d:%02d", sTime.Hours, sTime.Minutes, sTime.Seconds);
	sLcd.WriteString(1, 0, dateTime, LCD_ALIGN_CENTER);
}

/*******************************************************************************
 * STRUCTURE
 ******************************************************************************/
// Menu list function structure
sMENU_LIST sMenuList =
{
	MenuListInitialize,
	MenuListButtonPressed,
	MenuListUpdateDateTime,
};

