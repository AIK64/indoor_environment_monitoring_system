#include "dht11.h"	
#include "Delay.h"


/*
 * 函数名：DHT11_GPIO_Config
 * 描述  ：配置DHT11用到的I/O口
 * 输入  ：无
 * 输出  ：无
 */
 void DHT11_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	//GPIO
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );	 //使能PORTA口时钟  
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;  //SPI CS
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* Deselect the PA0 Select high */
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
}
/*
 * 函数名：DHT11_Mode_IPU
 * 描述  ：使DHT11-DATA引脚变为输入模式
 * 输入  ：无
 * 输出  ：无
 */
void DHT11_Mode_IPU(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	//GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;  //SPI CS
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);

}
/*
 * 函数名：DHT11_Mode_Out
 * 描述  ：使DHT11-DATA引脚变为输出模式
 * 输入  ：无
 * 输出  ：无
 */
void DHT11_Mode_Out(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	//GPIO
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;  //SPI CS
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

}



/*
 *主机给从机发送复位脉冲
 */
void DHT11_Rst(void)	   
{        
    /* IO设置为推挽输出*/	
	  DHT11_Mode_Out(); 
	  /*产生至少20ms的低电平复位信号 */
    DHT11_DQ_OUT_Low;  
		Delay_ms(20);
    /* 在产生复位信号后，需将总线拉高 */	
    DHT11_DQ_OUT_High; 
		Delay_us(30);
	  DHT11_Mode_IPU(); 
	
}

/*
 * 检测从机给主机返回的应答脉冲
 *从机接收到主机的复位信号后，会在15~60us后给主机发一个应答脉冲
 * 0：成功
 * 1：失败
 */
u8 DHT11_Answer_Check(void) 	   
{     
	  uint8_t retry = 0;
	  DHT11_Mode_IPU();
	  while(DHT11_DQ_IN && retry < 100)
		{
			retry ++;
			Delay_us(1);
		}
		if(retry >= 100)
		{
			return 1;
		}
		else
		{
			retry = 0;
		}
		while(!DHT11_DQ_IN && retry <100)
		{
			retry ++;
			Delay_us(1);
		}
		if(retry >= 100)
		{
			return 1;
		}
		return 0;
}

//从DHT11读取一个位
//返回值：1/0
u8 DHT11_Read_Bit(void) 			 // read one bit
{
	
	uint8_t retry;
	
	while(DHT11_DQ_IN && retry < 100)
	{
		retry ++;
		Delay_us(1);
	}
	retry = 0;
	while(!DHT11_DQ_IN && retry < 100)
	{
		retry ++;
		Delay_us(1);
	}
	
	Delay_us(40);
	
	if(DHT11_DQ_IN)
	{
		return 1;
	}
	else
  {
		return 0;
	}
	
}



//从DHT11读取一个字节
//返回值：读到的数据
u8 DHT11_Read_Byte(void)    // read one byte
{        
    u16 i,dat=0;
		for(i=0; i<8; i++) 
		{
			dat <<= 1;
			dat |= DHT11_Read_Bit();
		}					    
    return dat;
}

//初始化DHT11的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在    	 
u8 DHT11_Init(void)
{
  DHT11_GPIO_Config();
	DHT11_Rst();
	return DHT11_Answer_Check();
}  
//从DHT11得到温度值
//精度：+/-2C
//返回值：温度值 （0~50） 
u8 DHT11_Read_Data(uint8_t *temp,uint8_t *temp_decimal,uint8_t *humi,uint8_t *humi_decimal)
{

	uint8_t buf[5],i;
	
	DHT11_Rst();
	
	if(DHT11_Answer_Check() == 0 )
  {
		//检测到DHT11响应

		for(i = 0; i < 5 ; i++)
		{
				buf[i] = DHT11_Read_Byte();				
		}
		if((buf[0]+buf[1]+buf[2]+buf[3]) == buf[4])	//进行校验
		{
			*humi = buf[0];
			*humi_decimal=buf[1];
			*temp = buf[2];
			*temp_decimal=buf[3];
			
		}
	}
  else
	{
		return 1;
	}

	return 0;
} 

