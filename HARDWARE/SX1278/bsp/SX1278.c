#include "SX1278.h"
#include "spi.h"
#include "delay.h"
#include "platform.h"
#include "radio.h"
#include "sx1276.h"
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

const u8 TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ
const u8 RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ

//���SX1278�޸�SPI2����
void SX1278_SPI_Init(void)
{
    __HAL_SPI_DISABLE(&SPI2_Handler);               //�ȹر�SPI2
    SPI2_Handler.Init.CLKPolarity=SPI_POLARITY_LOW; //����ͬ��ʱ�ӵĿ���״̬Ϊ�͵�ƽ
    SPI2_Handler.Init.CLKPhase=SPI_PHASE_1EDGE;     //����ͬ��ʱ�ӵĵ�1�������أ��������½������ݱ�����
    HAL_SPI_Init(&SPI2_Handler);
    __HAL_SPI_ENABLE(&SPI2_Handler);                //ʹ��SPI2
}

//��ʼ��sx1278��IO��
void SX1278_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOE_CLK_ENABLE();			//����GPIOGʱ��  �ĳ�PE2,4  cs pe2  reset pe4
    
    GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_4; //PE2,4 cs / rest
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);     //��ʼ��
    
    SPI2_Init();    		                //��ʼ��SPI2  
    SX1278_SPI_Init();                //���NRF���ص��޸�SPI������
	  SX1278_CE=0; //ʹ��LORA SX1278 ʹ��Ӧ����reset��λ1  ��  �޸�   echo modified
	  SX1278_CSN=1;			                //SPIƬѡȡ��	 	
}
//���24L01�Ƿ����
//����ֵ:0���ɹ�;1��ʧ��	
u8 SX1278_Check(void)
{
	u8 buf[5]={0X00,0XA5,0XA5,0XA5,0XA5};

	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_64); //spi�ٶ�Ϊ11.25Mhz��24L01�����SPIʱ��Ϊ10Mhz,�����һ��û��ϵ��    	 
	
	SX1278_Read_Buf(0x06,buf,1); //����д��ĵ�ַ  
	if(buf[0]!=0x6C)  return 1;//���24L01����	
	return 0;		 //��⵽24L01
}	 	 
//SPIд�Ĵ���
//reg:ָ���Ĵ�����ַ
//value:д���ֵ
u8 SX1278_Write_Reg(u8 reg,u8 value)
{
	  u8 status;	
   	SX1278_CSN=0;                 //ʹ��SPI����
  	status =SPI2_ReadWriteByte(reg);//���ͼĴ����� 
  	SPI2_ReadWriteByte(value);      //д��Ĵ�����ֵ
  	SX1278_CSN=1;                 //��ֹSPI����	   
  	return(status);       		    //����״ֵ̬
}
//��ȡSPI�Ĵ���ֵ
//reg:Ҫ���ļĴ���
u8 SX1278_Read_Reg(u8 reg)
{
	  u8 reg_val;	    
   	SX1278_CSN=0;             //ʹ��SPI����		
  	SPI2_ReadWriteByte(reg);    //���ͼĴ�����
  	reg_val=SPI2_ReadWriteByte(0XFF);//��ȡ�Ĵ�������
  	SX1278_CSN=1;             //��ֹSPI����		    
  	return(reg_val);            //����״ֵ̬
}	
//��ָ��λ�ö���ָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ 
u8 SX1278_Read_Buf(u8 reg,u8 *pBuf,u8 len)
{
	  u8 status,u8_ctr;	       
  	SX1278_CSN=0;            //ʹ��SPI����
  	status=SPI2_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   	   
 	  for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPI2_ReadWriteByte(0XFF);//��������
  	SX1278_CSN=1;            //�ر�SPI����
  	return status;             //���ض�����״ֵ̬
}
//��ָ��λ��дָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ
u8 SX1278_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
	  u8 status,u8_ctr;	    
 	  SX1278_CSN=0;             //ʹ��SPI����
  	status = SPI2_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
  	for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI2_ReadWriteByte(*pBuf++); //д������	 
  	SX1278_CSN=1;             //�ر�SPI����
  	return status;              //���ض�����״ֵ̬
}				   
//����SX1278����һ������
//txbuf:�����������׵�ַ
//����ֵ:�������״��
u8 SX1278_TxPacket(u8 *txbuf)
{
 	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_8); //spi�ٶ�Ϊ6.75Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
				
	return 0xff;//����ԭ����ʧ��
}
//����SX1278����һ������
//txbuf:�����������׵�ַ
//����ֵ:0��������ɣ��������������
u8 SX1278_RxPacket(u8 *rxbuf)
{	    							   
	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_8); //spi�ٶ�Ϊ6.75Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
 
	return 1;//û�յ��κ�����
}					    
//�ú�����ʼ��SX1278��RXģʽ
//����RX��ַ,дRX���ݿ��,ѡ��RFƵ��,�����ʺ�LNA HCURR
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
void SX1278_RX_Mode(void)
{
	  SX1278_CE=0;	  

  	SX1278_CE=1; //CEΪ��,�������ģʽ 
}						 
//�ú�����ʼ��SX1278��TXģʽ
//����TX��ַ,дTX���ݿ��,����RX�Զ�Ӧ��ĵ�ַ,���TX��������,ѡ��RFƵ��,�����ʺ�LNA HCURR
//PWR_UP,CRCʹ��
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
//CEΪ�ߴ���10us,����������.	 
void SX1278_TX_Mode(void)
{														 
	SX1278_CE=0;	    

	SX1278_CE=1;//CEΪ��,10us����������
}
