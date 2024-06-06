#include "../Tools/base64.h"

const char*Base64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";  //Base64编码对照表
/********************************************************************
函数原型:GencBase64
功   能:base64编码函数
输入参数:indata: 需要编码数据
        inlen : 编码数据长度
        outdata：编码后输出数据
输   出：> 0 编码后输出数据长度 < 0 需要编码的数据里面有错误数据(实际不会有这种情况，所有数据都可以编码，都可以转换)
说   明:无
*--------------------------------------------------------------------
*修改时间		|	修改者		|	备注
*--------------------------------------------------------------------
*2018-04-23		        liuchunjie	        First
********************************************************************/
int16_t GencBase64(unsigned char* indata, uint16_t inlen,unsigned char *outdata)
{
    uint16_t i,j;
	//1、首先3字节组成1组，然后转换成4字节1组
	for(i = 0,j = 0;i < (inlen/3)*3;i = i + 3,j = j + 4)
	{
		outdata[j] = 0;    //清除要做操作缓冲，其实这里没必要，可以去掉
		outdata[j + 1] = 0;
		outdata[j + 2] = 0;
		outdata[j + 3] = 0;
		outdata[j] = indata[i] >> 2;
		outdata[j + 1] = (((indata[i]&0x03) << 4) | (indata[i + 1] >> 4));
		outdata[j + 2] = (((indata[i + 1]&0x0F) << 2) | ((indata[i + 2]&0xC0) >> 6));
		outdata[j + 3] = (indata[i + 2]&0x3F);
	}
	//2、其次对照Base64编码表，转换成对应字符编码
	for(i = 0;i < j;i++)
	{
		outdata[i] = Base64[outdata[i]];
	}
	if((inlen%3) == 1)//说明最后还有1个字节剩余，剩余1个字节，最后转换成Base64编码时最后要补2个'='字符，形成4字节1组，这样最后编码数据就是4的倍数
	{
		outdata[i] = 0;    //清除要操作缓冲
		outdata[i + 1] = 0;
		outdata[i + 2] = 0;
		outdata[i + 3] = 0;
		outdata[i] = indata[inlen - 1] >> 2;
		outdata[i + 1] = ((indata[inlen - 1]&0x03) << 4);
		outdata[i] = Base64[outdata[i]];
		outdata[i + 1] = Base64[outdata[i + 1]];
		outdata[i + 2] = '=';
		outdata[i + 3] = '=';
		i = i + 4;
	}
	if((inlen%3) == 2)//说明最后还有2个字节剩余，剩余2个字节，最后转换成Base64编码时最后要补1个'='字，形成4字节1组，这样最后编码数据就是4的倍数
	{
		outdata[i] = 0;    //清除要操作缓冲
		outdata[i + 1] = 0;
		outdata[i + 2] = 0;
		outdata[i + 3] = 0;
		outdata[i] = indata[inlen - 2] >> 2;
		outdata[i + 1] = (((indata[inlen - 2]&0x03) << 4) | (indata[inlen - 1] >> 4));
		outdata[i + 2] = (indata[inlen - 1]&0x0F) << 2;
		outdata[i] = Base64[outdata[i]];
		outdata[i + 1] = Base64[outdata[i + 1]];
		outdata[i + 2] = Base64[outdata[i + 2]];
		outdata[i + 3] = '=';
		i = i + 4;
	}
	return i;   //返回编码后数据总长度 
}
/********************************************************************
函数原型:GdecBase64
功   能:base64解码函数
输入参数:indata: 需要解码数据
        inlen : 解码数据长度
        outdata：解码后输出数据
输   出: > 0 解码后输出数据长度 < 0 需要解码的数据里面有错误数据(实际有可能有这种情况，比如为了测试程序的健壮性，可以添加一些错误数据在里面，这里所谓错误数据就是不在base64编码对照表里面数据，比如'('字符，就是不合法数据)
说   明: 解码函数是用来解码接收数据的，所以你不可能保证接收数据里面都是正确的
注   意：传输数据为了能够保证传输数据是4的倍数，后面会补充1个或者2个'='
*--------------------------------------------------------------------
修改时间			|	修改者		|	备注
*--------------------------------------------------------------------
*2018-04-23			liuchunjie	        First
********************************************************************/
static uint8_t base64buff[2048];
int16_t GdecBase64(unsigned char* indata, uint16_t inlen,unsigned char *outdata)
{
  uint16_t i,j;
	uint16_t tlength = 0;
	
	//1、首先去掉最后1个或者2个'='字符，最多就是2个
	for(i = 0;i < 2;i++)
	{
		if(indata[inlen - 1 - i] == '=')
 		{
		    inlen--;
		}
                else
	        {
		    break;
	        }
	}
	for(i = 0;i < inlen;i++)
	{
		for(j = 0;j < 64;j++)
		{
			if(*(indata + i) == *(Base64 + j))
			{
				break;
			}
		}
		if(j == 64)//传输数据里面有错误数据，数据没在base64编码对照表里面
		{
			return -1;
		}
		base64buff[i] = j;//找对对应索引值保存起来，为后面合成数据
	}
	//判断数据是否够4的倍数，不够补齐，补0x00
	if((i % 4) == 1)//说明数据最后剩余1数据，所以需要补充3个0x00,组成4的倍数
	{
	    base64buff[i] = 0x00;
		base64buff[i + 1] = 0x00;
		base64buff[i + 2] = 0x00;
		i = i + 3;
	}
	if((i % 4) == 2)//说明数据最后剩余2数据，所以需要补充2个0x00,组成4的倍数
	{
		base64buff[i] = 0x00;
		base64buff[i + 1] = 0x00;
		i = i + 2;
	}
	if((i % 4) == 3)//说明数据最后剩余3数据，所以需要补充1个0x00,组成4的倍数
	{
		base64buff[i] = 0x00;
		i = i + 1;
  }
    for(j = 0,tlength = 0;j < i;j = j + 4,tlength = tlength + 3)//上面数据已经组成4的倍数了，这样转换后的数据一定是3的倍数，这样解码后数据可能比原来多几个0x00数据，如果你不希望多出几个0x00,其实你也可以不组成4的倍数，我这里主要是为了方便解码
    {
        outdata[tlength] = (base64buff[j] << 2) | ((base64buff[j + 1]&0x30) >> 4);
        outdata[tlength + 1] = (((base64buff[j + 1]&0x0F) << 4) | ((base64buff[j + 2]&0x3C) >> 2));
        outdata[tlength + 2] = (((base64buff[j + 2]&0x03) << 6) | base64buff[j + 3]);		
    }
		
    return inlen/4*3 +(3&(inlen-1))%3;
}
















