/****************************************************************************/
/*	File   :                        RF.c                                    */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                      2019/8/08,14:14:6                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      XS01                           */
/****************************************************************************/
#include "main.h"
#include "Module.h"

#ifdef SUPPORT_RF_NET_FUNCTION

#define UartQueueLimit		((uint8_t)128)
#define UartBufLimit		((uint8_t)128)

unsigned char volatile UartQueueBuf[UartQueueLimit] = {0};	//队列缓存
unsigned char UartReadBuf[UartBufLimit] = {0};				//接收缓存
unsigned char UartSendBuf[UartBufLimit] = {0};				//接收缓存
volatile unsigned char *UartQueueIn = (unsigned char *)UartQueueBuf;
volatile unsigned char *UartQueueOut= (unsigned char *)UartQueueBuf;


static void UartHandleMsg(uint8_t DataOffset);
static void UartReceiveInput(unsigned char value);
bool UartSend(eUartMsgType UartMsgType, uint8_t MsgLevel1, uint8_t Data);


RfTag  RfAO;
void RfInit(void)
{
	RfAO.ProType = eProductType_XP02_Smart;
	RfAO.DevType = eTypeSmoke;
	RfAO.SmokeBeepNum = 0;

	RfAO.PairMode =  E2promRead(FLASH_ADDR_PAIRED_MODE_FLAG);

#ifdef SUPPORT_RF_NET_FUNCTION
	printf("rf mode = %d\n", RfAO.PairMode);
#endif

#ifdef SUPPORT_SPEAK_FUNCTION
	RfAO.TxLocation = E2promRead(FLASH_ADDR_SELF_LOCATION);
	RfAO.RxLocation = LocationMsgNoLocation;

	if(RfAO.TxLocation == 0xFF)
	{
		RfAO.TxLocation = LocationMsgNoLocation;
	}

	printf("Location:%d\n", RfAO.TxLocation);
#endif
}


/**********************************************************************/
//Description:		GetRfPairedMode
//Parameters:                        								
//Return:   											
//Date:  
//author:		quanwu.xu
/**********************************************************************/
ePairedMode GetRfPairedMode(void)
{
	return RfAO.PairMode;
}

/**********************************************************************/
//Description:		GetCrc8  														  
//Parameters:                        												  
//Return:   											
//Date:  
//author:		    song
/**********************************************************************/
static uint8_t GetCrc8(uint8_t *buf, uint8_t len)
{
  	int i;
  	char crc = 0;
 	while(len--!=0)
	{
		for (i = 0x80; i != 0; i /= 2)		{
			if ((crc & 0x80) != 0)		{
				crc *= 2;
				crc ^= 0x07;
			} 
			else{
			crc *= 2;
			}

			if ((*buf & i) != 0){
					crc ^= 0x07;
				}
		}
		buf++;
	}
	return crc;
}

void PollingRfTimeout(void* pTxt)
{
	uint16_t Temp = (uint16_t)pTxt;

	if(Temp == 1)
	{
		SysteSetSig(eEventSigExitMode);
	}
	else
	{
		SysteSetSig(eEventSigRadioTimeout);
	}
}

void RFLedFlash(void* SysMode)
{
	uint16_t Mode = (uint16_t)SysMode;
	switch (Mode)
	{
		case RadioFactoryMode:
		case RadioPairSlaverMode:
			PollingLED(100);
			break;

		case RadioPairHubMode:
		case RadioPairMasterMode:
			PollingLED(1000);
			break;

		case RadioIntranetSlaverTestMode:
			PollingRFLED(50);
			PollingBeep(50);
			break;
		
		case RadioIntranetMasterTestMode:
			PollingLED(50);
			PollingBeep(50);
			break;

		default:
			break;
	}
}

void PollingUartSendTimeout(void* SendCnt)
{
	uint16_t Cnt = (uint16)SendCnt;

	if (Cnt>=3)
	{
		Cnt = 0;
		TmrDeleteTimer(eTmrHdlUartSendTimeout);
	}
	else
	{
		UartSend(RfAO.MsgType, RfAO.MsgLevel1, RfAO.MsgData);
	}
}


DEFINE_ISR(UART_ISR, 0x30)
{	
	uint8_t Data = 0;
	PwrAO.standbycnt = LowPwrMaxCnt;
	if (_ur0f)
	{	
		while(_rxif0)
		{
			Data = _txr_rxr0;
			UartReceiveInput(Data);
		}
		_ur0f = 0;			
	}
}

