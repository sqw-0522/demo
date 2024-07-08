#include "rs485.h"

UART_HandleTypeDef g_rs458_handler;     /* RS485控制句柄(串口) */
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
};//reg是提前定义好的寄存器和寄存器数据，要读取和改写的部分内容

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

//参数1是数组首地址，参数2是要计算的长度（除了CRC校验位其余全算）
static inline uint16_t Modbus_CRC16(uint8_t* buf, int size) {
    uint16_t crc = crc16(buf, size);
    return (crc >> 8) | ((crc & 0xFF) << 8);
}

/**
 * @brief       RS485初始化函数
 *   @note      该函数主要是初始化串口
 * @param       baudrate: 波特率, 根据自己需要设置波特率值
 * @retval      无
 */
void rs485_init()
{
/* 时钟使能 */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_USART1_CLK_ENABLE();

    /* GPIO 初始化设置
	    PB3：RE
      PB4：TX
      PB5：RX
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
    HAL_GPIO_Init(RS485_TX_GPIO_PORT, &gpio_initure); /* 串口TX 脚 模式设置 */

    gpio_initure.Pin       = RS485_RX_GPIO_PIN;
    gpio_initure.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(RS485_RX_GPIO_PORT, &gpio_initure); /* 串口RX 脚 */

    /* USART 初始化设置 */
    g_rs458_handler.Instance        = USART1;              /* 选择485对应的串口 */
    g_rs458_handler.Init.BaudRate   = 9600;                /* 波特率 */
    g_rs458_handler.Init.WordLength = UART_WORDLENGTH_8B;  /* 字长为8位数据格式 */
    g_rs458_handler.Init.StopBits   = UART_STOPBITS_1;     /* 一个停止位 */
    g_rs458_handler.Init.Parity     = UART_PARITY_NONE;    /* 无奇偶校验位 */
    g_rs458_handler.Init.HwFlowCtl  = UART_HWCONTROL_NONE; /* 无硬件流控 */
    g_rs458_handler.Init.Mode       = UART_MODE_TX_RX;     /* 收发模式 */
    HAL_UART_Init(&g_rs458_handler);                       /* HAL_UART_Init()会使能UART1 */
		
		/* USART1 中断使能 */
		HAL_NVIC_SetPriority(USART1_IRQn,0,1);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
		
}

// Modbus初始化函数
void Modbus_Init()
{
  modbus.myadd = 0x02; //从机设备地址为2
  modbus.timrun = 0;    //modbus定时器停止计算
 	modbus.slave_add=0x01;//主机要匹配的从机地址（本设备作为主机时）
}


/* 主机读取从机的寄存器数据-功能码0x03
   参数1从机地址，参数2起始地址，参数3寄存器个数
*/
void Host_Read03_slave(uint8_t slave,uint16_t StartAddr,uint16_t num)
{
 	uint16_t crc;                              //计算的CRC校验位
	
 	modbus.slave_add=slave;                    //这是先把从机地址存储下来，后面接收数据处理时会用到
 	modbus.Host_Txbuf[0]=slave;                //这是要匹配的从机地址
 	modbus.Host_Txbuf[1]=0x03;                 //功能码
 	modbus.Host_Txbuf[2]=StartAddr/256;        //起始地址高位
 	modbus.Host_Txbuf[3]=StartAddr%256;        //起始地址低位
 	modbus.Host_Txbuf[4]=num/256;              //寄存器个数高位
 	modbus.Host_Txbuf[5]=num%256;              //寄存器个数低位
 	crc=Modbus_CRC16(&modbus.Host_Txbuf[0],6); //获取CRC校验位
 	modbus.Host_Txbuf[6]=crc%256;              //寄存器个数低位
 	modbus.Host_Txbuf[7]=crc/256;              //寄存器个数高位
 	//发送数据包装完毕(共8个字节)

////	uint16_t size = sizeof(modbus.Host_Txbuf);
//	
//	HAL_UART_Transmit(USART1,modbus.Host_Txbuf,size,1000);
//	
//	HAL_UART_Receive(USART1,modbus.rcbuf,5+num*2,1000);
//	HOST_ModbusRX();
}

//主机接收从机的消息进行处理功能码0x03
//void HOST_ModbusRX()
//{
//  uint16_t crc,rccrc;     //计算crc和接收到的crc
// 
//  if(modbus.reflag == 0)  //如果接收未完成则返回空
// 	{
// 	   return;
// 	}                       //接收数据结束
// 	
//  //（数组中除了最后两位CRC校验位其余全算）
// 	crc = Modbus_CRC16(&modbus.rcbuf[0],modbus.recount-2);                     //获取CRC校验位
// 	rccrc = modbus.rcbuf[modbus.recount-2]*256+modbus.rcbuf[modbus.recount-1]; //计算读取的CRC校验位
// 	
// 	if(crc == rccrc) //CRC检验成功 开始分析包
// 	{	
// 	   if(modbus.rcbuf[0] == modbus.slave_add)  // 检查地址是是对应从机发过来的
// 		 {
// 			 if(modbus.rcbuf[1]==3)  //功能码时03
// 		      Host_Func3();        //这是读取寄存器的有效数据位进行计算
// 		 }
// 		 
// 	}	
// 	modbus.recount = 0;  //接收计数清零
//  modbus.reflag = 0;  //接收标志清零
//}

