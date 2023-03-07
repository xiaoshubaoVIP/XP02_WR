/****************************************************************************/
/*	File   :             Smoke.c               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/7/25,16:26:22                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/
#include "base.h"
#include "Module.h"
#include "main.h"
#include "E2prom.h"

//smoke alarm level
#define SMOKE_ALARM_LEVEL_CANCEL			(70)
#define TEST_ALARM_LEVEL					(SMOKE_ALARM_LEVEL+150)
#define SMOKE_OPA_MIN						(3)//100mA=5*10+50
#define SMOKE_OPA_CURRENT					(9)//120mA=SMOKE_OPA_CURRENT*10+50
#define SMOKE_OPA_MAX 						(25)//180mA=13*10+50

#define SMOKE_CALIBRATION_SUCESS_MIN 		(500)
#define SMOKE_CALIBRATION_SUCESS_MAX 		(SMOKE_LIGHT_SUCESS_MAX-SMOKE_ALARM_LEVEL)		//
#define SMOKE_DARK_SUCESS_MIN				(0)
#define SMOKE_DARK_SUCESS_MAX				(50+SMOKE_DARK_SUCESS_MIN)						//
#define SMOKE_LIGHT_SUCESS_MIN				(500)
#ifdef SUPPORT_XP02_BOARD
#define SMOKE_LIGHT_SUCESS_CURRENT			(1400)     //1400-->900
#else
#define SMOKE_LIGHT_SUCESS_CURRENT			(900)     //1400-->900
#endif
#define SMOKE_LIGHT_SUCESS_MAX				(2000)
#define SMOKE_ERR_VALUE						(300)

#define SMOKE_CALIBRATION_DUST_INCREASE		(2)

//smoke set
#define EMC_DARK_BUFFER                         (70)     // for EMC buffer area


typedef enum _eSmokeTimeOut
{
	eSmokeCail = 0,
	eSmokeTest,
	eSmokeAlarm,
	eSmokeAlarmInterVal,
	eSmokeErr,
	eSmokeNetAlarmDelayCheck,
	eSmokeDelayInCoTest,
	eSmokeDelayInCoMasterTest,
	eSmokeExitAlarm,
	eSmokeExitMute,
}eSmokeTimeOut;

#define ADCSampleMaxNum  (2)
#define SmokeVddDiff	(20)
#define SmokeCaliAlarmNumMax (38)// counter for time 
#define SmokeCaliAlarmLevel (SMOKE_ALARM_LEVEL>>2)// alarm zone check
#define SmokeCaliCheckNum (3)// Three consecutive times
#define SmokeCaliLevel (30)

SmokeObjectTag SmokeAO;

#ifdef TEMPCALIOLD
	#define TEMPNUM (91)
	const uint16 NtcTbl[TEMPNUM]=
	{ 
		3741, 3721, 3699, 3677, 3654,  //-20?? ~ -16??
		3630, 3605, 3578, 3552, 3525,  //-15?? ~ -11??
		3496, 3466, 3435, 3404, 3371,  //-10?? ~ -6?? 
		3338, 3303, 3268, 3232, 3195,  //-5?? ~ 1??
		3157, //0?? 
		3118, 3079, 3038, 2998, 2956, //1?? ~ 5??
		2914, 2871, 2827, 2783, 2738, //6?? ~ 10??
		2694, 2648, 2603, 2557, 2511, //11?? ~ 15??
		2464, 2418, 2371, 2325, 2279, //16?? ~ 20??
		2232, 2186, 2140, 2094, 2048, //21?? ~ 25??
		2003, 1958, 1913, 1881, 1825, //26?? ~ 30??
		1782, 1739, 1697, 1655, 1614, //31?? ~ 35??
		1574, 1534, 1495, 1457, 1419, //36?? ~ 40??
		1383, 1345, 1310, 1275, 1241, //41?? ~ 45??
		1208, 1175, 1143, 1112, 1082, //46?? ~ 50??
		1052, 1023, 995,  967,  940,  //51?? ~ 55??
		913,  888,  863,  839,  816,  //56?? ~ 60??
		793,  770,  749,  728,  707,  //61?? ~ 65??
		687,  668,  649,  631,  613,  //66?? ~ 70??
	};
		
#ifdef SUPPORT_XP02_BOARD
	const uint16 SmokeTempRate[TEMPNUM]=
	{
		710,715,720,725,735,		//-20~-16
		745,755,763,769,774,		//-15~-13
		780,786,792,798,803,		//-10~-6
		810,816,822,828,834,		//-5~-1
		841,						//0
		847,853,859,866,872,		//1~5
		878,884,891,898,905,		//6~10
		911,917,923,929,936,		//11~15
		942,949,956,962,969,		//16~20
		975,981,987,994,1000,		//21~25
		1006,1012,1018,1025,1029,	//26~30
		1038,1046,1055,1061,1068,	//31~35
		1076,1082,1090,1096,1103,	//36~40
		1112,1118,1124,1131,1139,	//41~45
		1145,1153,1163,1169,1178,	//46~50
		1185,1193,1201,1209,1217,	//51~55
		1225,1231,1238,1248,1257,	//56~60
		1262,1271,1279,1289,1296,	//61~65
		1306,1313,1324,1334,1350	//66~70
	};
#else
	const uint16 SmokeTempRate[TEMPNUM]=
	{
		920,920,920,920,920,		//-20~-16
		925,930,932,934,936,		//-15~-11
		938,940,943,944,946,		//-10~-6
		948,951,953,956,957,		//-5~-1
		960,						//0
		962,963,965,966,969,		//1~5
		973,973,976,979,980,		//6~10
		982,982,985,989,990,		//11~15
		991,993,995,995,996,		//16~20
		997,997,998,998,1000,		//21~25
		1000,1000,1000,1000,999,	//26~30
		998,997,996,995,994,	//31~35
		993,993,993,994,995,	//36~40
		997,1000,1001,1002,1006,	//41~45
		1007,1011,1014,1018,1021,	//46~50
		1026,1029,1031,1036,1040,	//51~55
		1044,1048,1052,1059,1061,	//56~60
		1065,1070,1073,1081,1086,	//61~65
		1093,1098,1100,1106,1123	//66~70
	};

#endif
#else
	#define TEMPNUM (18)

	const uint16 NtcTbl[TEMPNUM]=
	{ 
		3741,   //-20?? ~ -16??
		3630,   //-15?? ~ -11??
		3496,   //-10?? ~ -6?? 
		3338,   //-5?? ~ 1??
		3118,  //1?? ~ 5??
		2914,  //6?? ~ 10??
		2694,  //11?? ~ 15??
		2464,  //16?? ~ 20??
		2232,  //21?? ~ 25??
		2003,  //26?? ~ 30??
		1782,  //31?? ~ 35??
		1574,  //36?? ~ 40??
		1383,  //41?? ~ 45??
		1208,  //46?? ~ 50??
		1052,   //51?? ~ 55??
		913,    //56?? ~ 60??
		793,   //61?? ~ 65??
		687,  //66?? ~ 70??
	};

	const uint16 SmokeTempRate[TEMPNUM]=
	{
		SM_TN_20, 
		SM_TN_15,
		SM_TN_10,
		SM_TN_5,   
		SM_TP_1, 
		SM_TP_6, 
		SM_TP_11,
		SM_TP_16,
		SM_TP_21,
		SM_TP_26,
		SM_TP_31,
		SM_TP_36,
		SM_TP_41,
		SM_TP_45,
		SM_TP_51,
		SM_TP_56,
		SM_TP_61,
		SM_TP_66,
	};

#endif

const uint16 SmokeCaliVol[SAMPLECALINUM]=
{
	SM_VOL_30,
	SM_VOL_29,
	SM_VOL_28,
	SM_VOL_27, 
	SM_VOL_26, 
	SM_VOL_25, 
};

static uint16_t SmokeVolCalibration(uint16 Data);

//;===========================================================
//;函数名称:S_ADJ_OPA
//;函数功能: 校正 OPA
//;输入: 
//;输出: 
//;备注: 
//;=========================================================

void S_ADJ_OPA(void )
{
/*    uint8_t R_TMP0 = 0;
	uint8_t R_TMP1 = 0;
	uint8_t R_TMP2 = 0;
S_ADJ_OPA_LOOP_ADD_S: 
	_sda0vos=0b11000000;

	if(_sda0o) 	R_TMP2=R_TMP2|0x01;	//R_TMP2.0=1;	
S_ADJ_OPA_LOOP_ADD:
	++R_TMP0;
	++_sda0vos;
	if(_sda0o!=(R_TMP2&0x01)) goto S_ADJ_OPA_LOOP_ADD_OK;	//R_TMP2.0
	_acc=0b00111111&_sda0vos;
	if(_acc==0b00111111) goto S_ADJ_OPA_LOOP_ADD_S;
	goto S_ADJ_OPA_LOOP_ADD;	
S_ADJ_OPA_LOOP_ADD_OK:

S_ADJ_OPA_LOOP_SUB_S:
	R_TMP1=		0b00111111;
	_sda0vos=	0b11111111;
	if(_sda0o) 	R_TMP2=R_TMP2|0x01;	//R_TMP2.0=1;
S_ADJ_OPA_LOOP_SUB:
	--R_TMP1;
	--_sda0vos;
	if(_sda0o!=(R_TMP2&0x01)) goto S_ADJ_OPA_LOOP_SUB_OK;	//R_TMP2.0
	_acc=0b00111111&_sda0vos;
	if(_acc==0b00000000) goto S_ADJ_OPA_LOOP_SUB_S;
	goto S_ADJ_OPA_LOOP_SUB;
S_ADJ_OPA_LOOP_SUB_OK:

	_sda0vos=(R_TMP1+R_TMP0)/2;
	*/
	unsigned char R_TMP0,R_TMP1;
	R_TMP0 = 0x00;
	R_TMP1 = 0x00;
	_sda0vos=0b11111111;
	GCC_DELAY(10000);
	while(_sda0o)
	{
		_sda0vos--;
		 GCC_DELAY(10000);
		 	
	}	
	R_TMP0=_sda0vos & 0b00111111;
	_sda0vos=0b11000000;
	GCC_DELAY(10000);
	while(!_sda0o)	 
	{
		_sda0vos++;
		GCC_DELAY(10000);
		
	}
	R_TMP1=_sda0vos & 0b00111111;
	_sda0vos=(R_TMP0+R_TMP1)/2;

		
}