/**********************************************************************/
//Description:		Memcpy()															  
//Parameters:                        												  
//Return:   																  
//Date			2019-01-12
//Author:		quanwu.xu
/**********************************************************************/
static void* Memcpy(void* dest, void* src, uint8_t Length)
{
	int i;
	char *d = (char *)dest, *s = (char *)src;

	if((dest == NULL) ||(src == NULL) || (Length == 0))
	{
		return NULL;
	}
	
	for (i=0; i<Length; i++) 
		d[i] = s[i];
	
	return dest;
}


/*****************************************************************************
函数名称 : set_wifi_uart_byte
功能描述 : 写uart字节
输入参数 : dest:缓存区其实地址;
           byte:写入字节值
返回参数 : 写入完成后的总长度
*****************************************************************************/
static uint8_t SetUartTxBufByte(uint8_t dest, uint8_t byte)
{
	unsigned char *obj = (unsigned char *)UartSendBuf + UART_MSG_DATA_START + dest;

	*obj = byte;
	dest += 1;

	return dest;
}
/*****************************************************************************
函数名称 : SetUartTxBufBuffer
功能描述 : 写uart_buffer
输入参数 : dest:目标地址
           src:源地址
           len:数据长度
返回参数 : 无
*****************************************************************************/
uint8_t SetUartTxBufBuffer(uint8_t dest, uint8_t *src, uint8_t len)
{
	unsigned char *obj = (unsigned char *)UartSendBuf + UART_MSG_DATA_START + dest;

	Memcpy(obj,src,len);

	dest += len;
	return dest;
}

/**********************************************************************/
//Description:	UartSendFrame()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
static void UartSendFrame(uint8_t *SendBuf, uint8_t len)
{
	uint8_t i = 0;

	if((NULL == SendBuf) || (0 == len))
	{
		return;
	}

	for(i=0; i<len; i++)
	{
		UartSendData(&SendBuf[i]);
	}
}

/**********************************************************************/
//Description:	UartWriteFrame()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
static void UartWriteFrame(uint8_t UartCmd, uint8_t length)
{
#ifdef NO_COMPILE_ERR
	unsigned char check_sum = 0;

	UartSendBuf[UART_MSG_HEAD] = 0xAF;
	UartSendBuf[UART_MSG_TYPE] = UartCmd;
	UartSendBuf[UART_MSG_LENGTH] = length;

	length += UART_MSG_FIXED_LENGTH;
	check_sum = GetCrc8((unsigned char *)UartSendBuf, length - 1);
	UartSendBuf[length - 1] = check_sum;

	UartSendFrame(UartSendBuf, length);
#else
	int i = 0;
	int j = 0;
	int k = 0;
	char crc = 0;
	
	UartSendBuf[UART_MSG_HEAD] = 0xAF;
	UartSendBuf[UART_MSG_TYPE] = UartCmd;
	UartSendBuf[UART_MSG_LENGTH] = length;

	length += UART_MSG_FIXED_LENGTH;


	//check_sum = GetCrc8((unsigned char *)UartSendBuf, length - 1);

	k = length - 1;
 	while(k--!=0)
	{
		for (i = 0x80; i != 0; i /= 2)
		{
			if ((crc & 0x80) != 0)
			{
				crc *= 2;
				crc ^= 0x07;
			}
			else
			{
				crc *= 2;
			}

			if ((UartSendBuf[j] & i) != 0)
			{
				crc ^= 0x07;
			}
		}
		j++;
	}

	UartSendBuf[length - 1] = crc;

	UartSendFrame(UartSendBuf, length);
#endif
}


/*****************************************************************************
函数名称 : UartReceiveInput
功能描述 : 收数据处理
输入参数 : value:串口收到字节数据
返回参数 : 无
使用说明 : 在MCU串口接收函数中调用该函数,并将接收到的数据作为参数传入
*****************************************************************************/
static void UartReceiveInput(unsigned char value)
{
	if((UartQueueIn > UartQueueOut) && ((unsigned int)(UartQueueIn - UartQueueOut) >= sizeof(UartQueueBuf)))
	{
		//数据队列满
		UartQueueIn= UartQueueOut;
	}
	else if((UartQueueIn < UartQueueOut) && ((UartQueueOut	- UartQueueIn) == 1))
	{
		//数据队列满
		UartQueueIn= UartQueueOut;
	}
	else
	{
		//队列不满
		if(UartQueueIn >= (unsigned char *)(UartQueueBuf + sizeof(UartQueueBuf)))
		{
			UartQueueIn = (unsigned char *)(UartQueueBuf);
		}

		*UartQueueIn ++ = value;
	}
}


