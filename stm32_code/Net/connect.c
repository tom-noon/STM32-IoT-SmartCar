#include "connect.h"
#include "esp8266_mqtt.h"
#include "esp8266_at.h"

#include "dht11.h"
#include "led.h"

//此处根据自己的wifi作调整
#define WIFI_NAME "1433223"
#define WIFI_PASSWD "66666666"

//此处是阿里云服务器的登陆配置
//#define MQTT_BROKERADDRESS "k18slscl1aW.iot-as-mqtt.cn-shanghai.aliyuncs.com"
//#define MQTT_CLIENTID "00001|securemode=2,signmethod=hmacsha1,timestamp=1715226178553|"//
//#define MQTT_USARNAME "STM32CAR&k18slscl1aW"
//#define MQTT_PASSWD "F8A4CA9F13191D157D2F46C50EB3FCA8F7CDAC42"

//#define	MQTT_PUBLISH_TOPIC "/sys/k18slscl1aW/STM32CAR/thing/event/property/post"
//#define MQTT_SUBSCRIBE_TOPIC "/sys/k18slscl1aW/STM32CAR/thing/service/property/set"

#define MQTT_BROKERADDRESS "47.120.77.39"//阿里云服务器ip（公）
#define MQTT_CLIENTID "STM32CAR"
#define MQTT_USARNAME "STM32CAR"
#define MQTT_PASSWD "STM32CAR"

#define MQTT_SUBSCRIBE_TOPIC "testtopic/setMode"//STM32需要订阅的主题
#define	MQTT_PUBLISH_TOPIC "testtopic/data"//上报的数据





char mqtt_message[512];	//MQTT的上报消息缓存


extern uint16_t temperature,temp_parameter;
extern uint16_t humidity,humi_parameter;

extern uint8_t led0;

/******************************  进入错误模式代码  *****************************/

//进入错误模式等待手动重启
void Enter_ErrorMode(uint8_t mode)
{
	while(1)
	{
		switch(mode){
			case 0:UART_Printf(&huart1,"ESP8266初始化失败！\r\n");break;
			case 1:UART_Printf(&huart1,"ESP8266连接热点失败！\r\n");break;
			case 2:UART_Printf(&huart1,"ESP8266连接阿里云服务器失败！\r\n");break;
			case 3:UART_Printf(&huart1,"ESP8266阿里云MQTT登陆失败！\r\n");break;
			case 4:UART_Printf(&huart1,"ESP8266阿里云MQTT订阅主题失败！\r\n");break;
			default:UART_Printf(&huart1,"Nothing\r\n");break;
		}
		UART_Printf(&huart1,"请重启开发板");

		HAL_Delay(200);
	}
}

/******************************  STM32 MQTT业务代码  *****************************/

//MQTT初始化函数
void ES8266_MQTT_Init(void)
{
	uint8_t status=0;

	//初始化
	if(ESP8266_Init())
	{
		UART_Printf(&huart1,"1.ESP8266初始化成功！\r\n");
		status++;
	}
	else Enter_ErrorMode(0);

	//连接热点
	if(status==1)
	{
		if(ESP8266_ConnectAP(WIFI_NAME,WIFI_PASSWD))
		{
			UART_Printf(&huart1,"2.ESP8266连接热点成功！\r\n");
			status++;
		}
		else Enter_ErrorMode(1);
	}
	
	//连接阿里云IOT服务器
	if(status==2)
	{
		if(ESP8266_ConnectServer("TCP",MQTT_BROKERADDRESS,1883)!=0)
		{
			UART_Printf(&huart1,"3.ESP8266连接阿里云服务器成功！\r\n");
			status++;
		}
		else Enter_ErrorMode(2);
	}
	
	//登陆MQTT
	if(status==3)
	{
		if(MQTT_Connect(MQTT_CLIENTID, MQTT_USARNAME, MQTT_PASSWD) != 0)
		{
			UART_Printf(&huart1,"4.ESP8266阿里云MQTT登陆成功！\r\n");
			status++;
		}
		else Enter_ErrorMode(3);
	}

	//订阅主题
	if(status==4)
	{
		if(MQTT_SubscribeTopic(MQTT_SUBSCRIBE_TOPIC,0,1) != 0)
		{
			UART_Printf(&huart1,"5.ESP8266阿里云MQTT订阅主题成功！\r\n");
		}
		else Enter_ErrorMode(4);
	}
}

//STM32数据上报
void STM32_Report(void)
{	
	// 获取温湿度信息
	DHT11_Read_Data1(&temperature,&humidity);//获取温度和湿度
	
	//外置LED
	if(led0) LED0_on;else LED0_off;
	
	
	//上报一次数据
	sprintf(mqtt_message,
	"{\"method\":\"testtopic/data\",\"id\":\"STM32\",\"params\":{\
		\"temp\":%d,\
		\"humi\":%d,\
		\"led0\":%d\
	},\"mode\":{\
	}}",
	temperature,
	humidity,
	led0
	);

	MQTT_PublishData(MQTT_PUBLISH_TOPIC,mqtt_message,0);
}

char temp_str[30];    // 临时子串
void ReadStrUnit(char * str,char *temp_str,int idx,int len)  // 从母串中获取与子串长度相等的临时子串
{
    int index;
    for(index = 0; index < len; index++)
    {
        temp_str[index] = str[idx+index];
    }
    temp_str[index] = '\0';
}
int GetSubStrPos(char *str1,char *str2)
{
    int idx = 0;
    int len1 = strlen(str1);
    int len2 = strlen(str2);

    if( len1 < len2)
    {
        //printf("error 1 \n"); // 子串比母串长
        return -1;
    }

    while(1)
    {
        ReadStrUnit(str1,temp_str,idx,len2);    // 不断获取的从 母串的 idx 位置处更新临时子串
        if(strcmp(str2,temp_str)==0)break;      // 若临时子串和子串一致，结束循环
        idx++;                                  // 改变从母串中取临时子串的位置
        if(idx>=len1)return -1;                 // 若 idx 已经超出母串长度，说明母串不包含该子串
    }

    return idx;    // 返回子串第一个字符在母串中的位置
}

//处理MQTT下发的消息
void deal_MQTT_message(uint8_t* buf,uint16_t len)
{
	uint8_t data[512];
	uint16_t data_len = len;
	for(int i=0;i<data_len;i++)
	{
		data[i] = buf[i];
		HAL_UART_Transmit(&huart1,&data[i],1,100);
	}
	memset(usart2_rxbuf,0,sizeof(usart2_rxbuf)); //清空接收缓冲  
	usart2_rxcounter=0;
	
	//查找是否是设置红灯
	int i = GetSubStrPos((char*)data,"led0");
	if( i>0 )
	{
		
	}
	
	//查找是否是设置绿灯
	i = GetSubStrPos((char*)data,"LEDG");
	if( i>0 )
	{
		
	}
	
	//查找是否是设置蓝灯
	i = GetSubStrPos((char*)data,"LEDB");
	if( i>0 )
	{
		
	}

}


