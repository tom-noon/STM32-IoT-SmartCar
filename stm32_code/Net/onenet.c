/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	onenet.c
	*
	*	版本： 		V1.1
	*
	*	说明： 		与onenet平台的数据交互接口层
	*
	*	修改记录：	V1.0：协议封装、返回判断都在同一个文件，并且不同协议接口不同。
	*				V1.1：提供统一接口供应用层使用，根据不同协议文件来封装协议相关的内容。
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "sys.h"

//网络设备
#include "esp8266.h"

//协议文件
#include "onenet.h"
#include "mqttkit.h"

//硬件驱动
#include "usart.h"
#include "delay.h"
//#include "stmflash.h"

//C库
#include <string.h>
#include <stdio.h>

//算法
#include "base64.h"
#include "hmac_sha1.h"

#include "led.h"
#include "cJSON.h"

//#define PROID		"HrWSi9G59c"  //"产品号"

//#define AUTH_INFO	"version=2018-10-31&res=products%2FHrWSi9G59c%2Fdevices%2FSTM32&et=1746716140&method=sha1&sign=Lz849Kejp0cCUI%2B4mhtEVz1JZok%3D"  //"鉴权信息"

//#define DEVID		"STM32"   //"设备ID"


#define PROID		"00001|securemode=2,signmethod=hmacsha1,timestamp=1715226178553|"  //"产品号"

#define AUTH_INFO	"F8A4CA9F13191D157D2F46C50EB3FCA8F7CDAC42"  //"鉴权信息"

#define DEVID		"STM32CAR&k18slscl1aW"   //"设备ID"



#define FLASH_SAVE_ADDR2  0X08069000
#define FLASH_SAVE_ADDR3  0X08068000
#define FLASH_SAVE_ADDR4  0X08067000


extern char PUB_BUF[256];
extern const char *pub_topics10;
extern const char *pub_topics11;


extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;


/*
************************************************************
*	函数名称：	OTA_UrlEncode
*
*	函数功能：	sign需要进行URL编码
*
*	入口参数：	sign：加密结果
*
*	返回参数：	0-成功	其他-失败
*
*	说明：		+			%2B
*				空格		%20
*				/			%2F
*				?			%3F
*				%			%25
*				#			%23
*				&			%26
*				=			%3D
************************************************************
*/
static unsigned char OTA_UrlEncode(char *sign)
{

	char sign_t[40];
	unsigned char i = 0, j = 0;
	unsigned char sign_len = strlen(sign);
	
	if(sign == (void *)0 || sign_len < 28)
		return 1;
	
	for(; i < sign_len; i++)
	{
		sign_t[i] = sign[i];
		sign[i] = 0;
	}
	sign_t[i] = 0;
	
	for(i = 0, j = 0; i < sign_len; i++)
	{
		switch(sign_t[i])
		{
			case '+':
				strcat(sign + j, "%2B");j += 3;
			break;
			
			case ' ':
				strcat(sign + j, "%20");j += 3;
			break;
			
			case '/':
				strcat(sign + j, "%2F");j += 3;
			break;
			
			case '?':
				strcat(sign + j, "%3F");j += 3;
			break;
			
			case '%':
				strcat(sign + j, "%25");j += 3;
			break;
			
			case '#':
				strcat(sign + j, "%23");j += 3;
			break;
			
			case '&':
				strcat(sign + j, "%26");j += 3;
			break;
			
			case '=':
				strcat(sign + j, "%3D");j += 3;
			break;
			
			default:
				sign[j] = sign_t[i];j++;
			break;
		}
	}
	
	sign[j] = 0;
	
	return 0;

}