/*****************************************************************************
函数名称 : UartGetQueueTotalData
功能描述 : 读取队列内数据
输入参数 : 无
返回参数 : 无
*****************************************************************************/
static unsigned char UartGetQueueTotalData(void)
{
	if(UartQueueIn != UartQueueOut)
		return 1;
	else
		return 0;
}
/*****************************************************************************
函数名称 : Queue_Read_Byte
功能描述 : 读取队列1字节数据
输入参数 : 无
返回参数 : 无
*****************************************************************************/
static unsigned char UartQueueReadByte(void)
{
	unsigned char value = 0;

	if(UartQueueOut != UartQueueIn)
	{
		//有数据
		if(UartQueueOut >= (unsigned char *)(UartQueueBuf + sizeof(UartQueueBuf)))
		{
			//数据已经到末尾
			UartQueueOut = (unsigned char *)(UartQueueBuf);
		}

		value = *UartQueueOut ++;   
	}

	return value;
}



/**********************************************************************/
//Description:	UartService()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void UartService(void)
{
	static uint16_t rx_in = 0;
	uint8_t offset = 0;
	uint8_t rx_value_len = 0;			 //数据帧长度

	uint8_t CharValue = 0;
	
	while((rx_in < sizeof(UartReadBuf)) && (UartGetQueueTotalData() > 0))
	{
		CharValue = UartQueueReadByte();
		UartReadBuf[rx_in ++] = CharValue;//UartQueueReadByte();
	}

	if(rx_in < UART_MSG_FIXED_LENGTH)
	{
		return;
	}

	//check real  data 
	while((rx_in - offset) > UART_MSG_FIXED_LENGTH)
	{
		if(UartReadBuf[offset + UART_MSG_HEAD] != UART_TX_FRAME_HEADER)
		{
			offset ++;	
			continue;
		}

		//rx_value_len = wifi_uart_rx_buf[offset + LENGTH_HIGH] * 0x100 + wifi_uart_rx_buf[offset + LENGTH_LOW] + PROTOCOL_HEAD;
		rx_value_len = UartReadBuf[offset + UART_MSG_LENGTH] + UART_MSG_FIXED_LENGTH;

		if(rx_value_len > UART_MSG_MAX_DATA_LENGTH)
		{
			offset += UART_MSG_LENGTH;
		}

		//data not receive over
		if((rx_in - offset) < rx_value_len)
		{
			break;
		}
		
		//数据接收完成,and compare check sum 
		if(GetCrc8((unsigned char *)UartReadBuf + offset, rx_value_len - 1) == UartReadBuf[offset + rx_value_len - 1])
		{
			printf("Crc is ok\n");
			UartHandleMsg(offset);
		}
		else
		{
			printf("Crc Err\n");
		}

		//offset is right start position
		offset += rx_value_len;
	}//end while
	
	rx_in -= offset;
	//copy remain data to first position
	if(rx_in > 0)
	{
		Memcpy(UartReadBuf, UartReadBuf + offset, rx_in);
	}	
}


/**********************************************************************/
//Description:	UartSendData()									
//Parameters:                        									
//Return:   															
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
bool UartSend(eUartMsgType UartMsgType, uint8_t MsgLevel1, uint8_t Data)
{
	uint8_t Length = 0;

	RfAO.MsgType = UartMsgType;
	RfAO.MsgLevel1 = MsgLevel1;
	RfAO.MsgData = Data;

	UART_IRQ_OUT_L();
	GCC_DELAY(1000);

	UartSendDataValue(0);
	Length = SetUartTxBufByte(Length,MsgLevel1);
	if(UartMsgType == eUartMsgPair)
	{
		Length = SetUartTxBufByte(Length,(uint8)RfAO.DevType);	
		Length = SetUartTxBufByte(Length,(uint8)RfAO.ProType);
	}
	
	if(Data != 0)
	{
		Length = SetUartTxBufByte(Length,Data);
	}
	
	UartWriteFrame(UartMsgType, Length);
	UartSendDataValue(0);

	UART_IRQ_INPUT();
	return TRUE;
}


