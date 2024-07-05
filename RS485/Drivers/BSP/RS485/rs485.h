#ifndef __RS485_H
#define __RS485_H

#include "py32f0xx_hal.h"

/******************************************************************************************/
/* RS485 引脚 和 串口 定义 
  PB3：RE
  PB4：TX
  PB5：RX
 */
#define RS485_RE_GPIO_PORT                  GPIOB
#define RS485_RE_GPIO_PIN                   GPIO_PIN_3

#define RS485_TX_GPIO_PORT                  GPIOB
#define RS485_TX_GPIO_PIN                   GPIO_PIN_4

#define RS485_RX_GPIO_PORT                  GPIOB
#define RS485_RX_GPIO_PIN                   GPIO_PIN_5

/******************************************************************************************/

typedef struct 
{
 	//作为从机时使用
  uint8_t  myadd;        //本设备从机地址
 	uint8_t  rcbuf[100];   //modbus接受缓冲区
 	uint8_t  timout;       //modbus数据持续时间
 	uint8_t  recount;      //modbus端口接收到的数据个数
 	uint8_t  timrun;       //modbus定时器是否计时标志
 	uint8_t  reflag;       //modbus一帧数据接受完成标志位
 	uint8_t  sendbuf[100]; //modbus发送缓冲区
 	
 	//作为主机添加部分
 	uint8_t Host_Txbuf[8]; //modbus发送数组	
 	uint8_t slave_add;		 //要匹配的从机设备地址（做主机实验时使用）
 	uint8_t Host_send_flag;//主机设备发送数据完毕标志位
 	int     Host_Sendtime; //发送完一帧数据后时间计数
 	uint8_t Host_time_flag;//发送时间到标志位，=1表示到发送数据时间了
 	uint8_t Host_End;      //接收数据后处理完毕
}MODBUS;

uint16_t crc16(uint8_t* data, int size);

void rs485_init();                                     /* RS485初始化 */
void Modbus_Init();
void Host_Read03_slave(uint8_t slave,uint16_t StartAddr,uint16_t num);
void HOST_ModbusRX();
void Host_Func3();
void Modbus_Event();
void Modbus_Func3();

#endif





