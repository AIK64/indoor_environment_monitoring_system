#include "stm32f10x.h"                  // Device header
#include "stdio.h"
#include "Delay.h"
#include "usart.h"
#include  "LED.h"
#include "Key.h"
#include "LightSensor.h"
#include "OLED.h"
#include "dht11.h"
#include "esp8266.h"
#include "MyI2C.h"
#include "gy30.h"
#include "ZE08.h"


double temperature,humidity;


int main(void)
{
	
	uint8_t dht11_temp = 1;
	uint8_t dht11_temp_decimal = 1;
	uint8_t dht11_humi = 1;
	uint8_t dht11_humi_decimal = 1;
	//int Light_intensity=0;
	//int32_t CH2O=0;
	double CH2O=0;
	
	OLED_Init();
	OLED_ShowString(0,0,"OLEDinitsuccess",OLED_8X16);
	OLED_Update();
	DHT11_Init();
	OLED_ShowString(0,0,"DHTinitsuccess",OLED_8X16);
	OLED_Update();
	Usart2_Init(115200);
	Usart1_Init(9600);
	MyI2C_Init();
	
	
	
	
	
	ESP8266_Init();
	
	
//	OLED_ShowChar(1,1,'A');
//	OLED_ShowString(2,1,"hello world!");
	OLED_Clear();
	OLED_Update();
		
	while (1)
	{
		
		DHT11_Read_Data(&dht11_temp,&dht11_temp_decimal,&dht11_humi,&dht11_humi_decimal);
		temperature=dht11_temp+((float)(dht11_temp_decimal))/10;
		humidity=dht11_humi+((float)(dht11_humi_decimal))/10;
		//Light_intensity=GY30_GetData();
		CH2O=ZE08_GetData();
		
		ESP8266_SendDataByMQTTcmd("temperature",temperature);
		ESP8266_SendDataByMQTTcmd("humidity",humidity);
		//ESP8266_SendDataByMQTTcmd("LightIntensity",Light_intensity);
		ESP8266_SendDataByMQTTcmd("CH2O",CH2O);
		
		ESP8266_Clear();
		
		
		OLED_ShowChinese(0,0,"温度：");
		OLED_Printf(48,0,OLED_8X16,"%4.1lf",temperature);
		OLED_ShowChinese(84,0,"℃");
		OLED_ShowChinese(0,16,"湿度：");
		OLED_Printf(48,16,OLED_8X16,"%.0lf%%",humidity);
		//OLED_ShowChinese(0,32,"光强：");
		//OLED_Printf(48,32,OLED_8X16,"%d LX",Light_intensity);
		OLED_ShowChinese(0,34,"甲醛：");
		OLED_Printf(48,34,OLED_8X16,"%.3lf%mg/m3",CH2O);
		

		OLED_Update();
		
		Delay_ms(2000);
	}
	
}