/**********************************************************************/
//Description:	RfSendMsg()									
//Parameters:                        									
//Return:   															
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
void UartSendMsg(eUartMsgType UartMsgType, uint8_t MsgLevel1, uint8_t Data)
{
	UartSend(UartMsgType, MsgLevel1, Data);
	TmrInsertTimer(eTmrHdlUartSendTimeout, TMR_PERIOD(1000), (void *)0);
}

/**********************************************************************/
//Description:	UartSendAck()									
//Parameters:                        									
//Return:   															
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
bool UartSendAck(uint8_t UartMsgType, uint8_t TypeAck,uint8_t UartMsgLevel)
{
	uint8_t Length = 0;
	Length = SetUartTxBufByte(Length,TypeAck);
	Length = SetUartTxBufByte(Length,UartMsgLevel);
	UartWriteFrame(UartMsgType, Length);
	
	return TRUE;
}


/**********************************************************************/
//Description:	UartPairMsgHandle()											
//Parameters:                        									
//Return:   															
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
static void UartPairMsgHandle(uint8_t PairStates, uint8_t MsgData)
{
	if(PairStates == UartMsgPairAck)
	{
		if(RfAO.MsgLevel1 == MsgData)
		{
			TmrDeleteTimer(eTmrHdlUartSendTimeout);
			if(MsgData == UartMsgPairResetId)								//RestID
			{
				SysteSetSig(eEventSigResetIDOK);
			}
		}
	}
	else
	{
		UartSendAck(eUartMsgPair, UartMsgPairAck, PairStates);

		switch (PairStates)
		{
			case UartMsgPairSuccess:
				SysteSetSig(eEventSigPairSuccess);
				break;

			case UartMsgPairFail:
				SysteSetSig(eEventSigPairFail);
				break;	

			default:
				break;
		}
	}
}


/**********************************************************************/
//Description:	UartSmokeMsgHandle()											
//Parameters:                        									
//Return:   															
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
static void UartSmokeMsgHandle(uint8_t SmokeMsgType, uint8_t MsgData)
{
	if(SmokeMsgType == UartFirmAlarmMsgAck)
	{
		if(RfAO.MsgLevel1 == MsgData)
		{
			TmrDeleteTimer(eTmrHdlUartSendTimeout);
		}
	}
	else
	{
		UartSendAck(eUartMsgFireAlarm, UartFirmAlarmMsgAck, SmokeMsgType);	

		//printf("RxLocation=%d\n", MsgData);
		switch (SmokeMsgType)
		{
			case UartFirmAlarmMsgMute:
				break;
				
			case UartFirmAlarmMsgTest:
				if(MsgData == DevTestStart)
				{
					if(GetRfPairedMode() == ePairedHub)
					{
						SysteSetSig(eEventSigRadioHubTest);
					}
					else
					{
						SysteSetSig(eEventSigRadioIntranetTestStart);
					}
				}
				else if(MsgData == DevTestStop)
				{
					SysteSetSig(eEventSigRadioIntranetTestStop);
				}
				break;
				
			case UartFirmAlarmMsgLowBat:
				break;
				
			case UartFirmAlarmMsgHeatAlarm:
			case UartFirmAlarmMsgSmokeAlarm:
				SysteSetSig(eEventSigRadioSmokeAlarm);
				RfAO.RxLocation = MsgData;
				break;

			case UartFirmAlarmMsgCoAlarm:
				SysteSetSig(eEventSigRadioCoAlarm);
				RfAO.RxLocation = MsgData;
				break;

			case UartFirmAlarmMsgCancelAlarm:
				SysteSetSig(eEventSigRadioCancelAlarm);
				break;

			default:
				break;
		}
	}
}


