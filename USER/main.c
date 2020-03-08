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
#include "sx1276-Hal.h"
/************************************************
这是第一次在keil里修改文件，尝试用atom自动识别，然后push到github
************************************************/

#define BUFFER_SIZE     30                          // Define the payload size here

static uint16_t BufferSize = BUFFER_SIZE;			// RF buffer size
static uint8_t  Buffer[BUFFER_SIZE];				// RF buffer
const uint8_t RFBuffer2[]="echo hello world hello lora hello world hello lorahello world hello lora hello world hello lorahello world hello";
uint8_t  BufferTx[BUFFER_SIZE];				// RF buffer
extern uint8_t RFBuffer;
tRadioDriver *Radio = NULL;
const uint8_t MY_TEST_Msg[] = "hello world hello lora hello world hello lorahello world hello lora hello world hello lorahello world hello lora";
uint32_t aa,bb,cc,dd;
void EXTI_Init(void);
__IO bool isCAD;
extern  __IO uint8_t RFLRState;
__IO uint8_t mode;

int main(void)
{
	uint8_t RegValue=0;
	double myrssi;
	u8 key;
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
	EXTI_Init();	
	SX1276Reset();	
//	while(1){
//		__WFI();
//		LED0 = !LED0;
//	}
	
	while(SX1278_Check())
	{
		//LCD_ShowString(30,130,200,16,16,"lora Error");
		printf("lora Error\r\n");
		delay_ms(200);
	}
	//LCD_ShowString(30,130,200,16,16,"lora OK"); 	
	printf("lora OK\r\n");
	Radio = RadioDriverInit();
	Radio->Init();	
	mode = 0; //echo added 为了调试
	Rxinit();//should initialize the receive mode
	while(1)
	{
		if(mode==1){
			mode=0;
			Radio->SetTxPacket( RFBuffer2, 128 );	
			TxInit();
				
		}else{
					SX1276Read(0x12,&RegValue);
					if( (RegValue&0x40) == 0x40 ){//received done
								Rxdoneprocess(RegValue);							
					}else if(RegValue & (1<<3)){//Tx done								
								Txdonepro(RegValue);
					}
		}
		__WFI();
	}

}

//外部中断初始化
void EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;    
    __HAL_RCC_GPIOC_CLK_ENABLE();               //开启GPIOC时钟
		__HAL_RCC_GPIOA_CLK_ENABLE(); 
	
		GPIO_Initure.Pin=GPIO_PIN_3;            //PC3
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //下拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
	
//	  GPIO_Initure.Pin=GPIO_PIN_1;            //PC1
//    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
//    GPIO_Initure.Pull=GPIO_PULLDOWN;        //下拉
//    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
//    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
	
			GPIO_Initure.Pin=GPIO_PIN_0;            //PC1
			GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
			GPIO_Initure.Pull=GPIO_PULLDOWN;        //下拉
			GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
			HAL_GPIO_Init(GPIOA,&GPIO_Initure);
		

    
    GPIO_Initure.Pin=GPIO_PIN_0;                //PC1
    GPIO_Initure.Mode=GPIO_MODE_IT_RISING;      //上升沿触发
    GPIO_Initure.Pull=GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
		
		GPIO_Initure.Pin=GPIO_PIN_3;                //PC1
    GPIO_Initure.Mode=GPIO_MODE_IT_RISING;      //上升沿触发
    GPIO_Initure.Pull=GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
	

	
	    //中断线1-PC1
    HAL_NVIC_SetPriority(EXTI0_IRQn,2,0);       //抢占优先级为2，子优先级为0
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);             //使能中断线0
		
			    //中断线1-PC3
    HAL_NVIC_SetPriority(EXTI3_IRQn,2,0);       //抢占优先级为2，子优先级为0
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);             //使能中断线3
}

void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);		//调用中断处理公用函数
}

void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);		//调用中断处理公用函数
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
		uint16_t gpio = GPIO_Pin;
    delay_ms(100);      //消抖
		
	switch(gpio)
    {
        case GPIO_PIN_0:
						printf("进入了pin1中断: %x\r\n",gpio);
							LED0 = !LED0;   
							mode=1;        
            break;
        case GPIO_PIN_3:
						printf("进入了pin3中断: %x\r\n",gpio);
							LED3 = !LED3;
            break;

    }
		
}
