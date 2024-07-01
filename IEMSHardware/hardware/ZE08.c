#include"ZE08.h"


u32 ch2o;//甲醛浓度 mg/m3*10000


uint8_t UART1_dat_ch2o[20];
uint8_t ch2o_cnt = 0;



double ZE08_GetData(void)
{
	double res=0;
	res=ch2o;
	res=res/1000;
	res=res*1.25;
	//return ch2o;
	return res;
}

//==========================================================
//	函数名称：	USART1_IRQHandler
//
//	函数功能：	串口1收发中断
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void USART1_IRQHandler(void)
{

	u8 res;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) /* 接收到数据 */
    {
        res = USART_ReceiveData(USART1);
        //回发
        //USART_SendData(USART3, res);                                          /* 发送数据到串口3 */
				UART1_dat_ch2o[ch2o_cnt]=res;
        switch (ch2o_cnt) {
            case 0:
                if (UART1_dat_ch2o[ch2o_cnt] == 0xFF) ch2o_cnt = 1;
                else ch2o_cnt = 0;
                break;

            case 1:
                if (UART1_dat_ch2o[ch2o_cnt] == 0x17) ch2o_cnt = 2;
                else ch2o_cnt = 0;
                break;

            case 2:
                if (UART1_dat_ch2o[ch2o_cnt] == 0x04) ch2o_cnt = 3;
                else ch2o_cnt = 0;
                break;

            case 3:
                if (UART1_dat_ch2o[ch2o_cnt] == 0x00) ch2o_cnt = 4;
                else ch2o_cnt = 0;
                break;

            case 4:
                ch2o_cnt = 5;
                break;

            case 5:
                ch2o_cnt = 6;
                break;

            case 6:
                if (UART1_dat_ch2o[ch2o_cnt] == 0x13) ch2o_cnt = 7;
                else ch2o_cnt = 0;
                break;

            case 7:
                if (UART1_dat_ch2o[ch2o_cnt] == 0x88) ch2o_cnt = 8;
                else ch2o_cnt = 0;
                break;

            case 8:  //校验接收的数据  正确则处理接收的数据
                if ((UART1_dat_ch2o[ch2o_cnt] + UART1_dat_ch2o[1] + UART1_dat_ch2o[2] + UART1_dat_ch2o[3] +
                     UART1_dat_ch2o[4] + UART1_dat_ch2o[5] + UART1_dat_ch2o[6] + UART1_dat_ch2o[7]) % 256 == 0) {
                    ch2o = UART1_dat_ch2o[4] * 256 + UART1_dat_ch2o[5];	//ppb浓度值
				   //ch2o=ch2o/10;
				   //res=res/1000;		//转换为ppm浓度
					//res=res*1.25;
						 
                    ch2o_cnt = 0;
                } else {
                    ch2o_cnt = 0;
                }
                break;
        }

    }


}
