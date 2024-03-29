/****************************************************************************/
/*	File   :             Adc.h               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/7/25,16:28:8                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/

#ifndef __ADC_H__
#define __ADC_H__
#include "base.h"

typedef enum
{
	eVddNormal = 0x00, 
	eVddIsink  =0x01,
}eVddType;

uint16 GetAdcSample(eAdcChannel Channel, bool CoFualtCheckFlag);
uint16 GetVddSample(eVddType Type);

#endif

