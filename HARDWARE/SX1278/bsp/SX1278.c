#include "SX1278.h"
#include "spi.h"
#include "delay.h"
#include "platform.h"
#include "radio.h"
#include "sx1276.h"
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

const u8 TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址
const u8 RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址

//针对SX1278修改SPI2驱动
void SX1278_SPI_Init(void)
{
    __HAL_SPI_DISABLE(&SPI2_Handler);               //先关闭SPI2
    SPI2_Handler.Init.CLKPolarity=SPI_POLARITY_LOW; //串行同步时钟的空闲状态为低电平
    SPI2_Handler.Init.CLKPhase=SPI_PHASE_1EDGE;     //串行同步时钟的第1个跳变沿（上升或下降）数据被采样
    HAL_SPI_Init(&SPI2_Handler);
    __HAL_SPI_ENABLE(&SPI2_Handler);                //使能SPI2
}

//初始化sx1278的IO口
void SX1278_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOE_CLK_ENABLE();			//开启GPIOG时钟  改成PE2,4  cs pe2  reset pe4
    
    GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_4; //PE2,4 cs / rest
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);     //初始化
    
    SPI2_Init();    		                //初始化SPI2  
    SX1278_SPI_Init();                //针对NRF的特点修改SPI的设置
	  SX1278_CE=0; //使能LORA SX1278 使能应该让reset置位1  林  修改   echo modified
	  SX1278_CSN=1;			                //SPI片选取消	 	
}
//检测24L01是否存在
//返回值:0，成功;1，失败	
u8 SX1278_Check(void)
{
	u8 buf[5]={0X00,0XA5,0XA5,0XA5,0XA5};

	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_64); //spi速度为11.25Mhz（24L01的最大SPI时钟为10Mhz,这里大一点没关系）    	 
	
	SX1278_Read_Buf(0x06,buf,1); //读出写入的地址  
	if(buf[0]!=0x6C)  return 1;//检测24L01错误	
	return 0;		 //检测到24L01
}	 	 
//SPI写寄存器
//reg:指定寄存器地址
//value:写入的值
u8 SX1278_Write_Reg(u8 reg,u8 value)
{
	  u8 status;	
   	SX1278_CSN=0;                 //使能SPI传输
  	status =SPI2_ReadWriteByte(reg);//发送寄存器号 
  	SPI2_ReadWriteByte(value);      //写入寄存器的值
  	SX1278_CSN=1;                 //禁止SPI传输	   
  	return(status);       		    //返回状态值
}
//读取SPI寄存器值
//reg:要读的寄存器
u8 SX1278_Read_Reg(u8 reg)
{
	  u8 reg_val;	    
   	SX1278_CSN=0;             //使能SPI传输		
  	SPI2_ReadWriteByte(reg);    //发送寄存器号
  	reg_val=SPI2_ReadWriteByte(0XFF);//读取寄存器内容
  	SX1278_CSN=1;             //禁止SPI传输		    
  	return(reg_val);            //返回状态值
}	
//在指定位置读出指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值 
u8 SX1278_Read_Buf(u8 reg,u8 *pBuf,u8 len)
{
	  u8 status,u8_ctr;	       
  	SX1278_CSN=0;            //使能SPI传输
  	status=SPI2_ReadWriteByte(reg);//发送寄存器值(位置),并读取状态值   	   
 	  for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPI2_ReadWriteByte(0XFF);//读出数据
  	SX1278_CSN=1;            //关闭SPI传输
  	return status;             //返回读到的状态值
}
//在指定位置写指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值
u8 SX1278_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
	  u8 status,u8_ctr;	    
 	  SX1278_CSN=0;             //使能SPI传输
  	status = SPI2_ReadWriteByte(reg);//发送寄存器值(位置),并读取状态值
  	for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI2_ReadWriteByte(*pBuf++); //写入数据	 
  	SX1278_CSN=1;             //关闭SPI传输
  	return status;              //返回读到的状态值
}				   
//启动SX1278发送一次数据
//txbuf:待发送数据首地址
//返回值:发送完成状况
u8 SX1278_TxPacket(u8 *txbuf)
{
 	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_8); //spi速度为6.75Mhz（24L01的最大SPI时钟为10Mhz）   
				
	return 0xff;//其他原因发送失败
}
//启动SX1278发送一次数据
//txbuf:待发送数据首地址
//返回值:0，接收完成；其他，错误代码
u8 SX1278_RxPacket(u8 *rxbuf)
{	    							   
	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_8); //spi速度为6.75Mhz（24L01的最大SPI时钟为10Mhz）   
 
	return 1;//没收到任何数据
}					    
//该函数初始化SX1278到RX模式
//设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
//当CE变高后,即进入RX模式,并可以接收数据了		   
void SX1278_RX_Mode(void)
{
	  SX1278_CE=0;	  

  	SX1278_CE=1; //CE为高,进入接收模式 
}						 
//该函数初始化SX1278到TX模式
//设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,选择RF频道,波特率和LNA HCURR
//PWR_UP,CRC使能
//当CE变高后,即进入RX模式,并可以接收数据了		   
//CE为高大于10us,则启动发送.	 
void SX1278_TX_Mode(void)
{														 
	SX1278_CE=0;	    

	SX1278_CE=1;//CE为高,10us后启动发送
}
