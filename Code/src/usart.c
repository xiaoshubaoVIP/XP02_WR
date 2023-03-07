#include "usart.h"
#include "PwrManager.h"
#include "main.h"

#ifdef SUPPORT_DEBUG
typedef char *va_list;
#define _INTSIZEOF(n) ( (sizeof(n) + sizeof(char) - 1) & ~(sizeof(char) - 1) )
#define va_start(ap,v) ( ap = (va_list)&v + _INTSIZEOF(v) )
#define va_arg(ap,t) ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define va_end(ap) ( ap = (va_list)0 )

/**********************************************************************/
//Description:		sky_itoa															  
//Parameters:       将int型数据转为2，8，10，16进制字符串
//Return:   																  
//Date:            
//author:		quanwu.xu 
/**********************************************************************/
static char *sky_itoa(int value, char *str, unsigned int radix)
//static char *sky_itoa(long value, char *str, unsigned int radix)
{
	const char list[] = "0123456789ABCDEF";
	unsigned int tmp_value;
//	unsigned long tmp_value;
	
	int i = 0, j, k = 0;

	if (NULL == str)
	{
		return NULL;
	}

	if (2 != radix && 8 != radix && 10 != radix && 16 != radix) 
	{	
		return NULL;
	}

	if (radix == 10 && value < 0) 
	{
		//十进制且为负数
		tmp_value = (unsigned int)(0 - value);
//		tmp_value = (unsigned long)(0 - value);		
		str[i++] = '-';
		k = 1;
	}
	else 
	{
		tmp_value = (unsigned int)value;	
//		tmp_value = (unsigned long)value;
	}

	//数据转换为字符串，逆序存储

	do {
		str[i ++] = list[tmp_value%radix];
		tmp_value /= radix;
	} while(tmp_value);

	str[i] = '\0';

	//将逆序字符串转换为正序
	char tmp;
	for (j = k; j < (i+k)/2; j++)
	{
		tmp = str[j];
		str[j] = str[i-j-1+k];
		str[i-j-1+k] = tmp;
	}

	return str;
}

/**********************************************************************/
//Description:		Printf														  
//Parameters:                        												  
//Return:   											
//Date:  
//author:		quanwu.xu 
/**********************************************************************/
void printf(char * Data, ...)
{
	const char *s;
	int d;  
	char buf[16];
	uint8 txdata;
	va_list ap;
	va_start(ap, Data);
  	while ( * Data != 0 ) 
	{				                          
    		if ( * Data == 0x5c ) 
		{									  
      			switch ( *++Data ) 
			{
				case 'r':							          
					txdata = 0x0d;
					UartSendData((const char *)&txdata);
					Data ++;
					break;

				case 'n':							          
					txdata = 0x0a;
					UartSendData((const char *)&txdata);
					Data ++;
					break;

			        default:
			          Data ++;
			          break;

     			 }			 
    		}
		else if ( * Data == '%')
		{									  
      			switch ( *++Data ) 
			{				
      				case 's':										 
        				s = va_arg(ap, const char *);
        				for ( ; *s; s++) 
						{
          					UartSendData((const char*)s);
        				}				
        				Data++;				
        				break;

			      case 'd':			
					d = va_arg(ap, int );							
					sky_itoa(d, buf, 10);					
					for (s = buf; *s; s++) {
						UartSendData((const char*)s);
					}					
					Data++;				
					break;

				case 'x': 
				{
					d = va_arg(ap, int);	
					sky_itoa(d, buf, 16);					
					for (s = buf; *s; s++) 
					{
						UartSendData((const char*)s);
					}					
					Data++;			
					break;
				}
#ifdef PRINTF_FLAOT_TYPE
			      case 'f':
				{
					double num = va_arg(ap, double);					
					sky_ftoa(num, buf, 4);
					for (s = buf; *s; s++) 
					{
						UartSendData(s);
					}					

						Data++;			
					break;
      				}
#endif
			      default:
					Data++;				
					break;				

      			}		 

	    	}
		else 
		{
	        	UartSendData((const char*)Data);
	        	Data++;
	  	}
  	}
}


#endif

#ifdef SUPPORT_UART
/**********************************************************************/
//Description:		UsartCfg														  
//Parameters:                        												  
//Return:   											
//Date:  
//author:		    zhengrong.peng
/**********************************************************************/
//Note: Baud rate=fH/[64×(N+1)] if BRGHn=0;
//Baud rate=fH/[16×(N+1)] if BRGHn=1.
void UsartCfg(void)
{
	// open rx intr mfe  bit3
	GpioSetBits(_intc3, GPIOBIT0_SELECT);		//MF3E, 多功能中断3使能
	GpioSetBits(_mfi3, GPIOBIT2_SELECT);		//串口中断控制位
	GpioSetBits(_u0cr1, GPIOBIT7_SELECT);
	GpioWrite(_brg0,25);//4m 25 9600,8m 51 115200,4m 12 19200,4m 12 38400 

	//_uarten = 1;

    //UBNO(Bit 6): Number of data transfer bits selection
   // _bno = 0;      //8 bit data transfer

    //UPREN(Bit 5): Parity function enable control
    //_pren = 0;     //Parity function is disabled

    //UPRT(Bit 4): Parity type selection bit
    //_prt

    //USTOPS(Bit 3): Number of Stop bits selection
    //_stops = 0;    //One stop bit format is used

    //UTXBRK(Bit 2): Transmit break character
    //_txbrk = 0;    //No break character is transmitted
	GpioSetBits(_u0cr2, GPIOBIT7_SELECT|GPIOBIT6_SELECT|GPIOBIT5_SELECT|GPIOBIT3_SELECT|GPIOBIT2_SELECT);
	//GpioSetBits(_u0cr2, GPIOBIT7_SELECT|GPIOBIT5_SELECT);

    //UBRGH(Bit 5): Baud Rate speed selection
    //_brgh = 1;     //High speed baud rate
    
    //UADDEN(Bit 4): Address detect function enable control
    //_adden = 0;    //Address detect function is disabled

    //UWAKE(Bit 3): RX pin wake-up UART function enable control
    _wake0 = 0;     //RX pin wake-up UART function is disabled

    //URIE(Bit 2): Receiver interrupt enable control
    //_rie = 0;      //Receiver interrupt disabled

    //UTIIE(Bit 1): Transmitter Idle interrupt enable control
    //_tiie = 0;     //Transmitter idle interrupt disabled

    //UTEIE(Bit 0): Transmitter Empty interrupt enable control
    //_teie = 0;     //Transmitter empty interrupt disabled
	

    //Baud rate= fH / [64 × (N+1)] if UBRGH=0.
    //Baud rate= fH / [16 × (N+1)] if UBRGH=1.
    //_brg = 25;       //N=fH/(16*BR)-1   9600

    //UTXEN(Bit 7): UART Transmitter enabled control
    //_txen = 1;     //UART transmitter is enabled
}

#endif


/********************END OF FILE**********************/




