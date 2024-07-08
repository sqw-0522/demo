#include "./BSP/ADC/adc.h"

ADC_HandleTypeDef       ntc_adc_handle;
ADC_ChannelConfTypeDef  sConfig;

/* ADC��ʼ������ */
void adc_init(void)
{
	__HAL_RCC_ADC_CLK_ENABLE();  /* ʹ��ADCʱ�� */
	
	ntc_adc_handle.Instance                   = ADC1;
	ntc_adc_handle.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV32;   /* ����ADCʱ�� */
  ntc_adc_handle.Init.Resolution            = ADC_RESOLUTION_12B;          /* 12 bit�ֱ��� */
	ntc_adc_handle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;         /* ���ݶ��뷽ʽ���Ҷ��� */
	ntc_adc_handle.Init.ScanConvMode          = ADC_SCAN_DISABLE;            /* ��ɨ��ģʽ������ͨ��0 */
	ntc_adc_handle.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;         /* ת����ɱ�־ */
	ntc_adc_handle.Init.LowPowerAutoWait      = DISABLE;
	ntc_adc_handle.Init.ContinuousConvMode    = DISABLE;
  ntc_adc_handle.Init.DiscontinuousConvMode = DISABLE;
  ntc_adc_handle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
  ntc_adc_handle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;/* ������������Ϊѡ������������� */
  ntc_adc_handle.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;     /* ������������DR�Ĵ����ᱻ�ϴε�ת���������*/
  ntc_adc_handle.Init.SamplingTimeCommon    = ADC_SAMPLETIME_41CYCLES_5; 	  /* ͨ������ʱ��Ϊ41.5��ADCʱ������*/
	HAL_ADC_Init(&ntc_adc_handle);  /* ��ʼ�� */
	
	sConfig.Channel      = ADC_CHANNEL_0;
	sConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;
	sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
	HAL_ADC_ConfigChannel(&ntc_adc_handle,&sConfig);
	
	HAL_ADCEx_Calibration_Start(&ntc_adc_handle);
	
}

/* ADC MSP��ʼ������ */
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

/* ���ADCת����Ľ������ */
uint32_t adc_get_result(void)
{
	HAL_ADC_Start(&ntc_adc_handle);
	HAL_ADC_PollForConversion(&ntc_adc_handle, 10 );
	return (uint16_t) HAL_ADC_GetValue(&ntc_adc_handle);
}




