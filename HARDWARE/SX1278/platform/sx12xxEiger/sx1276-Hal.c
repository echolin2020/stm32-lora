/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: (1) "AS IS" WITH NO WARRANTY; AND 
 * (2)TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, SEMTECH SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 * 
 * Copyright (C) SEMTECH S.A.
 */
/*! 
 * \file       sx1276-Hal.c
 * \brief      SX1276 Hardware Abstraction Layer
 *
 * \version    2.0.B2 
 * \date       Nov 21 2012
 * \author     Miguel Luis
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
//#include <stdint.h>
//#include <stdbool.h> 

#include "platform.h"

#if defined( USE_SX1276_RADIO )

//#include "ioe.h"
#include "spi.h"
#include "../../radio/sx1276-Hal.h"
#include "SX1278.h"
/*!
 * SX1276 RESET I/O definitions
 */
//#define RESET_IOPORT                                GPIOG
//#define RESET_PIN                                   GPIO_Pin_8

/*!
 * SX1276 SPI NSS I/O definitions
 */
//#define NSS_IOPORT                                  GPIOG
//#define NSS_PIN                                     GPIO_Pin_7     //ԭGPIO_Pin_15


void SX1276InitIo( void )
{

}

uint8_t SpiInOut( uint8_t outData )
{
    /* Send SPIy data */
    return SPI2_ReadWriteByte(outData);
}

void SX1276SetReset( uint8_t state )
{
    //GPIO_InitTypeDef GPIO_InitStructure;

    if( state == RADIO_RESET_ON )
    {
        SX1278_CE=0;
    }
    else
    {
				SX1278_CE=1;
    }
}

void SX1276Write( uint8_t addr, uint8_t data )
{
    SX1276WriteBuffer( addr, &data, 1 );
}

void SX1276Read( uint8_t addr, uint8_t *data )
{
    SX1276ReadBuffer( addr, data, 1 );
}

void SX1276WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;

    //NSS = 0;
    SX1278_CSN=0;

    SpiInOut( addr | 0x80 );
    for( i = 0; i < size; i++ )
    {
        SpiInOut( buffer[i] );
    }

    //NSS = 1;
    SX1278_CSN=1;
}

void SX1276ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;

    //NSS = 0;
    SX1278_CSN=0;

    SpiInOut( addr & 0x7F );

    for( i = 0; i < size; i++ )
    {
        buffer[i] = SpiInOut( 0 );
    }

    //NSS = 1;
    SX1278_CSN=1;
}

void SX1276WriteFifo( uint8_t *buffer, uint8_t size )
{
    SX1276WriteBuffer( 0, buffer, size );
}

void SX1276ReadFifo( uint8_t *buffer, uint8_t size )
{
    SX1276ReadBuffer( 0, buffer, size );
}

inline uint8_t SX1276ReadDio0( void )
{
    return 0;
}

inline uint8_t SX1276ReadDio1( void )
{
    return 0;
}

inline uint8_t SX1276ReadDio2( void )
{
    return 0;
}

inline uint8_t SX1276ReadDio3( void )
{
    return 0;
}

inline uint8_t SX1276ReadDio4( void )
{
    return 0;
}

inline uint8_t SX1276ReadDio5( void )
{
    return 0;
}


//射频芯片收发切换
inline void SX1276WriteRxTx( uint8_t txEnable )
{
//    if( txEnable != 0 )
//    {
//		Set_RF_Switch_TX(); //单片机将射频开关芯片切换成发射状态
////        IoePinOn( FEM_CTX_PIN );
////        IoePinOff( FEM_CPS_PIN );
//    }
//    else
//    {
//		Set_RF_Switch_RX();  //单片机将射频开关芯片切换成接收状态
////        IoePinOff( FEM_CTX_PIN );
////        IoePinOn( FEM_CPS_PIN );
//    }
}

#endif // USE_SX1276_RADIO
