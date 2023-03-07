/****************************************************************************/
/*	File   :             F:\Smoker\HTK-BA45F5240__0\branches\HTBA45F5240-NEW\src\main.c               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/10/23,18:31:54                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Discription :                                                     */
/****************************************************************************/

#include "main.h"
#include "Module.h"

#define SYSTEMTICKS     ((uint16_t)0x03ff)
#define SystemTimerL	(_stm0dl)
#define SystemTimerH	(_stm0dh)


uint16_t SMOKE_ALARM_LEVEL = 510;

SystemObjectTag SysAO;


/**********************************************************************/
//Description:		SystemVarInit														  
//Parameters:                        												  
//Return:   											
//Date:  
//author:		    song.jiang
/**********************************************************************/
static void SystemVarInit(void)
{ 
	 uint8_t i = 0;
	SysAO.haltFlag = eHaltFlagNull;		//halt flag
	SysAO.eEventFlg = eEventFlagNone;	//event flag
	SysAO.LifeTimeDays = 0;
	SysAO.LifeTimeUpdateFlag = 0;		//day update flag
	SysAO.SlpTimeCnt = 0;
	SysAO.EventMuteTimeCnt = 0;
	SysAO.StdbyTimeCnt = 0;
	SysAO.DebugOnFlag = FALSE;
	SysAO.PowerTestFlag = FALSE;
	SysAO.TempData = 45;

#ifdef SUPPORT_SPEAK_FUNCTION
	SysAO.LowPowerSpeakTimeCnt = 0;
	SysAO.EndLifeSpeakTimeCnt = 0;
#endif

	SysAO.eMode = SmokeCailMode;
	SysAO.eOldMode = SmokeCailMode;

	SysAO.LifeTimeDays = E2promRead(FLASH_ADDR_LIFE_TIME_DAYS_MSB);
	SysAO.LifeTimeDays <<= 8;
	SysAO.LifeTimeDays |= E2promRead(FLASH_ADDR_LIFE_TIME_DAYS_LSB);

	SysAO.sig = 0;
	SysAO.attrQF__ = 0;
	for(i=0;i<SIGQUEUE;i++)
	{
		SysAO.sigQue[i].sig = 0;
		SysAO.sigQue[i].atrr = 0;				
	}

	SysteSetSig(eEventSigEnteryMode);
}
/**************************************
*@brief:清RAM
*@param[in]:
*@param[in]:
*@retval: 
**************************************/
/*
void S_RAM_Init(unsigned char RAN_ADD)
{
	_mp1h =RAN_ADD;
	_mp1l =C_RAM_ADDR_START;
	_acc = C_RAM_ADDR_END-C_RAM_ADDR_START+1;
	while(_acc!=0)
	{
		_iar1=0;
		++_mp1l;
		_acc=_acc-1;	
	}
	
}
void S_RAM_Init_All(void)
{
	_mp1h = 0;
	while (_mp1h<=15)
	{
		_mp1l =C_RAM_ADDR_START;
		_acc = C_RAM_ADDR_END-C_RAM_ADDR_START+1;
		while(_acc!=0)
		{
			_iar1=0;
			++_mp1l;
			_acc--;	
		}
		_mp1h++;
	}
}
*/
/**********************************************************************/
//Description:		SysClkCfg														  
//Parameters:                        												  
//Return:   											
//Date:  
//author:		    song.jiang
/**********************************************************************/
static void SysClkCfg(void)
{  
    // HIRC1~HIRC0(bit3~bit2):HIRC frequency selection
    // 00:2M
    // 01:4M
    // 10:8M
    // 11:2M
    // 4M 
    // HIRCEN:bit0 HIRC enable
    GpioSetBits(_hircc,GPIOBIT2_SELECT|GPIOBIT0_SELECT);    

    // HIRCF:bit1       // HIRC stable flag
    while(!_hircf);

    // CKS2~CKS0(bit7~bit5):System clock selection
    // 000:fH
    // 001:fH/2
    // 010:fH/4
    // 011:fH/8
    // 100:fH/16
    // 101:fH/32
    // 110:fH/64
    // 111:fSUB
    //_cks2 = 0;          //fH
    //_cks1 = 0;
    //_cks0 = 0;
	

    // FHIDEN:bit1
    //_fhiden = 0;        // HIRC enable when CPU is switched off

    // FSIDEN:bit0
    //_fsiden = 0;        // LIRC enable when CPU is switched off
	GpioResetBits(_scc,0xff);


}


/**********************************************************************/
//Description:		TimerTb0Init														  
//Parameters:        time                  												  
//Return:   											
//Date:  
//author:		    song.jiang
/**********************************************************************/
 void TimerTb0Init(eTbnTime time)
{  
    //CLKSEL1~CLKSEL0(Bit 1~0): Prescaler clock source selection
    //00: fSYS
    //01: fSYS/4
    //1x: fSUB
    //_clksel0= 1;
    //_clksel0 = 1;     //fSUB
	GpioSetBits(_pscr,GPIOBIT1_SELECT|GPIOBIT0_SELECT);

    //TBn2~TBn0(Bit 2~0): Select Time Base n Time-out Period
    //000: 2^8/fPSC
    //001: 2^9/fPSC
    //010: 2^10/fPSC
    //011: 2^11/fPSC
    //100: 2^12/fPSC
    //101: 2^13/fPSC
    //110: 2^14/fPSC
    //111: 2^15/fPSC
    //_tb02 = 1;
    //_tb01 = 1;
    //_tb00 = 1;        //1024ms

#if 0
    //_tb0c = time;
    GpioWrite(_tb0c,time);
	
	  //_tb0on = 1; 		//time base on
	GpioSetBits(_tb0c,GPIOBIT7_SELECT);
	  
	 // _tb0e = 1;			//tb0 interrupt disable
	GpioResetBits(_intc3,GPIOBIT0_SELECT);
#else
	GpioWrite(_tb0c,time);
	  //_tb0on = 1; 		//time base on
	GpioSetBits(_tb0c,GPIOBIT7_SELECT);
	 // _tb0e = 1;			//tb0 interrupt disable
	GpioSetBits(_intc2,GPIOBIT1_SELECT);
#endif
	
}


