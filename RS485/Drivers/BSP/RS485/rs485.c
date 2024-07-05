#include "rs485.h"

UART_HandleTypeDef g_rs458_handler;     /* RS485���ƾ��(����) */
MODBUS modbus;

uint8_t Reg[] ={
	               0x01,
                 0x12,
                 0x13,
                 0x04,
	               0x25,
                 0x36,
                 0x07,
			           0X08,
};//reg����ǰ����õļĴ����ͼĴ������ݣ�Ҫ��ȡ�͸�д�Ĳ�������

uint16_t crc16(uint8_t* buf, int size) {
    uint16_t temp, flag;
    temp = 0xFFFF;
    for (uint16_t i = 0; i < size; i++) {
        temp = temp ^ buf[i];
        for (unsigned char j = 1; j <= 8; j++) {
            flag = temp & 0x0001;
            temp >>= 1;
            if (flag) {
                temp ^= 0xA001;
            }
        }
    }
    return temp;
}

//����1�������׵�ַ������2��Ҫ����ĳ��ȣ�����CRCУ��λ����ȫ�㣩
static inline uint16_t Modbus_CRC16(uint8_t* buf, int size) {
    uint16_t crc = crc16(buf, size);
    return (crc >> 8) | ((crc & 0xFF) << 8);
}

/**
 * @brief       RS485��ʼ������
 *   @note      �ú�����Ҫ�ǳ�ʼ������
 * @param       baudrate: ������, �����Լ���Ҫ���ò�����ֵ
 * @retval      ��
 */
void rs485_init()
{
/* ʱ��ʹ�� */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_USART1_CLK_ENABLE();

    /* GPIO ��ʼ������
	    PB3��RE
      PB4��TX
      PB5��RX
	  */
    GPIO_InitTypeDef gpio_initure;
	
	  gpio_initure.Pin       = RS485_RE_GPIO_PIN;
	  gpio_initure.Mode      = GPIO_MODE_OUTPUT_PP;
	  gpio_initure.Pull      = GPIO_PULLUP;
	  gpio_initure.Speed     = GPIO_SPEED_FREQ_HIGH;
	  HAL_GPIO_Init(RS485_RE_GPIO_PORT,&gpio_initure);
	
    gpio_initure.Pin       = RS485_TX_GPIO_PIN;
    gpio_initure.Mode      = GPIO_MODE_AF_PP;
    gpio_initure.Pull      = GPIO_PULLUP;
    gpio_initure.Speed     = GPIO_SPEED_FREQ_HIGH;
	  gpio_initure.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(RS485_TX_GPIO_PORT, &gpio_initure); /* ����TX �� ģʽ���� */

    gpio_initure.Pin       = RS485_RX_GPIO_PIN;
    gpio_initure.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(RS485_RX_GPIO_PORT, &gpio_initure); /* ����RX �� */

    /* USART ��ʼ������ */
    g_rs458_handler.Instance        = USART1;              /* ѡ��485��Ӧ�Ĵ��� */
    g_rs458_handler.Init.BaudRate   = 9600;                /* ������ */
    g_rs458_handler.Init.WordLength = UART_WORDLENGTH_8B;  /* �ֳ�Ϊ8λ���ݸ�ʽ */
    g_rs458_handler.Init.StopBits   = UART_STOPBITS_1;     /* һ��ֹͣλ */
    g_rs458_handler.Init.Parity     = UART_PARITY_NONE;    /* ����żУ��λ */
    g_rs458_handler.Init.HwFlowCtl  = UART_HWCONTROL_NONE; /* ��Ӳ������ */
    g_rs458_handler.Init.Mode       = UART_MODE_TX_RX;     /* �շ�ģʽ */
    HAL_UART_Init(&g_rs458_handler);                       /* HAL_UART_Init()��ʹ��UART1 */
		
		/* USART1 �ж�ʹ�� */
		HAL_NVIC_SetPriority(USART1_IRQn,0,1);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
		
}

// Modbus��ʼ������
void Modbus_Init()
{
  modbus.myadd = 0x02; //�ӻ��豸��ַΪ2
  modbus.timrun = 0;    //modbus��ʱ��ֹͣ����
 	modbus.slave_add=0x01;//����Ҫƥ��Ĵӻ���ַ�����豸��Ϊ����ʱ��
}