//;===========================================================
//;函数名称:S_ADJ_OPA
//;函数功能: 校正 OPA
//;输入: 
//;输出: 
//;备注: 
//;=======================================================
void S_ADJ_OPA1(void )
{
/*	uint8_t R_TMP0 = 0;
	 uint8_t R_TMP1 = 0;
	 uint8_t R_TMP2 = 0;

S_ADJ_OPA1_LOOP_ADD_S:
	_sda1vos=0b11000000;
	GCC_DELAY(10000);
	if(_sda1o) 	R_TMP2=R_TMP2|0x01;	//R_TMP2.0=1;	
S_ADJ_OPA1_LOOP_ADD:
	++R_TMP0;
	++_sda1vos;
	GCC_DELAY(10000);
	if(_sda1o!=(R_TMP2&0x01)) goto S_ADJ_OPA1_LOOP_ADD_OK;	//R_TMP2.0
	_acc=0b00111111&_sda1vos;
	if(_acc==0b00111111) goto S_ADJ_OPA1_LOOP_ADD_S;
	goto S_ADJ_OPA1_LOOP_ADD;	
S_ADJ_OPA1_LOOP_ADD_OK:

S_ADJ_OPA1_LOOP_SUB_S:
	R_TMP1=		0b00111111;
	_sda1vos=	0b11111111;
	GCC_DELAY(10000);
	if(_sda1o) 	R_TMP2=R_TMP2|0x01;	//R_TMP2.0=1;
S_ADJ_OPA1_LOOP_SUB:
	--R_TMP1;
	--_sda1vos;
	GCC_DELAY(10000);
	if(_sda1o!=(R_TMP2&0x01)) goto S_ADJ_OPA1_LOOP_SUB_OK;
	_acc=0b00111111&_sda1vos;
	if(_acc==0b00000000) goto S_ADJ_OPA1_LOOP_SUB_S;
	goto S_ADJ_OPA1_LOOP_SUB;
S_ADJ_OPA1_LOOP_SUB_OK:

	_sda1vos=(R_TMP1+R_TMP0)/2;
	*/
	unsigned char R_TMP0,R_TMP1;
	R_TMP0 = 0x00;
	R_TMP1 = 0x00;
	_sda1vos = 0b11111111;
	GCC_DELAY(10000);
	while(_sda1o)
	{
		_sda1vos--;
		 GCC_DELAY(10000);	
		 	
	}	
	R_TMP0=_sda1vos & 0b00111111;
	_sda1vos=0b11000000;
	GCC_DELAY(10000);
	while(!_sda1o)	 
	{
		_sda1vos++;
		GCC_DELAY(10000);
		
	}
	R_TMP1=_sda1vos & 0b00111111;
	_sda1vos=(R_TMP0+R_TMP1)/2;
		
}

/**********************************************************************/
//Description:		SmokeDetInit														  
//Parameters:                        												  
//Return:   											
//Date:  
//author:		    song
/**********************************************************************/

void SmokeDetInit(void)
{

    //uint8 Vanos = 0;
    //uint8 Vanos1 = 0;
    //uint8 Vanos2 = 0;
	SmokeObjectTag *pSmokeAO = &SmokeAO;

     pSmokeAO->SmokeDarkAvg = 0;
     pSmokeAO->SmokeLightAvg = 0;
     pSmokeAO->SmokeDetCnt = 0;
     pSmokeAO->SmokeInactCnt = 0;
     pSmokeAO->SmokeTransFaultCnt = 0;
     pSmokeAO->SmokeSensortive = 0;
     pSmokeAO->SmokeBasicTemp = 0;
     pSmokeAO->SmokeCompensationData = 0;
     pSmokeAO->SmokeCompensationFlag = 0;
     pSmokeAO->SmokeCompensationCnt = 0;
     pSmokeAO->SmokeCariFlag = 0;
     pSmokeAO->SmokeSampleTimeCnt = 3;
	 pSmokeAO->SmokeCaliOPA = SMOKE_OPA_CURRENT;
	//_isgenc = 0;
	//GpioWrite(_isgenc,0); 
	//Current value (mA) = 50 + 10 x 3 = 80mA
	//Sink current generator control for ISINK0 pin
    //Current value (mA) = 50 + 10 * (ISGDATA0[4:0])
    //_isgdata1=5;
    GpioWrite(_isgdata0,5);
	GpioWrite(_isgdata1,5);                     

    //SDA0BW1~SDA0BW0(Bit1~0): SD OPAMP0 bandwidth control
    //00: 5kHz
    //01: 40kHz
    //10: 600kHz
    //11: 2MHz
    //_sda0bw1 = 1;      // 600K
    //_sda0bw0 = 0;
	GpioSetBits(_sda0c,GPIOBIT6_SELECT|GPIOBIT1_SELECT|GPIOBIT0_SELECT);//SD OPAMP0 enable 

    //SDA1BW1~SDA1BW0(Bit1~0): SD OPAMP1 bandwidth control
    //00: 5kHz
    //01: 40kHz
    //10: 600kHz
    //11: 2MHz
    //_sda1bw1 = 1;      // 600K
    //_sda1bw0 = 0;
	GpioSetBits(_sda1c,GPIOBIT6_SELECT|GPIOBIT1_SELECT|GPIOBIT0_SELECT);//SD OPAMP1 enable

    //_sda0en = 1;         
    //_sda1en = 1;          

    //_sdsw = 0x27;     //for AC
    //Demo Code is 0x77
    //_sdsw = 0x77;
    GpioWrite(_sdsw,0x27);

    //R1= SDA0PGA[5:0]*100k                
    //_sdpgac0 = 20;       // 2M
    GpioWrite(_sdpgac0,20);
    //SDA1PGA5~SDA1PGA0(Bit5~0): R2 resistance control, R2= SDA1PGA[5:0] *100k
    //_sdpgac1 = 12;       // 1.2M
     GpioWrite(_sdpgac1,13);
    //SDA1PGA7~SDA1PGA6(Bit7~6): R3 resistance control
    //00: 10k
    //01: 20k
    //10: 30k
    //11: 40k
    //_sda1pga7 = 0;     //10k
    //_sda1pga6 = 0;
	//GpioWrite(_sdpgac1,10);

    //Operational Amplifier0 Input Offset Calibration bit: 7-6
    //_sda0ofm = 1;
    //_sda0rsp = 1;
	GpioSetBits(_sda0vos,GPIOBIT7_SELECT|GPIOBIT6_SELECT);


   
    if(E2promRead(FLASH_ADDR_SMOKE_CARIBRATION_FLAG) == SMOKE_CARIBRATION_FLAG)
    {
        _sda0vos = E2promRead(FLASH_ADDR_SMOKE_VANOS_0);
    }
    else
    {
		//Vanos1 = SmokeCaliOPA(0,1);

	    //Vanos2 = SmokeCaliOPA(63,0);

	    //Vanos = (Vanos1+Vanos2) >> 1;
        //_sda0vos = Vanos;
        S_ADJ_OPA();
    }

     //Operational Amplifier1 Input Offset Calibration
    //_sda1ofm = 1;
    //_sda1rsp = 1;
	GpioSetBits(_sda1vos,GPIOBIT7_SELECT|GPIOBIT6_SELECT);


    
    if(E2promRead(FLASH_ADDR_SMOKE_CARIBRATION_FLAG) == SMOKE_CARIBRATION_FLAG)
    {
        _sda1vos = E2promRead(FLASH_ADDR_SMOKE_VANOS_1);
    }
    else
    {
    	//Vanos1 = SmokeCaliOPA1(0,1);
		//Vanos2 = SmokeCaliOPA1(63,0);

    	//Vanos = (Vanos1+Vanos2) >> 1;
        //_sda1vos = Vanos;
		S_ADJ_OPA1();
    }

    //reset these register value for op;
    //_sda0ofm = 0;
    //_sda0rsp = 0;
	GpioResetBits(_sda0vos,GPIOBIT7_SELECT|GPIOBIT6_SELECT);
    //_sda1ofm = 0;
    //_sda1rsp = 0;	
	GpioResetBits(_sda1vos,GPIOBIT7_SELECT|GPIOBIT6_SELECT);

		//adc off
	  _adcen = 0;				// adc disable

	   //OPA off
	  _sds0 = 0;
	  _sda1en = 0;		 //SD OPAMP1 enable 
	  _sda0en = 0;		 //SD OPAMP0 enable 
#ifdef SUPPORT_DEBUG
		  printf("\n_sda0vos : %d\n",_sda0vos);
		  printf("_sda1vos : %d \n",_sda1vos);	  
#endif	
	
}


/**********************************************************************/
//Description:		GetVddVol	for debug 													  
//Parameters:                        												  
//Return:   											
//Date:  
//author:		    song
/**********************************************************************/
uint16 GetVol(uint16 Vdd, uint16 HexVal)
{
	uint16 CurrVolt;
	uint32 CalTmp;

	CalTmp = Vdd;
	CalTmp = CalTmp * HexVal;
	CurrVolt = CalTmp >> 12;                                   //Curr_Volt = Vdd*HexVal / 2^n

	return CurrVolt;
}


