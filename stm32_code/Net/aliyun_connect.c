#include "aliyun_connect.h"


#include "dht11.h"
#include "led.h"
#include "motor.h"
#include "hc_sro4.h"

//此处根据自己的wifi作调整
#define WIFI_NAME "1433223"
#define WIFI_PASSWD "66666666"

//此处是阿里云服务器的登陆配置

#define MQTT_BROKERADDRESS "47.120.77.39"//阿里云服务器ip（公）
#define MQTT_CLIENTID "STM32CAR"
#define MQTT_USARNAME "STM32CAR"
#define MQTT_PASSWD "STM32CAR"

#define MQTT_SUBSCRIBE_TOPIC "testtopic/cmd"//STM32需要订阅的主题
#define	MQTT_PUBLISH_TOPIC "testtopic/data"//上报的数据



char mqtt_message[512];	//MQTT的上报消息缓存

extern uint8_t mode;
extern uint8_t job_id,dir,runspeed;
extern uint16_t runtime;//ms级
extern float hc_distance;

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
	hc_distance=Hcsro4Read();//获取距离
	// 获取温湿度信息
	DHT11_Read_Data1(&temperature,&humidity);//获取温度和湿度
	
	//外置LED
	if(led0) LED0_on;else LED0_off;
	
	
	//上报一次数据
	sprintf(mqtt_message,
	"{\"Pub_Topic\":\"testtopic/data\",\"id\":2024,\
		\"mode\":%d,\
		\"temp1\":%d,\
		\"humi1\":%d,\
		\"spd\":%d,\
		\"dist\":%.1f\
	 }",
	mode,
	temperature,
	humidity,
	runspeed,
	hc_distance
	);

	MQTT_PublishData(MQTT_PUBLISH_TOPIC,mqtt_message,0);
	
	UART_Printf(&huart1,"testtopic/data,{\"id\":%d,\"temp1\":%d,\"humi1\":%d,\"spd\":%d}",hw_id,temperature,humidity,runspeed);

	UART_Printf(&huart1,"\nTips:	MQTT Publish Send OK\r\n");
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



void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[128];
	
	//每次发送前将接收串口接收总数置0,为了接收
	ESP8266_Clear();
	
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//发送命令
	ESP8266_ATSendString(cmdBuf);
	if(strstr((const char *)cmdBuf, ">") != NULL)				//收到‘>’时可以发送数据
	{
		ESP8266_Clear();
		HAL_UART_Transmit(&huart2,data,len,0xFF);//发送
	}

}

