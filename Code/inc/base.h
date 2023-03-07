/**
  ******************************************************************************
  * @file    base.h
  * @author  zhenrong.peng
  * @version V1.0
  * @date    18-05-2017
  * @brief   This file contains definitions for base include 
  *          
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BASE_H
#define __BASE_H

/* Includes ------------------------------------------------------------------*/
#include "BA45F5260.h"
#include "sys_cfg.h"
#include "gpio_cfg.h"
/* Exported types ------------------------------------------------------------*/
/*!< Signed integer types  */
typedef   signed char     int8;
typedef   signed short    int16;
typedef   signed long     int32;

/*!< Unsigned integer types  */
typedef unsigned char     uint8;
typedef unsigned short    uint16;
typedef unsigned long     uint32;


typedef     uint8  uint8_t;
typedef     uint16 uint16_t;
typedef     uint32 uint32_t;

#define NULL ((void *)0)

#define U8_MAX     (255)
#define S8_MAX     (127)
#define S8_MIN     (-128)
#define U16_MAX    (65535u)
#define S16_MAX    (32767)
#define S16_MIN    (-32768)
#define U32_MAX    (4294967295uL)
#define S32_MAX    (2147483647)
#define S32_MIN    (-2147483648uL)


typedef enum {FALSE = 0, TRUE = !FALSE} bool;

typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus, BitStatus, BitAction;

typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;

typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;

#define MAX_OF_TYPE(Type) \
	((Type)-1>0? (Type)-1: (1<<(sizeof(Type)*8-2))-1+(1<<(sizeof(Type)*8-2)))

#define MIN_OF_TYPE(Type) ((Type)-1>0? 0: -MAX_OF_TYPE(Type)-1)

#define MAX_OF_UNSIGNED_VAR(UnsignedVar) BITS_X2Y(0, sizeof(UnsignedVar)*8-1)

#define ARRAY_MBS(Array) (sizeof(Array)/sizeof(Array[0]))

//typedef enum {FALSE = 0, TRUE = !FALSE} bool;

typedef enum _eStatusFlag
{
    eReset = 0, 
    eSet = !eReset

}eStatusFlag;



typedef enum _eEventFlag
{
	eEventFlagNone=(uint16_t)0x00,
	eEventFlagLowBat=(uint16_t)0x01,
	eEventFlagLowBatMute=(uint16_t)0x02,
	eEventFlagLowBatForbitMute=(uint16_t)0x04,
	eEventFlagFault=(uint16_t)0x08,
	eEventFlagEndOfLife=(uint16_t)0x10,
	eEventFlagEndOfLifeMute=(uint16_t)0x20,
	eEventFlagEndOfLifeForbitMute=(uint16_t)0x40,
	eEventFlagLatchingSmokeAlarm=(uint16_t)0x80,
	eEventFlagLatchingCoAlarm = (uint16_t)0x0100,

	
	
}eEventFlag;


typedef enum _eErrorCode
{
    eErrCodeNone=0x00,
    eErrCodePtt=0x01,                         // push to test fault

}eErrorCode;
	
typedef enum
{
	ERROR_CODE_NONE=0x00,
	ERROR_CODE_PTT=0x01,							// push to test fault
	ERROR_CODE_SHORT=0x02,
	ERROR_CODE_OPEN=0x04,
	ERROR_CODE_SMOKE_TRANS_OPEN_SHORT=0x08, 		//smoke trans ir open/short
	ERROR_CODE_SMOKE_RECV_OPEN_SHORT=0x10,			//smoke recv ir open/short
	ERROR_CODE_MEMORY=0x20, 						//memory fault
	
}Error_Code;


typedef enum _eTbnTime
{
    eTbnTime8ms=0,                 // 000: 2^8/fPSC=8ms
    eTbnTime16ms,                  // 001: 2^9/fPSC=16ms
    eTbnTime32ms,                  // 010: 2^10/fPSC=32ms
    eTbnTime64ms,                  // 011: 2^11/fPSC=64ms
    eTbnTime128ms,                 // 100: 2^12/fPSC=128ms
    eTbnTime256ms,                 // 101: 2^13/fPSC=256ms
    eTbnTime512ms,                 // 110: 2^14/fPSC=512ms
    eTbnTime1024ms,                // 111: 2^15/fPSC=1024ms

}eTbnTime;



typedef enum _eWakeupTime
{
    eWakeupTime8ms=0,          // 000: 2^8/fLIRC=8ms
    eWakeupTime32ms,           // 001: 2^10/fLIRC=32ms
    eWakeupTime128ms,          // 010: 2^12/fLIRC=128ms
    eWakeupTime512ms,          // 011: 2^14/fLIRC=512ms
    eWakeupTime1024ms,         // 100: 2^15/fLIRC=1024ms
    eWakeupTime2048ms,         // 101: 2^16/fLIRC=2048ms
    eWakeupTime4096ms,         // 110: 2^17/fLIRC=4096ms
    eWakeupTime8192ms,         // 111: 2^18/fLIRC=8192ms

}eWakeupTime;

typedef enum _eAdcChannel
{
    eAdcTempChannel = 0x00, 
    eAdcBatChannel  =0x01,
    eAdcCoChannel  =0x02,
}eAdcChannel;


#endif /* __BASE_H */




/************************END OF FILE***********************/