/**********************************************************************/
//Description:		SmokeAlarm	for one smoke alarm cycle  (3 time) 													  
//Parameters:                        												  
//Return:   											
//Date:  
//author:		    song
/**********************************************************************/
void PollingSmoke(void *pTxt)
{
	uint16_t Temp = (uint16_t)pTxt;
	static uint8_t TimeCnt = 0;
	
	switch (Temp)
	{
		case eSmokeCail:
			SysteSetSig(eEventSigSmokeCailTimeout);
			break;
	
		case eSmokeTest:
			SysteSetSig(eEventSigSmokeTest);
			break;

		case eSmokeAlarm:
			TimeCnt = 0;
			SysteSetSig(eEventSigSmokeAlarmTimeout);
			break;

		case eSmokeAlarmInterVal:
			SysteSetSig(eEventSigSmokeAlarmIntervalTimeout);
			break;
		
		case eSmokeErr:
			SysteSetSig(eEventSigSmokeErrTimeout);
			break;

		case eSmokeDelayInCoTest:
			SysteSetSig(eEventSigSmokeTestDelayInCotest);
			break;

		case eSmokeExitAlarm:
			if(TimeCnt++ > 4)
			{
				SysteSetSig(eEventSigExitMode);
			}
			else
			{
				PollingGreenLED(500);
			}
			break;

		case eSmokeExitMute:
			if(TimeCnt++ > 2)
			{
				SysteSetSig(eEventSigExitMode);
			}
			else
			{
				PollingGreenLED(500);
			}
			break;

		default:
			break;
	}
}

/**********************************************************************/
//Description:		SmokeAlarm	for one smoke alarm cycle  (3 time) 													  
//Parameters:                        												  
//Return:   											
//Date:  
//author:		    song
/**********************************************************************/
void PollingMuteTimeout(void *pTxt)
{
	uint16_t MuteMode = (uint16_t)pTxt;
	switch (MuteMode)
	{
		case SmokeMuteMode:
			SysteSetSig(eEventSigSmokeMuteTimeout);
			break;
	
		case CoMuteMode:
			SysteSetSig(eEventSigCoMuteTimeout);
			break;

		default:
			break;
	}
}

#ifdef SUPPORT_ISINK0_FOR_IR
/**********************************************************************/
//Description:		AdcGetSmokeSample	for check smoke ADC Value 													  
//Parameters:       IRCtl:led on/off    IRTest:test alarm  delayTime :led on time               												  
//Return:   											
//Date:  
//author:		    song
/**********************************************************************/
void AdcGetSmokeSample(uint8 IRCtl, uint8 IRTest,uint8_t SampleTimes)
{ 

	uint8_t i = 0;
    uint16 SmokeDark = 0;
    uint16 SmokeLight = 0;
    
    //SACKS2~SACKS0(Bit 2~0): A/D conversion clock source select
    //000: fSYS
    //001: fSYS/2
    //010: fSYS/4
    //011: fSYS/8
    //100: fSYS/16
    //101: fSYS/32
    //110: fSYS/64
    //111: fSYS/128
    /*********************************************************************************************************************
     fSYS  SACKS[2:0]=   SACKS[2:0]=   SACKS[2:0]=  SACKS[2:0]=   SACKS[2:0]=    SACKS[2:0]=   SACKS[2:0]=    SACKS[2:0]=
           000(fSYS)     001(fSYS/2)   010(fSYS/4)  011(fSYS/8)   100(fSYS/16)   101(fSYS/32)  110(fSYS/64)   111(fSYS/128)
     **********************************************************************************************************************      
     1MHz  1??s          2??s          4??s         8??s          16??s *        32??s *       64??s *        128??s *
     2MHz  500ns         1??s          2??s         4??s          8??s           16??s *       32??s *        64??s *
     4MHz  250ns *       500ns         1??s         2??s          4??s           8??s          16??s *        32??s *
     8MHz  125ns *       250ns *       500ns        1??s          2??s           4??s          8??s           16??s *
    **********************************************************************************************************************/
    _sacks2 = 0; 
    _sacks1 = 0;
    _sacks0 = 1;
    

    //_vbgren = 1;
    //_vbgen = 1;
    _adcen = 1;             // AD enable

    /***********************************************************************
      SAINS[2:0](Bit 7~5)   SACS[3:0](Bit 3~0)           Input Signals            Description
      000,101~111           0000~0011                       AN0~AN3                 External pin analog input
                            0100~1111                       ??                      Non-existed channel, input is floating.
      001                   0100~1111                       VBGREF                  Internal high performance Bandgap reference voltage
      010                   0100~1111                       OPA0O                   Internal SD operational amplifier 0 output signal
      011                   0100~1111                       OPA1O                   Internal SD operational amplifier 1 output signal
      100                   0100~1111                       LINEV                   Internal PLT operational amplifier output signal
    *************************************************************************/
    _sains2 = 1;            //OPA1O 
    _sains1 = 0;
    _sains0 = 0;
    
    _sacs3 = 1;             //??adc ??????
    _sacs2 = 1;
    _sacs1 = 1;
    _sacs0 = 1;

    //SAVRS1~SAVRS0(Bit4~3): A/D converter reference voltage select
    //00: From external VREF pin
    //01: Internal A/D converter power, VDD
    //1x: From external VREF pin
    _savrs1 = 0;            //VDD 
    _savrs0 = 1; 

    _adrfs = 1;             //output data format:lsb 11 bit

    //ADE                   //interrupt
    //OPA on
    _sda0en = 1;       //SD OPAMP0 enable   
    GCC_DELAY(100);     // 100us
    _sda1en = 1;       //SD OPAMP1 enable 
    _sds0 = 1;  
    
    GCC_DELAY(900);     // 900us

    SmokeAO.SmokeDarkAvg = 3300;
	for(i=0;i<SampleTimes;++i)
	{
	    //start conversion
	    _start = 0;
	    _start = 1;
	    _start = 0;
	    
	    //wait the conversion compelete
	    while(_adbz);  

		SmokeDark= (uint16)(_sadoh);  
	    SmokeDark = (uint16)((uint16)((uint16)SmokeDark << 8) | _sadol); 
	    if(SmokeDark < SmokeAO.SmokeDarkAvg)     //get mix
		{
    		SmokeAO.SmokeDarkAvg = SmokeDark;
		}
	}
    //IR led onCurrent value (mA)=50+5*(ISGDATA1[4:0])
    #if 1
    if(IRCtl)
    {
        if(IRTest)
        {
            _isgdata0 = 20; //100
        }
        else
        {
            _isgdata0 =SmokeAO.SmokeCaliOPA;	//100mA
        }        
        _isgen = 1;        //Sink current generator enable
        _isgs0 = 1;        //Enable ISINK1 pin sink current
        ISINK0_ON();  
    }
    else
    {
        _isgdata0 = 1;
        _isgen = 1;        //Sink current generator enable
        _isgs0 = 1;        //Enable ISINK1 pin sink current  
        ISINK0_ON();          
    }
	
	GCC_DELAY(SmokeDelayDefault);    //50us

    SmokeAO.SmokeLightAvg = 0;
    _emi = 0;
   	for(i = 0;i < SampleTimes;++i)
	{
        //start conversion
        _start = 0;
        _start = 1;
        _start = 0;
        
        //wait the conversion compelete
#ifdef SUPPORT_OPA1_OUTPUT
	#ifdef SUPPORT_XP02_BOARD
		_pb5 = 1;
	#else
		_pb6 = 1;
	#endif
#endif
        while(_adbz);           
        SmokeLight= (uint16)(_sadoh);  
        SmokeLight = (uint16)((uint16)((uint16)SmokeLight << 8) | _sadol);
        if(SmokeAO.SmokeLightAvg < SmokeLight)   ////get max
		{
        	SmokeAO.SmokeLightAvg = SmokeLight;
		}
#ifdef SUPPORT_OPA1_OUTPUT
	#ifdef SUPPORT_XP02_BOARD
			_pb5 = 0;
	#else
			_pb6 = 0;
	#endif

#endif
    }
    _emi = 1;
    ISINK0_OFF();
    _isgen = 0;    
    _isgs0 = 0;		 //Disable ISINK0 pin sink current 
   #endif


/////////////////////////////////////////
//Read ISINK1_OFF/////////Current value(mA)=50+10*(ISGDATA0[4:0]) BLUE LED
////////////////////////////////////////
		//adc off
	  _adcen = 0;				// adc disable
	   //OPA off	  
	  _sda1en = 0;		 //SD OPAMP1 enable 
	  _sda0en = 0;		 //SD OPAMP0 enable    
  	  _sds0 = 0;
	  //IR led off
	  _isgen = 0;		 //Sink current generator disable
	  _isgs0 = 0;		 //Disable ISINK0 pin sink current
	  ISINK1_OFF();
	  ISINK0_OFF();
}
#else
/**********************************************************************/
//Description:		AdcGetSmokeSample	for check smoke ADC Value 													  
//Parameters:       IRCtl:led on/off    IRTest:test alarm  delayTime :led on time               												  
//Return:   											
//Date:  
//author:		    song
/**********************************************************************/
void AdcGetSmokeSample(uint8 IRCtl, uint8 IRTest,uint8_t SampleTimes)
{ 

	uint8_t i = 0;
    uint16 SmokeDark = 0;
    uint16 SmokeLight = 0;
    
    //SACKS2~SACKS0(Bit 2~0): A/D conversion clock source select
    //000: fSYS
    //001: fSYS/2
    //010: fSYS/4
    //011: fSYS/8
    //100: fSYS/16
    //101: fSYS/32
    //110: fSYS/64
    //111: fSYS/128
    /*********************************************************************************************************************
     fSYS  SACKS[2:0]=   SACKS[2:0]=   SACKS[2:0]=  SACKS[2:0]=   SACKS[2:0]=    SACKS[2:0]=   SACKS[2:0]=    SACKS[2:0]=
           000(fSYS)     001(fSYS/2)   010(fSYS/4)  011(fSYS/8)   100(fSYS/16)   101(fSYS/32)  110(fSYS/64)   111(fSYS/128)
     **********************************************************************************************************************      
     1MHz  1??s          2??s          4??s         8??s          16??s *        32??s *       64??s *        128??s *
     2MHz  500ns         1??s          2??s         4??s          8??s           16??s *       32??s *        64??s *
     4MHz  250ns *       500ns         1??s         2??s          4??s           8??s          16??s *        32??s *
     8MHz  125ns *       250ns *       500ns        1??s          2??s           4??s          8??s           16??s *
    **********************************************************************************************************************/
    _sacks2 = 0; 
    _sacks1 = 0;
    _sacks0 = 1;
    

    //_vbgren = 1;
    //_vbgen = 1;
    _adcen = 1;             // AD enable

    /***********************************************************************
      SAINS[2:0](Bit 7~5)   SACS[3:0](Bit 3~0)           Input Signals            Description
      000,101~111           0000~0011                       AN0~AN3                 External pin analog input
                            0100~1111                       ??                      Non-existed channel, input is floating.
      001                   0100~1111                       VBGREF                  Internal high performance Bandgap reference voltage
      010                   0100~1111                       OPA0O                   Internal SD operational amplifier 0 output signal
      011                   0100~1111                       OPA1O                   Internal SD operational amplifier 1 output signal
      100                   0100~1111                       LINEV                   Internal PLT operational amplifier output signal
    *************************************************************************/
    _sains2 = 1;            //OPA1O 
    _sains1 = 0;
    _sains0 = 0;
    
    _sacs3 = 1;             //??adc ??????
    _sacs2 = 1;
    _sacs1 = 1;
    _sacs0 = 1;

    //SAVRS1~SAVRS0(Bit4~3): A/D converter reference voltage select
    //00: From external VREF pin
    //01: Internal A/D converter power, VDD
    //1x: From external VREF pin
    _savrs1 = 0;            //VDD 
    _savrs0 = 1; 

    _adrfs = 1;             //output data format:lsb 11 bit

    //ADE                   //interrupt
    //OPA on
    _sda0en = 1;       //SD OPAMP0 enable   
    GCC_DELAY(100);     // 100us
    _sda1en = 1;       //SD OPAMP1 enable 
    _sds0 = 1;  
    
    GCC_DELAY(900);     // 900us

    SmokeAO.SmokeDarkAvg = 0;
	for(i=0;i<SampleTimes;++i)
	{
	    //start conversion
	    _start = 0;
	    _start = 1;
	    _start = 0;
	    
	    //wait the conversion compelete
	    while(_adbz);  

		SmokeDark= (uint16)(_sadoh);  
	    SmokeDark = (uint16)((uint16)((uint16)SmokeDark << 8) | _sadol); 
	    if(SmokeAO.SmokeDarkAvg < SmokeDark)     ////get max
		{
    		SmokeAO.SmokeDarkAvg = SmokeDark;
		}
	}
    //IR led onCurrent value (mA)=50+5*(ISGDATA1[4:0])
    #if 1
    if(IRCtl)
    {
        if(IRTest)
        {
            _isgdata1 = 20; //100
        }
        else
        {
            _isgdata1 =SmokeAO.SmokeCaliOPA;	//100mA
        }        
        _isgen = 1;        //Sink current generator enable
        _isgs1 = 1;        //Enable ISINK1 pin sink current
        ISINK1_ON();  
    }
    else
    {
        _isgdata1 = 1;
        _isgen = 1;        //Sink current generator enable
        _isgs1 = 1;        //Enable ISINK1 pin sink current  
        ISINK1_ON();          
    }
	
	GCC_DELAY(SmokeDelayDefault);    //50us

    SmokeAO.SmokeLightAvg = 0;
    _emi = 0;
   	for(i = 0;i < SampleTimes;++i)
	{
        //start conversion
        _start = 0;
        _start = 1;
        _start = 0;
        
        //wait the conversion compelete
#ifdef SUPPORT_OPA1_OUTPUT
		SMOKE_ALARM_STATE_WRITE_H_PIN = 1;
#endif
        while(_adbz);           
        SmokeLight= (uint16)(_sadoh);  
        SmokeLight = (uint16)((uint16)((uint16)SmokeLight << 8) | _sadol);
        if(SmokeAO.SmokeLightAvg < SmokeLight)   ////get max
		{
        	SmokeAO.SmokeLightAvg = SmokeLight;
		}
#ifdef SUPPORT_OPA1_OUTPUT
		SMOKE_ALARM_STATE_WRITE_H_PIN = 0;
#endif
    }
    _emi = 1;
    ISINK1_OFF();
    _isgen = 0;    
    _isgs1 = 0;		 //Disable ISINK0 pin sink current 
   #endif


	//adc off
	_adcen = 0;				// adc disable
	//OPA off	  
	_sda1en = 0;		 //SD OPAMP1 enable 
	_sda0en = 0;		 //SD OPAMP0 enable    
	  _sds0 = 0;
	//IR led off
	_isgen = 0;		 //Sink current generator disable
	_isgs1 = 0;		 //Disable ISINK1 pin sink current
	ISINK1_OFF();
	ISINK0_OFF();
}
#endif