/**********************************************************************/
//Description:		SysWakeUpInit														  
//Parameters:                        												  
//Return:   											
//Date:  
//author:		    song.jiang
/**********************************************************************/
void SysWakeUpInit(eWakeupTime time)
{  
    uint8 temp = (uint8)time;
    //WE4~WE0(Bit 7~3): WDT function software control
    //01010/10101: Enable
    //Others: Reset MCU
    _we4 = 0;
    _we3 = 1;
    _we2 = 0;
    _we1 = 1;
    _we0 = 0;           //enable

    //WS2~WS0(Bit 2~0): WDT time-out period selection
    //000: 2^8/fLIRC
    //001: 2^10/fLIRC
    //010: 2^12/fLIRC
    //011: 2^14/fLIRC
    //100: 2^15/fLIRC
    //101: 2^16/fLIRC
    //110: 2^17/fLIRC
    //111: 2^18/fLIRC
    //_ws2 = 1;
    //_ws1 = 1;
    //_ws0 = 1;        //8196ms
    _wdtc = (_wdtc & 0x50) | temp; 


}



/**********************************************************************/
//Description:		SetupGpio														  
//Parameters:                        												  
//Return:   											
//Date:  
//author:		    song.jiang
/**********************************************************************/
static void SetupGpio(void)
{
#ifdef SUPPORT_XP02_BOARD
   //IO set
    //PA
    GpioWrite(_pa,0x00);
	GpioWrite(_papu,0xFF);
	GpioWrite(_pac,0x00);
	GpioWrite(_pawu,0x00);	
	GpioSetBits(_pas0,0x00);
	GpioSetBits(_pas1,0x00);
	//PA0 PA0_G OUT
	//PA1 INT_DET OUT 目前没有使用
	//PA2 PA2_R OUT
	//PA3 ADJUST OUT 目前没有使用
	//PA4 AC_DET 目前没有使用
	//PA5 PA5 OUT 目前没有使用
	//PA6 RF_IRQ 目前没有使用
	
	//PA7 Temp Sensor AN1	
	GpioWrite(_papu,GPIOBIT0_SELECT|GPIOBIT2_SELECT);
	
	GpioSetBits(_pac,GPIOBIT7_SELECT);
	//PAS17~PAS16(Bit 1~0):10(AN1) Temp Sensor
	GpioSetBits(_pas1,GPIOBIT7_SELECT);//PA7 AN1
	
	//PA4, Key Input
	GpioSetBits(_pa,GPIOBIT4_SELECT);
	GpioSetBits(_papu,GPIOBIT4_SELECT);
	GpioSetBits(_pac,GPIOBIT4_SELECT);
	GpioSetBits(_pawu,GPIOBIT4_SELECT);

	
#ifdef SUPPORT_OPA1_OUTPUT
		_pas03 = 1; 												//A1 out
		_pas02 = 0;
#endif


#ifdef SUPPORT_OPPUT	
	GpioResetBits(_pac,GPIOBIT1_SELECT);
	GpioResetBits(_papu,GPIOBIT1_SELECT);
	GpioSetBits(_pas0,GPIOBIT3_SELECT);
#endif

	/**********************************************************************************/

	//PB0, AD Input
	GpioWrite(_pb,0x00);
	GpioWrite(_pbpu,0xFF);
	GpioWrite(_pbc,0x00);
	GpioSetBits(_pbs0,0x00);
	GpioSetBits(_pbs1,0x00);
	
	//PB0 KEY PullPu IN Int0
	GpioSetBits(_pbc,GPIOBIT0_SELECT);
	GpioResetBits(_pbpu,GPIOBIT0_SELECT);
	//  _int1s1:_int1s0 = 00   diable  bit [3:2]
    //  _int1s1:_int1s0 = 01   rising edge
    //  _int1s1:_int1s0 = 10   falling edge
    //  _int1s1:_int1s0 = 11   rising and falling edge
	//_int0s1 = 1;
	//_int0s0 = 0;
	GpioWrite(_integ,0x00);
	GpioSetBits(_integ,GPIOBIT0_SELECT);
	GpioSetBits(_integ,GPIOBIT1_SELECT);
	//_int0e = 1;   //enable  int1 Int0E:Bit2 
	GpioSetBits(_intc0,GPIOBIT2_SELECT);

	//PB1 UARTTX OUT
	//PBS03~PBS02(bit3-2):11(Tx)
	GpioSetBits(_pbs0,GPIOBIT3_SELECT|GPIOBIT2_SELECT);	
	//PB2 INT_OUT_L OUT 目前没有使用
	
	//PB3 RX
	//PBS07~PBS06(bit7-6):11(Rx)
	GpioSetBits(_pbs0,GPIOBIT7_SELECT|GPIOBIT6_SELECT);
	GpioSetBits(_pbc,GPIOBIT3_SELECT);
	GpioSetBits(_pbpu,GPIOBIT3_SELECT|GPIOBIT1_SELECT);


	//PB4 Temp_V Out
	//PB6 BAT_DET OUT
	GpioResetBits(_pbc,GPIOBIT4_SELECT|GPIOBIT6_SELECT);
	GpioSetBits(_pbpu,GPIOBIT4_SELECT|GPIOBIT6_SELECT);

#ifdef SUPPORT_XP02_BOARD
	GpioSetBits(_pbc,GPIOBIT4_SELECT);
	GpioSetBits(_pbpu,GPIOBIT4_SELECT);
	GpioSetBits(_pb,GPIOBIT4_SELECT);
	GCC_DELAY(1000);
	if(_pb4 == 0)
	{
		SysAO.PowerTestFlag = TRUE;
	}

	GpioResetBits(_pbc,GPIOBIT4_SELECT);
	GpioSetBits(_pbpu,GPIOBIT4_SELECT);
	GpioResetBits(_pb,GPIOBIT4_SELECT);
#endif

	/**********************************************************************************/
	//PC
	GpioWrite(_pc,0x00);
	GpioWrite(_pcpu,0xFF);
	GpioWrite(_pcc,0x00);	
	GpioSetBits(_pcs0,0x00);
	GpioSetBits(_pcs1,0x00);
	//PC0 CO_OUT IN AN7
	GpioSetBits(_pcc,GPIOBIT0_SELECT);
	GpioSetBits(_pcs0,GPIOBIT1_SELECT);
	GpioSetBits(_pcpu,GPIOBIT3_SELECT);
	//PC1 CO Test OUT 
	//PC2 ISINK0_EN OUT 
	//PC3 ISINK1_EN OUT 
	//PC4 BUZZER Out
	//PC5 NC OUT 目前没有使用
	//PC6 BAT_DET OUT
	GpioSetBits(_pcpu,GPIOBIT1_SELECT|GPIOBIT2_SELECT|GPIOBIT3_SELECT|GPIOBIT4_SELECT|GPIOBIT5_SELECT|GPIOBIT6_SELECT);

	/**********************************************************************************/

	//Pd
	GpioWrite(_pd,0x00);
	GpioWrite(_pdpu,0xFF);
	GpioWrite(_pdc,0x00);
	GpioSetBits(_pds0,0x00);
	
	_sledc0 = 0b11111111;
	_sledc1 = 0b11111111;


	/**********************************************************************************/
#ifdef SUPPORT_RF_NET_FUNCTION
	GpioSetBits(_pac,GPIOBIT1_SELECT);					//IRQ States pin
	GpioSetBits(_papu,GPIOBIT1_SELECT);
	GpioSetBits(_pawu,GPIOBIT1_SELECT);
#endif

#ifdef SUPPORT_SPEAK_FUNCTION
	GpioResetBits(_pac,GPIOBIT5_SELECT);				//Enable pin
	GpioSetBits(_papu,GPIOBIT5_SELECT);
	GpioResetBits(_pa,GPIOBIT5_SELECT);


	GpioResetBits(_pcc,GPIOBIT5_SELECT);				//Data pin
	GpioSetBits(_pcpu,GPIOBIT5_SELECT);
	GpioResetBits(_pc,GPIOBIT5_SELECT);

	GpioSetBits(_pac,GPIOBIT3_SELECT);					//States pin
	GpioResetBits(_papu,GPIOBIT3_SELECT);

	//PA0, Set Key Input
	GpioSetBits(_pa,GPIOBIT0_SELECT);
	GpioSetBits(_papu,GPIOBIT0_SELECT);
	GpioSetBits(_pac,GPIOBIT0_SELECT);
	GpioSetBits(_pawu,GPIOBIT0_SELECT);
#endif

	GpioSetBits(_pbc,GPIOBIT7_SELECT);
	GpioSetBits(_pbs1,GPIOBIT7_SELECT);
	GpioResetBits(_pbs1,GPIOBIT6_SELECT);

#else
	   //IO set
		//PA
		GpioWrite(_pa,0x00);
		GpioWrite(_papu,0xFF);  //除能 上拉
		GpioWrite(_pac,0x00);   //默认输出
		GpioWrite(_pawu,0x00);	//除能 默认不具有唤醒功能
		GpioSetBits(_pas0,0x00);  //默认引脚不共用
		GpioSetBits(_pas1,0x00);  //PA0~PA3~PA7
		//PA0 ------------Pair_key
		//PA1 INT_DET OUT 目前没有使用--->PA1_G OUT
		//PA2 
		//PA3 ADJUST OUT 目前没有使用   ---》RF-IRQ
		//PA4 AC_DET 目前没有使用
		//PA5 PA5 OUT 目前没有使用  ------->PA5_R OUT
		//PA6 RF_IRQ 目前没有使用-------->快速进入低功耗 
		//PA7 Temp Sensor AN1	
		//PA1 PA5 out
		GpioWrite(_papu,GPIOBIT1_SELECT|GPIOBIT5_SELECT);//上拉

		GpioSetBits(_pac,GPIOBIT7_SELECT);   //PAC_7 输入    
		//PAS17~PAS16(Bit 1~0):10(AN1) Temp Sensor
		GpioSetBits(_pas1,GPIOBIT7_SELECT);//PA7 AN1 模拟   不需要上拉
		
		
		//PA4, Key Input
		GpioSetBits(_pa,GPIOBIT4_SELECT);    //PA4
		GpioSetBits(_papu,GPIOBIT4_SELECT);  //上拉使能
		GpioSetBits(_pac,GPIOBIT4_SELECT);   //输入
		GpioSetBits(_pawu,GPIOBIT4_SELECT);   //休眠模式下唤醒使能
	
		
#ifdef SUPPORT_OPA1_OUTPUT
			_pas03 = 1; 												//A1 out
			_pas02 = 0;
#endif
	
	
#ifdef SUPPORT_OPPUT	
		GpioResetBits(_pac,GPIOBIT1_SELECT);  
		GpioResetBits(_papu,GPIOBIT1_SELECT);
		GpioSetBits(_pas0,GPIOBIT3_SELECT);
#endif
	
		/**********************************************************************************/

		//PB0, AD Input
		GpioWrite(_pb,0x00);
		GpioWrite(_pbpu,0xFF);   //默认上拉
		GpioWrite(_pbc,0x00);     //默认输出
		GpioSetBits(_pbs0,0x00);
		GpioSetBits(_pbs1,0x00);
		
		//PB1 UARTTX OUT
		//PBS03~PBS02(bit3-2):11(Tx)
		GpioSetBits(_pbs0,GPIOBIT3_SELECT|GPIOBIT2_SELECT);  //设置PB1为TX0模式
		//PB2 INT_OUT_L OUT 目前没有使用
		
		//PB3 RX
		//PBS07~PBS06(bit7-6):11(Rx)
		GpioSetBits(_pbs0,GPIOBIT7_SELECT|GPIOBIT6_SELECT);  //设置PB3为:RX0/TX0模式
		GpioSetBits(_pbc,GPIOBIT3_SELECT);                    //设置为输入
		
		GpioSetBits(_pbpu,GPIOBIT3_SELECT|GPIOBIT1_SELECT); //PB1/PB3设置为上拉
	
	
		//PB4 Temp_V Out
		//PB6 BAT_DET OUT

		GpioResetBits(_pbc,GPIOBIT4_SELECT);   //输出
		GpioSetBits(_pbpu,GPIOBIT4_SELECT);    //上拉 

			
#ifdef SUPPORT_XP01_BOARD 
			GpioSetBits(_pac,GPIOBIT6_SELECT);	   //输入
			GpioSetBits(_papu,GPIOBIT6_SELECT);   //上拉
			GpioSetBits(_pa,GPIOBIT6_SELECT);
			GCC_DELAY(1000);
			if(_pa6 == 0)
			{
				SysAO.PowerTestFlag = TRUE;
			}
		
			GpioResetBits(_pac,GPIOBIT6_SELECT);
			GpioSetBits(_papu,GPIOBIT6_SELECT);
			GpioResetBits(_pa,GPIOBIT6_SELECT);
#endif	
	
		/**********************************************************************************/
		//PC
		GpioWrite(_pc,0x00);
		GpioWrite(_pcpu,0xFF);
		GpioWrite(_pcc,0x00);	
		GpioSetBits(_pcs0,0x00);
		GpioSetBits(_pcs1,0x00);
		
		//PC0 CO_OUT IN AN7
		GpioSetBits(_pcc,GPIOBIT0_SELECT);      //输入
		GpioSetBits(_pcs0,GPIOBIT1_SELECT);     //设置为AN7 模拟输入不需要上拉
		
		//PC1 CO Test OUT 
		//PC2 ISINK0_EN OUT ---->没使用
		//PC3 ISINK1_EN OUT ---->没使用
		//PC4 BUZZER Out
		//PC5 NC OUT 目前没有使用 ---->pair key    输入、上拉
		//PC6 BAT_DET OUT   ---->没有
		GpioSetBits(_pcpu,GPIOBIT1_SELECT|GPIOBIT4_SELECT);

	#ifdef SUPPORT_RF_NET_FUNCTION
		//PC5
		GpioResetBits(_pcc,GPIOBIT5_SELECT);  //输入
		GpioSetBits(_pcpu,GPIOBIT5_SELECT);    //上拉 
		GpioSetBits(_pc,GPIOBIT5_SELECT);      //PC5
		//PA3 
		GpioSetBits(_pac,GPIOBIT3_SELECT);		//输入			//IRQ States PA3 
		GpioSetBits(_papu,GPIOBIT3_SELECT);     //上拉
		GpioSetBits(_pawu,GPIOBIT3_SELECT);      //唤醒模式
		
	#endif
	
	/**********************************************************************************/
		//Pd
		GpioWrite(_pd,0x00);
		GpioWrite(_pdpu,0xFF);
		GpioWrite(_pdc,0x00);
		GpioSetBits(_pds0,0x00);
		
		_sledc0 = 0b11111111;
		_sledc1 = 0b11111111;
		
	#ifdef SUPPORT_XP01_WR_5260
		//PA0, Pair Key Input
		GpioSetBits(_pa,GPIOBIT0_SELECT);      //
		GpioSetBits(_papu,GPIOBIT0_SELECT);   //上拉
		GpioSetBits(_pac,GPIOBIT0_SELECT);    //输入
		GpioSetBits(_pawu,GPIOBIT0_SELECT);   // 唤醒模式
	#endif
#endif
}

