#include "./BSP/ADC/adc.h"

ADC_HandleTypeDef       ntc_adc_handle;
ADC_ChannelConfTypeDef  sConfig;

/* ADC初始化函数 */
void adc_init(void)
{
	__HAL_RCC_ADC_CLK_ENABLE();  /* 使能ADC时钟 */
	
	ntc_adc_handle.Instance                   = ADC1;
	ntc_adc_handle.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV32;   /* 设置ADC时钟 */
  ntc_adc_handle.Init.Resolution            = ADC_RESOLUTION_12B;          /* 12 bit分辨率 */
	ntc_adc_handle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;         /* 数据对齐方式：右对齐 */
	ntc_adc_handle.Init.ScanConvMode          = ADC_SCAN_DISABLE;            /* 非扫描模式，仅用通道0 */
	ntc_adc_handle.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;         /* 转换完成标志 */
	ntc_adc_handle.Init.LowPowerAutoWait      = DISABLE;
	ntc_adc_handle.Init.ContinuousConvMode    = DISABLE;
  ntc_adc_handle.Init.DiscontinuousConvMode = DISABLE;
  ntc_adc_handle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
  ntc_adc_handle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;/* 参数丢弃，因为选择了软件触发器 */
  ntc_adc_handle.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;     /* 如果发生溢出，DR寄存器会被上次的转换结果覆盖*/
  ntc_adc_handle.Init.SamplingTimeCommon    = ADC_SAMPLETIME_41CYCLES_5; 	  /* 通道采样时间为41.5个ADC时钟周期*/
	HAL_ADC_Init(&ntc_adc_handle);  /* 初始化 */
	
	sConfig.Channel      = ADC_CHANNEL_0;
	sConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;
	sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
	HAL_ADC_ConfigChannel(&ntc_adc_handle,&sConfig);
	
	HAL_ADCEx_Calibration_Start(&ntc_adc_handle);
	
}

/* ADC MSP初始化函数 */
void HAL_ADC_MSPInit(ADC_HandleTypeDef *hadc)
{
	if(hadc ->Instance == ADC1)
	{
		GPIO_InitTypeDef gpio_init_struct;
	
	  __HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_ADC_CLK_ENABLE();
	
	  gpio_init_struct.Pin = GPIO_PIN_1;
	  gpio_init_struct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOB,&gpio_init_struct);
		
		
	}
}

/* 获得ADC转换后的结果函数 */
uint32_t adc_get_result(void)
{
	HAL_ADC_Start(&ntc_adc_handle);
	HAL_ADC_PollForConversion(&ntc_adc_handle, 10 );
	return (uint16_t) HAL_ADC_GetValue(&ntc_adc_handle);
}