/**********************************************************************/
//Description:		GetSmokeTemp	for temp Value 													  
//Parameters:                     												  
//Return:   			-20 to 70								
//Date:  
//author:		    song
/**********************************************************************/
uint8_t GetTempIndex(void)
{
	uint8_t index = 0;
	uint16 TempSample;
	TempSample = GetAdcSample(eAdcTempChannel, FALSE);
	while(TempSample < NtcTbl[index])
	{
		index++;
		if(index > TEMPNUM-1)
		{
			index = TEMPNUM-1;
			break;
		}
	}
#ifdef TEMP_DEBUG
	printf("T_adc:%d, ", TempSample);
	printf("T=%d\n", index);
#endif
	return  index;
}



/**********************************************************************/
//Description:		GetSmokeData	for smoke adc Value dark and light													  
//Parameters:                        												  
//Return:   			light adc increase dark adc 								
//Date:  
//author:		    song
/**********************************************************************/
uint16 GetSmokeData(void)
{
	uint32 SmokeData = 0;
	AdcGetSmokeSample(1, 0,SmokeAO.SmokeSampleTimeCnt); 

#ifdef SUPPORT_SMOKE_DEBUG
		printf("D:%d\n", SmokeAO.SmokeDarkAvg);
		printf("L:%d\n", SmokeAO.SmokeLightAvg);
#endif

	SmokeAO.SmokeDarkAvg = GetVol(PwrAO.BatVolVdd, SmokeAO.SmokeDarkAvg);	
	SmokeAO.SmokeLightAvg = GetVol(PwrAO.BatVolVdd, SmokeAO.SmokeLightAvg);	
	SmokeAO.SmokeLightAvg = SmokeVolCalibration(SmokeAO.SmokeLightAvg);

	if(SmokeAO.SmokeLightAvg <= SmokeAO.SmokeDarkAvg) 
	{
		SmokeData = 0;
	}
	else
	{
		SmokeData = SmokeAO.SmokeLightAvg-SmokeAO.SmokeDarkAvg;
	}
	
#ifdef SUPPORT_SMOKE_DEBUG
	printf("D:%dmV\n", SmokeAO.SmokeDarkAvg);
	printf("L:%dmV\n", SmokeAO.SmokeLightAvg);
#endif

	return (uint16)SmokeData;
}

/**********************************************************************/
//Description:		SmokeTempCalibration	 													  
//Parameters:        data  :check adc           												  
//Return:   		calibration adc									
//Date:  
//author:		    song
/**********************************************************************/

static uint16_t SmokeTempCalibration(uint16 Data)
{
	uint32_t SmokeData = Data;
	SmokeData=SmokeData*SmokeTempRate[SysAO.TempData];
	SmokeData=SmokeData/1000;		 //temperature	compensation

	return (uint16_t)SmokeData;
}

/**********************************************************************/
//Description:		SmokeSensorCheck									
//Parameters:                        									
//Return:   											
//Date:  
//author:		    quanwu.xu
/**********************************************************************/
bool SmokeSensorCheck(void)
{
	bool ErrFlag = FALSE;
	uint16 SmokeData = 0; 

	SmokeData = GetSmokeData();
	if(SmokeData<SMOKE_ERR_VALUE)
	{
		SmokeAO.SmokeTransFaultCnt = 8;
		ErrFlag = TRUE;
	}
	else
	{
#ifdef SUPPORT_SMOKE_DEBUG 
		printf("Smoke Test Ok\n");
#endif
	}

	return ErrFlag;
}

/**********************************************************************/
//Description:		SmokeVolCalibration	 													  
//Parameters:        data  :check vol           												  
//Return:   		 calibration vol									
//Date:  
//author:		    song
/**********************************************************************/
static uint16_t SmokeVolCalibration(uint16 Data)
{
	uint32_t SmokeData = Data;
	SmokeData=SmokeData*SmokeCaliVol[PwrAO.BatVolIndex];
	SmokeData=SmokeData/1000;		 //temperature	compensation
	return (uint16_t)SmokeData;
}

/**********************************************************************/