#ifdef SUPPORT_CARIBRATION_DEBUG
/**********************************************************************/
//Description:		GetVddVol											
//Parameters:        voltage referance                					
//Return:   											
//Date:  
//author:		    song.jiang
/**********************************************************************/
void E2promDisplay(void)
{
	uint8 i = 0;
	uint8 j = 0;
	printf("eeprom data(00-FF):\r\n");
	for(i=0; i<8; i++)
	{
		printf("%x:", i*8);
		for(j=0; j<8; j++)
		{
			printf("%x ", E2promRead(i*8+j));
		}
		printf("\n");
	}
}
#endif


/**********************************************************************/
//Description:		SysInit														  
//Parameters:                        												  
//Return:   											
//Date:  
//author:		    song.jiang
/**********************************************************************/
static void SysInit(void)
{
	uint16_t BatVol = 0;
#ifdef SUPPORT_SPEAK_FUNCTION
	uint8_t i = 0;
#endif

	SysClkCfg();
	STimerInit();
	TimerTb0Init(eTbnTime1024ms);
	TmrInitHeader();
	SystemVarInit();
	SetupGpio();
	
	BeepInit();
	
#ifdef SUPPORT_UART
	UsartCfg();
#endif

	KeyInit();	
	PwrInit();
	SysWakeUpInit(eWakeupTime8192ms);
	SmokeDetInit();
	InitCO_Data();	
	//EMI  INTC0(BIT0)5
	//global interrupt enable
	GpioSetBits(_intc0,GPIOBIT0_SELECT);
	

#ifdef SENSOR_TEMP_TEST
	printf("\nTemp Test\n");
#endif

#ifdef SUPPORT_DEBUG
	//model info version info
	printf("X-Sense Model: %s,Ver: %s\n", MODEL_NAME, FW_VERSION);
#if defined SUPPORT_UL2034_UL217
	printf("UL(USA)");
#else
	printf("EN(EU)");
#endif
	printf("  Threshold = %dmV\n", SMOKE_ALARM_LEVEL);
	E2promDisplay();
#endif

	BatVol = BatteryCheckHandle();
#ifdef SUPPORT_DEBUG
	printf("Bat:");
	PrintVol(BatVol);
	printf("mV\n");
#endif

#ifdef SUPPORT_SPEAK_FUNCTION
	SpeakerInit();
#endif

#ifdef SUPPORT_RF_NET_FUNCTION
	RfInit();

#ifdef SUPPORT_SPEAK_FUNCTION
	if(SET_KEY_PIN==0)
	{
		GCC_DELAY(150000);		// 150ms
		if(SET_KEY_PIN==0)
		{
			for(i=0; i<8; i++)
			{
				SpeakerSendCmd(CMD_SPEAKER_TEST);
				_clrwdt();
			}
		}
	}
#endif
#ifdef SUPPORT_XP02_BOARD
	#ifdef SUPPORT_CO_FUNCTION
		if ((TEST_KEY_PIN==0) && (LoadSmokeData() == TRUE) &&
				(LoadCOData() == TRUE))
	#else
		if ((TEST_KEY_PIN==0) && (LoadSmokeData() == TRUE))
	#endif
	{
		GCC_DELAY(150000);		// 150ms
		if(TEST_KEY_PIN==0)
		{
			SysAO.eMode = RadioFactoryMode;
			SysteSetSig(eEventSigEnteryMode);
		}
	}
	_papu4 = 1;
#else
	#ifdef SUPPORT_CO_FUNCTION
		if ((PAIR_KEY_PIN==0) && (LoadSmokeData() == TRUE) &&
				(LoadCOData() == TRUE))
	#else
		if ((PAIR_KEY_PIN==0) && (LoadSmokeData() == TRUE))
	#endif
	{
		GCC_DELAY(150000);		// 150ms
		if(PAIR_KEY_PIN==0)
		{
			SysAO.eMode = RadioFactoryMode;
			SysteSetSig(eEventSigEnteryMode);
		}
	}
	_papu0 = 1;
#endif
	
#endif
}