//void Host_Func3()
//{
// 	int i;
// 	int count=(int)modbus.rcbuf[2];//这是数据个数
// 	
// 	printf("从机返回 %d 个寄存器数据：\r\n",count/2);
// 	for(i=0;i<count;i=i+2)
// 	{
// 		printf("Temp_Hbit= %d Temp_Lbit= %d temp= %d\r\n",(int)modbus.rcbuf[3+i],(int)modbus.rcbuf[4+i],(int)modbus.rcbuf[4+i]+((int)modbus.rcbuf[3+i])*256);
//  }
//}

// Modbus事件处理函数
void Modbus_Event()
{
  uint16_t crc,rccrc;//crc和接收到的crc
	
	modbus.recount = 8;
	
  //没有收到数据包
  if(modbus.reflag == 0)  //如果接收未完成则返回空
 	{
 	  return;
 	}
	
//	if(modbus.rcbuf[0] == modbus.myadd)
//	{
//		Modbus_Func3();
//	}
	
 	//收到数据包(接收完成)
 	//通过读到的数据帧计算CRC
 	//参数1是数组首地址，参数2是要计算的长度（除了CRC校验位其余全算）
 	crc = Modbus_CRC16(&modbus.rcbuf[0],modbus.recount-2); //获取CRC校验位
	
 	// 读取数据帧的CRC
 	rccrc = modbus.rcbuf[modbus.recount-2]*256+modbus.rcbuf[modbus.recount-1];//计算读取的CRC校验位	
 	//等价于下面这条语句
 	//rccrc=modbus.rcbuf[modbus.recount-1]|(((u16)modbus.rcbuf[modbus.recount-2])<<8);//获取接收到的CRC
 	if(crc == rccrc) //CRC检验成功 开始分析包
 	{	
 	   if(modbus.rcbuf[0] == modbus.myadd)  // 检查地址是否时自己的地址
 		 {
 		   switch(modbus.rcbuf[1])   //分析modbus功能码
 			 {
 			   case 0:             break;
 				 case 1:             break;
 				 case 2:             break;
 				 case 3:      Modbus_Func3();      break;//这是读取寄存器的数据
 				 case 4:             break;
 				 case 5:             break;
          case 6:            break;              //这是写入单个寄存器数据
 				 case 7:             break;
 				 case 8:             break;
 				 case 9:             break;
 				 case 16:       		 break;              //写入多个寄存器数据
 			 }
 		 }
 		 else if(modbus.rcbuf[0] == 0) //广播地址不予回应
 		 {
 		    
 		 }	 
 	}	
 	modbus.recount = 0;//接收计数清零
  modbus.reflag = 0; //接收标志清零
}


/*
********************************************************************************
主机：03
01  03      00 01     00 01          D5 CA	从地址01开始读读取一个寄存器的数据内容
ID 功能码  起始地址  读取寄存器的个数
从机返回：
01  03       02       00 03          F8 45 返回了两个字节的数据，数据是00 03
ID  功能码  几个字节  返回的数据内容
********************************************************************************
*/
// Modbus 3号功能码函数
// Modbus 主机读取寄存器值
void Modbus_Func3()
{
  uint16_t Regadd,Reglen,crc;
 	uint8_t i,j;	
 	//得到要读取寄存器的首地址
 	Regadd = modbus.rcbuf[2]*256+modbus.rcbuf[3];//读取的首地址
 	//得到要读取寄存器的数据长度
 	Reglen = modbus.rcbuf[4]*256+modbus.rcbuf[5];//读取的寄存器个数
 	//发送回应数据包
 	i = 0;
 	modbus.sendbuf[i++] = modbus.myadd;      //ID号：发送本机设备地址
 	modbus.sendbuf[i++] = 0x03;              //发送功能码
  modbus.sendbuf[i++] = ((Reglen*2)%256);   //返回字节个数
 	for(j=0;j<Reglen;j++)                    //返回数据
 	{
 		//reg是提前定义好的16位数组（模仿寄存器）
 	  modbus.sendbuf[i++] = Reg[Regadd+j]/256;//高位数据
 		modbus.sendbuf[i++] = Reg[Regadd+j]%256;//低位数据
 	}
 	crc = Modbus_CRC16(modbus.sendbuf,i);    //计算要返回数据的CRC
 	modbus.sendbuf[i++] = crc%256;//校验位低位
 	modbus.sendbuf[i++] = crc/256;//校验位高位
 	//数据包打包完成
	
	HAL_GPIO_WritePin(RS485_RE_GPIO_PORT,RS485_RE_GPIO_PIN,GPIO_PIN_SET);
	HAL_UART_Transmit(&g_rs458_handler,(uint8_t *)modbus.sendbuf,Reglen*2+5,HAL_MAX_DELAY);
	HAL_GPIO_WritePin(RS485_RE_GPIO_PORT,RS485_RE_GPIO_PIN,GPIO_PIN_RESET);
}



