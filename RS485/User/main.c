#include "main.h"
#include "led.h"
#include "OLED.h"
#include "rs485.h"

uint8_t data = 0x02;
extern MODBUS modbus;
extern UART_HandleTypeDef g_rs458_handler;

int main(void)
{	
	int i = 0;
	HAL_Init();
	led_init();
	OLED_Init();
	rs485_init();
	Modbus_Init();
	
	OLED_Clear();
	
	HAL_GPIO_WritePin(RS485_RE_GPIO_PORT,RS485_RE_GPIO_PIN,GPIO_PIN_SET);
	HAL_UART_Transmit(&g_rs458_handler,"\"",1,HAL_MAX_DELAY);	
	HAL_GPIO_WritePin(RS485_RE_GPIO_PORT,RS485_RE_GPIO_PIN,GPIO_PIN_RESET);
	
	while(1)
	{
		if(HAL_UART_Receive(&g_rs458_handler,(uint8_t *)modbus.rcbuf,8,HAL_MAX_DELAY) == HAL_OK)
		{
			HAL_Delay(100);
//			OLED_ShowHexNum(0,1,data,1,OLED_8X16);
//			for(i=0;i<8;i++)
//			{
//				OLED_ShowHexNum(i*8,1,modbus.rcbuf[i],1,OLED_8X16);
//			}
//		HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_3);
			if(HAL_UART_Transmit(&g_rs458_handler,(uint8_t *)modbus.rcbuf,8,HAL_MAX_DELAY) == HAL_OK)
			{
				for(i=0;i<8;i++)
				{
					OLED_ShowHexNum(i*8,1,modbus.rcbuf[i],1,OLED_8X16);
				}
			}
			modbus.reflag = 1;
			Modbus_Event();
			OLED_Update();
		}
		HAL_Delay(100);
	}
}



