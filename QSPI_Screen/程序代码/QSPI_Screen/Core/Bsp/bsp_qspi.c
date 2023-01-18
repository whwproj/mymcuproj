#include "../Bsp/bsp_qspi.h"



/*
**********************************************************************************
 *Function:     QspiSingleIOWrite
 *Description:  QSPI单线IO传输数据
 *Calls:       
 *Called By:   
 *Input:        dat：要传输的数据
 *Output:      
 *Return:      
 *Others:       
*********************************************************************************
*/
void QspiSingleIOWriteData(uint8_t dat) 
{    
	uint8_t i;
    
        
	for(i=0;i<8;i++){              
		QSPI_CLK_Clr();
		if(dat&0x80)
		{
			QSPI_DA0_Set();
		}
		else
		{
			QSPI_DA0_Clr();
		}
		QSPI_CLK_Set();
		dat<<=1;
	}   
}

/*
**********************************************************************************
 *Function:     Nv3030bQspiQuadIOWrite
 *Description:  QSPI四线IO传输数据
 *Calls:        
 *Called By:    
 *Input:        dat：要传输的数据
 *Output:      
 *Return:      
 *Others:       
*********************************************************************************
*/	 
void QspiQuadIOWriteData(uint8_t dat) 
{    
	uint8_t i;

	        
	for(i=0;i<2;i++){              
	QSPI_CLK_Clr();
	if(dat&0x80)
	{
		QSPI_DA3_Set();
	}
	else
	{
		QSPI_DA3_Clr();
	}
	dat<<=1;

	if(dat&0x80)
	{
		QSPI_DA2_Set();
	}
	else
	{
		QSPI_DA2_Clr();
	}
	dat<<=1;

	if(dat&0x80)
	{
		QSPI_DA1_Set();
	}
	else
	{
		QSPI_DA1_Clr();
	}
	dat<<=1;

	if(dat&0x80)
	{
		QSPI_DA0_Set();
	}
	else
	{
		QSPI_DA0_Clr();
	}
	QSPI_CLK_Set();
	dat<<=1;                
	}    
          
}

/*
**********************************************************************************
 *Function:     QspiSingleIOWrData8
 *Description:  QSPI单线IO传输1字节数据
 *Calls:       
 *Called By:   
 *Input:        dat：要传输的数据
 *Output:      
 *Return:      
 *Others:       
*********************************************************************************
*/		
void QspiSingleIOWrData8(uint8_t dat)
{
   QspiSingleIOWriteData(dat);
}

/*
**********************************************************************************
 *Function:     QspiQuadIOWrData8
 *Description:  QSPI四线IO传输1字节数据
 *Calls:       
 *Called By:   
 *Input:        dat：要传输的数据
 *Output:      
 *Return:      
 *Others:       
*********************************************************************************
*/		
void QspiQuadIOWrData8(uint8_t dat)
{
   QspiQuadIOWriteData(dat);
}

/*
**********************************************************************************
 *Function:     QspiWrData16
 *Description:  QSPI传输4字节数据
 *Calls:       
 *Called By:   
 *Input:        l_line:传输数据时使用的线数，
                       SINGLE_LINE：用单根IO传输
                       QUAD_LINE：  用4根IO传输
                   dat：要传输的数据
 *Output:      
 *Return:      
 *Others:       
*********************************************************************************
*/
void QspiWrData16(ENUM_QSPI_IO_NUM l_line, uint16_t dat)
{
	if(SINGLE_LINE == l_line){
		QspiSingleIOWriteData(dat>>8);
		QspiSingleIOWriteData(dat);
	}
	else{
		QspiQuadIOWriteData(dat>>8);
		QspiQuadIOWriteData(dat);
	}
}

/*
**********************************************************************************
 *Function:     QspiWrComm
 *Description:  QSPI写寄存器
 *Calls:       
 *Called By:   
 *Input:        l_line:传输数据时使用的线数，
                       SINGLE_LINE：用单根IO传输
                       QUAD_LINE：  用4根IO传输
                  dat：要传输的数据
 *Output:      
 *Return:      
 *Others:       
*********************************************************************************
*/
void QspiWrComm(uint8_t dat)
{
//        QSPI_RS_Clr();//写命令
	QspiSingleIOWriteData(0x02);
	QspiSingleIOWriteData(0x00);
	QspiSingleIOWriteData(dat);
	QspiSingleIOWriteData(0x00);
//        QSPI_RS_Set();//写数据
}

static void write_cmd(uint8_t cmd) {
	QspiWrComm(cmd);
}
static void write_dat(uint8_t data) {
	QspiSingleIOWriteData(data);
}

