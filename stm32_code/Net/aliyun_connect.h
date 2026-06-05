#ifndef __ALIYUN_CONNECT_H
#define __ALIYUN_CONNECT_H

#include "sys.h"

#include "esp8266_mqtt.h"
#include "esp8266_at.h"

#include "mqttkit.h"
#include "cJSON.h"


//ESP8266通过MQTT连接阿里云物联网--初始化函数
void ES8266_MQTT_Init(void);
//STM32数据上报
void STM32_Report(void);
//处理MQTT下发的消息
void deal_MQTT_message(uint8_t* buf,uint16_t len);

#endif
