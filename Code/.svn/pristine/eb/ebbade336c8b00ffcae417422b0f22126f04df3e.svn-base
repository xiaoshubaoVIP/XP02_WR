/****************************************************************************/
/*	File   :             PwrManager.h               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/7/25,16:29:32                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/
#ifndef __PWR_H__
#define  __PWR_H__

#define LowPwrMaxCnt (9)
#define SAMPLECALINUM (6)

typedef struct Pwr PwrTag;
struct Pwr
{
	uint16 BatVolVdd;             // battery voltage no load
	uint16_t CailVddValue;
	uint8 BatVolIndex;            //compensation voltage

	uint8_t BatLevel;
	uint8_t BatOldLevel;

	uint8 BatCheckCntForbitMute;		//battery low check counter
	uint8 BatCheckCntlow;				//battery low check counter
	uint8 BatCheckCnt1in3;
	uint8 BatCheckCnt2in3;
	volatile  uint8  standbycnt;		// halt counter
};
extern PwrTag PwrAO;
void PwrInit(void);

void EnteryLowPower(void);
void LowBatFunction(void);
void BatteryCheck(void);
uint16_t BatteryCheckHandle(void);

#endif

