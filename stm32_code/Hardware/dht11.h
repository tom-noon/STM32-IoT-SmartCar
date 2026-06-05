#ifndef __DHT11_H
#define __DHT11_H 

#include "stm32f1xx_hal.h"
//#include "stm32f103xe.h"
#include "sys.h"
#include "delay.h"


//////////////////////////////////////////////////////////////////////////////////	 
//DHT11数字温湿度传感器驱动代码	   						  
//////////////////////////////////////////////////////////////////////////////////

//IO方向设置
//#define DHT11_IO_IN1()  {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=8<<12;}
//#define DHT11_IO_OUT1() {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=3<<12;}

//#define DHT11_IO_IN2()  {GPIOG->CRH&=0XFFFFF0FF;GPIOG->CRH|=8<<8;}
//#define DHT11_IO_OUT2() {GPIOG->CRH&=0XFFFFF0FF;GPIOG->CRH|=3<<8;}

#define DHT11_IO_IN1()   {GPIOG->CRH&=0XFFFF00FF;GPIOG->CRH|=0x88<<8;}
#define DHT11_IO_OUT1()  {GPIOG->CRH&=0XFFFF00FF;GPIOG->CRH|=0x33<<8;}

//IO操作函数											   
#define	DHT11_DQ_OUT1 PGout(11) //数据端口	PG11 
#define	DHT11_DQ_IN1  PGin(11)  //数据端口	PG11 


u8 DHT11_Init(void);//初始化DHT11
u8 DHT11_Read_Data1(u16 *temp,u16 *humi);//读取温湿度
u8 DHT11_Read_Byte1(void);//读出一个字节
u8 DHT11_Read_Bit1(void);//读出一个位
u8 DHT11_Check1(void);//检测是否存在DHT11
void DHT11_Rst1(void);//复位DHT11    
#endif





