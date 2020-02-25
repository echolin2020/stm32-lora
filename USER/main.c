#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
//#include "lcd.h"
//#include "sdram.h"
#include "SX1278.h"
#include "timer.h"
//SX1278
#include "platform.h"
#include "radio.h"
#include "sx1276.h"
#include "stdio.h"
#include "stm32f4xx_hal_rcc.h"
#include "sx12xxEiger.h"
/************************************************
���ǵ�һ����keil���޸��ļ���������atom�Զ�ʶ��Ȼ��push��github
************************************************/

#define BUFFER_SIZE     30                          // Define the payload size here

static uint16_t BufferSize = BUFFER_SIZE;			// RF buffer size
static uint8_t  Buffer[BUFFER_SIZE];				// RF buffer
tRadioDriver *Radio = NULL;
const uint8_t MY_TEST_Msg[] = "SX1278_TEST";
uint32_t aa,bb,cc,dd;

int main(void)
{
	u8 key,mode;
	u16 t=0,i=0;		
	uint8_t txN=0;	
	char sendBuf[20];	 
	HAL_Init();                     //��ʼ��HAL��   
	Stm32_Clock_Init(336,8,2,7);   //����ʱ��,168Mhz
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);//echo added
	BoardInit();								//echo added
	TIM3_Init(10-1,8400-1);   			//1ms lora�� �� �޸� echo modified
	uart_init(115200);              //��ʼ��USART	
	delay_init(168);                //��ʼ����ʱ���� echo modified
	delay_ms(10);
	LED_Init();                     //��ʼ��LED 
	while(1){
		delay_ms(100);
		printf("���ǵ�����hello\r\n");
		delay_ms(1);
		printf("1\r\n");
		delay_ms(1);
		printf("2\r\n");
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
		}
	KEY_Init();                     //��ʼ������
	SX1278_Init();    		    		  //��ʼ��SX1278 
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
		//if(t==100)LCD_ShowString(10,150,230,16,16,"KEY0:RX_Mode  KEY1:TX_Mode"); //��˸��ʾ��ʾ��Ϣ
 		if(t==200)
		{	
			//LCD_Fill(10,150,230,150+16,WHITE);
			t=0; 
		}
		delay_ms(5);	  
	}   
 	//LCD_Fill(10,150,240,166,WHITE);//����������ʾ		  
 	//POINT_COLOR=BLUE;//��������Ϊ��ɫ	   
	if(mode==0)//RXģʽ
	{
		//LCD_ShowString(30,150,200,16,16,"lora RX_Mode");	
		//LCD_ShowString(30,170,200,16,16,"Received DATA:");	
		Radio->StartRx();   //RFLR_STATE_RX_INIT  
		while(1)
		{	  		    		    				 
			while( Radio->Process( ) == RF_RX_DONE)//һ�����յ���Ϣ,����ʾ����.
			{			
				Radio->GetRxPacket( Buffer, ( uint16_t* )&BufferSize );			
				if( strlen(Buffer) != 0 )
				{
					Buffer[strlen(Buffer)]=0;//�����ַ���������
					printf("LoRa RX: %s\r\n",Buffer);		
					//LCD_ShowString(0,190,lcddev.width-1,32,16,Buffer);   					
					LED0=!LED0;
					for(i=0;i<BUFFER_SIZE;i++)
						Buffer[i] = 0;
				}
				Radio->StartRx( );			 
			} 				    
		}
	}else//TXģʽ
	{							    
		//LCD_ShowString(30,150,200,16,16,"lora TX_Mode");	
		Radio->SetTxPacket( MY_TEST_Msg, 11 );
		mode=' ';//�ӿո����ʼ  
		while(1)
		{	  		   				 
			while(Radio->Process( ) == RF_TX_DONE)
			{
				sprintf(sendBuf,"SX1278_TEST_%d",txN++);
				//LCD_Fill(0,170,lcddev.width,170+16*3,WHITE);//�����ʾ
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
