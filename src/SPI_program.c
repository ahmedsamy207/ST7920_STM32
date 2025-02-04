/*
 *<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    SPI_program.c   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *
 *  Author  : Ahmed Samy Kamal
 *  Date    : 11/7/2022
 *  Version : V01
 *  Layer   : MCAL
 * 
 */ 
 
//=====================================================================================================================
#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "DIO_interface.h"
#include "SPI_interface.h"
#include "SPI_config.h"
#include "SPI_private.h"
//=====================================================================================================================

void (*MSPI1_CallBack) (u8);

void MSPI_voidInit ( void ){

#if   MSP1_STATUS == MSPI_ENABLE

#if		MSPI1_CLOCK_MODE == MSPI_MODE0

	CLR_BIT( MSPI1 -> CR1 , 0 ); CLR_BIT( MSPI1 -> CR1 , 1 );

#elif	MSPI1_CLOCK_MODE == MSPI_MODE1

	SET_BIT( MSPI1 -> CR1 , 0 ); CLR_BIT( MSPI1 -> CR1 , 1 );

#elif	MSPI1_CLOCK_MODE == MSPI_MODE2

	CLR_BIT( MSPI1 -> CR1 , 0 ); SET_BIT( MSPI1 -> CR1 , 1 );

#elif	MSPI1_CLOCK_MODE == MSPI_MODE3

	SET_BIT( MSPI1 -> CR1 , 0 ); SET_BIT( MSPI1 -> CR1 , 1 );

#endif

#if    MSPI1_MASTER_SLAVE == MSPI_MASTER

	SET_BIT( MSPI1 -> CR1 , 2 );

#elif  MSPI1_MASTER_SLAVE == MSPI_SLAVE

	CLR_BIT( MSPI1 -> CR1 , 2 );

#endif

#if   MSPI1_DATA_ORDER == MSPI_MSB_FIRST

	CLR_BIT( MSPI1 -> CR1 , 7 );

#elif MSPI1_DATA_ORDER == MSPI_LSB_FIRST

	SET_BIT( MSPI1 -> CR1 , 7 );

#endif

#if   MSPI1_SS_MANAGE  == HW_SLAVE_MANAGEMENT

	CLR_BIT( MSPI1 -> CR1 , 9 );

#elif MSPI1_SS_MANAGE == SW_SLAVE_MANAGEMENT

	SET_BIT( MSPI1 -> CR1 , 8 );
	SET_BIT( MSPI1 -> CR1 , 9 );

#endif

#if   MSPI1_DATA_SIZE == MSPI_8BIT_DATA

	CLR_BIT( MSPI1 -> CR1 , 11 );

#elif MSPI1_DATA_SIZE == MSPI_16BIT_DATA

	SET_BIT( MSPI1 -> CR1 , 11 );

#endif

#if   MSPI1_INT_STATUS == MSPI_INT_DISABLE

	MSPI1 -> CR2 = 0 ;

#elif MSPI1_INT_STATUS == MSPI_TXE_INT_ENABLE

	SET_BIT( MSPI1 -> CR2 , 7 );

#elif MSPI1_INT_STATUS == MSPI_RXNE_INT_ENABLE

	SET_BIT( MSPI1 -> CR2 , 6 );

#endif

	MSPI1 -> CR1 &= 0xFFC7 ;
	MSPI1 -> CR1 |= ( MSPI1_PRESCALLER << 3 ) ;

	SET_BIT( MSPI1 -> CR1 , 6 );

#elif MSP1_STATUS == MSPI_DISABLE
	CLR_BIT( MSPI1 -> CR1 , 6 );
#endif

}


void MSPI1_voidSendReceiveSynch(u8 Copy_u8DataToTransmit, u8 *Copy_DataToReceive)
{
	/* Clear For Slave Select Pin */
	MDIO_voidSetPinValue(MSPI1_SLAVE_PIN,DIO_LOW);
	
	/* Send Data */
	MSPI1 -> DR = Copy_u8DataToTransmit;
	
	/* Wait Busy Flag to finish */
	while (GET_BIT(MSPI1 -> SR, 7) == 1);

 	/* Return to the received data */
	*Copy_DataToReceive = MSPI1 -> DR;    // return  MSPI1 -> DR;
	
	/* Set Salve Select Pin */
	MDIO_voidSetPinValue(MSPI1_SLAVE_PIN,DIO_HIGH);
}

u16 SPI1_u16Transceive(u16 u16SendDataCpy)
{

	u16 u16ReceiveDataLoc = 0;
	while(GET_BIT(MSPI1->SR, SR_TXE)==0);
	MSPI1->DR = u16SendDataCpy;
	while(GET_BIT(MSPI1->SR, SR_RXNE)==0);
	u16ReceiveDataLoc = (u16)MSPI1->DR;
	return u16ReceiveDataLoc;
}


void MSPI1_voidSendReceiveAsynch(u8 Copy_u8DataToTransmit, void (*CallBack)(u8))
{
	/* Clear For Slave Select Pin */
	MDIO_voidSetPinValue(MSPI1_SLAVE_PIN,DIO_LOW);
	
	/* Send Data */
	MSPI1 -> DR = Copy_u8DataToTransmit;
	
}


void MSPI1_voidSetCallBack( void (*ptr) (u8) ){

	MSPI1_CallBack = ptr;

}

void SPI1_IRQHandler(void){

	MSPI1_CallBack(MSPI1 -> DR);

}
