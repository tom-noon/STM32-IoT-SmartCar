#include "sys.h"
#include "usart.h"

//C库
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "esp8266.h"
#include "onenet.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  


extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;


#define FLASH_SAVE_ADDR2  0X08069000
#define FLASH_SAVE_ADDR3  0X08068000
#define FLASH_SAVE_ADDR4  0X08067000

extern char PUB_BUF[256];
extern const char *pub_topics10;

uint8_t dt_temp;




u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记

u8 USART1_RX_BUF[USART1_REC_LEN];
u16 USART1_RX_STA=0;




//Usart_SendString->HAL_UART_Transmit  --常量

//HAL_UART_Transmit(&huart1,pStr,strlen((const char*)pStr),0xFF);
//HAL_UART_Receive_IT(&huart1,&rx_value,1);

//Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
//USART_ReceiveData(USART1);



//Usart_SendString->HAL_UART_Transmit->UART_Printf  --含变量
//void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)


// 重定向printf函数
int fputc(int ch,FILE *f)
{
    uint8_t temp[1]={ch};
    HAL_UART_Transmit(&huart1,temp,1,20);
	return 0;
}



void UART_Printf(UART_HandleTypeDef *huart, char *fmt,...)
{
	
	unsigned char UsartPrintfBuf[296];
	va_list ap;
	unsigned char *pStr = UsartPrintfBuf;
	
	va_start(ap, fmt);
	vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//格式化
	va_end(ap);
	
	HAL_UART_Transmit(huart,pStr,strlen((const char*)pStr),0xFF);
	
}
