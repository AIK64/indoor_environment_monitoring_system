#ifndef __DHT11_H
#define __DHT11_H 
  
#include "stm32f10x.h"

#define DHT11_DQ_OUT_Low GPIO_ResetBits(GPIOA,GPIO_Pin_0)  //数据端口	PB11 
#define DHT11_DQ_OUT_High GPIO_SetBits(GPIOA,GPIO_Pin_0)  //数据端口	PB11
#define DHT11_DQ_IN GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)   //数据端口	PB11

void DHT11_GPIO_Config(void);

void DHT11_Mode_IPU(void);
void DHT11_Mode_Out(void);


u8 DHT11_Init(void);//初始化DHT11
u8 DHT11_Read_Data(uint8_t *temp,uint8_t *temp_decimal,uint8_t *humi,uint8_t *humi_decimal);

u8 DHT11_Read_Byte(void);//读出一个字节
u8 DHT11_Read_Bit(void);//读出一个位

u8 DHT11_Answer_Check(void);//检测是否存在DHT11

void DHT11_Rst(void);//复位DHT11


#endif