uint16_t GetTicks(void)
{
	uint16_t Ret = 0;
	static uint16_t lastTick10ms = 0;
	uint16_t  Tmp = 0;
	uint16_t  tick = ((uint16_t)SystemTimerH<<8) + (uint16_t)SystemTimerL; 
	uint16_t  dTick = (tick>=lastTick10ms)?(tick-lastTick10ms):(SYSTEMTICKS-lastTick10ms+tick);
	
	if (dTick < 320)//<10ms
	{   
		Ret = 0;
	}
	else//10ms
	{   
		Ret = dTick/320;
		Tmp = (dTick%320);
		lastTick10ms = (tick>Tmp)?(tick-Tmp):(SYSTEMTICKS-Tmp+tick);
	}
	return (Ret);
}


/**********************************************************************/
//Description:		SensorDataCheck														  
//Parameters:                        												  
//Return:   											
//Date:  
//author:		   song.jiang
/**********************************************************************/
static void SensorDataCheck(void)
{
	if((SmokeAO.SmokeCariFlag == SMOKE_CARIBRATION_FLAG) && 
			((SysAO.SlpTimeCnt%SMOKE_ALARM_CHECK_COUNT) == (SMOKE_ALARM_CHECK_COUNT-1)))
	{
		SmokeAlarmCheck();
	}

#ifdef SUPPORT_CO_FUNCTION
	if((COAO.CoCariFlag == CO_CARIBRATION_FLAG) && 
								((COAO.fault_short_count)||(COAO.fault_open_count)))//error
	{
		COSelfCheck(&COAO);
	}
	else//OK
	{
		if(COAO.CoCariFlag == CO_CARIBRATION_FLAG)
		{
			if(((SysAO.SlpTimeCnt%CO_CHECK_COUNT)==(CO_CHECK_COUNT-1))) 			//30s time  check
			{
				CoAlarmCheck();
			}
			
			if(((SysAO.SlpTimeCnt%CO_Self_CHECK_COUNT)==(CO_Self_CHECK_COUNT-1))) 	//60s time check 
			{ 
				COSelfCheck(&COAO);
			}
		}
	}
#endif
}

