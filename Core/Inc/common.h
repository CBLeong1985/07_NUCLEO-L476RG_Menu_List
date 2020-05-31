/*******************************************************************************
 * Filename:			common.h
 * Revised:				Date: 2019.11.04
 * Revision:			V001
 * Description:		    All common define, enumrate, structure
*******************************************************************************/

#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
// Standard C Lbrary Header
#include "stdarg.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "math.h"

// ST Library Header
#include "stm32l4xx_hal.h"

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
#define B(x)	((x&0x0000000FLU)?1:0) \
				+((x&0x000000F0LU)?2:0) \
				+((x&0x00000F00LU)?4:0) \
				+((x&0x0000F000LU)?8:0) \
				+((x&0x000F0000LU)?16:0) \
				+((x&0x00F00000LU)?32:0) \
				+((x&0x0F000000LU)?64:0) \
				+((x&0xF0000000LU)?128:0)

/*******************************************************************************
 * ENUMERATED
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _COMMON_H_ */
