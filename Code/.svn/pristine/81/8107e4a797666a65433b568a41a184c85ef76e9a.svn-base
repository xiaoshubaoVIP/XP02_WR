/****************************************************************************/
/*	File   :             Holtck_Timer.h               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/7/29,16:43:5                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/

#ifndef __HOLTEK_TIMER__
#define __HOLTEK_TIMER__
#include "base.h"
//STMC0 REGISTER
#define STMC0_STCK_MASK  ((uint8_t)(0x7<<4))
#define STMC0_STRP_MASK  ((uint8_t)(0x7))

typedef enum{
	STMC0_STCK1_4 = ((uint8_t)(0x00<<4)),
	STMC0_STCK1_1 = ((uint8_t)(0x01<<4)),
	STMC0_STCK1_16 = ((uint8_t)(0x02<<4)),
	STMC0_STCK1_64 = ((uint8_t)(0x03<<4)),
	STMC0_STCK_SUB = ((uint8_t)(0x04<<4)),
	STMC0_STCK_RESERVED = ((uint8_t)(0x05<<4)),
	STMC0_STCK_UP = ((uint8_t)(0x06<<4)),
	STMC0_STCK_DOWN = ((uint8_t)(0x07<<4)),

}STMC0_STCK;

typedef enum{
	STMC0_STPAU = ((uint8_t)(0x01<<7)),
	STMC0_STON = ((uint8_t)(0x01<<3)),
}STMC0;

//STMC1 REGISTER
#define STMC1_MODE_MASK  ((uint8_t)(0x3<<6))
#define STMC1_STI_MASK  ((uint8_t)(0x3<<4))

typedef enum{
	STMC1_MODE_OUT = ((uint8_t)(0x00<<6)),
	STMC1_MODE_RESERVED = ((uint8_t)(0x01<<6)),
	STMC1_MODE_PWM = ((uint8_t)(0x02<<6)),
	STMC1_MODE_TIMER = ((uint8_t)(0x03<<6)),
}STMC1_MODE;
	
typedef enum{
	STMC1_STI_RESERVED = ((uint8_t)(0x00<<4)),
	STMC1_STI_LOW = ((uint8_t)(0x01<<4)),
	STMC1_STI_HIGH = ((uint8_t)(0x02<<4)),
	STMC1_STI_REVERSAL = ((uint8_t)(0x03<<4)),
}STMC1_STI;

typedef enum{
	STMI_STOC = ((uint8_t)(0x01<<3)),
	STMI_STPOL = ((uint8_t)(0x01<<2)),
	STMI_STDPX = ((uint8_t)(0x01<<1)),
	STMI_STCCLR = ((uint8_t)(0x01)),

}STMC1;

void STimerInit(void);


#endif


