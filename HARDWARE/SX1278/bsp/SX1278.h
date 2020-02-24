#ifndef __SX1278_H
#define __SX1278_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F429������
//SX1278��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/16
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//24L01������
#define SX1278_CE     PEout(4)  //24L01Ƭѡ�ź� lora reset
#define SX1278_CSN    PEout(2)  //SPIƬѡ�ź�	 lora cs

//24L01���ͽ������ݿ�ȶ���
#define TX_ADR_WIDTH    5   	//5�ֽڵĵ�ַ���
#define RX_ADR_WIDTH    5   	//5�ֽڵĵ�ַ���
#define TX_PLOAD_WIDTH  32  	//32�ֽڵ��û����ݿ��
#define RX_PLOAD_WIDTH  32  	//32�ֽڵ��û����ݿ��
									   	   

void SX1278_Init(void);//��ʼ��
void SX1278_RX_Mode(void);//����Ϊ����ģʽ
void SX1278_TX_Mode(void);//����Ϊ����ģʽ
u8 SX1278_Write_Buf(u8 reg, u8 *pBuf, u8 u8s);//д������
u8 SX1278_Read_Buf(u8 reg, u8 *pBuf, u8 u8s);//��������		  
u8 SX1278_Read_Reg(u8 reg);			//���Ĵ���
u8 SX1278_Write_Reg(u8 reg, u8 value);//д�Ĵ���
u8 SX1278_Check(void);//���24L01�Ƿ����
u8 SX1278_TxPacket(u8 *txbuf);//����һ����������
u8 SX1278_RxPacket(u8 *rxbuf);//����һ����������

#endif