void Init_LCD(void)
{
     QSPI_DA0_Clr();
     QSPI_DA1_Set();
     QSPI_DA2_Set();
     QSPI_DA3_Set();
     QSPI_PWR_EN_Set(); 
     //QSPI_RS_Set();
     QSPI_BLK_Clr(); 
			QSPI_CS_Clr();	
     QSPI_CLK_Clr();
	
QSPI_RES_Set();	Delay_Ms(50);
QSPI_RES_Clr();	Delay_Ms(50);
QSPI_RES_Set();	Delay_Ms(120);
	 

//------end Reset Sequence-----//

write_cmd(0xfe);	
write_cmd(0xef);
write_cmd(0x80);	
write_dat(0x11);
write_cmd(0x81);	
write_dat(0x70);//
write_cmd(0x82);	
write_dat(0x09);	
write_cmd(0x83);	
write_dat(0x03);	
write_cmd(0x84);	
write_dat(0x62);//
write_cmd(0x89);	
write_dat(0x18);	
write_cmd(0x8A);	
write_dat(0x40);
write_cmd(0x8B);	
write_dat(0x0A);


	
write_cmd(0x3a);//像素格式设置	
write_dat(0x05);//16位像素
write_cmd(0x36);//对应帧内存的读写扫描方向
write_dat(0x40);
write_cmd(0xEC);//设置反向显示模式
write_dat(0x07);
		
		
write_cmd(0x74);//
write_dat(0x01);
write_dat(0x80);
write_dat(0x00);
write_dat(0x00);
write_dat(0x00);
write_dat(0x00);
	
write_cmd(0x98);//y梯度调整寄存器为正极性
write_dat(0x3E);
write_cmd(0x99);//
write_dat(0x3E);

				
write_cmd(0xA1);//
write_dat(0x01);
write_dat(0x04);
	
write_cmd(0xA2);//
write_dat(0x01);
write_dat(0x04);
	
write_cmd(0xCB);//
write_dat(0x02);
	

	
write_cmd(0x7C);//
write_dat(0xB6);
write_dat(0x24);
	
write_cmd(0xAC);//
write_dat(0x74);


write_cmd(0xF6);//
write_dat(0x80);


write_cmd(0xB5);//
write_dat(0x09);
write_dat(0x09);
	

write_cmd(0xEB);//
write_dat(0x01); 
write_dat(0x81); //81



write_cmd(0x60);//
write_dat(0x38); 
write_dat(0x06);//0c
write_dat(0x13); 
write_dat(0x56);

write_cmd(0x63);//
write_dat(0x38); 
write_dat(0x08); //e
write_dat(0x13); 
write_dat(0x56);

write_cmd(0x61);//度显示状态
write_dat(0x3B); 
write_dat(0x1b); 
write_dat(0x58); 
write_dat(0x38);

write_cmd(0x62);//
write_dat(0x3B); 
write_dat(0x1b); 
write_dat(0x58); 
write_dat(0x38);







write_cmd(0x64);//
write_dat(0x38); 
write_dat(0x0a); 
write_dat(0x73); 
write_dat(0x16); 
write_dat(0x13); 
write_dat(0x56);

write_cmd(0x66);//
write_dat(0x38);
write_dat(0x0b); 
write_dat(0x73); 
write_dat(0x17); 
write_dat(0x13); 
write_dat(0x56); 




write_cmd(0x68);
write_dat(0x00); 
write_dat(0x0B);
write_dat(0x22);
write_dat(0x0B); 
write_dat(0x22); 
write_dat(0x1C);
write_dat(0x1C); 

write_cmd(0x69);
write_dat(0x00); 
write_dat(0x0B); 
write_dat(0x26);
write_dat(0x0B); 
write_dat(0x26); 
write_dat(0x1C); 
write_dat(0x1C); 









write_cmd(0x6A);
write_dat(0x15); 
write_dat(0x00); 

write_cmd(0x6E);
write_dat(0x08); 
write_dat(0x02); 
write_dat(0x1a); 
write_dat(0x00); 
write_dat(0x12); 
write_dat(0x12); 
write_dat(0x11); 
write_dat(0x11); 
write_dat(0x14); 
write_dat(0x14); 
write_dat(0x13); 
write_dat(0x13); 
write_dat(0x04); 
write_dat(0x19); 
write_dat(0x1e); 
write_dat(0x1d); 
write_dat(0x1d); 
write_dat(0x1e); 
write_dat(0x19); 
write_dat(0x04); 
write_dat(0x0b); 
write_dat(0x0b); 
write_dat(0x0c); 
write_dat(0x0c); 
write_dat(0x09); 
write_dat(0x09); 
write_dat(0x0a); 
write_dat(0x0a); 
write_dat(0x00); 
write_dat(0x1a);
write_dat(0x01); 
write_dat(0x07); 

write_cmd(0x6C);
write_dat(0xCC); 
write_dat(0x0C); 
write_dat(0xCC); 
write_dat(0x84); 
write_dat(0xCC); 
write_dat(0x04); 
write_dat(0x50); 


write_cmd(0x7D);
write_dat(0x72);





write_cmd(0x70);
write_dat(0x02);
write_dat(0x03);
write_dat(0x09);
write_dat(0x07);
write_dat(0x09);
write_dat(0x03);
write_dat(0x09);
write_dat(0x07);
write_dat(0x09); 
write_dat(0x03);

write_cmd(0x90);
write_dat(0x06);
write_dat(0x06);
write_dat(0x05);
write_dat(0x06);

write_cmd(0x93);
write_dat(0x45);
write_dat(0xFF);
write_dat(0x00);


write_cmd(0xC3);
write_dat(0x15);
write_cmd(0xC4);
write_dat(0x36);
write_cmd(0xC9);
write_dat(0x3d);


write_cmd(0xF0);   
write_dat(0x47);//gVR1_N[5:0]
write_dat(0x07);//gVR2_N[5:0]
write_dat(0x0A);//gVR4_N[4:0]
write_dat(0x0A);//gVR6_N[4:0]
write_dat(0x00);//gVR0_N[3:0] gVR13_N[3:0] 7
write_dat(0x29);//gVR20_N[6:0]

write_cmd(0xF2);   
write_dat(0x47);//gVR1_P[5:0]
write_dat(0x07);//gVR2_P[5:0]
write_dat(0x0a);//gVR4_P[4:0]
write_dat(0x0A);//gVR6_P[4:0]
write_dat(0x00);//gVR0_P[3:0] gVR13_P[3:0]
write_dat(0x29);//gVR20_P[6:0]

write_cmd(0xF1);    
write_dat(0x42);//gVR43_N[6:0]
write_dat(0x91);//gVR27_N[2:0] gVR57_N[4:0]
write_dat(0x10);//gVR36_N[2:0] gVR59_N[4:0]
write_dat(0x2D);//gVR61_N[5:0]
write_dat(0x2F);// gVR62_N[5:0]
write_dat(0x6F);//gVR50_N[3:0] gVR63_N[3:0]

write_cmd(0xF3);   
write_dat(0x42);//gVR43_P[6:0]
write_dat(0x91);//gVR27_P[2:0] gVR57_P[4:0]
write_dat(0x10);//gVR36_P[2:0] gVR59_P[4:0]
write_dat(0x2D);//gVR61_P[5:0]
write_dat(0x2F);// gVR62_P[5:0]
write_dat(0x6F);//gVR50_P[3:0] gVR63_P[3:0]




write_cmd(0xF9);   
write_dat(0x30);

write_cmd(0xBE);
write_dat(0x11); 

write_cmd(0xFB);
write_dat(0x00); 
write_dat(0x00); 

/*
//write_cmd(0xB1);   
//write_dat(0x08);    // SPI_ 2dat 
*/

write_cmd(0x11);//退出睡眠
  Delay_Ms(120);	 
write_cmd(0x29);//开启显示
  Delay_Ms(120);	
write_cmd(0x2C);//
  write_dat(0x00);
  write_dat(0x00);
  write_dat(0x00);
  write_dat(0x00);
write_cmd(0x2C);
  write_dat(0x00);
  write_dat(0x00);
  write_dat(0x00);
  write_dat(0x00);
Delay_Ms(120);

	
}

