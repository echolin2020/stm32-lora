#ifndef __SX1278_H
#define __SX1278_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//SX1278驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/1/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//24L01操作线
#define SX1278_CE     PEout(4)  //24L01片选信号 lora reset
#define SX1278_CSN    PEout(2)  //SPI片选信号	 lora cs

//24L01发送接收数据宽度定义
#define TX_ADR_WIDTH    5   	//5字节的地址宽度
#define RX_ADR_WIDTH    5   	//5字节的地址宽度
#define TX_PLOAD_WIDTH  32  	//32字节的用户数据宽度
#define RX_PLOAD_WIDTH  32  	//32字节的用户数据宽度
									   	   

void SX1278_Init(void);//初始化
void SX1278_RX_Mode(void);//配置为接收模式
void SX1278_TX_Mode(void);//配置为发送模式
u8 SX1278_Write_Buf(u8 reg, u8 *pBuf, u8 u8s);//写数据区
u8 SX1278_Read_Buf(u8 reg, u8 *pBuf, u8 u8s);//读数据区		  
u8 SX1278_Read_Reg(u8 reg);			//读寄存器
u8 SX1278_Write_Reg(u8 reg, u8 value);//写寄存器
u8 SX1278_Check(void);//检查24L01是否存在
u8 SX1278_TxPacket(u8 *txbuf);//发送一个包的数据
u8 SX1278_RxPacket(u8 *rxbuf);//接收一个包的数据

#endif