/**********************************************************************/
//Description:		LedFlickerCheck	 for  standby led index													  
//Parameters:                        												  
//Return:   											
//Date:  
//author:		   song.jiang
/**********************************************************************/

static void LedFlashEventHandle(void)
{
	if((SysAO.eEventFlg & eEventFlagLowBat) || (SysAO.eEventFlg & eEventFlagLowBatForbitMute) 
															|| (SysAO.eEventFlg & eEventFlagLowBatMute))	//Lowbat
	{
		if(( SysAO.StdbyTimeCnt%STANDBY_MODE_CYCLE_COUNT) == (STANDBY_MODE_CYCLE_COUNT-1))
		{
#ifdef SUPPORT_SYSTEM_MODE_DEBUG
			printf("low bat event\n");
#endif

			if(SysAO.eEventFlg & eEventFlagLowBatMute)
			{
				ErrIndex(1, 0, 1);
			}
			else
			{
#ifdef SUPPORT_SPEAK_FUNCTION
				if(SysAO.LowPowerSpeakTimeCnt < 15)
				{
					SysAO.LowPowerSpeakTimeCnt++;
					SpeakerSendCmd(CMD_REPLACE_BAT_LOW_BAT);
				}
#endif
				ErrIndex(1, 1, 1);
			}
		}
	}
	else if((SysAO.eEventFlg & eEventFlagEndOfLife) || (SysAO.eEventFlg & eEventFlagEndOfLifeForbitMute)
															|| (SysAO.eEventFlg & eEventFlagEndOfLifeMute))	//Endlife
	{
		if(( SysAO.StdbyTimeCnt%STANDBY_MODE_CYCLE_COUNT) == (STANDBY_MODE_CYCLE_COUNT-1))
		{
#ifdef SUPPORT_SYSTEM_MODE_DEBUG
			printf("end life event\n");
#endif

			if(SysAO.eEventFlg & eEventFlagEndOfLifeMute)
			{
				ErrIndex(1, 0, 3);
			}
			else
			{
#ifdef SUPPORT_SPEAK_FUNCTION
				if(SysAO.EndLifeSpeakTimeCnt < 15)
				{
					SysAO.EndLifeSpeakTimeCnt++;
					SpeakerSendCmd(CMD_END_LIFE);
				}
#endif
				ErrIndex(1, 1, 3);
			}
		}
	}
	else
	{
#ifdef SUPPORT_SYSTEM_MODE_DEBUG
		printf("standby\n");
#endif
		if(( SysAO.StdbyTimeCnt%STANDBY_MODE_CYCLE_COUNT) == (STANDBY_MODE_CYCLE_COUNT-1))				//None
		{
			SysAO.TempData = GetTempIndex();
			G_LED_ON();
			GCC_DELAY(LED_ON_TIME_STANDBY);
			G_LED_OFF();
		}
	}
	
	SysAO.StdbyTimeCnt++;
	if( SysAO.StdbyTimeCnt >= STANDBY_MODE_CYCLE_COUNT)
	{
		SysAO.StdbyTimeCnt = 0;
	}
}

void ErrIndex(uint8_t LedFlag,uint8_t BeepFlag,uint8_t Num)
{
	uint8 i=0;
	G_LED_OFF();
	GCC_DELAY(150000);		// 150ms
	for(i=0; i<Num; i++)			//beep num times
	{
		if(LedFlag)
		{
			Y_LED_ON();
		}
		if(BeepFlag)
		{
			BEEP_ON();
		}
		GCC_DELAY(30000);
		if(LedFlag)
		{
			Y_LED_OFF();
		}
		if(BeepFlag)
		{
			BEEP_OFF();
		}				
		GCC_DELAY(150000);		// 150ms
		GCC_DELAY(150000);		// 150ms
	}
}


/**********************************************************************/
//Description:		LoadData for calibration 							
//Parameters:                        									
//Return:   											
//Date:  
//author:		    song.jiang
/**********************************************************************/
void PowerOnDisplay(void)
{
	uint8_t i = 0;
	for(i=0; i<8; i++)
	{
		Y_LED_ON();
		_clrwdt();
		GCC_DELAY(250000);
		GCC_DELAY(250000);
		Y_LED_OFF();

		G_LED_ON();
		_clrwdt();
		GCC_DELAY(250000);
		GCC_DELAY(250000);
		G_LED_OFF();

		R_LED_ON();
		_clrwdt();
		GCC_DELAY(250000);
		GCC_DELAY(250000);
		R_LED_OFF();

		_clrwdt();
		GCC_DELAY(250000);
		GCC_DELAY(250000);
	}
	
}

/**********************************************************************/
//Description:		PowerOnWelcomeAndDisplay
//Parameters:                        								
//Return:   											
//Date:  
//author:		   quanwu.xu
/**********************************************************************/
void PowerOnWelcomeAndDisplay(void)
{
	if(SysAO.PowerTestFlag == FALSE)
	{
 		PowerOnDisplay();
#ifdef SUPPORT_SPEAK_FUNCTION
		SpeakerSendCmdForPowerOn(CMD_WELCOME_HELLO);
		if(RfAO.TxLocation == LocationMsgNoLocation)
		{
			SpeakerSendCmdForPowerOn(CMD_LOCATION_SET_GUIDE);
		}
		else
		{
			SpeakerSendCmdForPowerOn(CMD_TEST_OPERATION_GUIDE);
 		}
#endif
	}
}

