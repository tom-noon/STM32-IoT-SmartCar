/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	esp8266.c
	*
	*	日期： 		2017-05-08
	*
	*	版本： 		V1.0
	*
	*	说明： 		ESP8266的简单驱动
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "sys.h"

//网络设备驱动
#include "esp8266.h"
#include "onenet.h"

//硬件驱动
#include "delay.h"
#include "usart.h"
#include "cJSON.h"

//C库
#include <string.h>
#include <stdio.h>


#define WIFI_SSID 						"1433223"								//	WIFI的名称 必须用2.4G的wifi不能用5G的，且不能用中文、空格
#define WIFI_PSWD 						"66666666"				          //	WIFI密码

////云：47.120.77.39
////私网：172.25.84.10
#define SERVER_HOST						"47.120.77.39"			          //	服务器域名或IP
#define SERVER_PORT						"1883"								       //	MQTT服务器端口（一般为1883不用改）


#define ESP8266_WIFI_INFO			"AT+CWJAP=\"" WIFI_SSID "\",\"" WIFI_PSWD "\"\r\n"
#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"" SERVER_HOST "\"," SERVER_PORT "\r\n"


extern u16 relink_time;


extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;


#define esp8266_buf_size 512

extern unsigned char esp8266_buf[esp8266_buf_size];
extern unsigned short esp8266_cnt, esp8266_cntPre ;

extern u8 ESP8266_INIT_OK;

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
	
	HAL_UART_Transmit(&huart2, (unsigned char *)cmd, strlen((const char *)cmd),0xFF);
	
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
		
		delay_ms(10);
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

	char cmdBuf[32];
	
	ESP8266_Clear();								//清空接收缓存
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//发送命令
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//收到‘>’时可以发送数据
	{
		HAL_UART_Transmit(&huart2, data, len, 0xFF);		//发送设备连接请求数据
	}

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
			if(ptrIPD == NULL)											//如果没找到，可能是IPD头的延迟，还是需要等待一会，但不会超过设定的时间
			{
				//UsartPrintf("\"IPD\" not found\r\n");
			}
			else
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
		delay_ms(5);
		timeOut--;	//延时等待
	} while(timeOut > 0);
	
	return NULL;														//超时还未找到，返回空指针

}


unsigned char *ESP8266_GetIPD_nodelay(unsigned short timeOut)
{
	static unsigned short i=0;
	char *ptrIPD = NULL;
	
	if(i < timeOut)
	{
		if(ESP8266_WaitRecive() == REV_OK)								//如果接收完成
		{
			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//搜索“IPD”头
			if(ptrIPD == NULL)											//如果没找到，可能是IPD头的延迟，还是需要等待一会，但不会超过设定的时间
			{
				//UART_Printf(&huart1,"\"IPD\" not found\r\n");
			}
			else
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
		i+=1;
	} 
	else
	{
		i=0;
		return NULL;														//超时还未找到，返回空指针
	}
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
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOC_CLK_ENABLE();

	//ESP8266复位引脚     RX----PA2(huart2tx)        TX----PA3(huart2rx)     RST     PC7
	GPIO_InitStruct.Pin = GPIO_PIN_7;	 
 	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 		 //推挽输出
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;//速度50MHz
 	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);	  //初始化PC7
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	delay_ms(250);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	delay_ms(500);
	
	ESP8266_Clear();
	
	delay_ms(100);
	
	UART_Printf(&huart1,"0. AT - 测试MCU-8266通讯\r\n");
	while(ESP8266_SendCmd("AT\r\n", "OK"))
		delay_ms(500);
	
	UART_Printf(&huart1,"1. AT+RST - 软复位8266\r\n");
	ESP8266_SendCmd("AT+RST\r\n", "");
		delay_ms(500);
	ESP8266_SendCmd("AT+CIPCLOSE\r\n", "");
		delay_ms(500);
	UART_Printf(&huart1,"2. AT+CWMODE=1,1 - 设置8266工作模式为STA\r\n");
	while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))
		delay_ms(500);
	
	UART_Printf(&huart1,"3. AT+CWDHCP=1,1 - 使能STA模式下DHCP\r\n");
	while(ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK"))
		delay_ms(500);
	
	UART_Printf(&huart1,"4. AT+CWJAP - 连接WIFI\r\n");
	while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP"))
		delay_ms(500);
	
	UART_Printf(&huart1,"5. AT+CIPSTART - 连接服务器\r\n");
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
		delay_ms(500);
	//ESP8266_INIT_OK = 1;
	UART_Printf(&huart1,"6. ESP8266 Init OK - ESP8266初始化成功\r\n");
}


_Bool relink_status(void)
{
	static unsigned char status=0;
	unsigned char return_status=0;
	
	switch(status)
	{
		case 0:
			UART_Printf(&huart1,"正在尝试 - 连接WIFI\r\n");
			if(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP")== 0)
			{
				status=1;
			}
			break;
		case 1:
			UART_Printf(&huart1,"正在尝试 - 连接服务器\r\n");
			if(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT")==0)
			{
				status=2;
			}
			break;
		case 2:
			UART_Printf(&huart1,"6. ESP8266 Init OK - ESP8266初始化成功\r\n");
			if(OneNet_DevLink() == 0)
			{
				return_status=1;
				status=0;
			}
			break;
		default :
			break;
	}
	if(relink_time >= 30000)
	{
		relink_time=0;
		return_status = 0;
		status=0;
	}
	return return_status;
}





//HAL_UART_RxCpltCallback代替
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
//void USART2_IRQHandler(void)
//{

//	if( __HAL_UART_GET_FLAG(&huart2, UART_IT_RXNE) ) //接收中断
//	{
//		if(esp8266_cnt >= sizeof(esp8266_buf))	esp8266_cnt = 0; //防止串口被刷爆
//		esp8266_buf[esp8266_cnt++] = huart2.Instance->DR;
//		__HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_RXNE);
//	}

//}
