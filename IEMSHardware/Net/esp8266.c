/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	esp8266.c
	*	作者： 		zh
	*	日期： 		
	*	版本： 		V1.0
	*	说明： 		ESP8266的简单驱动
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络设备驱动
#include "esp8266.h"

//硬件驱动
#include "delay.h"
#include "usart.h"

#include "OLED.h"


//C库
#include <string.h>
#include <stdio.h>

#define ESP8266_WIFI_INFO		"AT+CWJAP=\"k40\",\"540540540\"\r\n"



#define ESP8266_ONENET_INFO		"AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,1\r\n"  	//新版OneNET地址

//#define ESP8266_USERCFG_INFO "AT+MQTTUSERCFG=0,1,\"test\",\"a32kUt5E2v\",\"version=2018-10-31&res=products%2Fa32kUt5E2v%2Fdevices%2Ftest&et=1871122235&method=md5&sign=468KIyWGaVD7JkIa4RE3UQ%3D%3D\",0,0,\"\"\r\n"
#define ESP8266_USERCFG_INFO "AT+MQTTUSERCFG=0,1,\"IEMSHardware\",\"a32kUt5E2v\",\"version=2018-10-31&res=products%2Fa32kUt5E2v%2Fdevices%2FIEMSHardware&et=1871122235&method=md5&sign=VmZJzEEg5%2BQLemcCxreJIg%3D%3D\",0,0,\"\"\r\n"

#define PROID		"a32kUt5E2v"  //产品ID

#define AUTH_INFO	"version=2018-10-31&res=products%2Fa32kUt5E2v%2Fdevices%2FIEMSHardware&et=1871122235&method=md5&sign=VmZJzEEg5%2BQLemcCxreJIg%3D%3D"  //鉴权信息

#define DEVID		"IEMSHardware"   //设备名称

#define MessageHeader   "AT+MQTTPUB=0,\"$sys/a32kUt5E2v/IEMSHardware/dp/post/json\",\"{\"id\":123,\"dp\":{\""
#define MessageEnd      "\":[{\"v\":11.11}]}}\",0,0\r\n"


unsigned char esp8266_buf[128];
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;


//==========================================================
//	函数名称：	ESP8266_Clear
//
//	函数功能：	清空缓存
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;

}

//==========================================================
//	函数名称：	ESP8266_WaitRecive
//
//	函数功能：	等待接收完成
//
//	入口参数：	无
//
//	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
//
//	说明：		循环调用检测是否接收完成
//==========================================================
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
	{
		esp8266_cnt = 0;							//清0接收计数
			
		return REV_OK;								//返回接收完成标志
	}
		
	esp8266_cntPre = esp8266_cnt;					//置为相同
	
	return REV_WAIT;								//返回接收未完成标志

}

//==========================================================
//	函数名称：	ESP8266_SendCmd
//
//	函数功能：	发送命令
//
//	入口参数：	cmd：命令
//				res：需要检查的返回指令
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 200;

	Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//如果收到数据
		{
			if(strstr((const char *)esp8266_buf, res) != NULL)		//如果检索到关键词
			{
				ESP8266_Clear();									//清空缓存
				
				return 0;
			}
		}
		Delay_ms(10);
	}
	
	return 1;

}

//==========================================================
//	函数名称：	ESP8266_SendData
//
//	函数功能：	发送数据
//
//	入口参数：	data：数据
//				len：长度
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[200];
	
	ESP8266_Clear();								//清空接收缓存
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//发送命令
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//收到‘>’时可以发送数据
	{
		Usart_SendString(USART2, data, len);		//发送设备连接请求数据
	}

}



//==========================================================
//	函数名称：	ESP8266_SendDataByMQTTcmd
//
//	函数功能：	使用MQTT指令向OneNet平台发送数据
//
//	入口参数：	dataname:表示数据名的字符串
//				dataval:数据的值
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_SendDataByMQTTcmd(char* DataName,double DataVal)
{
	char Header[]="AT+MQTTPUB=0,\"$sys/a32kUt5E2v/IEMSHardware/dp/post/json\",\"{\\\"id\\\":123\\,\\\"dp\\\":{\\\"";
	char End[]="\\\":[{\\\"v\\\":%.5f}]}}\",0,0\r\n";
	char messageformat[200]="";
	char message[200];
	strcat(messageformat,Header);
	strcat(messageformat,DataName);
	strcat(messageformat,End);
	
	sprintf(message,messageformat,DataVal);
	
	ESP8266_SendCmd(message, "OK");
	
	
	//OLED_ShowString(3,1,"messageSended");
}

//==========================================================
//	函数名称：	ESP8266_GetIPD
//
//	函数功能：	获取平台返回的数据
//
//	入口参数：	等待的时间(乘以10ms)
//
//	返回参数：	平台返回的原始数据
//
//	说明：		不同网络设备返回的格式不同，需要去调试
//				如ESP8266的返回格式为	"+IPD,x:yyy"	x代表数据长度，yyy是数据内容
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	
	do
	{
		if(ESP8266_WaitRecive() == REV_OK)								//如果接收完成
		{
			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//搜索“IPD”头
			if(ptrIPD != NULL)										
			{
				ptrIPD = strchr(ptrIPD, ':');							//找到':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
				
			}
		}
		Delay_ms(5);													//延时等待
	} while(timeOut--);
	
	return NULL;														//超时还未找到，返回空指针

}

//==========================================================
//	函数名称：	ESP8266_Init
//
//	函数功能：	初始化ESP8266
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_Init(void)
{
	
	GPIO_InitTypeDef GPIO_Initure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//ESP8266复位引脚
	GPIO_Initure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initure.GPIO_Pin = GPIO_Pin_1;					//GPIOA1-复位
	GPIO_Initure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_Initure);
	
	
	GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET);
	Delay_ms(250);
	GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET);
	Delay_ms(500);
	
	ESP8266_Clear();

	OLED_ShowString(0,0,"start8266Init",OLED_8X16);
	OLED_Update();
	
	while(ESP8266_SendCmd("AT\r\n", "OK"))
		Delay_ms(500);
	
	while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))
		Delay_ms(500);
	
	OLED_ShowString(0,0,"Getting wifi..",OLED_8X16);
	OLED_Update();
	while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP"))
		Delay_ms(500);
	
	OLED_ShowString(0,0,"wifi_connected",OLED_8X16);
	OLED_Update();
	
	
	while(ESP8266_SendCmd(ESP8266_USERCFG_INFO, "OK"))
		Delay_ms(500);
		
	OLED_ShowString(0,0,"usercfg_success",OLED_8X16);
	OLED_Update();
	
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "OK"))
		Delay_ms(500);
	
	OLED_ShowString(0,0,"8266init_succes",OLED_8X16);
	OLED_Update();

}

//==========================================================
//	函数名称：	USART2_IRQHandler
//
//	函数功能：	串口2收发中断
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void USART2_IRQHandler(void)
{

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收中断
	{
		if(esp8266_cnt >= sizeof(esp8266_buf))	esp8266_cnt = 0; //防止串口被刷爆
		esp8266_buf[esp8266_cnt++] = USART2->DR;
		
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}

}