/**********************************************************************/
//Description:		SmokeAlarmCheck														  
//Parameters:                        												  
//Return:   											
//Date:  
//author:		    song
/**********************************************************************/
void SmokeAlarmCheck(void)
{
	static uint16 SmokeData=0;
	uint16 SmokeDarkVal = SmokeAO.SmokeDarkAvg;
	uint16_t Sensortive = 0;

	SmokeData = GetSmokeData();


	if((SmokeDarkVal>SmokeAO.SmokeDarkAvg+EMC_DARK_BUFFER) 
		|| (SmokeAO.SmokeDarkAvg>SmokeDarkVal+EMC_DARK_BUFFER)
		||(SmokeAO.SmokeDarkAvg>SmokeAO.SmokeLightAvg+EMC_DARK_BUFFER
		))
		{		 
		//for EMC, Dazzling.
		//If the adjacent two Smoke_DarkAverage changes greater than 40mV,Invalid. 
			SmokeAO.SmokeDetCnt = 0;
			SmokeAO.SmokeTransFaultCnt = 0;
			#ifdef SUPPORT_SMOKE_DEBUG	
			printf("\nSmoke EMC\n");
			printf("\n");
			#endif
	}
	else
	{
		if(SmokeData>SMOKE_ERR_VALUE)
		{
			SmokeAO.SmokeTransFaultCnt = 0;
			SmokeData = SmokeTempCalibration(SmokeData);
			if(SysAO.eMode == SmokeErrMode)					//err mode
			{

			}
			else
			{
			
#ifdef SUPPORT_SMOKE_DEBUG
				printf("Smoke:%d,",SmokeData);
				printf("AlarmCnt :%d\n",SmokeAO.SmokeDetCnt);
#endif
				if(SmokeData >= SmokeAO.SmokeSensortive)
				{ 
					SmokeAO.SmokeCompensationFlag=0;
					SmokeAO.SmokeInactCnt = 0;
					SmokeAO.SmokeDetCnt++;
					if(SmokeAO.SmokeDetCnt >= 3)
					{
#ifdef SUPPORT_SMOKE_DEBUG
						printf("Event:smoke alarm\n");
#endif
						SmokeAO.SmokeDetCnt = 3;
			#ifndef SENSOR_TEMP_TEST
						SysteSetSig(eEventSigSmokeAlarm);
			#endif
					}
					else if(SmokeAO.SmokeDetCnt == 1)
					{
						PwrAO.standbycnt = 7;
					}
					else if(SmokeAO.SmokeDetCnt == 2)
					{
						PwrAO.standbycnt = 8;
					}
				}
				else  //if(SmokeData<SmokeAO.SmokeSensortive)  // light_average-dark_average < sensertive
				{
					if((SysAO.eMode == SmokeAlarmMode) || (SysAO.eMode == SmokeMuteMode))
					{
						SmokeAO.SmokeInactCnt++;
						if(SmokeAO.SmokeInactCnt >= SmokeCancelAlarmTime)
						{
#ifdef SUPPORT_SMOKE_DEBUG
							printf("Event:cancel alarm\n");
#endif
							SmokeAO.SmokeInactCnt = 0;                   
							SysteSetSig(eEventSigSmokeAlarmCancel);
							SmokeAO.SmokeDetCnt = 0; 
						}
					}
					else // daily dusty and damping compensation
					{
						SmokeAO.SmokeDetCnt = 0; // for vaild check or check err only one time
						if( SmokeAO.SmokeCompensationFlag)
						{
							SmokeAO.SmokeCompensationFlag = 0;
							SmokeAO.SmokeCompensationCnt++;
							SmokeAO.SmokeCompensationData += SmokeData;
							
							#ifdef SUPPORT_SMOKE_DEBUG
							printf("SmokeCompensationData:%d,",SmokeAO.SmokeCompensationData/SmokeAO.SmokeCompensationCnt);
							PrintVol(SmokeAO.SmokeCompensationData/SmokeAO.SmokeCompensationCnt);
							printf("Cnt :%d\n",SmokeAO.SmokeCompensationCnt);
							#endif
							if(SmokeAO.SmokeCompensationCnt >= 16)
							{
								SmokeAO.SmokeCompensationCnt = 0;
								SmokeAO.SmokeCompensationData >>= 4;
								Sensortive = SmokeAO.SmokeSensortive-SMOKE_ALARM_LEVEL;
								if((SmokeAO.SmokeCompensationData < (Sensortive+(SMOKE_ALARM_LEVEL>>SMOKE_CALIBRATION_DUST_INCREASE)))&&
									(SmokeAO.SmokeCompensationData > (Sensortive-(SMOKE_ALARM_LEVEL>>SMOKE_CALIBRATION_DUST_INCREASE))))
								{
									SmokeAO.SmokeSensortive = SmokeAO.SmokeCompensationData + SMOKE_ALARM_LEVEL;
									#ifdef SUPPORT_SMOKE_DEBUG
									printf("compensation success! \n");
									printf("Sensortive:%d",SmokeAO.SmokeSensortive);
									PrintVol(SmokeAO.SmokeSensortive);
									#endif
									if((SmokeAO.SmokeSensortive < PwrAO.BatVolVdd-EMC_BATTERY_DEFFER)&&(SmokeAO.SmokeLightAvg < PwrAO.BatVolVdd - SMOKE_ALARM_LEVEL- EMC_BATTERY_DEFFER))//
									{
										E2promWrite(FLASH_ADDR_SMOKE_BASIC_CAMP_MSB, SmokeAO.SmokeCompensationData >> 8);  
										E2promWrite(FLASH_ADDR_SMOKE_BASIC_CAMP_LSB, SmokeAO.SmokeCompensationData & 0xff);  
										E2promWrite(FLASH_ADDR_SMOKE_BASIC_TEMPERATURE, SysAO.TempData); 
									}else
									{
										SmokeAO.SmokeSensortive = PwrAO.BatVolVdd - EMC_BATTERY_DEFFER;
									}
								}
								SmokeAO.SmokeCompensationData = 0;
							}
						}
					}
				}
			}
		}
		else
		{
			#ifdef SUPPORT_SMOKE_DEBUG
			printf("Smoke err\r\n");
			#endif
			SmokeAO.SmokeTransFaultCnt++;
			if (SmokeAO.SmokeTransFaultCnt>=3)
			{
				SmokeAO.SmokeTransFaultCnt = 3;
				SysteSetSig(eEventSigSmokeErr);
			}
		}
	}

#if defined(SUPPORT_SMOKE_DEBUG) || defined(SENSOR_TEMP_TEST)
		printf("_isgdata1:%d", _isgdata1);	
		printf("\n");
	
		printf("\nT:%dC\n", SysAO.TempData-20);
		
		printf("D:%d, DVol:", SmokeAO.SmokeDarkAvg);	
		PrintVol(SmokeAO.SmokeDarkAvg);
			
		printf("   ");
		printf("L:%d, LVol:", SmokeAO.SmokeLightAvg);	
		PrintVol(SmokeAO.SmokeLightAvg);
			
		printf("   ");
		printf("L-D:%d, D_LVol:", SmokeData);	
		PrintVol(SmokeData);
		printf("\n");
#endif

	if(SysAO.DebugOnFlag == TRUE)
	{
		printf("Smoke:%d,",SmokeData);
		printf("Sensortive:%d\n",SmokeAO.SmokeSensortive);
	
		printf("T=%dC, D=", SysAO.TempData-20);
		PrintVol(SmokeAO.SmokeDarkAvg);
		printf(", L=");
		PrintVol(SmokeAO.SmokeLightAvg);
		printf("\n");
	}
}


/**********************************************************************/
//Description:		LoadData for calibration 							
//Parameters:                        									
//Return:   											
//Date:  
//author:		    song.jiang
/**********************************************************************/
uint8_t SmokeOPACheck(uint16_t data)
{
	static uint16 SmokeDataFirst=0;
	uint8_t ret = 0;
	GetSmokeData();

	if (SmokeAO.SmokeLightAvg>data)
	{
		if(SmokeAO.SmokeCaliOPA>0)
		{
			if((SmokeAO.SmokeLightAvg-data)>(data-SmokeDataFirst))
			{
				SmokeAO.SmokeCaliOPA--;
			}
		}
		E2promWrite(FLASH_ADDR_SMOKE_CARIBRATION_OPA, SmokeAO.SmokeCaliOPA);
		ret = 1;
	}

	SmokeDataFirst=SmokeAO.SmokeLightAvg;
	return ret;

}


/**********************************************************************/
//Description:		SmokeSetOPA							
//Parameters:                        									
//Return:   											
//Date:  
//author:		    song.jiang
/**********************************************************************/
uint8_t SmokeSetOPA(void)
{
	uint8_t ret = 0;
    uint8_t opa = 0;
	
	for (opa = SMOKE_OPA_MIN; opa <= SMOKE_OPA_MAX; ++opa)
	{
		
		SmokeAO.SmokeCaliOPA = opa;
		
		if(1==SmokeOPACheck(SMOKE_LIGHT_SUCESS_CURRENT))
		{
			break;
		}else
		{
			;
		}
		
		Y_LED_ON();
		printf("opa:%d\n", SmokeAO.SmokeCaliOPA);
		printf("D:%d, DVol:", SmokeAO.SmokeDarkAvg);		
		PrintVol(SmokeAO.SmokeDarkAvg);
		printf("   ");
		printf("L:%d, LVol:", SmokeAO.SmokeLightAvg);	
		PrintVol(SmokeAO.SmokeLightAvg);
		printf("   ");
		GCC_DELAY(5000);
		Y_LED_OFF();

		PwrAO.standbycnt = LowPwrMaxCnt-1;
		EnteryLowPower();
	}
	

	if (opa>SMOKE_OPA_MAX)
	{
		if(1==SmokeOPACheck(SMOKE_LIGHT_SUCESS_MIN))
		{
			;
		}else
		{
			ret = 16;
		}
	}
	else
	{
			;	
	}
	return ret;
}


/**********************************************************************/
//Description:		LoadData for calibration 							
//Parameters:                        									
//Return:   											
//Date:  
//author:		    song.jiang
/**********************************************************************/
bool LoadSmokeData(void)
{
	bool CailSmokeRes = FALSE;

	SmokeAO.SmokeCariFlag = E2promRead(FLASH_ADDR_SMOKE_CARIBRATION_FLAG);
	if(SmokeAO.SmokeCariFlag == SMOKE_CARIBRATION_FLAG)
	{
		SmokeAO.SmokeSensortive = E2promRead(FLASH_ADDR_SMOKE_BASIC_CAMP_MSB);
		SmokeAO.SmokeSensortive <<= 8;
		SmokeAO.SmokeSensortive |= E2promRead(FLASH_ADDR_SMOKE_BASIC_CAMP_LSB);

		SmokeAO.SmokeSensortive += SMOKE_ALARM_LEVEL;   

		SmokeAO.SmokeBasicTemp = E2promRead(FLASH_ADDR_SMOKE_BASIC_TEMPERATURE);
		SmokeAO.SmokeVanos0 = E2promRead(FLASH_ADDR_SMOKE_VANOS_0);
		SmokeAO.SmokeVanos1 = E2promRead(FLASH_ADDR_SMOKE_VANOS_1);
		SmokeAO.SmokeCaliOPA = E2promRead(FLASH_ADDR_SMOKE_CARIBRATION_OPA);
		printf("Smoke: OPA=%d, ", SmokeAO.SmokeCaliOPA);
		printf("S=%d\n", SmokeAO.SmokeSensortive);
//		PwrAO.standbycnt = LowPwrMaxCnt;//must for current electricity test

		CailSmokeRes = TRUE;
	}
	else//caribration for dust test 
	{
	#ifdef SUPPORT_CARIBRATION_DEBUG
		printf("start:\n");
	#endif
		SmokeAO.SmokeSampleTimeCnt = 3;  
		SmokeSetOPA();
	}

	return CailSmokeRes;
}