/* ������ȡ�ӻ��ļĴ�������-������0x03
   ����1�ӻ���ַ������2��ʼ��ַ������3�Ĵ�������
*/
void Host_Read03_slave(uint8_t slave,uint16_t StartAddr,uint16_t num)
{
 	uint16_t crc;                              //�����CRCУ��λ
	
 	modbus.slave_add=slave;                    //�����ȰѴӻ���ַ�洢����������������ݴ���ʱ���õ�
 	modbus.Host_Txbuf[0]=slave;                //����Ҫƥ��Ĵӻ���ַ
 	modbus.Host_Txbuf[1]=0x03;                 //������
 	modbus.Host_Txbuf[2]=StartAddr/256;        //��ʼ��ַ��λ
 	modbus.Host_Txbuf[3]=StartAddr%256;        //��ʼ��ַ��λ
 	modbus.Host_Txbuf[4]=num/256;              //�Ĵ���������λ
 	modbus.Host_Txbuf[5]=num%256;              //�Ĵ���������λ
 	crc=Modbus_CRC16(&modbus.Host_Txbuf[0],6); //��ȡCRCУ��λ
 	modbus.Host_Txbuf[6]=crc%256;              //�Ĵ���������λ
 	modbus.Host_Txbuf[7]=crc/256;              //�Ĵ���������λ
 	//�������ݰ�װ���(��8���ֽ�)

////	uint16_t size = sizeof(modbus.Host_Txbuf);
//	
//	HAL_UART_Transmit(USART1,modbus.Host_Txbuf,size,1000);
//	
//	HAL_UART_Receive(USART1,modbus.rcbuf,5+num*2,1000);
//	HOST_ModbusRX();
}

//�������մӻ�����Ϣ���д�������0x03
//void HOST_ModbusRX()
//{
//  uint16_t crc,rccrc;     //����crc�ͽ��յ���crc
// 
//  if(modbus.reflag == 0)  //�������δ����򷵻ؿ�
// 	{
// 	   return;
// 	}                       //�������ݽ���
// 	
//  //�������г��������λCRCУ��λ����ȫ�㣩
// 	crc = Modbus_CRC16(&modbus.rcbuf[0],modbus.recount-2);                     //��ȡCRCУ��λ
// 	rccrc = modbus.rcbuf[modbus.recount-2]*256+modbus.rcbuf[modbus.recount-1]; //�����ȡ��CRCУ��λ
// 	
// 	if(crc == rccrc) //CRC����ɹ� ��ʼ������
// 	{	
// 	   if(modbus.rcbuf[0] == modbus.slave_add)  // ����ַ���Ƕ�Ӧ�ӻ���������
// 		 {
// 			 if(modbus.rcbuf[1]==3)  //������ʱ03
// 		      Host_Func3();        //���Ƕ�ȡ�Ĵ�������Ч����λ���м���
// 		 }
// 		 
// 	}	
// 	modbus.recount = 0;  //���ռ�������
//  modbus.reflag = 0;  //���ձ�־����
//}

//void Host_Func3()
//{
// 	int i;
// 	int count=(int)modbus.rcbuf[2];//�������ݸ���
// 	
// 	printf("�ӻ����� %d ���Ĵ������ݣ�\r\n",count/2);
// 	for(i=0;i<count;i=i+2)
// 	{
// 		printf("Temp_Hbit= %d Temp_Lbit= %d temp= %d\r\n",(int)modbus.rcbuf[3+i],(int)modbus.rcbuf[4+i],(int)modbus.rcbuf[4+i]+((int)modbus.rcbuf[3+i])*256);
//  }
//}

