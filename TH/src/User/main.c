#include "main.h"
#include "OLED.h"

int main(void)
{	
	HAL_Init();
	OLED_Init();
	
	OLED_Clear();
	
	OLED_ShowNum(0,1,123,3,OLED_8X16);
	OLED_Update();
}