void BlockWrite(uint16_t Xstart,uint16_t Xend,uint16_t Ystart,uint16_t Yend) {
	uint16_t i,j;   
	
	//Xstart:0 Xend:320
  write_cmd(0x2a);   
	write_dat(Xstart>>8);
	write_dat(Xstart&0xff);
	write_dat(Xend>>8);
	write_dat(Xend&0xff);

	//Ystart:0 Yend:320
	write_cmd(0x2b);   
	write_dat(Ystart>>8);
	write_dat(Ystart&0xff);
	write_dat(Yend>>8);
	write_dat(Yend&0xff);

	write_dat(0x12);
	write_dat(0x00);
	write_dat(0x2c);//储存器写
	write_dat(0x00);
	
	for(i=0; i<320; i++){
		for(j=0; j<320; j++){
			QspiQuadIOWriteData((uint8_t)(0xEF5D>>8));
			QspiQuadIOWriteData((uint8_t)(0xEF5D));
		}
	}
}

void DispColor(unsigned int color)
{
     
    BlockWrite(0,320,0,320);

//    QspiSingleIOWriteData(0x12);
//    QspiQuadIOWriteData(0x00);
//    QspiQuadIOWriteData(0x2c);
//    QspiQuadIOWriteData(0x00);
    
		
    
}



