#ifdef SUPPORT_SPEAK_FUNCTION
/**********************************************************************/
//Description:		SpeakSmokeAlarmVoice
//Parameters:											
//Return:   											
//Date:  
//author:		    quanwu.xu
/**********************************************************************/
void SpeakSmokeAlarmVoice(uint8_t Location, uint8_t T3Cnt)
{
	if(Location == LocationMsgNoLocation)
	{
		switch(T3Cnt)
		{
			case 1:
			case 4:
			case 7:
				SpeakerSendCmd(CMD_WARNING);
				break;

			case 2:
			case 5:
				SpeakerSendCmd(CMD_SMOKE_DETECTED);
				break;

			case 3:
			case 6:
				SpeakerSendCmd(CMD_EVACUATE);
				break;

			case 8:
			case 10:
				SpeakerSendCmd(((uint32_t)CMD_WARNING<<16) | (CMD_SMOKE_DETECTED<<8) | CMD_EVACUATE);
				break;

			default:
				break;
		}
	}
	else
	{
		switch(T3Cnt)
		{
			case 1:
			case 5:
				SpeakerSendCmd(CMD_WARNING);
				break;

			case 2:
			case 6:
				SpeakerSendCmd(CMD_SMOKE_DETECTED);
				break;

			case 3:
			case 7:
				SpeakerSendCmd((CMD_IN<<8) | Location);
				break;

			case 4:
				SpeakerSendCmd(CMD_EVACUATE);
				break;


			case 8:
			case 10:
				SpeakerSendCmd(CMD_WARNING);
				SpeakerSendCmd(((uint32_t)CMD_SMOKE_DETECTED<<16)|(CMD_IN<<8)|Location);
				SpeakerSendCmd(CMD_EVACUATE);
				break;

			default:
				break;
		}
	}
}

#endif

/**********************************************************************/
//Description:		SmokeCaliMain 										
//Parameters:                        									
//Return:   											
//Date:  
//author:		    quanwu.xu
/**********************************************************************/
void SmokeCaliMain(void)
{
	static uint8 i = 0;
	uint16_t BatVol = 0;
	uint32 SmokeData = 0; 
	static	uint32 SmokeCaliData = 0;
	CailResult Result = CailSuccess;

	switch(SysAO.sig)
	{
		case eEventSigEnteryMode:
			SmokeCaliData = 0;
			
			if(LoadSmokeData() == FALSE)
			{
				printf("Start Cail Smoke\n");
				
				SysAO.TempData	= GetTempIndex();
				
				TmrInsertTimer(eTmrHdlSmoke, TMR_PERIOD(2000), (void *)eSmokeCail);
				BatVol = BatteryCheckHandle();
#ifdef TEMPCALIOLD
				if(SysAO.TempData<38||SysAO.TempData>55)
#else
				if(SysAO.TempData<5||SysAO.TempData>13)
#endif
				{
					while(1)
					{
						printf("temp err= %d\n", (SysAO.TempData-20));
						CailRes(CailFail, eCail_Temp_Err);
					}
				}
				else if((BatVol < CAIL_BAT_LEVEL_MIX) || (BatVol > CAIL_BAT_LEVEL_MAX))
				{
					while(1)
					{
						printf("bat err=%dmV\n", BatVol);
						CailRes(CailFail, eCail_BatErr);
					}
				}
				E2promWrite(FLASH_ADDR_CARIBRATION_VDD_MSB, BatVol >> 8);  
				E2promWrite(FLASH_ADDR_CARIBRATION_VDD_LSB, BatVol & 0xff);  
			}
			else
			{
#ifdef SUPPORT_CO_FUNCTION
				SysAO.eMode = CoCailMode;
#else
				PowerOnWelcomeAndDisplay();

				SysAO.eMode = NormalMode;
#endif
			}
			break;

		case eEventSigSmokeCailTimeout:
			if(i++<16)
			{
				SmokeData = GetSmokeData();
			
				SmokeCaliData += SmokeData;
#ifdef SUPPORT_CARIBRATION_DEBUG
				printf("\nT:%dC\n", (SysAO.TempData-20));
				printf("D:%d, DVol:", SmokeAO.SmokeDarkAvg);	
				PrintVol(SmokeAO.SmokeDarkAvg);
				
				printf("   ");
				printf("L:%d, LVol:", SmokeAO.SmokeLightAvg);	
				PrintVol(SmokeAO.SmokeLightAvg);
				
				printf("   ");
				printf("L-D:%d, D_LVol:", SmokeData);	
				PrintVol(SmokeData);
				printf("\n");
				
				printf("_isgdata0:%d", _isgdata0);
				printf("_isgdata1:%d", _isgdata1);	
				printf("\n");
				
#endif
				if((SmokeAO.SmokeDarkAvg>SMOKE_DARK_SUCESS_MAX) ||
							(SmokeAO.SmokeLightAvg<(SMOKE_LIGHT_SUCESS_CURRENT - 200)) ||
											(SmokeData < SMOKE_ALARM_LEVEL) ||
													(SmokeData > SMOKE_CALIBRATION_SUCESS_MAX))
				{
					while(1)
					{
#ifdef SUPPORT_CARIBRATION_DEBUG
						printf("\nD=%d, L=%d, ", SmokeAO.SmokeDarkAvg, SmokeAO.SmokeLightAvg);
						printf("L-D=%d\n", SmokeData);
						printf("T=%d\n", (SysAO.TempData-20));
#endif
						CailRes(CailFail, eCail_Smoke);
					}
				}
				else
				{
					CailStepDisplay(eCail_Smoke);
				}
			}
			else
			{
				i = 0;
				SmokeCaliData >>= 4;

				if((SmokeCaliData > SMOKE_CALIBRATION_SUCESS_MIN) && 
					(SmokeCaliData < SMOKE_CALIBRATION_SUCESS_MAX) && 
						(SmokeAO.SmokeDarkAvg<SMOKE_DARK_SUCESS_MAX) && 
							(SmokeAO.SmokeLightAvg<SMOKE_LIGHT_SUCESS_MAX) && 
								(SmokeAO.SmokeLightAvg>SMOKE_LIGHT_SUCESS_MIN))//cal sucess
				{
					SmokeAO.SmokeSampleTimeCnt = 3;
					SmokeAO.SmokeSensortive = SmokeCaliData + SMOKE_ALARM_LEVEL;
					SmokeAO.SmokeBasicTemp = (uint8)SysAO.TempData;
					SmokeAO.SmokeCariFlag = SMOKE_CARIBRATION_FLAG;    
					SysAO.LifeTimeDays = 0;
					SmokeAO.SmokeVanos0 = _sda0vos;
					SmokeAO.SmokeVanos1 = _sda1vos;

					E2promWrite(FLASH_ADDR_SMOKE_BASIC_CAMP_MSB, SmokeCaliData >> 8);  
					E2promWrite(FLASH_ADDR_SMOKE_BASIC_CAMP_LSB, SmokeCaliData & 0xff);  
					E2promWrite(FLASH_ADDR_SMOKE_BASIC_TEMPERATURE, (uint8)SysAO.TempData); 
					E2promWrite(FLASH_ADDR_SMOKE_CARIBRATION_FLAG, SMOKE_CARIBRATION_FLAG); 
			
					E2promWrite(FLASH_ADDR_LIFE_TIME_DAYS_MSB, SysAO.LifeTimeDays >> 8);
					E2promWrite(FLASH_ADDR_LIFE_TIME_DAYS_LSB, SysAO.LifeTimeDays & 0xff);
					
					E2promWrite(FLASH_ADDR_SMOKE_VANOS_0, SmokeAO.SmokeVanos0); 
					E2promWrite(FLASH_ADDR_SMOKE_VANOS_1, SmokeAO.SmokeVanos1); 
					E2promWrite(FLASH_ADDR_SMOKE_CARIBRATION_OPA, SmokeAO.SmokeCaliOPA);

					Result = CailSuccess;
				}											  
				else																	 //cal fail
				{													  
					Result = CailFail;
				}

				while(1)
				{
#ifdef SUPPORT_CARIBRATION_DEBUG
					printf("\nDark=%d, Light=%d, ", SmokeAO.SmokeDarkAvg, SmokeAO.SmokeLightAvg);
					printf("Cari Data=%d\n", SmokeCaliData);
					printf("T=%d\n", (SysAO.TempData-20));
#endif
					CailRes(Result, eCail_Smoke);
				}
			}
			break;

		default:
			break;
	}
}