/*
************************************************************
*	函数名称：	OTA_Authorization
*
*	函数功能：	计算Authorization
*
*	入口参数：	ver：参数组版本号，日期格式，目前仅支持格式"2018-10-31"
*				res：产品id
*				et：过期时间，UTC秒值
*				access_key：访问密钥
*				dev_name：设备名
*				authorization_buf：缓存token的指针
*				authorization_buf_len：缓存区长度(字节)
*
*	返回参数：	0-成功	其他-失败
*
*	说明：		当前仅支持sha1
************************************************************
*/
#define METHOD		"sha1"
static unsigned char OneNET_Authorization(char *ver, char *res, unsigned int et, char *access_key, char *dev_name,
											char *authorization_buf, unsigned short authorization_buf_len, _Bool flag)
{
	
	size_t olen = 0;
	
	char sign_buf[64];								//保存签名的Base64编码结果 和 URL编码结果
	char hmac_sha1_buf[64];							//保存签名
	char access_key_base64[64];						//保存access_key的Base64编码结合
	char string_for_signature[72];					//保存string_for_signature，这个是加密的key

//----------------------------------------------------参数合法性--------------------------------------------------------------------
	if(ver == (void *)0 || res == (void *)0 || et < 1564562581 || access_key == (void *)0
		|| authorization_buf == (void *)0 || authorization_buf_len < 120)
		return 1;
	
//----------------------------------------------------将access_key进行Base64解码----------------------------------------------------
	memset(access_key_base64, 0, sizeof(access_key_base64));
	BASE64_Decode((unsigned char *)access_key_base64, sizeof(access_key_base64), &olen, (unsigned char *)access_key, strlen(access_key));
//	UsartPrintf(USART_DEBUG, "access_key_base64: %s\r\n", access_key_base64);
	
//----------------------------------------------------计算string_for_signature-----------------------------------------------------
	memset(string_for_signature, 0, sizeof(string_for_signature));
	if(flag)
		snprintf(string_for_signature, sizeof(string_for_signature), "%d\n%s\nproducts/%s\n%s", et, METHOD, res, ver);
	else
		snprintf(string_for_signature, sizeof(string_for_signature), "%d\n%s\nproducts/%s/devices/%s\n%s", et, METHOD, res, dev_name, ver);
//	UsartPrintf(USART_DEBUG, "string_for_signature: %s\r\n", string_for_signature);
	
//----------------------------------------------------加密-------------------------------------------------------------------------
	memset(hmac_sha1_buf, 0, sizeof(hmac_sha1_buf));
	
	hmac_sha1((unsigned char *)access_key_base64, strlen(access_key_base64),
				(unsigned char *)string_for_signature, strlen(string_for_signature),
				(unsigned char *)hmac_sha1_buf);
	
//	UsartPrintf(USART_DEBUG, "hmac_sha1_buf: %s\r\n", hmac_sha1_buf);
	
//----------------------------------------------------将加密结果进行Base64编码------------------------------------------------------
	olen = 0;
	memset(sign_buf, 0, sizeof(sign_buf));
	BASE64_Encode((unsigned char *)sign_buf, sizeof(sign_buf), &olen, (unsigned char *)hmac_sha1_buf, strlen(hmac_sha1_buf));

//----------------------------------------------------将Base64编码结果进行URL编码---------------------------------------------------
	OTA_UrlEncode(sign_buf);
//	UsartPrintf(USART_DEBUG, "sign_buf: %s\r\n", sign_buf);
	
//----------------------------------------------------计算Token--------------------------------------------------------------------
	if(flag)
		snprintf(authorization_buf, authorization_buf_len, "version=%s&res=products%%2F%s&et=%d&method=%s&sign=%s", ver, res, et, METHOD, sign_buf);
	else
		snprintf(authorization_buf, authorization_buf_len, "version=%s&res=products%%2F%s%%2Fdevices%%2F%s&et=%d&method=%s&sign=%s", ver, res, dev_name, et, METHOD, sign_buf);
//	UsartPrintf(USART_DEBUG, "Token: %s\r\n", authorization_buf);
	
	return 0;

}