/**********************************************************************/
//Description:		CailRes
//Parameters:                        								
//Return:   											
//Date:  
//author:			quanwu.xu
/**********************************************************************/
void CailRes(CailResult Res, eCailStep eStep)
{
	uint8_t i = 0;
	G_LED_OFF();
	R_LED_OFF();

	PwrAO.standbycnt = LowPwrMaxCnt-2;
	EnteryLowPower();
	_clrwdt();
	if(Res == CailFail) 					//fail
	{
		switch (eStep)
		{
			case eCail_Smoke:				//Y-G
				Y_LED_ON();
				GCC_DELAY(50000);
				Y_LED_OFF();
				GCC_DELAY(150000);
				GCC_DELAY(150000);
				G_LED_ON();
				GCC_DELAY(50000);
				G_LED_OFF();
				break;
				
			case eCail_Co_Step1:			//G-R
				G_LED_ON();
				GCC_DELAY(50000);
				G_LED_OFF();
				GCC_DELAY(150000);
				GCC_DELAY(150000);
				R_LED_ON();
				GCC_DELAY(50000);
				R_LED_OFF();
				break;
				
			case eCail_Co_Step2:			//R-G
				R_LED_ON();
				GCC_DELAY(50000);
				R_LED_OFF();
				GCC_DELAY(150000);
				GCC_DELAY(150000);
				G_LED_ON();
				GCC_DELAY(50000);
				G_LED_OFF();
				break;
				
			case eCail_Co_Step3:			//R-Y
				R_LED_ON();
				GCC_DELAY(50000);
				R_LED_OFF();
				GCC_DELAY(150000);
				GCC_DELAY(150000);
				Y_LED_ON();
				GCC_DELAY(50000);
				Y_LED_OFF();
				break;
		
			case eCail_Temp_Err:			//Y-R
				Y_LED_ON();
				GCC_DELAY(50000);
				Y_LED_OFF();
				GCC_DELAY(150000);
				GCC_DELAY(150000);
				R_LED_ON();
				GCC_DELAY(50000);
				R_LED_OFF();
				break;
		
			case eCail_BatErr:				//R-Y-G
				R_LED_ON();
				GCC_DELAY(50000);
				R_LED_OFF();
				GCC_DELAY(150000);
				GCC_DELAY(150000);
				Y_LED_ON();
				GCC_DELAY(50000);
				Y_LED_OFF();
				GCC_DELAY(150000);
				GCC_DELAY(150000);
				G_LED_ON();
				GCC_DELAY(50000);
				G_LED_OFF();
				break;
		
			default:
				break;
		}
	}
	else									//success
	{
		for(i=0; i<eStep; i++)
		{
			_clrwdt();
			G_LED_ON();
			GCC_DELAY(50000);
			G_LED_OFF();
			GCC_DELAY(200000);
			GCC_DELAY(200000);
		}
	}
}


/**********************************************************************/
//Description:		CailStepDisplay
//Parameters:                        								
//Return:   											
//Date:  
//author:			quanwu.xu
/**********************************************************************/
void CailStepDisplay(eCailStep eStep)
{
	uint8_t i = 0;
	G_LED_OFF();
	R_LED_OFF();
	Y_LED_OFF();
	
	for(i=0; i<eStep; i++)
	{
		_clrwdt();
		Y_LED_ON();
		GCC_DELAY(50000);
		Y_LED_OFF();
		GCC_DELAY(200000);
		GCC_DELAY(200000);
	}
	GCC_DELAY(200000);
}

/**********************************************************************/
//Description:		SysTimeUpdate	for update time by 8s													  
//Parameters:                        												  
//Return:   											
//Date:  
//author:		    SONG
/**********************************************************************/
static void SysTimeUpdate(void)
{
	SysAO.SlpTimeCnt++;

	if(SysAO.SlpTimeCnt>ONE_DAY)											// 10800=1 day
	{
		SysAO.SlpTimeCnt = 0;
		SysAO.LifeTimeUpdateFlag = 1;
	}
	
	if(SysAO.SlpTimeCnt%SMOKE_CALI_COUNT==(SMOKE_CALI_COUNT-1))				//Smoke Compensation
	{
		SmokeAO.SmokeCompensationFlag = 1;
	}

	if(SysAO.LifeTimeUpdateFlag==1) 										//(1 day), Save
	{
		SysAO.LifeTimeUpdateFlag = 0;
		SysAO.LifeTimeDays++;

		E2promWrite(FLASH_ADDR_LIFE_TIME_DAYS_MSB, SysAO.LifeTimeDays >> 8);
		E2promWrite(FLASH_ADDR_LIFE_TIME_DAYS_LSB, SysAO.LifeTimeDays & 0xff);
	}

	if(SysAO.LifeTimeDays>LIFE_TIME_FORBIT_MUTE)							//Life end, forbit mute
	{
		SysAO.eEventFlg |= eEventFlagEndOfLifeForbitMute;
	}
	else if(SysAO.LifeTimeDays>LIFE_TIME)									//Life end
	{
		if(SysAO.eEventFlg & eEventFlagEndOfLifeMute)
		{
			if(SysAO.EventMuteTimeCnt>LIFETIME_MUTE_COUT)
			{
				SysAO.eEventFlg &= ~eEventFlagEndOfLifeMute;
				SysAO.eEventFlg |= eEventFlagEndOfLife;
			}
			else
			{
				SysAO.EventMuteTimeCnt++;
			}
		}
		else
		{
			SysAO.eEventFlg |= eEventFlagEndOfLife;
		}
	}

#ifdef SUPPORT_LIFE_TIME_DEBUG
	if((SysAO.SlpTimeCnt%450) == 0) 									//60min=60*60=3600/8=450
	{
		printf("Days: %d,\n", SysAO.LifeTimeDays);
		printf("Sec:%d\n", SysAO.SlpTimeCnt);
	}
#endif
}

/**********************************************************************/
//Description:		PollingLED	for one  led on and off 													  
//Parameters:          time for on time              												  
//Return:   											
//Date:  
//author:		    SONG
/**********************************************************************/
void PollingLED(uint16 Time)
{
	G_LED_OFF();
	R_LED_ON();	
	TmrInsertTimer(eTmrHdlLedOff, TMR_PERIOD(Time), NULL);
}

void PollingLEDOff(void)
{
	R_LED_OFF();
}

/**********************************************************************/
//Description:		PollingBeep	for one  beep on and off 				
//Parameters:            time for on time            					
//Return:   											
//Date:  
//author:		    SONG
/**********************************************************************/

void PollingBeep(uint16 Time)  
{
	BEEP_ON();
	TmrInsertTimer(eTmrHdlBeepOff, TMR_PERIOD(Time), NULL);
}

void PollingBeepOff(void)
{
	BEEP_OFF();
}