/**********************************************************************/
//Description:		SmokeTestMain
//Parameters:                        									
//Return:   											
//Date:  
//author:		    quanwu.xu
/**********************************************************************/
void SmokeTestMain(void)
{
	static bool DebugOpenEntryFlag = FALSE;
	static bool ErrFlag = FALSE;
	static uint8_t TimeoutCnt = 0;
	static uint8_t IntervalTimeoutCnt = 0;

	printf("s=%d\n", SysAO.sig);
	switch(SysAO.sig)
	{
		case eEventSigEnteryMode:
#ifdef SUPPORT_SMOKE_DEBUG
			printf("Smoke Test, Mode=%d\n");
#endif
			DebugOpenEntryFlag = FALSE;
			TimeoutCnt = 0;
			IntervalTimeoutCnt = 0;
			ErrFlag = SmokeSensorCheck();

			printf("\nSmokeSensortive:%d\n", SmokeAO.SmokeSensortive);
			printf("D:%d\n", SmokeAO.SmokeDarkAvg);
			printf("L:%d\n", SmokeAO.SmokeLightAvg);

#ifdef SUPPORT_SPEAK_FUNCTION
			SpeakerSendCmd(CMD_TEST_REMIND);
#endif
			TmrInsertTimer(eTmrHdlSmoke, TMR_PERIOD(500), (void *)eSmokeTest);
			break;

		case eEventSigSmokeTest:
			TmrDeleteTimer(eTmrHdlSmoke);

#ifdef SUPPORT_SPEAK_FUNCTION
			SpeakerSendCmd(CMD_TEST_SMOKE);
#endif
			if(ErrFlag == TRUE)
			{
				SysteSetSig(eEventSigSmokeErr);
			}
			else
			{
				TmrInsertTimer(eTmrHdlSmoke, TMR_PERIOD(500), (void *)eSmokeAlarm);
			}
			break;

		case eEventSigSmokeErr:
			TmrDeleteTimer(eTmrHdlSmoke);
			SysAO.eMode = SmokeErrMode;
			break;
	
		case eEventSigSmokeAlarmTimeout:
			if(++TimeoutCnt > 5)
			{
				TimeoutCnt = 0;
				R_LED_OFF();
				BEEP_OFF();
				TmrDeleteTimer(eTmrHdlSmoke);
				if(++IntervalTimeoutCnt > 1)
				{
					IntervalTimeoutCnt = 0;
					SysteSetSig(eEventSigSmokeTestCancel);
				}
				else
				{
					TmrInsertTimer(eTmrHdlSmoke, TMR_PERIOD(1000), (void *)eSmokeAlarmInterVal);
#ifdef SUPPORT_SPEAK_FUNCTION
					IntervalTimeoutCnt = 0;
					SysteSetSig(eEventSigSmokeTestCancel);
#endif
				}
			}
			else
			{
				R_LED_TOGGLE();
				BEEP_TOGGLE();
			}
			break;

		case eEventSigSmokeAlarmIntervalTimeout:
			TmrDeleteTimer(eTmrHdlSmoke);
			TmrInsertTimer(eTmrHdlSmoke, TMR_PERIOD(500), (void *)eSmokeAlarm);
			break;

		case eEventSigSmokeTestCancel:
			TmrDeleteTimer(eTmrHdlSmoke);

#ifdef SUPPORT_CO_FUNCTION
			TmrInsertTimer(eTmrHdlSmoke, TMR_PERIOD(1000), (void *)eSmokeDelayInCoTest);
#else
		#ifdef SUPPORT_SPEAK_FUNCTION
			if((SysAO.eEventFlg & eEventFlagLowBat) || (SysAO.eEventFlg & eEventFlagLowBatForbitMute) 
															|| (SysAO.eEventFlg & eEventFlagLowBatMute))
			{
				SpeakerSendCmd((CMD_TEST_FINISHED<<8) | CMD_REPLACE_BAT_LOW_BAT);
			}
			else
			{
				SpeakerSendCmd((CMD_TEST_FINISHED<<8) | CMD_NORMAL_WORKING_STATUS);
			}
		#endif

		#ifdef SUPPORT_RF_NET_FUNCTION
			if(GetRfPairedMode() ==  ePairedHub)
			{
				UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgTest, DevTestSuccess);
			}
		#endif

			SysAO.eMode = NormalMode;
#endif
			break;

		case eEventSigKey1DoubleClik:
			DebugOpenEntryFlag = TRUE;
			break;

		case eEventSigKey1LongClik:
			if(DebugOpenEntryFlag == TRUE)
			{
				SysAO.DebugOnFlag = TRUE;
				G_LED_ON();
				GCC_DELAY(30000);
				G_LED_OFF();
				printf("Debug Flag=%d\n", SysAO.DebugOnFlag);
			}
			break;

		case eEventSigKey1Clik:
#ifdef SUPPORT_SPEAK_FUNCTION
			R_LED_OFF();
			BEEP_OFF();
			TmrDeleteTimer(eTmrHdlSmoke);
			SysAO.eMode = NormalMode;
			SpeakerSendCmd(CMD_TEST_CANCELLED);
#endif
			break;

		case eEventSigSmokeTestDelayInCotest:
			R_LED_OFF();
			BEEP_OFF();
			TmrDeleteTimer(eTmrHdlSmoke);
			SysAO.eMode = CoTestMode;
			break;

		default:
			break;
	}
}


/**********************************************************************/
//Description:		SmokeAlarmMain
//Parameters:                        								
//Return:   											
//Date:  
//author:			quanwu.xu
/**********************************************************************/
void SmokeAlarmMain(void)
{
	static uint8_t TimeoutCnt = 0;
#ifdef SUPPORT_SPEAK_FUNCTION
	static uint8_t T3TimeCnt = 0;
#endif
	uint16 SmokeData = 0; 

	switch(SysAO.sig)
	{
		case eEventSigEnteryMode:
			printf("Smoke Alarm Mode\n");
			TimeoutCnt = 0;

#ifdef SUPPORT_SPEAK_FUNCTION
			T3TimeCnt = 0;
#endif

#ifdef SUPPORT_RF_NET_FUNCTION
			UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgSmokeAlarm, RfAO.TxLocation);
#endif

			TmrInsertTimer(eTmrHdlSmoke, TMR_PERIOD(500), (void *)eSmokeAlarm);
			break;

		case eEventSigSmokeAlarmTimeout:
			if(++TimeoutCnt > 5)
			{
				TimeoutCnt = 0;
				R_LED_OFF();
				BEEP_OFF();

				BatteryCheckHandle();
				SmokeAlarmCheck();

				TmrDeleteTimer(eTmrHdlSmoke);
#ifdef SUPPORT_SPEAK_FUNCTION
				T3TimeCnt++;
				if(T3TimeCnt >= 11)
				{
					T3TimeCnt = 9;
				}

				if(T3TimeCnt < 8)
				{
					TmrInsertTimer(eTmrHdlSmoke, TMR_PERIOD(1000), (void *)eSmokeAlarmInterVal);
					SpeakSmokeAlarmVoice(RfAO.TxLocation, T3TimeCnt);
				}
				else
				{
					SpeakSmokeAlarmVoice(RfAO.TxLocation, T3TimeCnt);
					TmrInsertTimer(eTmrHdlSmoke, TMR_PERIOD(1000), (void *)eSmokeAlarmInterVal);
				}

#else
				TmrInsertTimer(eTmrHdlSmoke, TMR_PERIOD(1000), (void *)eSmokeAlarmInterVal);
#endif
			}
			else
			{
				R_LED_TOGGLE();
				BEEP_TOGGLE();
			}
			break;

		case eEventSigSmokeAlarmIntervalTimeout:
			TmrDeleteTimer(eTmrHdlSmoke);
			TmrInsertTimer(eTmrHdlSmoke, TMR_PERIOD(500), (void *)eSmokeAlarm);
			break;

		case eEventSigKey1Clik:
		case eEventSigSmokeKeyMute:
			SmokeData = GetSmokeData();
			printf("D:%dmV\n", SmokeAO.SmokeDarkAvg);
			printf("L:%dmV\n", SmokeAO.SmokeLightAvg);
			printf("L-D:%dmV\n", SmokeData);

			R_LED_OFF();
			BEEP_OFF();
#ifdef SUPPORT_SPEAK_FUNCTION
			if(SmokeData >= SmokeAO.SmokeSensortive)
			{
				TimeoutCnt = 0;
				T3TimeCnt = 0;

				TmrDeleteTimer(eTmrHdlSmoke);
				
				SpeakerSendCmd(CMD_SMOKE_ALARM_CANNOT_MUTED);

				TmrInsertTimer(eTmrHdlSmoke, TMR_PERIOD(500), (void *)eSmokeAlarm);
			}
			else
#endif
			{
				TmrDeleteTimer(eTmrHdlSmoke);
				SysAO.eMode = SmokeMuteMode;
			}
			break;

		case eEventSigSmokeAlarmCancel:
			R_LED_OFF();
			BEEP_OFF();
			TmrDeleteTimer(eTmrHdlSmoke);

#ifdef SUPPORT_RF_NET_FUNCTION
			UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgCancelAlarm, 0);
#endif

#ifdef SUPPORT_SPEAK_FUNCTION
			SpeakerSendCmd(CMD_CANCEL_ALARM);
#endif

			TmrInsertTimer(eTmrHdlSmoke, TMR_PERIOD(1000), (void *)eSmokeExitAlarm);
			break;

		case eEventSigExitMode:
			R_LED_OFF();
			BEEP_OFF();
			TmrDeleteTimer(eTmrHdlSmoke);
			SysAO.eMode = NormalMode;
			break;


		default:
			break;
				
	}
}

/**********************************************************************/
//Description:		SmokeMuteMain
//Parameters:                        								
//Return:   											
//Date:  
//author:			quanwu.xu
/**********************************************************************/
void SmokeMuteMain(void)
{
	static uint8_t TimeoutCnt = 0;
	static uint8_t MuteTimeCnt = 0;
#ifdef SUPPORT_CO_FUNCTION
	static uint8_t CoCheckCnt = 0;
#endif
	switch(SysAO.sig)
	{
		case eEventSigEnteryMode:
			printf("Smoke Mute Mode\n");
			
			TimeoutCnt = 0;
			MuteTimeCnt = 0;
#ifdef SUPPORT_CO_FUNCTION
			CoCheckCnt = 0;
#endif

#ifdef SUPPORT_SPEAK_FUNCTION
			SpeakerSendCmd(CMD_SMOKE_ALARM_MUTED);
#endif

#ifdef SUPPORT_RF_NET_FUNCTION
			UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgMute, 0);
#endif

			TmrInsertTimer(eTmrHdlSmoke, TMR_PERIOD(500), (void *)eSmokeAlarm);
			TmrInsertTimer(eTmrHdlMuteTimeout, TMR_PERIOD(60000), (void *)SmokeMuteMode);

#ifdef SUPPORT_UL2034_UL217
			SmokeAO.SmokeSensortive += SMOKE_ALARM_LEVEL;
#else
			SmokeAO.SmokeSensortive += 5000;
#endif
			break;
		
		case eEventSigKey1Clik:
			break;

		case eEventSigSmokeAlarmTimeout:
			if(++TimeoutCnt > 5)
			{
				TimeoutCnt = 0;
				R_LED_OFF();

				BatteryCheckHandle();
				SmokeAlarmCheck();

				TmrDeleteTimer(eTmrHdlSmoke);
				TmrInsertTimer(eTmrHdlSmoke, TMR_PERIOD(500), (void *)eSmokeAlarmInterVal);
			}
			else
			{
				R_LED_TOGGLE();
			}
			break;

		case eEventSigSmokeAlarmIntervalTimeout:
#ifdef SUPPORT_CO_FUNCTION
			if(CoCheckCnt++>5)								//30s
			{
				CoCheckCnt = 0;
				CoAlarmCheck();
			}
#endif
			TmrDeleteTimer(eTmrHdlSmoke);
			TmrInsertTimer(eTmrHdlSmoke, TMR_PERIOD(500), (void *)eSmokeAlarm);
			break;

		case eEventSigSmokeMuteTimeout:
			R_LED_OFF();
			if(++MuteTimeCnt >= SMOKE_MUTE_TIMEOUT_COUNT)	//last for 9min
			{
				SysteSetSig(eEventSigSmokeMuteFinished);
			}
			break;

		case eEventSigSmokeMuteFinished:
			TmrDeleteTimer(eTmrHdlMuteTimeout);
			TmrDeleteTimer(eTmrHdlSmoke);
		
#ifdef SUPPORT_SPEAK_FUNCTION
			SpeakerSendCmd(CMD_CANCEL_ALARM);
#endif

#ifdef SUPPORT_RF_NET_FUNCTION
			if(GetRfPairedMode() == ePairedHub)
			{
				UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgCancelAlarm, 0);
			}
