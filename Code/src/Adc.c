/****************************************************************************/
/*	File   :             Adc.c               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/7/25,16:30:31                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/
#include "base.h"
#include "Adc.h"
#include "Co.h"
/**********************************************************************/
//Description:		GetAdcSample														  
//Parameters:                        												  
//Return:   											
//Date:  
//author:		    song
/**********************************************************************/
uint16 GetAdcSample(eAdcChannel Channel, bool CoFualtCheckFlag)
{ 
    uint8 i;
    uint16 TempTmp = 0;
    uint16 AdcTmp = 0;
    
	//Step 1, Clk
    _sacks2 = 0;         // fSYS/4
    _sacks1 = 1;
    _sacks0 = 0;

	//Step 2, Enable
	_adcen = 1;			// AD enable

	//Step 3, External
    _sains2 = 0;		//External input 
    _sains1 = 0;
    _sains0 = 0;

	//Step 4, Channel
	if(Channel == eAdcTempChannel)
	{
	    _sacs3 = 0;         //AN1
	    _sacs2 = 0;
	    _sacs1 = 0;
	    _sacs0 = 1;
	}
	else if(Channel == eAdcBatChannel)
	{
	    _sacs3 = 0;         //AN6
	    _sacs2 = 1;
	    _sacs1 = 1;
	    _sacs0 = 0;	
	}
	else if(Channel ==eAdcCoChannel)
	{
	    _sacs3 = 0;         //AN7
	    _sacs2 = 1;
	    _sacs1 = 1;
	    _sacs0 = 1;	
	}

	//Step 6, Rerf VDD
	//SAVRS1~SAVRS0(Bit4~3): A/D converter reference voltage select
	//00: From external VREF pin
	//01: Internal A/D converter power, VDD
	// 1x:From external VREF pin
	_savrs1 = 0;            //VDD 
	_savrs0 = 1; 

	//Step 7, Data format
	_adrfs = 1;				//output data format:lsb 11 bit
	
	_vbgen = 0;				//VBGRC disable

	if(Channel == eAdcTempChannel)
	{
		TEMPERATURE_DETECT_POWER_ON();               // temperature sensor power on 
	}
	else if(Channel == eAdcBatChannel)
	{
	#ifdef SUPPORT_XP02_BOARD
		BAT_DETECT_POWER_ON();
	#endif
	}
	else if((Channel == eAdcCoChannel) && (CoFualtCheckFlag == TRUE))
	{
		CO_DETECT_FAULT_ON();
		GCC_DELAY(10000);       // 10ms
		CO_DETECT_FAULT_OFF();
	}

	//Step 9, 10
	GCC_DELAY(100);       		// 100us
	for(i=0; i<5; i++)
	{
		//start conversion
		_start = 0;
		_start = 1;
		_start = 0;

		//wait the conversion compelete
		while(_adbz);           
	
		TempTmp= (uint16)(_sadoh);  
		TempTmp = (uint16)((uint16)((uint16)TempTmp << 8) | _sadol);
		if(i !=0)															//give up the first data
		{
			AdcTmp += TempTmp;
		}
	}
	AdcTmp >>= 2;

	if(Channel == eAdcTempChannel)
	{
		TEMPERATURE_DETECT_POWER_OFF();										// temperature sensor power off  
	}
	else if(Channel == eAdcBatChannel)
	{
	#ifdef SUPPORT_XP02_BOARD
		BAT_DETECT_POWER_OFF();
	#endif
	}
	else if((Channel == eAdcCoChannel) && (CoFualtCheckFlag == TRUE))
	{
		CO_DETECT_FAULT_OFF();
	}

	//Step 11
	_adcen = 0;             // adc disable

	return AdcTmp;
}



