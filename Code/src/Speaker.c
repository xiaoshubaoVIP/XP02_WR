#include "main.h"
#include "Module.h"
#include "Speaker.h"

#ifdef SUPPORT_SPEAK_FUNCTION

#define SetDataPin()		{SPEAK_DATA_PIN = 1;}
#define ResetDataPin()		{SPEAK_DATA_PIN = 0;}
#define	VOICE_IS_BUSY() 	(SPEAK_BUSY_PIN != RESET)

/**********************************************************************/
//Description:	SpeakerSendCmd											
//Parameters:                        									
//Return:   															
//Date:           quanwu.xu  
/**********************************************************************/
void SendCmd(uint8_t VoiceData)//25ms
{
	uint8_t i = 0;

	SetDataPin();
	GCC_DELAY(8000);							//8ms
	ResetDataPin();
	GCC_DELAY(1000);							//1ms

	for(i = 0; i < 8; i++)
	{
		if(VoiceData & 0x80)				// output 1
		{
			SetDataPin() ;
			GCC_DELAY(1500);				
			ResetDataPin();
			GCC_DELAY(500);
		}
		else								// output 0
		{
			SetDataPin() ;
			GCC_DELAY(500);
			ResetDataPin();
			GCC_DELAY(1500);
		}
		VoiceData = VoiceData <<1;			// �ȷ��͸�bit	
	}
}

/**********************************************************************/
//Description:	SpeakerSendCmdForPowerOn								
//Parameters:                        									
//Return:   															
//Date:           quanwu.xu  
/**********************************************************************/
void SpeakerSendCmdForPowerOn(uint8_t VoiceData)
{
	EnableVoiceVcc();
	
	GCC_DELAY(50000);
	while(VOICE_IS_BUSY());
	SendCmd(CMD_MUSIC_VOLUME_15);
	SendCmd(VoiceData);
	GCC_DELAY(5000);
	while(VOICE_IS_BUSY())
	{
		_clrwdt();
	}

	DisableVoiceVcc();
}


/**********************************************************************/
//Description:	SpeakerSendCmd											
//Parameters:                        									
//Return:   															
//Date:           quanwu.xu  
/**********************************************************************/
void SpeakerSendCmd(uint32_t VoiceData)
{
	uint8_t Data = 0;
	
	EnableVoiceVcc();
	GCC_DELAY(50000);
	while(VOICE_IS_BUSY());
	SendCmd(CMD_MUSIC_VOLUME_15);

	Data = VoiceData>>24;
	if(Data != 0)
	{
		//SpeakerSend(Data);
		SendCmd(Data);
		GCC_DELAY(5000);
		while(VOICE_IS_BUSY())
		{
			_clrwdt();
			Polling();
			KeyHandleMsg();
#ifdef SUPPORT_RF_NET_FUNCTION
			PollingUartSend();
#endif	

#ifdef SUPPORT_RF_NET_FUNCTION
			UartService();
#endif
		}
	}

	Data = (VoiceData>>16)&0xFF;
	if(Data != 0)
	{
		//SpeakerSend(Data);
		SendCmd(Data);
		GCC_DELAY(5000);
		while(VOICE_IS_BUSY())
		{
			_clrwdt();
			Polling();
			KeyHandleMsg();
#ifdef SUPPORT_RF_NET_FUNCTION
			PollingUartSend();
#endif	

#ifdef SUPPORT_RF_NET_FUNCTION
			UartService();
#endif
		}
	}

	Data = (VoiceData>>8)&0xFF;
	if(Data != 0)
	{
		//SpeakerSend(Data);
		SendCmd(Data);
		GCC_DELAY(5000);
		while(VOICE_IS_BUSY())
		{
			_clrwdt();
			Polling();
			KeyHandleMsg();
#ifdef SUPPORT_RF_NET_FUNCTION
			PollingUartSend();
#endif	

#ifdef SUPPORT_RF_NET_FUNCTION
			UartService();
#endif
		}
	}

	Data = VoiceData&0xFF;
	//SpeakerSend(Data);
	SendCmd(Data);
	GCC_DELAY(5000);
	while(VOICE_IS_BUSY())
	{
		_clrwdt();
		Polling();
		KeyHandleMsg();
#ifdef SUPPORT_RF_NET_FUNCTION
		PollingUartSend();
#endif	

#ifdef SUPPORT_RF_NET_FUNCTION
		UartService();
#endif
	}

	DisableVoiceVcc();
}

/**********************************************************************/
//Description:	SpeakerInit											
//Parameters:                        									
//Return:   															
//Date:           quanwu.xu  
/**********************************************************************/
void SpeakerInit(void)
{
	EnableVoiceVcc();
	GCC_DELAY(10000);

	SpeakerSendCmd(CMD_MUSIC_VOLUME_15);

	DisableVoiceVcc();
}
#endif