#if 0
/*
**********************************************************************************
 *Function:     Nv3030bQspiAddressSet
 *Description:  设置起始和结束地址
 *Calls:       
 *Called By:   
 *Input:        x1,x2 设置列的起始和结束地址
                    y1,y2 设置行的起始和结束地址
 *Output:      
 *Return:      
 *Others:       
*********************************************************************************
*/
void QspiAddressSet(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
	QSPI_CS_Clr();
	QspiWrComm(0x2a);//列地址设置
	QspiWrData16(SINGLE_LINE,x1);
	QspiWrData16(SINGLE_LINE,x2);
	QSPI_CS_Set();

	QSPI_CS_Clr();
	QspiWrComm(0x2b);//行地址设置
	QspiWrData16(SINGLE_LINE,y1);
	QspiWrData16(SINGLE_LINE,y2);
	QSPI_CS_Set();

	QSPI_CS_Clr();
	QspiWrComm(0x2c);//储存器写
	QSPI_CS_Set();
}

/*
**********************************************************************************
 *Function:     Nv3030BInit
 *Description:  Nv3030B初始化
 *Calls:       
 *Called By:   
 *Input:        
 *Output:      
 *Return:      
 *Others:       
*********************************************************************************
*/
void Nv3030BInit(void)
{
    NV3030BGpioInit();
    
    QSPI_CS_Clr();
    QSPI_RES_Set();
    nrf_delay_ms(10);
    QSPI_RES_Clr();
    nrf_delay_ms(100);
    QSPI_RES_Set();
    nrf_delay_ms(100);
      
    QspiWrComm(0xFD); //私有访问
    QspiSingleIOWrData8(0x06);    //进入私有寄存器模式
    QspiSingleIOWrData8(0x08);
    QSPI_CS_Set();
    nrf_delay_ms(1);
    
    //QspiWrComm(0x60);         //时钟频率调整
    //QspiQuadIOWrData8(0x03);
    //QspiQuadIOWrData8(0x01);
    //QspiQuadIOWrData8(0x01);
    QSPI_CS_Clr();
    QspiWrComm(0x61);//VDD设置
    QspiSingleIOWrData8(0x06);   //唤醒时VDD1.68
    QspiSingleIOWrData8(0x04);   //睡眠时VDD1.29
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0x62);//
    QspiSingleIOWrData8(0x00);
    QspiSingleIOWrData8(0x44);
    QspiSingleIOWrData8(0x40);
    //QspiQuadIOWrData8(0x03);
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0x63);//
    QspiSingleIOWrData8(0x41);
    QspiSingleIOWrData8(0x07);
    QspiSingleIOWrData8(0x12);
    QspiSingleIOWrData8(0x12);
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0x64);
    QspiSingleIOWrData8(0x37);
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0x65);
    QspiSingleIOWrData8(0x09);
    QspiSingleIOWrData8(0x10);
    QspiSingleIOWrData8(0x21);
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0x66);
    QspiSingleIOWrData8(0x09);
    QspiSingleIOWrData8(0x10);
    QspiSingleIOWrData8(0x21);
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0x67);
    QspiSingleIOWrData8(0x21);
    QspiSingleIOWrData8(0x40);
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0x68);
    QspiSingleIOWrData8(0x5d);
    QspiSingleIOWrData8(0x4c);//
    QspiSingleIOWrData8(0x2c);//
    QspiSingleIOWrData8(0x1c);//
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0xB1);
    QspiSingleIOWrData8(0x0f); //0c
    QspiSingleIOWrData8(0x02); //00
    QspiSingleIOWrData8(0x07); //01
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0xB4);
    QspiSingleIOWrData8(0x02);
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0xB5);
    QspiSingleIOWrData8(0x02);
    QspiSingleIOWrData8(0x02);//
    QspiSingleIOWrData8(0x0a);//
    QspiSingleIOWrData8(0x14);//
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0xb6);
    QspiSingleIOWrData8(0x44);
    QspiSingleIOWrData8(0x01);
    QspiSingleIOWrData8(0x9f);
    QspiSingleIOWrData8(0x00);
    QspiSingleIOWrData8(0x02);
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0xdf);
    QspiSingleIOWrData8(0x11);
    QSPI_CS_Set();

    ///NV3030A2 GAMMA/
    QSPI_CS_Clr();
    QspiWrComm(0xe0); //gmama set 2.2
    QspiSingleIOWrData8(0x01); //PKP0[4:0]
    QspiSingleIOWrData8(0x06); //PKP1[4:0]
    QspiSingleIOWrData8(0x0e); //PKP2[4:0]
    QspiSingleIOWrData8(0x10); //PKP3[4:0]
    QspiSingleIOWrData8(0x0e); //PKP4[4:0]
    QspiSingleIOWrData8(0x0c); //PKP5[4:0]
    QspiSingleIOWrData8(0x0a); //PKP6[4:0]
    QspiSingleIOWrData8(0x16); //PKP6[4:0]
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0xe3);
    QspiSingleIOWrData8(0x16); //PKN0[4:0]
    QspiSingleIOWrData8(0x13); //PKN1[4:0]
    QspiSingleIOWrData8(0x14); //PKN2[4:0]
    QspiSingleIOWrData8(0x10); //PKN3[4:0]
    QspiSingleIOWrData8(0x0f); //PKN4[4:0]
    QspiSingleIOWrData8(0x0f); //PKN5[4:0]
    QspiSingleIOWrData8(0x04); //PKN6[4:0]
    QspiSingleIOWrData8(0x01); //PKN6[4:0]
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0xe1);
    QspiSingleIOWrData8(0x14); //PRP0[6:0]
    QspiSingleIOWrData8(0x68); //PRP1[6:0]
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0xe4);
    QspiSingleIOWrData8(0x68); //PRN0[6:0]
    QspiSingleIOWrData8(0x14); //PRN1[6:0]
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0xe2);
    QspiSingleIOWrData8(0x00); //VRP0[5:0]
    QspiSingleIOWrData8(0x0a); //VRP1[5:0]
    QspiSingleIOWrData8(0x09); //VRP2[5:0]
    QspiSingleIOWrData8(0x30); //VRP3[5:0]
    QspiSingleIOWrData8(0x39); //VRP4[5:0]
    QspiSingleIOWrData8(0x3f); //VRP5[5:0]
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0xe5);
    QspiSingleIOWrData8(0x3f); //VRN0[5:0]
    QspiSingleIOWrData8(0x33); //VRN1[5:0]
    QspiSingleIOWrData8(0x28); //VRN2[5:0]
    QspiSingleIOWrData8(0x09); //VRN3[5:0]
    QspiSingleIOWrData8(0x0a); //VRN4[5:0]
    QspiSingleIOWrData8(0x00); //VRN5[5:0]
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0xe6);
    QspiSingleIOWrData8(0x00);
    QspiSingleIOWrData8(0xff);
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0xe7);
    QspiSingleIOWrData8(0x01);
    QspiSingleIOWrData8(0x04);
    QspiSingleIOWrData8(0x03);
    QspiSingleIOWrData8(0x03);
    QspiSingleIOWrData8(0x00);
    QspiSingleIOWrData8(0x12);
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0xE8);
    QspiSingleIOWrData8(0x00);
    QspiSingleIOWrData8(0x60);
    QspiSingleIOWrData8(0x00);
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0xec);
    QspiSingleIOWrData8(0x62);
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0xf1);
    QspiSingleIOWrData8(0x00);
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0xfd); //私有访问
    QspiSingleIOWrData8(0xfa);    //退出私有访问
    QspiSingleIOWrData8(0xfc);
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0x3a);
    QspiSingleIOWrData8(0x55);
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0x35);
    QspiSingleIOWrData8(0x00);
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0x11); // exit sleep
    nrf_delay_ms(200);
    QSPI_CS_Set();

    QSPI_CS_Clr();
    QspiWrComm(0x29); // display on
    nrf_delay_ms(10);
    QSPI_CS_Set();
}

/*
**********************************************************************************
 *Function:     DispColor
 *Description:  用统一颜色刷屏
 *Calls:       
 *Called By:   
 *Input:        
 *Output:      
 *Return:      
 *Others:       
*********************************************************************************
*/
void DispColor(unsigned int color)
{
    unsigned int i,j;    
    QspiAddressSet(0,0,240,320);

    QSPI_CS_Clr();
    QspiSingleIOWriteData(0x12);
    QspiQuadIOWriteData(0x00);
    QspiQuadIOWriteData(0x2c);
    QspiQuadIOWriteData(0x00);
    
    for(i=0; i<320; i++)
          for(j=0; j<240; j++){
                    QspiWrData16(QUAD_LINE,color);
                }
    QSPI_CS_Set();
}
#endif

