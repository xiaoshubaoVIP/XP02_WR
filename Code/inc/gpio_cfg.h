/**
  ******************************************************************************
  * @file    gpio_cfg.h
  * @author  zhenrong.peng
  * @version V1.0
  * @date    01-04-2017
  * @brief   This file contains definitions for STM8L051F3's port config hardware
  *          resources.
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_CFG_H
#define __GPIO_CFG_H

/* Includes ------------------------------------------------------------------*/
#include "BA45F5260.h"

//#define  BOARD_OLD 

#define GPIOBIT0_SELECT (0x01)
#define GPIOBIT1_SELECT (0x01<<1)
#define GPIOBIT2_SELECT (0x01<<2)
#define GPIOBIT3_SELECT (0x01<<3)
#define GPIOBIT4_SELECT (0x01<<4)
#define GPIOBIT5_SELECT (0x01<<5)
#define GPIOBIT6_SELECT (0x01<<6)
#define GPIOBIT7_SELECT (0x01<<7)

#define GpioWrite(GPIOx,GPIO_PortVal) (GPIOx = (GPIO_PortVal))
#define GpioSetBits(GPIOx,GPIO_PortBit) (GPIOx |=(GPIO_PortBit) )
#define GpioToggleBit(GPIOx,GPIO_PortBit) (GPIOx ^=(GPIO_PortBit) )

#define GpioResetBits(GPIOx,GPIO_PortBit) (GPIOx &= ~(GPIO_PortBit))
#define GpioReadInputData(GPIOx)  (GPIOx)
#define GpioReadInputBit(GPIOx,GPIO_PortBit)  (GPIOx&(GPIO_PortBit))

#define REGWrite(REG,REG_PortVal) 			(REG = (REG_PortVal))
#define REGSetBits(REG,REG_PortBit) 		(REG |=(REG_PortBit) )
#define REGResetBits(REG,REG_PortBit) 		(REG &= ~(REG_PortBit))
#define REGReadInputData(REG)  				(REG)
#define REGReadInputBit(REG,REG_PortBit)  	(REG&(REG_PortBit))

#define PORTAWAKE (_pawu)     
#define PORTAPULL (_papu)     
#define PORTA     (_pa)      
#define PORTAINOUT  (_pac)      

   
#define PORTBPULL (_pbpu)     
#define PORTB     (_pb)      
#define PORTBINOUT  (_pbc)  

//PORT
#define  PORTACONFIG    (_pas0)
#define  PORTBCONFIG    (_pbs0)
#define  PORTA1CONFIG    (_pas1)
#define  PORTB1CONFIG    (_pbs1)

#ifdef  SUPPORT_XP02_BOARD
/* Exported types ------------------------------------------------------------*/

/**
 * @brief LED
 */
#ifdef BOARD_OLD
#define RED_LED_PIN                   	_pa3
#define RED_LED_ON()               		RED_LED_PIN = 1
#define RED_LED_OFF()               	RED_LED_PIN = 0
#define RED_LED_TOGGLE()				RED_LED_PIN = ~RED_LED_PIN
#else
#define R_LED_PIN                   	_pa2
#define R_LED_ON()               		R_LED_PIN = 1
#define R_LED_OFF()               		R_LED_PIN = 0
#define R_LED_TOGGLE()					R_LED_PIN = ~R_LED_PIN

#define G_LED_PIN                   	_pb4
#define G_LED_ON()               		G_LED_PIN = 1
#define G_LED_OFF()               		G_LED_PIN = 0
#define G_LED_TOGGLE()					G_LED_PIN = ~G_LED_PIN

#define Y_LED_ON()						{R_LED_PIN = 1; G_LED_PIN = 1;}
#define Y_LED_OFF()						{R_LED_PIN = 0; G_LED_PIN = 0;}
#define Y_LED_TOGGLE()					{R_LED_PIN = ~R_LED_PIN; G_LED_PIN = ~G_LED_PIN;}
#endif


/**
 * @brief BEEP
 */
#define BEEP_VCC_EN_PIN					_pa5
#define BEEP_PIN		_pc4
#define BeepInit()		{_pcc4=0; _pcpu4 =1; _pc4=0;}
#define BEEP_ON()		{BEEP_VCC_EN_PIN = 0;BEEP_PIN = 1;}
#define BEEP_OFF()		{BEEP_VCC_EN_PIN = 1;BEEP_PIN = 0;}
#define BEEP_TOGGLE()	{BEEP_VCC_EN_PIN = ~BEEP_VCC_EN_PIN;BEEP_PIN = ~BEEP_PIN;}



/**
 * @brief Key push-button
 */
#define TEST_KEY_PIN                      	_pa4


/**
 * @brief adc
 */