#endif

#ifdef SUPPORT_UL2034_UL217
			SmokeAO.SmokeSensortive -= SMOKE_ALARM_LEVEL;
#else
			SmokeAO.SmokeSensortive -= 5000;
#endif

			TmrInsertTimer(eTmrHdlSmoke, TMR_PERIOD(1000), (void *)eSmokeExitMute);
			break;
		
		case eEventSigExitMode:
			R_LED_OFF();
			TmrDeleteTimer(eTmrHdlSmoke);
			SysAO.eMode = NormalMode;
			break;

#ifdef SUPPORT_RF_NET_FUNCTION
		case eEventSigRadioSmokeAlarm:
			TmrDeleteTimer(eTmrHdlMuteTimeout);
			TmrDeleteTimer(eTmrHdlSmoke);
			SysAO.eMode = RadioSmokeAlarmMode;
#ifdef SUPPORT_UL2034_UL217
			SmokeAO.SmokeSensortive -= SMOKE_ALARM_LEVEL;
#else
			SmokeAO.SmokeSensortive -= 5000;
#endif
			break;

		case eEventSigRadioCoAlarm:
			TmrDeleteTimer(eTmrHdlMuteTimeout);
			TmrDeleteTimer(eTmrHdlSmoke);
			SysAO.eMode = RadioCoAlarmMode;
#ifdef SUPPORT_UL2034_UL217
			SmokeAO.SmokeSensortive -= SMOKE_ALARM_LEVEL;
#else
			SmokeAO.SmokeSensortive -= 5000;
#endif
			break;
#endif

		case eEventSigSmokeAlarm:
			TmrDeleteTimer(eTmrHdlMuteTimeout);
			TmrDeleteTimer(eTmrHdlSmoke);

#ifdef SUPPORT_SPEAK_FUNCTION
			SpeakerSendCmd(CMD_SMOKE_ALARM_CANNOT_MUTED);

#endif
			SysAO.eMode = SmokeAlarmMode;
#ifdef SUPPORT_UL2034_UL217
			SmokeAO.SmokeSensortive -= SMOKE_ALARM_LEVEL;
#else
			SmokeAO.SmokeSensortive -= 5000;
#endif
			break;


#ifdef SUPPORT_CO_FUNCTION
		case eEventSigCoAlarm:
			TmrDeleteTimer(eTmrHdlMuteTimeout);
			TmrDeleteTimer(eTmrHdlSmoke);
			SysAO.eMode = CoAlarmMode;
#ifdef SUPPORT_UL2034_UL217
			SmokeAO.SmokeSensortive -= SMOKE_ALARM_LEVEL;
#else
			SmokeAO.SmokeSensortive -= 5000;
#endif
			break;
#endif
		
		default:
			break;
				
	}	
}

/**********************************************************************/
//Description:		SmokeErrMain
//Parameters:                        								
//Return:   											
//Date:  
//author:			quanwu.xu
/**********************************************************************/
void SmokeErrMain(void)
{
	static uint8_t CheckTimeCnt = 0;
#ifdef SUPPORT_SPEAK_FUNCTION
	static uint8_t SmokeErrSpeakTimeCnt = 0;
#endif
	switch(SysAO.sig)
	{
		case eEventSigEnteryMode:
#ifdef SUPPORT_SPEAK_FUNCTION
			SmokeErrSpeakTimeCnt = 0;
#endif
		case eEventSigKey1Clik:
			printf("Smoke Err Mode\n");
			CheckTimeCnt = 0;
#ifdef SUPPORT_SPEAK_FUNCTION
			SpeakerSendCmd(CMD_SMOKE_SENSOR_ERR);
#endif

#ifdef SUPPORT_RF_NET_FUNCTION  //reserved
			if(GetRfPairedMode() == ePairedHub)
			{
				UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgFault, DevSmokeSenorFault);
			}
#endif

			ErrIndex(1,1,2);
			break;

		case eEventSigWakeup:
			if(SmokeSensorCheck() == TRUE)
			{
				if(++CheckTimeCnt>5)
				{
#ifdef SUPPORT_SPEAK_FUNCTION
					if(SmokeErrSpeakTimeCnt < 15)
					{
						SmokeErrSpeakTimeCnt++;
						SpeakerSendCmd(CMD_SMOKE_SENSOR_ERR);
					}
#endif
					CheckTimeCnt = 0;
					ErrIndex(1,1,2);
				}
			}
			else
			{
				SysteSetSig(eEventSigSmokeErrRecovery);
			}
			printf("\nThreshold:%d\n", SmokeAO.SmokeSensortive);
			printf("D:%d\n", SmokeAO.SmokeDarkAvg);
			printf("L:%d\n", SmokeAO.SmokeLightAvg);
			break;
			
		case eEventSigSmokeErrRecovery:
			SysAO.eMode = NormalMode;
#ifdef SUPPORT_RF_NET_FUNCTION
			if(GetRfPairedMode() == ePairedHub)
			{
				UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgFault, DevFaultCancel);
			}
#endif
			break;
		
		default:
			break;
	}
}

#ifdef SUPPORT_RF_NET_FUNCTION
/**********************************************************************/
//Description:		NetSmokeAlarmMain
//Parameters:                        								
//Return:   											
//Date:  
//author:			quanwu.xu
/**********************************************************************/
void RfSmokeAlarmMain(void)
{
	static uint8_t TimeoutCnt = 0;
#ifdef SUPPORT_SPEAK_FUNCTION
	static uint8_t T3TimeCnt = 0;
#endif

	switch(SysAO.sig)
	{
		case eEventSigEnteryMode:
			printf("net smoke alarm\n");
			TimeoutCnt = 0;

#ifdef SUPPORT_SPEAK_FUNCTION
			T3TimeCnt = 0;
#endif
			TmrInsertTimer(eTmrHdlSmoke, TMR_PERIOD(500), (void *)eSmokeAlarm);
			break;

		case eEventSigSmokeAlarmTimeout:
			if(++TimeoutCnt > 5)
			{
				TimeoutCnt = 0;
				BEEP_OFF();
				G_LED_OFF();
				R_LED_OFF();
				
				CoAlarmCheck();
				SmokeAlarmCheck();

				TmrDeleteTimer(eTmrHdlSmoke);
#ifdef SUPPORT_SPEAK_FUNCTION
				T3TimeCnt++;
				if(T3TimeCnt >= 11)
				{
					T3TimeCnt = 9;
				}
				SpeakSmokeAlarmVoice(RfAO.RxLocation, T3TimeCnt);
#endif

				TmrInsertTimer(eTmrHdlSmoke, TMR_PERIOD(500), (void *)eSmokeAlarmInterVal);
			}
			else
			{
				BEEP_TOGGLE();
				if(TimeoutCnt%2 == 0) 
				{
					R_LED_OFF();
					G_LED_ON();
				}
				else
				{
					R_LED_ON();
					G_LED_OFF();
				}
			}
			break;

		case eEventSigSmokeAlarmIntervalTimeout:
			TmrDeleteTimer(eTmrHdlSmoke);
			TmrInsertTimer(eTmrHdlSmoke, TMR_PERIOD(500), (void *)eSmokeAlarm);
			break;

		case eEventSigSmokeAlarm:
			R_LED_OFF();
			G_LED_OFF();
			BEEP_OFF();
			TmrDeleteTimer(eTmrHdlSmoke);
			#ifdef SUPPORT_SPEAK_FUNCTION
				RfAO.RxLocation = LocationMsgNoLocation;
			#endif
				
			SysAO.eMode = SmokeAlarmMode;
			break;


#ifdef SUPPORT_CO_FUNCTION
		case eEventSigCoAlarm:
			R_LED_OFF();
			G_LED_OFF();
			BEEP_OFF();
			TmrDeleteTimer(eTmrHdlSmoke);
			#ifdef SUPPORT_SPEAK_FUNCTION
				RfAO.RxLocation = LocationMsgNoLocation;
			#endif
			
			SysAO.eMode = CoAlarmMode;
			break;
#endif

		case eEventSigKey1Clik:
#ifdef SUPPORT_RF_NET_FUNCTION
			UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgCancelAlarm, 0);
#endif
		case eEventSigRadioCancelAlarm:
			R_LED_OFF();
			G_LED_OFF();
			BEEP_OFF();
			TmrDeleteTimer(eTmrHdlSmoke);
			
#ifdef SUPPORT_SPEAK_FUNCTION
			RfAO.RxLocation = LocationMsgNoLocation;
#endif
			SysAO.eMode = NormalMode;
			break;
			
		default:
			break;
	}
}
#endif