void PollingGreenLED(uint16 Time)
{
	R_LED_OFF();
	G_LED_ON();
	TmrInsertTimer(eTmrHdlGreenLedOff, TMR_PERIOD(Time), NULL);
}

void PollingGreenLEDOff(void)
{
	G_LED_OFF();
}

void PollingGREENLEDFlicker(void* Time)
{
	G_LED_ON();
	PollingGreenLED((uint16)Time);
}

void PollingRFLED(uint16 Time)
{
	R_LED_ON();	
	TmrInsertTimer(eTmrHdlLedOff, TMR_PERIOD(Time), NULL);
	TmrInsertTimer(eTmrHdlGreenLedFlicker, TMR_PERIOD(800), (void*)200);
}

void SysteSetSig(eEventSig sig)
{
	uint8_t i = 0;
	for(i=0;i<SIGQUEUE;i++)
	{
		if(0==SysAO.sigQue[i].atrr)
		{
			SysAO.sigQue[i].sig = sig;
			SysAO.sigQue[i].atrr = 1;		
			break;
		}
	}
	if(i>=SIGQUEUE)
	{
		SysAO.sigQue[0].sig = sig;
		SysAO.sigQue[0].atrr = 1;
	}
	
}

eEventSig SysteGetSig(void)
{
	uint8_t i = 0;
	//	uint8_t sig = eEventSigNULL;
	eEventSig sig = eEventSigNULL;
	for(i=0;i<SIGQUEUE;i++)
	{
		if(1==SysAO.sigQue[i].atrr)
		{
			sig = SysAO.sigQue[i].sig;
			SysAO.sigQue[i].atrr = 0;
			break;
		}
	}
	return sig;
}


DEFINE_ISR(Lvd_Int_ISR, 0x14)
{
	
}


DEFINE_ISR(Timebase0_Int_ISR, 0x24)
{
	_tb0f=0;
}

DEFINE_ISR(Adc_Input_Int_ISR, 0x08)
{
	_int0f = 0;
}

DEFINE_ISR(Dec_Int_ISR, 0x0C)
{
	_int1f = 0;
}

#if 0
/**********************************************************************/
//Description:		main	for entery									
//Parameters:                        									
//Return:   											
//Date:  
//author:		    song.jiang
/**********************************************************************/
DEFINE_ISR(STM1_Int_ISR, 0x30)
{
	_stm1dl = 0x00;
	_stm1dh = 0x00;

	_mf3f = 0;
	_stm1af = 0;
}
#endif

#ifdef SUPPORT_SPEAK_FUNCTION

typedef enum _eSetLocationTimeOut
{
	eSetLocationNull = 0,
	eSetLocationDealySpeak,
}eSetLocationTimeOut;

/**********************************************************************/
//Description:	PollingSetLocation()									
//Parameters:                        										
//Return:   																
//Date:         quanwu.xu  
/**********************************************************************/
void PollingSetLocation(void *pTxt)
{
	uint16_t Temp = (uint16_t)pTxt;
	switch (Temp)
	{
		case eSetLocationNull:
			SysteSetSig(eEventSigSetLocationFail);
			break;
			
		default:
			break;
	}
}

/**********************************************************************/
//Description:	PollingSetLocation()									
//Parameters:                        										
//Return:   																
//Date:         quanwu.xu  
/**********************************************************************/
void PollingSetLocationDisplay(void)
{
	PollingLED(250);
}

/**********************************************************************/
//Description:		LocationSettingMain									
//Parameters:                        									
//Return:   											
//Date:  
//author:		    song.jiang
/**********************************************************************/
void LocationSettingMain(void)
{
	static bool Flag = FALSE;
	static uint8_t SetLocation = LocationMsgEntryway;
	
	switch(SysAO.sig)
	{
		case eEventSigEnteryMode:
			Flag = FALSE;
			SetLocation = RfAO.TxLocation;
		
			if(SetLocation == LocationMsgNoLocation)
			{
				SpeakerSendCmd(CMD_LOCATION_NOT_SET);
			}
			else
			{
				SpeakerSendCmd(((uint8_t)SetLocation<<8) | CMD_LOCATION_CHANGE);
			}
			SetLocation = LocationMsgEntryway - 1;
			
			TmrInsertTimer(eTmrHdlSetLocation, TMR_PERIOD(60000), (void *)eSetLocationNull);
			TmrInsertTimer(eTmrHdlSetLocationDisplay, TMR_PERIOD(3000), NULL);
			break;

		case eEventSigKey3Clik:
			if(Flag == FALSE)
			{
				Flag = TRUE;
				SpeakerSendCmd(CMD_LOCATION_SELECT_NEXT);
			}
			else
			{
				if(SetLocation < LocationMsgNoLocation)
				{
					SetLocation++;
				}
				else
				{
					SetLocation = LocationMsgEntryway;
				}
				SpeakerSendCmd((uint8_t)SetLocation);
			}
			TmrInsertTimer(eTmrHdlSetLocation, TMR_PERIOD(60000), (void *)eSetLocationNull);
			break;


		case eEventSigKey3DoubleClik:
			TmrInsertTimer(eTmrHdlSetLocation, TMR_PERIOD(60000), (void *)eSetLocationNull);
			if(SetLocation > LocationMsgEntryway)
			{
				SetLocation--;
			}
			else
			{
				SetLocation = LocationMsgNoLocation;
			}
			SpeakerSendCmd((uint8_t)SetLocation);
			break;

		case eEventSigKey3LongClik:
			if(Flag == FALSE)
			{
				Flag = TRUE;
				SpeakerSendCmd(CMD_LOCATION_SELECT_NEXT);
			}
			else
			{
				TmrDeleteTimer(eTmrHdlSetLocation);
				TmrDeleteTimer(eTmrHdlSetLocationDisplay);
				RfAO.TxLocation = SetLocation;
				E2promWrite(FLASH_ADDR_SELF_LOCATION, RfAO.TxLocation);
				//SpeakerSendCmd((uint8_t)WiredNetAO.SelfLocation);
				SpeakerSendCmd(((uint8_t)RfAO.TxLocation<<8) | CMD_LOCATION_SAVED);
				SysAO.eMode = NormalMode;
			}
			break;

		case eEventSigSetLocationFail:
			TmrDeleteTimer(eTmrHdlSetLocation);
			TmrDeleteTimer(eTmrHdlSetLocationDisplay);
			SpeakerSendCmd(CMD_UNABLE_SET_LOCATION);
			SysAO.eMode = NormalMode;
			break;

		default:
			break;
	}
}

#endif

