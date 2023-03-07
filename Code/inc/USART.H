/**
  ******************************************************************************
  * @file    usart.h
  * @author  zhenrong.peng
  * @version V1.0
  * @date    01-04-2017
  * @brief   Header for usart.c
  *****************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H
#define __USART_H

/* Includes ------------------------------------------------------------------*/
#include "base.h"




#define UartSendDataValue(Data)	do{_txr_rxr0 = Data;while(_tidle0 == 0);} while(0);	
#define UartSendData(Data)	do{_txr_rxr0 = *Data;while(_tidle0 == 0);} while(0);	


typedef enum{
	eUMD = (0x01<<4),
	eSIMEN = (0x01<<1),
	eSIMICF = (0x01<<0),
}eSIMC0;

typedef enum{
	eUPERR = (0x01<<7),
	eUNF = (0x01<<6),
	eUFERR = (0x01<<5),
	eUOERR = (0x01<<4),
	eURIDLE = (0x01<<3),
	eURXIF = (0x01<<2),
	eUTIDLE = (0x01<<1),
	eUTXIF = (0x01<<0),
}eUUSR;


typedef enum{
	eUREN = (0x01<<7),
	eUBNO = (0x01<<6),
	eUPREN = (0x01<<5),
	eUPRT = (0x01<<4),
	eUSTOPS = (0x01<<3),
	eUTXBRK = (0x01<<2),
	eUTX8 = (0x01<<0),
}eUUCR1;

typedef enum{
	eUTXEN = (0x01<<7),
	eURXEN = (0x01<<6),
	eUBRGH = (0x01<<5),
	eUADDEN = (0x01<<4),
	eUWAKE = (0x01<<3),
	eURIE = (0x01<<2),
	eUTIIE = (0x01<<1),
	eUTEIE = (0x01<<0),
}eUUCR2;

#define SIMC0     (_simc0)
#define UUSR 	  (_uusr)
#define UUCR1     (_uucr1)
#define UUCR2	  (_uucr2)
#define UTXR_RXR  (_utxr_rxr)	
#define UBRG  	  (_ubrg)	

#define SetUSIME()  (_sime =1)
#define GetUSIMF()  (_simf)

void UsartCfg(void);
void printf(char * Data, ...);
//void PrintVol(uint16 Temp);
#define  PrintVol(Temp) \
do{\
	printf("%d", Temp/1000);\
	printf(".%d", Temp%1000/100);\
	printf("%d", Temp%100/10);\
	printf("%d", Temp%10);\
}while(0)

#endif /* __USART_H */



/************************END OF FILE**************************/