//==========================================================
//	函数名称：	OneNet_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	无
//
//	返回参数：	1-成功	0-失败
//
//	说明：		与onenet平台建立连接
//==========================================================
_Bool OneNet_DevLink(void)
{
	char ProductID[64];
	char Auth_Info[200];
	char DeviceID[64];
	
//	u32 MCU_ID;
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包

	unsigned char *dataPtr;
	
	_Bool status = 1;
	
//	MCU_ID = *(__IO uint32_t*)(0x1FFFF7E8);	//获取MCU的唯一标识
//	sprintf(ProductID,"%s_%X",PROID,MCU_ID);
//	sprintf(Auth_Info,"%s_%X",AUTH_INFO,MCU_ID);
//	sprintf(DeviceID,"%s_%X",DEVID,MCU_ID);
	
	sprintf(ProductID,"%s",PROID);
	sprintf(Auth_Info,"%s",AUTH_INFO);
	sprintf(DeviceID,"%s",DEVID);
	
//	OneNET_Authorization("2018-10-31", PROID, 1746716140, AUTH_INFO, DEVID,
//								Auth_Info, sizeof(Auth_Info), 0);
	
	
	UART_Printf(&huart1,"OneNet_DevLink\r\n"
							"PROID: %s,	AUIF: %s,	DEVID:%s	--- 连接中....\r\n"
                        ,ProductID, Auth_Info, DeviceID);
	
	if(MQTT_PacketConnect(ProductID, Auth_Info, DeviceID, 6000, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		UART_Printf(&huart1,"123");
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//上传平台
		
		dataPtr = ESP8266_GetIPD(250);									//等待平台响应
		UART_Printf(&huart1,"\nMQTT_UnPacketRecv(dataPtr)=%d\n",MQTT_UnPacketRecv(dataPtr));
		UART_Printf(&huart1,"\nMQTT_UnPacketConnectAck(dataPtr)=%d\n",MQTT_UnPacketConnectAck(dataPtr));
		if(dataPtr != NULL)
		{
			UART_Printf(&huart1,"\nNOTNULL\n");
			
			UART_Printf(&huart1,"\nMQTT_UnPacketRecv(dataPtr)=%d\n",MQTT_UnPacketRecv(dataPtr));
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				UART_Printf(&huart1,"\nMQTT_UnPacketConnectAck(dataPtr)=%d\n",MQTT_UnPacketConnectAck(dataPtr));
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:UART_Printf(&huart1,"Tips:	连接成功\r\n");status = 0;break;
					
					case 1:UART_Printf(&huart1,"WARN:	连接失败：协议错误\r\n");break;
					case 2:UART_Printf(&huart1,"WARN:	连接失败：非法的clientid\r\n");break;
					case 3:UART_Printf(&huart1,"WARN:	连接失败：服务器失败\r\n");break;
					case 4:UART_Printf(&huart1,"WARN:	连接失败：用户名或密码错误\r\n");break;
					case 5:UART_Printf(&huart1,"WARN:	连接失败：非法链接(比如token非法)\r\n");break;
					
					default:UART_Printf(&huart1,"ERR:	连接失败：未知错误\r\n");break;
				}
			}
		}
		else UART_Printf(&huart1,"\nNULL\n");
		
		MQTT_DeleteBuffer(&mqttPacket);								//删包
	}
	else
		UART_Printf(&huart1,"WARN:	MQTT_PacketConnect Failed\r\n");
	
	return status;
	
}

//==========================================================
//	函数名称：	OneNet_Subscribe
//
//	函数功能：	订阅
//
//	入口参数：	topics：订阅的topic
//				topic_cnt：topic个数
//
//	返回参数：	SEND_TYPE_OK-成功	SEND_TYPE_SUBSCRIBE-需要重发
//
//	说明：		
//==========================================================
void OneNet_Subscribe(const char *topics[], unsigned char topic_cnt)
{
	
	unsigned char i = 0;
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//协议包
	
	for(; i < topic_cnt; i++)
		UART_Printf(&huart1,"Subscribe Topic: %s\r\n", topics[i]);
	
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, topics, topic_cnt, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqttPacket);											//删包
	}

}

void OneNet_PublishRec(void)
{
		
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//协议包
		
	if(MQTT_PacketPublishRec(MQTT_PUBLISH_ID,&mqttPacket) == 0)
	{
		UART_Printf(&huart1,"Rec ok");
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//向平台发送Rec报文
		
		MQTT_DeleteBuffer(&mqttPacket);											//删包
	}

}


//==========================================================
//	函数名称：	OneNet_Publish
//
//	函数功能：	发布消息
//
//	入口参数：	topic：发布的主题
//				msg：消息内容
//
//	返回参数：	SEND_TYPE_OK-成功	SEND_TYPE_PUBLISH-需要重送
//
//	说明：		
//==========================================================
void OneNet_Publish_lv0(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//协议包
	
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqttPacket) == 0)
	{
		
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqttPacket);											//删包
	}

}
void OneNet_Publish_lv1(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//协议包
	
	//↓qos2
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL1, 0, 1, &mqttPacket) == 0)
	{
		
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqttPacket);											//删包
	}

}