/**********************************************************************/
//Description:	UartHandleMsg()											
//Parameters:                        									
//Return:   															
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
static void UartHandleMsg(uint8_t DataOffset)
{
#ifdef SUPPORT_NET_FUNCTION_DEBUG
	uint8_t i = 0;
	uint8_t PrintLen = 0;
#endif
	uint8_t DataBuf[4] = {0};
	eUartMsgType UartMsgType = (eUartMsgType)UartReadBuf[DataOffset + UART_MSG_TYPE];
	uint8_t UartMsgDataLength = UartReadBuf[DataOffset + UART_MSG_LENGTH];

	Memcpy((void *)DataBuf, (void *)&UartReadBuf[DataOffset + UART_MSG_DATA_START], UartMsgDataLength);
#ifdef SUPPORT_NET_FUNCTION_DEBUG
	PrintLen = UartMsgDataLength + UART_MSG_FIXED_LENGTH;	
	printf("Uart Rx: ");
	for(i=0; i<PrintLen; i++)
	{
		printf("%x ", UartReadBuf[DataOffset + i]);	
	}
	printf("\n");	
	
	printf("Type=%d, Data: ", UartMsgType);
	for(i=0; i<UartMsgDataLength; i++)
	{
		printf("%x ", DataBuf[i]);	
	}
	printf("\n");
#endif
	switch(UartMsgType)
	{	
		case eUartMsgPair:
			UartPairMsgHandle(DataBuf[0], DataBuf[1]);
			break;

		case eUartMsgFireAlarm:
			UartSmokeMsgHandle(DataBuf[0], DataBuf[1]);
			break;
			
		default:
			break;
	}
}

/**********************************************************************/
//Description:		RfPairMain
//Parameters:                        								
//Return:   											
//Date:  
//author:			quanwu.xu
/**********************************************************************/
void RfPairMain(uint8_t PairMode)
{
	static bool ResetFlag = FALSE;

	printf("pair mode, sig=%d\n", SysAO.sig);
	switch(SysAO.sig)
	{
		case eEventSigEnteryMode:
			ResetFlag = FALSE;
			
			switch(PairMode)
			{
				case RadioFactoryMode:
					UartSend(eUartMsgPair, UartMsgPairFactoryMode, 0);
					
					TmrInsertTimer(eTmrHdlRFLed, TMR_PERIOD(500), (void*)RadioFactoryMode);
					TmrInsertTimer(eTmrHdlRFTimeout, TMR_PERIOD(10000), NULL);
					break;
			
				case RadioPairSlaverMode:
#ifdef SUPPORT_SPEAK_FUNCTION
					SpeakerSendCmd(CMD_READ_TO_CONNECT);
#endif
					PollingBeep(200);
					UartSendMsg(eUartMsgPair, UartMsgPairSlaverMode, 0);
					TmrInsertTimer(eTmrHdlRFLed, TMR_PERIOD(250), (void*)RadioPairSlaverMode);
					TmrInsertTimer(eTmrHdlRFTimeout, TMR_PERIOD(60000), NULL);
					break;

				case RadioPairMasterMode:
#ifdef SUPPORT_SPEAK_FUNCTION
					SpeakerSendCmd(CMD_SEARCHING_DEVICE);
#endif
					PollingBeep(200);
					UartSendMsg(eUartMsgPair, UartMsgPairMasterMode, 0);
					TmrInsertTimer(eTmrHdlRFLed, TMR_PERIOD(2000), (void*)RadioPairMasterMode);
					TmrInsertTimer(eTmrHdlRFTimeout, TMR_PERIOD(60000), NULL);
					break;
				
				case RadioPairHubMode:
#ifdef SUPPORT_SPEAK_FUNCTION
					SpeakerSendCmd(CMD_READ_TO_CONNECT);
#endif
					PollingBeep(200);
					UartSendMsg(eUartMsgPair, UartMsgPairHubMode, 0);
					TmrInsertTimer(eTmrHdlRFLed, TMR_PERIOD(1000), (void*)RadioPairMasterMode);
					TmrInsertTimer(eTmrHdlRFTimeout, TMR_PERIOD(60000), NULL);
					break;
			}
			break;

		case eEventSigPairSuccess:
			if(PairMode == RadioPairHubMode)
			{
				RfAO.PairMode = ePairedHub;
				PwrAO.BatOldLevel = BatLevelNull;
			}
			else
			{
				RfAO.PairMode = ePairedIntranet;
			}
			E2promWrite(FLASH_ADDR_PAIRED_MODE_FLAG, RfAO.PairMode);
			
			SysAO.eMode = NormalMode;
			TmrDeleteTimer(eTmrHdlRFLed);
			TmrDeleteTimer(eTmrHdlRFTimeout);
#ifdef SUPPORT_SPEAK_FUNCTION
			SpeakerSendCmd(CMD_CONNECT_SUCCESS);
#endif
			PollingBeep(200);
			PollingLED(200);
			break;

		case eEventSigResetIDOK:
			TmrDeleteTimer(eTmrHdlRFLed);
			TmrDeleteTimer(eTmrHdlRFTimeout);
#ifdef SUPPORT_SPEAK_FUNCTION
			SpeakerSendCmd(CMD_DISCONNECTED);
#endif
			RfAO.PairMode = ePairedNull;
			E2promWrite(FLASH_ADDR_PAIRED_MODE_FLAG, RfAO.PairMode);

			TmrInsertTimer(eTmrHdlRFTimeout, TMR_PERIOD(250), (void *)1);
			BEEP_ON();
			R_LED_ON();
			break;
			
		case eEventSigKey1Clik:
			if(ResetFlag == FALSE)
			{
#ifdef SUPPORT_SPEAK_FUNCTION
				SpeakerSendCmd(CMD_CONNECT_FAILED);
#endif
				SysAO.eMode = NormalMode;
				TmrDeleteTimer(eTmrHdlRFLed);
				TmrDeleteTimer(eTmrHdlRFTimeout);
				UartSendMsg(eUartMsgPair, UartMsgPairStop, 0);
			}
			break;

		case eEventSigKey1LongClik:
			if(SysAO.eMode==RadioPairMasterMode)
			{
				ResetFlag = TRUE;
				UartSendMsg(eUartMsgPair, UartMsgPairResetId, 0);
			}
			break;

		case eEventSigRadioTimeout:
#ifdef SUPPORT_SPEAK_FUNCTION
			SpeakerSendCmd(CMD_CONNECT_FAILED);
#endif
		case eEventSigExitMode:
			BEEP_OFF();
			R_LED_OFF();
			SysAO.eMode = NormalMode;
			TmrDeleteTimer(eTmrHdlRFLed);
			TmrDeleteTimer(eTmrHdlRFTimeout);
			break;

		default:
			break;
	}
}


