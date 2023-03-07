/****************************************************************************/
/*	File   :             Key.h               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/7/25,16:26:59                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/

#ifndef __KEY_H__
#define __KEY_H__

#include "base.h"


#define	TEST_KEY_is_L() 		(TEST_KEY_PIN == RESET)	//Key Press L
#define	TEST_KEY_is_H() 		(TEST_KEY_PIN != RESET)	//Key Press H

#ifdef SUPPORT_SPEAK_FUNCTION
#define SET_KEY_is_L()			(SET_KEY_PIN == RESET)	//Key Press L
#define SET_KEY_is_H()			(SET_KEY_PIN != RESET)	//Key Press L
#endif

#ifdef SUPPORT_XP01_WR_5260
#define PAIR_KEY_is_L()			(PAIR_KEY_PIN == RESET)	//Key Press L
#define PAIR_KEY_is_H()			(PAIR_KEY_PIN != RESET)	//Key Press H

#endif

void KeyInit(void);
uint8_t GetKeyClickFlag(void);
void KeyHandleMsg(void);

#endif