void OneNet_Puback(void)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//协议包
	

	if(MQTT_PacketPublishAck(MQTT_PUBACK_ID, &mqttPacket) == 0)
	{
//		UART_Printf(&huart1,"Publish Topic: %s, Msg: %s\r\n", topic, msg);			
		UART_Printf(&huart1,"puback ok");
		
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//向平台发送puback报文
		
		MQTT_DeleteBuffer(&mqttPacket);											//删包
	}
	

}


//==========================================================
//	函数名称：	OneNet_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据 也就是*cmd
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//协议包
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short topic_len = 0;
	unsigned short req_len = 0;
	
	unsigned char type = 0;
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;
	
	cJSON *json , *json_value;
	
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		case MQTT_PKT_CMD:															//命令下发
			
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//解出topic和消息体
			if(result == 0)
			{
				UART_Printf(&huart1,"cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);

				MQTT_DeleteBuffer(&mqttPacket);									//删包
			}
		break;
			
		case MQTT_PKT_PUBLISH:														//接收的Publish消息
		
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
			if(result == 0)
			{
				UART_Printf(&huart1,"topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
																	cmdid_topic, topic_len, req_payload, req_len);
				
				// 对数据包req_payload进行JSON格式解析
				json = cJSON_Parse(req_payload);
				if (!json)UART_Printf(&huart1,"Error before: [%s]\n",cJSON_GetErrorPtr());
				else
				{
					
					json_value = cJSON_GetObjectItem(json,"id");//搜索id关键字
					if(strstr(json_value->string,"id") != NULL)//搜索到了id关键字
					{
						if(json_value->valueint==hw_id)
						{
							
							//led1
							json_value = cJSON_GetObjectItem(json,"l1");//搜索LED1关键字
							if(strstr(json_value->string,"l1") != NULL)//搜索到了LED1关键字
							{
								if(json_value->valueint)
								{
									//开灯
								}
								else 
								{
									//关灯
								}
							}
							
							json_value = cJSON_GetObjectItem(json,"l2");//搜索LED2关键字
							if(strstr(json_value->string,"l2") != NULL)//搜索到了LED2关键字
							{
								if(json_value->valueint)
								{
									//开灯
								}
								else 
								{
									//关灯
								}
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
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				UART_Printf(&huart1,"Tips:	MQTT Publish Send OK\r\n");
			else
				UART_Printf(&huart1,"Tips:	MQTT Publish Send FAILED\r\n");
			
		break;
			
		case MQTT_PKT_PUBREC:														//发送Publish消息，平台回复的Rec，设备需回复Rel消息
		
			if(MQTT_UnPacketPublishRec(cmd) == 0)
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
			
			if(MQTT_UnPacketPublishRel(cmd, pkt_id) == 0)
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
		
			if(MQTT_UnPacketPublishComp(cmd) == 0)
			{
				UART_Printf(&huart1,"Tips:	Rev PublishComp\r\n");
			}
		
		break;
			
		case MQTT_PKT_SUBACK:														//发送Subscribe消息的Ack
		
			if(MQTT_UnPacketSubscribe(cmd) == 0)
				UART_Printf(&huart1,"Tips:	MQTT Subscribe OK\r\n");
			else
				UART_Printf(&huart1,"Tips:	MQTT Subscribe Err\r\n");
		
		break;
			
		case MQTT_PKT_UNSUBACK:														//发送UnSubscribe消息的Ack
		
			if(MQTT_UnPacketUnSubscribe(cmd) == 0)
				UART_Printf(&huart1,"Tips:	MQTT Subscribe OK\r\n");
			else
				UART_Printf(&huart1,"Tips:	MQTT Subscribe Err\r\n");
		
		break;
		
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//清空缓存
	
	if(result == -1)
		return;
	
	dataPtr = strchr(req_payload, '}');					//搜索'}'

	if(dataPtr != NULL && result != -1)					//如果找到了
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//判断是否是下发的命令控制数据
		{
			numBuf[num++] = *dataPtr++;
		}
		
		num = atoi((const char *)numBuf);				//转为数值形式
		
	}

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