//ntc sensor
#ifdef BOARD_OLD
#define TEMPERATURE_DETECT_NTC_POWER_PIN    _pb2
#define TEMPERATURE_DETECT_POWER_ON()       TEMPERATURE_DETECT_NTC_POWER_PIN = 1
#define TEMPERATURE_DETECT_POWER_OFF()      TEMPERATURE_DETECT_NTC_POWER_PIN = 0
#else
#define TEMPERATURE_DETECT_NTC_POWER_PIN    _pb4
#define TEMPERATURE_DETECT_POWER_ON()       TEMPERATURE_DETECT_NTC_POWER_PIN = 1
#define TEMPERATURE_DETECT_POWER_OFF()      TEMPERATURE_DETECT_NTC_POWER_PIN = 0
#endif


#define BAT_DETECT_NTC_POWER_PIN			_pb6
#define BAT_DETECT_POWER_ON()				BAT_DETECT_NTC_POWER_PIN = 1  
#define BAT_DETECT_POWER_OFF()				BAT_DETECT_NTC_POWER_PIN = 0



//smoke control

#ifdef SUPPORT_RF_NET_FUNCTION
#define UART_INT_IRQ_PIN				_pa1
#define UART_IRQ_OUT_L()				{GpioResetBits(_pac,GPIOBIT1_SELECT);\
										GpioResetBits(_pa, GPIOBIT1_SELECT);}
#define UART_IRQ_INPUT()				{GpioSetBits(_pa, GPIOBIT1_SELECT);\
										GpioSetBits(_pac, GPIOBIT1_SELECT);}
#endif

#ifdef SUPPORT_SPEAK_FUNCTION
#define SPEAK_VCC_EN_PIN					_pa5
#define SPEAK_DATA_PIN						_pc5
#define SPEAK_BUSY_PIN						_pa3
#endif

#ifdef SUPPORT_SPEAK_FUNCTION
#define SET_KEY_PIN							_pa0
#endif

#elif defined SUPPORT_XP01_BOARD 
/* Exported types ------------------------------------------------------------*/


#define R_LED_PIN                   	_pa5
#define R_LED_ON()               		R_LED_PIN = 1
#define R_LED_OFF()               		R_LED_PIN = 0
#define R_LED_TOGGLE()					R_LED_PIN = ~R_LED_PIN

#define G_LED_PIN                   	_pa1
#define G_LED_ON()               		G_LED_PIN = 1
#define G_LED_OFF()               		G_LED_PIN = 0
#define G_LED_TOGGLE()					G_LED_PIN = ~G_LED_PIN

#define Y_LED_ON()						{R_LED_PIN = 1; G_LED_PIN = 1;}
#define Y_LED_OFF()						{R_LED_PIN = 0; G_LED_PIN = 0;}
#define Y_LED_TOGGLE()					{R_LED_PIN = ~R_LED_PIN; G_LED_PIN = ~G_LED_PIN;}


/**
 * @brief BEEP
 */
//#define BEEP_VCC_EN_PIN					_pa5
#define BEEP_PIN		_pc4
#define BeepInit()		{_pcc4=0; _pcpu4 =1; _pc4=0;}
#define BEEP_ON()		BEEP_PIN = 1;
#define BEEP_OFF()		BEEP_PIN = 0;
#define BEEP_TOGGLE()   BEEP_PIN = ~BEEP_PIN;
//#define BEEP_TOGGLE()	{BEEP_VCC_EN_PIN = ~BEEP_VCC_EN_PIN;BEEP_PIN = ~BEEP_PIN;}



/**
 * @brief Key push-button
 */
#define TEST_KEY_PIN                      	_pa4



#define TEMPERATURE_DETECT_NTC_POWER_PIN    _pb4
#define TEMPERATURE_DETECT_POWER_ON()       TEMPERATURE_DETECT_NTC_POWER_PIN = 1
#define TEMPERATURE_DETECT_POWER_OFF()      TEMPERATURE_DETECT_NTC_POWER_PIN = 0

#ifdef  SUPPORT_XP01_WR_5260 
#define PAIR_KEY_PIN                      _pa0
#endif

//smoke control

#ifdef SUPPORT_RF_NET_FUNCTION
#define UART_INT_IRQ_PIN				_pa3
#define UART_IRQ_OUT_L()				{GpioResetBits(_pac,GPIOBIT3_SELECT);\
										GpioResetBits(_pa, GPIOBIT3_SELECT);}
#define UART_IRQ_INPUT()				{GpioSetBits(_pa, GPIOBIT3_SELECT);\
										GpioSetBits(_pac, GPIOBIT3_SELECT);}
#endif


#endif
/**
 * @brief Definition for COM port1
 */


/* Exported Macros ------------------------------------------------------------*/

/**
  * @}
  */

#endif /* __GPIO_CFG_H */




/************************END OF FILE***********************/