// Modbus�¼�������
void Modbus_Event()
{
  uint16_t crc,rccrc;//crc�ͽ��յ���crc
	
	modbus.recount = 8;
	
  //û���յ����ݰ�
  if(modbus.reflag == 0)  //�������δ����򷵻ؿ�
 	{
 	  return;
 	}
	
//	if(modbus.rcbuf[0] == modbus.myadd)
//	{
//		Modbus_Func3();
//	}
	
 	//�յ����ݰ�(�������)
 	//ͨ������������֡����CRC
 	//����1�������׵�ַ������2��Ҫ����ĳ��ȣ�����CRCУ��λ����ȫ�㣩
 	crc = Modbus_CRC16(&modbus.rcbuf[0],modbus.recount-2); //��ȡCRCУ��λ
	
 	// ��ȡ����֡��CRC
 	rccrc = modbus.rcbuf[modbus.recount-2]*256+modbus.rcbuf[modbus.recount-1];//�����ȡ��CRCУ��λ	
 	//�ȼ��������������
 	//rccrc=modbus.rcbuf[modbus.recount-1]|(((u16)modbus.rcbuf[modbus.recount-2])<<8);//��ȡ���յ���CRC
 	if(crc == rccrc) //CRC����ɹ� ��ʼ������
 	{	
 	   if(modbus.rcbuf[0] == modbus.myadd)  // ����ַ�Ƿ�ʱ�Լ��ĵ�ַ
 		 {
 		   switch(modbus.rcbuf[1])   //����modbus������
 			 {
 			   case 0:             break;
 				 case 1:             break;
 				 case 2:             break;
 				 case 3:      Modbus_Func3();      break;//���Ƕ�ȡ�Ĵ���������
 				 case 4:             break;
 				 case 5:             break;
          case 6:            break;              //����д�뵥���Ĵ�������
 				 case 7:             break;
 				 case 8:             break;
 				 case 9:             break;
 				 case 16:       		 break;              //д�����Ĵ�������
 			 }
 		 }
 		 else if(modbus.rcbuf[0] == 0) //�㲥��ַ�����Ӧ
 		 {
 		    
 		 }	 
 	}	
 	modbus.recount = 0;//���ռ�������
  modbus.reflag = 0; //���ձ�־����
}


/*
********************************************************************************
������03
01  03      00 01     00 01          D5 CA	�ӵ�ַ01��ʼ����ȡһ���Ĵ�������������
ID ������  ��ʼ��ַ  ��ȡ�Ĵ����ĸ���
�ӻ����أ�
01  03       02       00 03          F8 45 �����������ֽڵ����ݣ�������00 03
ID  ������  �����ֽ�  ���ص���������
********************************************************************************
*/
// Modbus 3�Ź����뺯��
// Modbus ������ȡ�Ĵ���ֵ
void Modbus_Func3()
{
  uint16_t Regadd,Reglen,crc;
 	uint8_t i,j;	
 	//�õ�Ҫ��ȡ�Ĵ������׵�ַ
 	Regadd = modbus.rcbuf[2]*256+modbus.rcbuf[3];//��ȡ���׵�ַ
 	//�õ�Ҫ��ȡ�Ĵ��������ݳ���
 	Reglen = modbus.rcbuf[4]*256+modbus.rcbuf[5];//��ȡ�ļĴ�������
 	//���ͻ�Ӧ���ݰ�
 	i = 0;
 	modbus.sendbuf[i++] = modbus.myadd;      //ID�ţ����ͱ����豸��ַ
 	modbus.sendbuf[i++] = 0x03;              //���͹�����
  modbus.sendbuf[i++] = ((Reglen*2)%256);   //�����ֽڸ���
 	for(j=0;j<Reglen;j++)                    //��������
 	{
 		//reg����ǰ����õ�16λ���飨ģ�¼Ĵ�����
 	  modbus.sendbuf[i++] = Reg[Regadd+j]/256;//��λ����
 		modbus.sendbuf[i++] = Reg[Regadd+j]%256;//��λ����
 	}
 	crc = Modbus_CRC16(modbus.sendbuf,i);    //����Ҫ�������ݵ�CRC
 	modbus.sendbuf[i++] = crc%256;//У��λ��λ
 	modbus.sendbuf[i++] = crc/256;//У��λ��λ
 	//���ݰ�������
	
	HAL_GPIO_WritePin(RS485_RE_GPIO_PORT,RS485_RE_GPIO_PIN,GPIO_PIN_SET);
	HAL_UART_Transmit(&g_rs458_handler,(uint8_t *)modbus.sendbuf,Reglen*2+5,HAL_MAX_DELAY);
	HAL_GPIO_WritePin(RS485_RE_GPIO_PORT,RS485_RE_GPIO_PIN,GPIO_PIN_RESET);
}



