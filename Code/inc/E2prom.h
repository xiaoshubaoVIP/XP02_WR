/****************************************************************************/
/*	File   :             E2prom.h               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/7/25,16:27:44                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/


#ifndef __EEPROM_H__
#define __EEPROM_H__

#define ADC2_ADDR_H  0x3FF5  
#define ADC2_ADDR_L  0x3FF6
#define T2_ADDR  	 0x3FF7
void E2promWrite(uint8 addr, uint8 data);
uint8 E2promRead(uint8 addr);
uint16 Read_Flash_Temp(uint16 addr);

#endif

