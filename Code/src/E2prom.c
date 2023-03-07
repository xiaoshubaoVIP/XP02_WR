/****************************************************************************/
/*	File   :             E2prom.c               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/7/25,16:33:48                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/
#include "base.h"
#include "E2prom.h"


/**********************************************************************/
//Description:		E2promWrite														  
//Parameters:                        												  
//Return:   											
//Date:  
//author:		    zhengrong.peng
/**********************************************************************/
void E2promWrite(uint8 addr, uint8 data)
{ 
    
	//_eea = addr;
	//_eed = data;
	_emi=0;
	GpioWrite(_eea, addr);
	GpioWrite(_eed, data);
	//_mp1l = 0x40;       //MP1 points to EEC register
	//_mp1h = 0x01;       //setup Bank Pointer
	GpioWrite(_mp1l, 0x40);
	GpioWrite(_mp1h, 0x01);

	//_emi = 0;           //disable global interrupt
//	GpioResetBits(_intc0, GPIOBIT0_SELECT);

	//_iar1 = 0x08;
	GpioWrite(_iar1, GPIOBIT3_SELECT);
	//_iar1 |= 0x04;
	GpioSetBits(_iar1, GPIOBIT2_SELECT);

	//_emi = 1;           //enable global interrupt
//	GpioSetBits(_intc0, GPIOBIT0_SELECT);

	while(_iar1 & GPIOBIT2_SELECT);

	//_eec = 0;           //disable EEPROM read/write
	//_iar1 = 0;
	//_mp1h = 0;
   	GpioWrite(_iar1, 0x0);
	GpioWrite(_mp1h, 0x0);
	_emi=1;
}

/**********************************************************************/
//Description:		E2promRead														  
//Parameters:                        												  
//Return:   											
//Date:  
//author:		    zhengrong.peng
/**********************************************************************/
uint8 E2promRead(uint8 addr)
{ 
    uint8 ReadData=0;
   
    //_eea = addr;
	_emi=0;
	GpioWrite(_eea, addr);
	  //_mp1l = 0x40;		 //MP1 points to EEC register
	   //_mp1h = 0x01;		 //setup Bank Pointer
	   GpioWrite(_mp1l, 0x40);
	   GpioWrite(_mp1h, 0x01);

   // _iar1 = 0x02;
    //_iar1 |= 0x01;
 //   GpioWrite(_iar1, GPIOBIT1_SELECT|GPIOBIT0_SELECT);
	GpioWrite(_iar1, GPIOBIT1_SELECT);
	GpioSetBits(_iar1, GPIOBIT0_SELECT);
//	_iar1=_iar1|0x03;


    while(_iar1 & 0x01);
    //_iar1 = 0;
    //_mp1h = 0;
   	GpioWrite(_iar1, 0x0);
	GpioWrite(_mp1h, 0x0);

    ReadData = _eed;
	_emi=1;
    return ReadData;
}

/*
uint16 Read_Flash_Temp(uint16 addr)
{
	uint8 FD_H=0;
	uint8 FD_L=0;
	uint16 FlashTemp=0;
	_fmod0=0;
	_fmod1=1;
	_fmod2=1;
	_frden=1;
	_farh=addr>>8;
	_farl=addr&0xFF;
	_frd=1;
	while(!_frd);
	FD_L=_fd0l;
	FD_H=_fd0h;
	_frden=0;
	FlashTemp=FD_H;
	FlashTemp=FlashTemp<<8|FD_L;
	return FlashTemp;			
}
*/