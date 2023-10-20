#include "../Bsp/bsp_w25qxx.h"

//器件使能
static void W25Qx_Enable() {
	HAL_GPIO_WritePin(W25Q_CS_GPIO_Port, W25Q_CS_Pin, GPIO_PIN_RESET); // Chip select
}

//器件失能
static void W25Qx_Disable() {
	HAL_GPIO_WritePin(W25Q_CS_GPIO_Port, W25Q_CS_Pin, GPIO_PIN_SET); // Chip disselect
}
 
//发送1个字节数据
static void sip_SendByte(uint8_t byte) {
	HAL_SPI_Transmit(&hspi1,(uint8_t*) &byte,1,HAL_MAX_DELAY);
}

//接收1个字节数据
static uint8_t spi_ReceiveByte() {
	uint8_t byte;
	HAL_SPI_Receive(&hspi1,(uint8_t*) &byte, 1, HAL_MAX_DELAY);
	return byte;
}

/**
 * @brief    SPI发送指定长度的数据
 * @param    buf  ―― 发送数据缓冲区首地址
 * @param    size ―― 要发送数据的字节数
 * @retval   成功返回HAL_OK
 */
//static HAL_StatusTypeDef SPI_Transmit(uint8_t* send_buf, uint16_t size)
//{
//   return HAL_SPI_Transmit(&hspi1, send_buf, size, 100);
//}

/**
 * @brief   SPI接收指定长度的数据
 * @param   buf  ―― 接收数据缓冲区首地址
 * @param   size ―― 要接收数据的字节数
 * @retval  成功返回HAL_OK
 */
//static HAL_StatusTypeDef SPI_Receive(uint8_t* recv_buf, uint16_t size)
//{
//   return HAL_SPI_Receive(&hspi1, recv_buf, size, 100);
//}

//SPI发送数据的同时接收指定长度的数据
//static HAL_StatusTypeDef SPI_TransmitReceive(uint8_t* send_buf, uint8_t* recv_buf, uint16_t size)
//{
//   return HAL_SPI_TransmitReceive(&hspi1, send_buf, recv_buf, size, 100);
//}

//W25Qx写使能,将WEL置1 
static void W25Qx_Write_Enable()   
{
	W25Qx_Enable();                            //使能器件   
  sip_SendByte(W25X_WriteEnable); 
	W25Qx_Disable();                            //取消片选     	      
}

//W25Qx写失能,将WEL置0 
//static void W25Qx_Write_Disable()   
//{
//	W25Qx_Enable();                            //使能器件   
//  sip_SendByte(W25X_WriteDisable); 
//	W25Qx_Disable();                            //取消片选     	      
//}
 
//读取寄存器状态
static uint8_t W25Qx_ReadSR(void)   
{  
	uint8_t byte=0;   
	W25Qx_Enable();                         //使能器件   
	sip_SendByte(W25X_ReadStatusReg);    		//发送读取状态寄存器命令
	byte = spi_ReceiveByte();             	//读取一个字节
	W25Qx_Disable();                        //取消片选     
	return byte;   
} 
 
//等待空闲
void W25Qx_Wait_Busy()   
{   
	while((W25Qx_ReadSR()&WIP_Flag)==WIP_Flag);   // 等待BUSY位清空
}
 
//擦除地址所在的一个扇区
static void Erase_one_Sector(uint32_t Address)
{
	W25Qx_Write_Enable();    //SET WEL
	W25Qx_Wait_Busy(); 		
	W25Qx_Enable();          //使能器件 
	sip_SendByte(W25X_SectorErase);      //发送扇区擦除指令 
	sip_SendByte((uint8_t)((Address)>>16));  //发送24bit地址    
	sip_SendByte((uint8_t)((Address)>>8));   
	sip_SendByte((uint8_t)Address);  
	W25Qx_Disable();         //取消片选     	      
	W25Qx_Wait_Busy(); 			 //等待擦除完成
}
 
//擦除地址所在的扇区
void Erase_Write_data_Sector(uint32_t Address,uint32_t Write_data_NUM) {
	//总共4096个扇区
	//计算 写入数据开始的地址 + 要写入数据个数的最后地址 所处的扇区	
	uint16_t Star_Sector,End_Sector,Num_Sector;
	Star_Sector = Address / 4096;						//数据写入开始的扇区
	End_Sector = (Address + Write_data_NUM) / 4096;		//数据写入结束的扇区
	Num_Sector = End_Sector - Star_Sector;  			//数据写入跨几个扇区
 
	//开始擦除扇区
	for(uint16_t i=0;i <= Num_Sector;i++)
	{
		Erase_one_Sector(Address);
		Address += 4096;
	}
 
}
 