/**********************************************************************/
//Description:		AdcGetVddSample														  
//Parameters:                        												  
//Return:   											
//Date:  
//author:		    song
/**********************************************************************/
uint16 GetVddSample(eVddType Type)
{ 
    uint8 i;
    uint16 BatTmp = 0;
    uint16 AdcTmp = 0;
    
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
     1MHz  1��s          2��s          4��s         8��s          16��s *        32��s *       64��s *        128��s *
     2MHz  500ns         1��s          2��s         4��s          8��s           16��s *       32��s *        64��s *
     4MHz  250ns *       500ns         1��s         2��s          4��s           8��s          16��s *        32��s *
     8MHz  125ns *       250ns *       500ns        1��s          2��s           4��s          8��s           16��s *
    **********************************************************************************************************************/
    //_sacks2 = 0;         // fSYS/4
    //_sacks1 = 1;
    //_sacks0 = 0;
    GpioResetBits(_sadc1, GPIOBIT0_SELECT|GPIOBIT2_SELECT);
    GpioSetBits(_sadc1, GPIOBIT1_SELECT);
    //_vbgren = 1;//bit0
    
	GpioSetBits(_vbgrc, GPIOBIT0_SELECT);
	//_vbgen = 1;  //bit3
	GpioSetBits(_lvdc, GPIOBIT3_SELECT);
	GCC_DELAY(300); 		// 300us


    //_adcen = 1; //bit5            // AD enable

	GpioSetBits(_sadc0, GPIOBIT5_SELECT);

    /***********************************************************************
      SAINS[2:0](Bit 7~5)   SACS[3:0](Bit 3~0)           Input Signals            Description
      000,101~111           0000~0011                       AN0~AN3                 External pin analog input
                            0100~1111                       ��                      Non-existed channel, input is floating.
      001                   0100~1111                       VBGREF                  Internal high performance Bandgap reference voltage
      010                   0100~1111                       OPA0O                   Internal SD operational amplifier 0 output signal
      011                   0100~1111                       OPA1O                   Internal SD operational amplifier 1 output signal
      100                   0100~1111                       LINEV                   Internal PLT operational amplifier output signal
    *************************************************************************/
    //_sains2 = 0;            // VBGREF
    //_sains1 = 0;
    //_sains0 = 1;
    GpioResetBits(_sadc1, GPIOBIT6_SELECT|GPIOBIT7_SELECT);
	GpioSetBits(_sadc1, GPIOBIT5_SELECT);
    //_sacs3 = 0;             //inter input, no define
    //_sacs2 = 1;
    //_sacs1 = 0;
    //_sacs0 = 0;
	GpioResetBits(_sadc0, GPIOBIT0_SELECT|GPIOBIT1_SELECT|GPIOBIT3_SELECT);
	GpioSetBits(_sadc0, GPIOBIT2_SELECT);

    //SAVRS1~SAVRS0(Bit4~3): A/D converter reference voltage select
    //00: From external VREF pin
    //01: Internal A/D converter power, VDD
   // 1x : From external VREF pin
    //_savrs1 = 0;            //VDD 
    //_savrs0 = 1;  
	GpioResetBits(_sadc1, GPIOBIT4_SELECT);
	GpioSetBits(_sadc1, GPIOBIT3_SELECT);	
    //_adrfs = 1;             //output data format:lsb 11 bit
	GpioSetBits(_sadc0, GPIOBIT4_SELECT);
	if (eVddIsink==Type)
	{
		//_isgdata1 = 0;
		GpioWrite(_isgdata1, 0);
	    //_isgen = 1;            //Sink current generator enable bit7
	    //_isgs1 = 1;            //Enable ISINK1 pin sink current bit1
		GpioSetBits(_isgenc, GPIOBIT7_SELECT|GPIOBIT1_SELECT);
	}

    GCC_DELAY(100);         // 100us
    for(i=0; i<2; i++)
    {
        //start conversion bit7
        //_start = 0;
        //_start = 1;
        //_start = 0;
		GpioResetBits(_sadc0, GPIOBIT7_SELECT);
		GpioSetBits(_sadc0, GPIOBIT7_SELECT);
		GpioResetBits(_sadc0, GPIOBIT7_SELECT);

        //wait the conversion compelete
        while(_adbz);           

        BatTmp= (uint16)(_sadoh);  
        BatTmp = (uint16)((uint16)((uint16)BatTmp << 8) | _sadol);
       
        AdcTmp += BatTmp;
       
    }
    AdcTmp >>= 1;
    if (eVddIsink==Type)
	{
    	//_isgen = 0;            //Sink current generator enable
    	//_isgs1 = 0;            //Disable ISINK1 pin sink current
	 	GpioResetBits(_isgenc, GPIOBIT7_SELECT|GPIOBIT1_SELECT);
    }
    //_vbgren = 0;
    //_vbgen = 0;
    GpioResetBits(_vbgrc, GPIOBIT0_SELECT);
	
	GpioResetBits(_lvdc, GPIOBIT3_SELECT);  
    //_adcen = 0;             // adc disable
	GpioResetBits(_sadc0, GPIOBIT5_SELECT);

    return AdcTmp;
}

