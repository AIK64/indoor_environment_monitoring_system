#include "stm32f10x.h"                  // Device header

#include "Delay.h"
#include "MyI2C.h"


uint16_t GY30_GetData(void)
{
   
    MyI2C_Start();
    MyI2C_SendByte(0x46);
    if(MyI2C_ReceiveAck()!=0) return 0;
    MyI2C_SendByte(0x01);
    if(MyI2C_ReceiveAck()!=0) return 0;
    MyI2C_Stop();
    
    MyI2C_Start();
    MyI2C_SendByte(0x46);
    if(MyI2C_ReceiveAck()!=0) return 0;
    MyI2C_SendByte(0x10);
    if(MyI2C_ReceiveAck()!=0) return 0;
    MyI2C_Stop();
    Delay_ms(200);
    
    uint16_t Light=0;
    MyI2C_Start();
    MyI2C_SendByte(0x47);
    if(MyI2C_ReceiveAck()!=0) return 0;
    Light|=MyI2C_ReceiveByte();
    Light<<=8;
    MyI2C_SendAck(0);
    Light|=MyI2C_ReceiveByte();
    MyI2C_SendAck(1);
    MyI2C_Stop();
    
    return Light;
}