//擦除整个芯片
void Erase_W25Qx_Chip(void){  
	W25Qx_WAKEUP();
	W25Qx_Write_Enable(); //SET WEL 
	W25Qx_Wait_Busy();   
	W25Qx_Enable();       //使能器件   
	sip_SendByte(W25X_ChipErase);//发送片擦除命令  
	W25Qx_Disable();      //取消片选     	      
	W25Qx_Wait_Busy();   	//等待芯片擦除结束
} 
 

/**
	* @brief  读取W25Qx数据
	* @param 	 pBuffer，存储读出数据的指针
	* @param   ReadAddr，读取地址
	* @param   NumByteToRead，读取数据长度
	* @retval  无
	*/
void W25Qx_Read_data(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 
 	uint16_t i=0;   										    
	W25Qx_Enable();             //使能器件   
	sip_SendByte(W25X_ReadData);         //发送读取命令   
	sip_SendByte((uint8_t)((ReadAddr)>>16));  //发送24bit地址    
	sip_SendByte((uint8_t)((ReadAddr)>>8));   
	sip_SendByte((uint8_t)ReadAddr);   
	for(;i<NumByteToRead;i++) { 
		pBuffer[i]=spi_ReceiveByte();   //循环读数  
	}
	W25Qx_Disable(); 				    	      
}

 
//写字，一次最多一页
static void W25Qx_Write_Word(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
 	uint16_t i; 
 
	W25Qx_Write_Enable();                  //SET WEL
	W25Qx_Enable();                        //使能器件
	sip_SendByte(W25X_PageProgram);
	sip_SendByte((uint8_t)((WriteAddr) >> 16)); //写入的目标地址   
	sip_SendByte((uint8_t)((WriteAddr) >> 8));   
	sip_SendByte((uint8_t)WriteAddr);   
	for (i=0; i<NumByteToWrite; i++) {
		sip_SendByte(*pBuffer++);//循环写入字节数据  
	}
	W25Qx_Disable();
	W25Qx_Wait_Busy();		//写完之后需要等待芯片操作完。
}
 
//多页写入数据
void W25Qx_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{
	uint16_t Word_remain;
	Word_remain=256 - WriteAddr % 256; 	//定位页剩余的字数	
	
	if(NumByteToWrite <= Word_remain)
		Word_remain=NumByteToWrite;		//定位页能一次写完
	while(1)
	{
		W25Qx_Write_Word(pBuffer,WriteAddr,Word_remain);	
		if(NumByteToWrite==Word_remain)
		{
			break;	//判断写完就 break
		}	
	 	else //没写完，翻页了
		{
			pBuffer += Word_remain;		//直针后移当页已写字数
			WriteAddr += Word_remain;	
			NumByteToWrite -= Word_remain;	//减去已经写入了的字数
			if(NumByteToWrite>256)
				Word_remain=256; 		//一次可以写入256个字
			else 
				Word_remain=NumByteToWrite; 	//不够256个字了
		}
	}	    
} 

 
 /**
  * @brief  读取FLASH ID
  * @param 	无
  * @retval FLASH ID
  */
uint32_t W25Qx_ReadJedecDeviceID(void)
{
  uint32_t Temp;
  W25Qx_Enable();
  sip_SendByte(W25X_JedecDeviceID);
  Temp = (uint32_t)spi_ReceiveByte()<<16;
  Temp |= (uint32_t)spi_ReceiveByte()<<8;
  Temp |= spi_ReceiveByte();
  W25Qx_Disable();
  return Temp;
}

/**
  * @brief  读取W25Qx Device ID
  * @param 	无
  * @retval W25Qx Device ID
  */
uint32_t W25Qx_ReadDeviceID(void)
{
  uint32_t Temp = 0;
  W25Qx_Enable();
  sip_SendByte(W25X_DeviceID);
  sip_SendByte(Dummy_Byte);
  sip_SendByte(Dummy_Byte);
  sip_SendByte(Dummy_Byte);
  Temp = spi_ReceiveByte();
  W25Qx_Disable();
  return Temp;
}

//进入掉电模式
void W25Qx_PowerDown(void){ 
  W25Qx_Enable();
  sip_SendByte(W25X_PowerDown);
  W25Qx_Disable();
}   

//唤醒
void W25Qx_WAKEUP(void){
  W25Qx_Enable();
  sip_SendByte(W25X_ReleasePowerDown);
  W25Qx_Disable(); 
}   

//获取结构体信息
void w25qFlash_read( void ) {
	//W25Qx_Read_data((uint8_t*)&ipinfo_str, STR_ADDR3, sizeof(BOOT_STR));
}

//写入数据