/**********************************************************************/
//Description:		RfIntranetTestMain
//Parameters:                        								
//Return:   											
//Date:  
//author:			quanwu.xu
/**********************************************************************/
void RfIntranetTestMain(uint8_t TestMode)
{
	static uint8_t TimeCnt = 0;
	switch(SysAO.sig)
	{
		case eEventSigEnteryMode:
			if(TestMode == RadioIntranetSlaverTestMode)
			{
				TmrInsertTimer(eTmrHdlRFLed, TMR_PERIOD(1000), (void*)RadioIntranetSlaverTestMode);
			}
			else
			{
				UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgTest, DevTestStart);
				TmrInsertTimer(eTmrHdlRFLed, TMR_PERIOD(1000), (void*)RadioIntranetMasterTestMode);
			}
			TimeCnt = 0;
			TmrInsertTimer(eTmrHdlRFTimeout, TMR_PERIOD(60000), NULL);
			break;

		case eEventSigKey1Clik:
		case eEventSigRadioIntranetTestStop:
			if(TestMode == RadioIntranetSlaverTestMode)
			{
				SysAO.eMode = NormalMode;
				TmrDeleteTimer(eTmrHdlRFLed);
				TmrDeleteTimer(eTmrHdlRFTimeout);
				TmrDeleteTimer(eTmrHdlGreenLedFlicker);
			}
			break;

		case eEventSigRadioIntranetTestKeyStop:
			if(TestMode == RadioIntranetMasterTestMode)
			{
				UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgTest, DevTestStop);
			}
			SysAO.eMode = NormalMode;
			TmrDeleteTimer(eTmrHdlRFLed);
			TmrDeleteTimer(eTmrHdlRFTimeout);
			break;

		case eEventSigRadioTimeout:
			if(TimeCnt++ >= 1)
			{
				SysAO.eMode = NormalMode;
				TmrDeleteTimer(eTmrHdlRFLed);
				TmrDeleteTimer(eTmrHdlRFTimeout);
				TmrDeleteTimer(eTmrHdlGreenLedFlicker);
			}
			break;

		default:
			break;
	}

}

#else
# if 0
DEFINE_ISR(UART_ISR, 0x3C)
{		
	_urf = 0;
	//_ur1f = 0;

}
#endif
#endif