//处理MQTT下发的消息
void deal_MQTT_message(uint8_t* buf,uint16_t len)
{
	UART_Printf(&huart1,"\nUSART2_DATA comes:\n");
	
	//copy一份缓存
	uint8_t data[512];
	uint16_t data_len = len;
	for(int i=0;i<data_len;i++) data[i] = buf[i];//将buf里的数据复制到接下来要用的data
	
	ESP8266_Clear(); //清空接收缓冲
	
	
	//解析开始
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//协议包
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short topic_len = 0;
	unsigned short req_len = 0;
	
	unsigned char type = 0;
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	short result = 0;

	cJSON *json , *json_value;
	
	type = MQTT_UnPacketRecv(data);
	switch(type)
	{
		case MQTT_PKT_CMD:															//命令下发
			
			result = MQTT_UnPacketCmd(data, &cmdid_topic, &req_payload, &req_len);	//解出topic和消息体
			if(result == 0)
			{
				UART_Printf(&huart1,"cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);

				MQTT_DeleteBuffer(&mqttPacket);									//删包
			}
		break;
			
		case MQTT_PKT_PUBLISH:														//接收的Publish消息
		
			result = MQTT_UnPacketPublish(data, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
			if(result == 0)
			{
				UART_Printf(&huart1,"topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
																	cmdid_topic, topic_len, req_payload, req_len);
				
				// 对数据包req_payload进行JSON格式解析
				/*
				{ "id": 2024,"led0": 1}
				*/
				json = cJSON_Parse(req_payload);
				if (!json)UART_Printf(&huart1,"Error before: [%s]\n",cJSON_GetErrorPtr());
				else
				{
					json_value = cJSON_GetObjectItem(json,"id");//搜索id关键字
					
					if(strstr(json_value->string,"id") != NULL)//搜索到了id关键字
					{
						if(json_value->valueint==hw_id)
						{
							//led0
							json_value = cJSON_GetObjectItem(json,"led0");//搜索led0关键字
							if(strstr(json_value->string,"led0") != NULL)//搜索到led0关键字
							{
								if(json_value->valueint) led0=1;//开灯
								else led0=0;//关灯
							}
							
							//setMode
							json_value = cJSON_GetObjectItem(json,"mode");//搜索mode关键字
							if(strstr(json_value->string,"mode") != NULL)//搜索到mode关键字
							{
								mode=json_value->valueint;
							}
							
//							//doPathFinding job_id
//							json_value = cJSON_GetObjectItem(json,"job_id");
//							if(strstr(json_value->string,"job_id") != NULL)
//							{
//								
//							}
							
							//direction
							json_value = cJSON_GetObjectItem(json,"dir");//搜索dir关键字
							if(strstr(json_value->string,"dir") != NULL)//搜索到dir关键字
							{//1 2 3 4 front back left right
								dir=json_value->valueint;
							}
							
							//speed
							json_value = cJSON_GetObjectItem(json,"speed");//搜索speed关键字
							if(strstr(json_value->string,"speed") != NULL)//搜索到speed关键字
							{//直行或倒车才能变速 左转或右转车速和时间固定
								runspeed=json_value->valueint;
//								if( (dir==1) || (dir==2) ) runspeed=json_value->valueint;
//							  else if( (dir==3) || (dir==4) ) runspeed=2;
							}
							
							//runtime
							json_value = cJSON_GetObjectItem(json,"runtime");//搜索runtime关键字
							if(strstr(json_value->string,"runtime") != NULL)//搜索到runtime关键字
							{//直行或倒车才能变速 左转或右转车速和时间固定
								runtime=json_value->valueint;
//								if( (dir==1) || (dir==2) ) runtime=json_value->valueint;
//								else if( (dir==3) || (dir==4) ) runtime=1000;
							}
							
						}
					}
					
					
				}
				cJSON_Delete(json);
				
//				OneNet_PublishRec();//qos=2
//				OneNet_Puback();//qos=1
			}
		break;
			
		case MQTT_PKT_PUBACK:														//发送Publish消息，平台回复的Ack
		
			if(MQTT_UnPacketPublishAck(data) == 0)
				UART_Printf(&huart1,"Tips:	MQTT Publish Send OK\r\n");
			else
				UART_Printf(&huart1,"Tips:	MQTT Publish Send FAILED\r\n");
			
		break;
			
		case MQTT_PKT_PUBREC:														//发送Publish消息，平台回复的Rec，设备需回复Rel消息
		
			if(MQTT_UnPacketPublishRec(data) == 0)
			{
				UART_Printf(&huart1,"Tips:	Rev PublishRec\r\n");
				if(MQTT_PacketPublishRel(MQTT_PUBLISH_ID, &mqttPacket) == 0)
				{
					UART_Printf(&huart1,"Tips:	Send PublishRel\r\n");
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}
			}
		
		break;
			
		case MQTT_PKT_PUBREL:														//收到Publish消息，设备回复Rec后，平台回复的Rel，设备需再回复Comp
			
			if(MQTT_UnPacketPublishRel(data, pkt_id) == 0)
			{
				UART_Printf(&huart1,"Tips:	Rev PublishRel\r\n");
				if(MQTT_PacketPublishComp(MQTT_PUBLISH_ID, &mqttPacket) == 0)
				{
					UART_Printf(&huart1,"Tips:	Send PublishComp\r\n");
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}
			}
		
		break;
		
		case MQTT_PKT_PUBCOMP:														//发送Publish消息，平台返回Rec，设备回复Rel，平台再返回的Comp
		
			if(MQTT_UnPacketPublishComp(data) == 0)
			{
				UART_Printf(&huart1,"Tips:	Rev PublishComp\r\n");
			}
		
		break;
			
		case MQTT_PKT_SUBACK:														//发送Subscribe消息的Ack
		
			if(MQTT_UnPacketSubscribe(data) == 0)
				UART_Printf(&huart1,"Tips:	MQTT Subscribe OK\r\n");
			else
				UART_Printf(&huart1,"Tips:	MQTT Subscribe Err\r\n");
		
		break;
			
		case MQTT_PKT_UNSUBACK:														//发送UnSubscribe消息的Ack
		
			if(MQTT_UnPacketUnSubscribe(data) == 0)
				UART_Printf(&huart1,"Tips:	MQTT Subscribe OK\r\n");
			else
				UART_Printf(&huart1,"Tips:	MQTT Subscribe Err\r\n");
		
		break;
		
		default:
			result = -1;
		break;
	}
	
	
	if(result == -1)
		return;
	

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}
	

}


