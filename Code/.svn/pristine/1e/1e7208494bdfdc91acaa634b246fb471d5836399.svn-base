/****************************************************************************/
/*	File   :             Holtek_Timer.c               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/7/29,16:43:38                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/
#include "Holtek_Timer.h"

void STimerInit(void)
{
	//Bit 7~6 STnM1~STnM0: Select STMn Operating Mode
	//00: Compare Match Output Mode
	//01: Capture Input Mode
	//10: PWM Output Mode or Single Pulse Output Mode
	//11: Timer/Counter Mode
	//Bit 5~4 STnIO1~STnIO0: Select STMn External Pins Function
	//Compare Match Output Mode
	//00: No change
	//01: Output low
	//10: Output high
	//11: Toggle output
	//PWM Output Mode/Single Pulse Output Mode
	//00: PWM output inactive state
	//01: PWM output active state
	//10: PWM output
	//11: Single Pulse Output
	//Capture Input Mode
	//00: Input capture at rising edge of STPnI
	//01: Input capture at falling edge of STPnI
	//10: Input capture at both rising and falling edges of STPnI
	//11: Input capture disabled
	//Timer/Counter Mode
	//Unused
	//Bit 3 STnOC: STPn Output Control
	//Compare Match Output Mode
	//0: Initial low
	//1: Initial high
	//PWM Output Mode/Single Pulse Output Mode
	//0: Active low
	//1: Active high
	//Bit 2 STPnOL: STPn Output Polarity Control
	//0: Non-inverted
	//1: Inverted
	//Bit 1 STnDPX: STMn PWM Duty/Period Control
	//0: CCRP 每 period; CCRA 每 duty
	//1: CCRP 每 duty; CCRA 每 period
	//Bit 0 STnCCLR: STMn Counter Clear Condition Selection
	//0: Comparator P match
	//1: Comparator A match
	
	REGResetBits(_stm0c1, STMC1_MODE_MASK);//_stm0c1=0x3F
	REGSetBits(_stm0c1, STMC1_MODE_TIMER);//_stm0c1=0xC0
			
	//stm0c0
	//BIT7: stpnau  0:RUN 1:Pause
	//BIT6~4:STnCK2~STnCK0: 
	//000:fsys/4 
	//001:fsys
	//010:fh/16 
	//011:fh/64
	//100:fsub
	//101:fsub
	//110:STCKn rising edge clock
	//111:STCKn falling edge clock
	//BIT3: 0:Off 1:On
	//Bit 2~0 STnRP2~STnRP0:
	//000: 1024 STMn clocks
	//001: 128 STMn clocks
	//010: 256 STMn clocks
	//011: 384 STMn clocks
	//100: 512 STMn clocks
	//101: 640 STMn clocks
	//110: 768 STMn clocks
	//111: 896 STMn clocks
	REGResetBits(_stm0c0, STMC0_STCK_MASK);//_stm0c0=0x8F
	REGSetBits(_stm0c0, STMC0_STCK_SUB);//_stm0c0=0x40
	
	//timer 
	
	REGWrite(_stm0al, 0xff);
	REGWrite(_stm0ah, 0x03);
		
	REGSetBits(_stm0c1, STMI_STCCLR);
	//on 
	REGSetBits(_stm0c0, STMC0_STON);

}

