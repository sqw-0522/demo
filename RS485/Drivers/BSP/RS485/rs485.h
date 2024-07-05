#ifndef __RS485_H
#define __RS485_H

#include "py32f0xx_hal.h"

/******************************************************************************************/
/* RS485 ���� �� ���� ���� 
  PB3��RE
  PB4��TX
  PB5��RX
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
 	//��Ϊ�ӻ�ʱʹ��
  uint8_t  myadd;        //���豸�ӻ���ַ
 	uint8_t  rcbuf[100];   //modbus���ܻ�����
 	uint8_t  timout;       //modbus���ݳ���ʱ��
 	uint8_t  recount;      //modbus�˿ڽ��յ������ݸ���
 	uint8_t  timrun;       //modbus��ʱ���Ƿ��ʱ��־
 	uint8_t  reflag;       //modbusһ֡���ݽ�����ɱ�־λ
 	uint8_t  sendbuf[100]; //modbus���ͻ�����
 	
 	//��Ϊ������Ӳ���
 	uint8_t Host_Txbuf[8]; //modbus��������	
 	uint8_t slave_add;		 //Ҫƥ��Ĵӻ��豸��ַ��������ʵ��ʱʹ�ã�
 	uint8_t Host_send_flag;//�����豸����������ϱ�־λ
 	int     Host_Sendtime; //������һ֡���ݺ�ʱ�����
 	uint8_t Host_time_flag;//����ʱ�䵽��־λ��=1��ʾ����������ʱ����
 	uint8_t Host_End;      //�������ݺ������
}MODBUS;

uint16_t crc16(uint8_t* data, int size);

void rs485_init();                                     /* RS485��ʼ�� */
void Modbus_Init();
void Host_Read03_slave(uint8_t slave,uint16_t StartAddr,uint16_t num);
void HOST_ModbusRX();
void Host_Func3();
void Modbus_Event();
void Modbus_Func3();

#endif





