#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
//#include "lcd.h"
//#include "sdram.h"
#include "SX1278.h"
#include "SX1276.h"
#include "timer.h"
//SX1278
#include "platform.h"
#include "radio.h"
#include "sx1276.h"
#include "stdio.h"
#include "stm32f4xx_hal_rcc.h"
#include "sx12xxEiger.h"
#include "sx1276-LoRaMisc.h"
#include "sx1276-LoRa.h"
/************************************************
这是第一次在keil里修改文件，尝试用atom自动识别，然后push到github
************************************************/

#define BUFFER_SIZE     30                          // Define the payload size here

static uint16_t BufferSize = BUFFER_SIZE;			// RF buffer size
static uint8_t  Buffer[BUFFER_SIZE];				// RF buffer
tRadioDriver *Radio = NULL;
const uint8_t MY_TEST_Msg[] = "SX1278_TEST";
uint32_t aa,bb,cc,dd;
void EXTI_Init(void);
__IO bool isCAD;
extern  uint8_t RFLRState;

int main(void)
{
	
	double myrssi;
	u8 key,mode;
	u16 t=0,i=0;	

	uint8_t txN=0;	
	char sendBuf[20];	 
	HAL_Init();                     //初始化HAL库   
	Stm32_Clock_Init(336,8,2,7);   //设置时钟,168Mhz
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);//echo added
	BoardInit();								//echo added
	TIM3_Init(10-1,8400-1);   			//1ms lora用 林 修改 echo modified
	uart_init(115200);              //初始化USART	
	delay_init(168);                //初始化延时函数 echo modified
	delay_ms(10);
	LED_Init();                     //初始化LED 

	KEY_Init();                     //初始化按键
	SX1278_Init();    		    		  //初始化SX1278 
	
	
	SX1276Reset();	
	while(SX1278_Check())
	{
		//LCD_ShowString(30,130,200,16,16,"lora Error");
		delay_ms(200);
		//LCD_Fill(30,130,239,130+16,WHITE);
 		delay_ms(200);
	}
	//LCD_ShowString(30,130,200,16,16,"lora OK"); 	
	Radio = RadioDriverInit();
	Radio->Init();	
 	while(1)
	{	
		key=KEY_Scan(0);
		if(key==KEY0_PRES)
		{
			mode=0;   
			break;
		}else if(key==KEY1_PRES)
		{
			mode=1;
			break;
		}
		t++;
		//if(t==100)LCD_ShowString(10,150,230,16,16,"KEY0:RX_Mode  KEY1:TX_Mode"); //闪烁显示提示信息
 		if(t==200)
		{	
			//LCD_Fill(10,150,230,150+16,WHITE);
			t=0; 
		}
		delay_ms(5);	  
	}   
 	//LCD_Fill(10,150,240,166,WHITE);//清空上面的显示		  
 	//POINT_COLOR=BLUE;//设置字体为蓝色	  
		mode = 0; //echo added 为了调试
	if(mode==0)//RX模式
	{
		//LCD_ShowString(30,150,200,16,16,"lora RX_Mode");	
		//LCD_ShowString(30,170,200,16,16,"Received DATA:");	
		printf("lora RX_Mode\r\n");
		Radio->StartRx();   //RFLR_STATE_RX_INIT  
		
		SX1276LoRaStartCAD();  //RFLRState = RFLR_STATE_CAD_INIT;
		SX1276Write( REG_LR_IRQFLAGS, 0xFF );
		__HAL_GPIO_EXTI_CLEAR_IT(EXTI1_IRQn);
		EXTI_Init();
		
		while(1)
		{	  		    		    				 
			while( Radio->Process( ) == RF_RX_DONE)//一旦接收到信息,则显示出来.
			{			
				Radio->GetRxPacket( Buffer, ( uint16_t* )&BufferSize );			
				if( strlen(Buffer) != 0 )
				{
					myrssi = SX1276ReadRssi();
					printf("Rssi = %12.6f\r\n", myrssi);
					
					Buffer[strlen(Buffer)]=0;//加入字符串结束符
					printf("LoRa RX: %s\r\n",Buffer);		
					//LCD_ShowString(0,190,lcddev.width-1,32,16,Buffer);   					
					LED0=!LED0;
					for(i=0;i<BUFFER_SIZE;i++)
						Buffer[i] = 0;
				}
				//Radio->StartRx( );	
				SX1276LoRaStartCAD();				
			} 				    
		}
	}else//TX模式
	{							    
		//LCD_ShowString(30,150,200,16,16,"lora TX_Mode");	
		Radio->SetTxPacket( MY_TEST_Msg, 11 );
		mode=' ';//从空格键开始  
		while(1)
		{	  		   				 
			while(Radio->Process( ) == RF_TX_DONE)
			{
				sprintf(sendBuf,"SX1278_TEST_%d",txN++);
				//LCD_Fill(0,170,lcddev.width,170+16*3,WHITE);//清空显示
				//LCD_ShowString(30,170,239,32,16,"Sended DATA:");	
				//LCD_ShowString(0,190,lcddev.width-1,32,16,sendBuf); 
				printf("LoRa TX: %s\r\n",sendBuf);
				LED1=!LED1;
				delay_ms(500);
				Radio->SetTxPacket( sendBuf, strlen(sendBuf) );   //RFLR_STATE_TX_INIT
			}					    
		};
	} 
}

//外部中断初始化
void EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;    
    __HAL_RCC_GPIOC_CLK_ENABLE();               //开启GPIOC时钟
	
		GPIO_Initure.Pin=GPIO_PIN_3;            //PC3
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //下拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
	
	  GPIO_Initure.Pin=GPIO_PIN_1;            //PC1
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //下拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
	

    
    GPIO_Initure.Pin=GPIO_PIN_1;                //PC1
    GPIO_Initure.Mode=GPIO_MODE_IT_RISING;      //上升沿触发
    GPIO_Initure.Pull=GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
	

	
	    //中断线1-PC1
    HAL_NVIC_SetPriority(EXTI1_IRQn,2,0);       //抢占优先级为2，子优先级为0
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);             //使能中断线1
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    delay_ms(100);      //消抖
		LED2=!LED2;
		isCAD=1;
}
