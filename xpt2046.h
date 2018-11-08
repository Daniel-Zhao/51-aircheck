#ifndef	  __XPT2046_H_
#define   __XPT2046_H_
//---包含头文件---//
#include<reg52.h>
#include<intrins.h>

//---重定义关键词---//
#ifndef uchar
#define uchar unsigned char
#endif//结束条件编译

#ifndef uint
#define uint  unsigned int
#endif

#ifndef ulong
#define ulong  unsigned long
#endif

//---定义使用的IO口---//
sbit DCLK = P1^0;	  //时钟	 
sbit CS   = P1^1;	  //片选
sbit DIN  = P1^2;	  //输入 数据输入端口
sbit DOUT = P1^3;	  //输出

uint Read_AD_Data(uchar cmd);
uint SPI_Read(void);
void SPI_Write(uchar dat);

#endif