/**********************************************************************/
//Description:		NormalMain									
//Parameters:                        									
//Return:   											
//Date:  
//author:		    song.jiang
/**********************************************************************/
void NormalMain(void)
{
	switch(SysAO.sig)
	{
		case eEventSigEnteryMode:
			SysAO.StdbyTimeCnt = 0;
			break;
	#ifdef SUPPORT_RF_NET_FUNCTION
		case eEventSigKey1Clik:
	#else
		case eEventSigKey1Clik:
		case eEventSigKey1LongClik:
	#endif
			if(SysAO.eEventFlg & eEventFlagLowBat)
			{
				SysAO.eEventFlg &= ~eEventFlagLowBat;
				SysAO.eEventFlg |=eEventFlagLowBatMute;
				SysAO.EventMuteTimeCnt = 0;
#ifdef SUPPORT_SPEAK_FUNCTION
				SpeakerSendCmd(CMD_ENTRY_MUTE_MODE);
#else
				ErrIndex(1, 0, 1);
#endif
			}
			else if(SysAO.eEventFlg & eEventFlagEndOfLife)
			{
				SysAO.eEventFlg &= ~eEventFlagEndOfLife;
				SysAO.eEventFlg |=eEventFlagEndOfLifeMute;
				SysAO.EventMuteTimeCnt = 0;
#ifdef SUPPORT_SPEAK_FUNCTION
				SpeakerSendCmd(CMD_ENTRY_MUTE_MODE);
#else
				ErrIndex(1, 0, 3);
#endif
			}
			else
			{
				SysAO.eMode = SmokeTestMode;
			}
			break;

#ifdef SUPPORT_SPEAK_FUNCTION
		case eEventSigKey3Clik:
			SysAO.eMode = LocationSetMode;
			break;

		case eEventSigKey3LongClik:
			RfAO.TxLocation = LocationMsgNoLocation;
			E2promWrite(FLASH_ADDR_SELF_LOCATION, RfAO.TxLocation);
			SpeakerSendCmd(CMD_UNABLE_SET_LOCATION);
			break;
#endif

#ifndef SENSOR_TEMP_TEST
#ifdef SUPPORT_RF_NET_FUNCTION
		case eEventSigKey1LongClik:
			if(GetRfPairedMode() == ePairedIntranet)
			{
				SysAO.eMode = RadioIntranetMasterTestMode;
			}
			else
			{
				SysAO.eMode = SmokeTestMode;
			}
			break;

		case eEventSigRadioHubTest:
			SysAO.eMode = RadioHubTestMode;
			break;

		case eEventSigKey1DoubleClik:
			SysAO.eMode = RadioPairSlaverMode;
			break;
		
		case eEventSigKey1FourTimesClik:
			SysAO.eMode = RadioPairMasterMode;
			break;

		case eEventSigRadioIntranetTestStart:
			SysAO.eMode = RadioIntranetSlaverTestMode;
			break;

		case eEventSigRadioSmokeAlarm:	
			SysAO.eMode = RadioSmokeAlarmMode;
			break;
		
		case eEventSigRadioCoAlarm: 
			SysAO.eMode = RadioCoAlarmMode;
			break;
#endif

		case eEventSigSmokeAlarm:
			SysAO.eMode = SmokeAlarmMode;
			break;
		
		case eEventSigSmokeErr:
			SysAO.eMode = SmokeErrMode;
			break;

#ifdef SUPPORT_CO_FUNCTION
		case eEventSigCoAlarm:
			SysAO.eMode = CoAlarmMode;
			break;
		
		case eEventSigCoErr:
			SysAO.eMode = CoErrMode;
			break;
#endif

#endif
		case eEventSigWakeup:
			SysTimeUpdate();
			BatteryCheck();
			SensorDataCheck();
			LedFlashEventHandle();
			break;

		default:
			break;
	}
}

/**********************************************************************/
//Description:		main	
//Parameters:	
//Return:
//Date:  
//author:		    quanwu.xu
/**********************************************************************/
void main(void)
{
	SysInit();

	while(1)
	{
		_clrwdt();
		Polling();
	#ifdef SUPPORT_RF_NET_FUNCTION
		PollingUartSend();
	#endif	
		KeyHandleMsg();
	#ifdef SUPPORT_RF_NET_FUNCTION
		UartService();
	#endif
		SysAO.sig = SysteGetSig();
		if(eEventSigNULL!=SysAO.sig)
		{
#ifdef SUPPORT_SYSTEM_MODE_DEBUG
			printf("sig: %d, ",SysAO.sig);
			printf("eMode=%d\n", SysAO.eMode);
			printf("Event=%d\n", SysAO.eEventFlg);
#endif
			switch(SysAO.eMode)
			{

				case NormalMode:
					NormalMain();
					break;
				
				case SmokeCailMode:
					SmokeCaliMain();
					break;

				case SmokeTestMode:
			#ifdef SUPPORT_RF_NET_FUNCTION
				case RadioHubTestMode:
			#endif
					SmokeTestMain();
					break;
				
				case SmokeAlarmMode:
					SmokeAlarmMain();
					break;
				
				case SmokeMuteMode:
					SmokeMuteMain();
					break;

				case SmokeErrMode:
					SmokeErrMain();
					break;


#ifdef SUPPORT_CO_FUNCTION
				case CoCailMode:
					CoCaliMain();
					break;
#endif

				case CoTestMode:
					CoTestMain();
					break;
			
#ifdef SUPPORT_CO_FUNCTION
				case CoAlarmMode:
					CoAlarmMain();
					break;

				case CoErrMode:
					CoErrMain();
					break;

				case CoMuteMode:
					CoMuteMain();
					break;
#endif

#ifdef SUPPORT_RF_NET_FUNCTION
				case RadioFactoryMode:
				case RadioPairSlaverMode:
				case RadioPairMasterMode:
				case RadioPairHubMode:
					RfPairMain(SysAO.eMode);
					break;

				case RadioSmokeAlarmMode:
					RfSmokeAlarmMain();
					break;
				
				case RadioCoAlarmMode:
					RfCoAlarmMain();
					break;

				case RadioIntranetMasterTestMode:
				case RadioIntranetSlaverTestMode:
					RfIntranetTestMain(SysAO.eMode);
					break;
#endif

#ifdef SUPPORT_SPEAK_FUNCTION
				case LocationSetMode:
					LocationSettingMain();
					break;
#endif

				default:
					break;
			}
			
			if(SysAO.eOldMode != SysAO.eMode)
			{
				SysteSetSig(eEventSigEnteryMode);
				SysAO.eOldMode = SysAO.eMode;
			}
		}
		else if (SysAO.haltFlag == eHaltFlagNull)
		{
			EnteryLowPower();
		}
	}
}




