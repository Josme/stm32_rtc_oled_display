/*! ----------------------------------------------------------------------------
 * @file		deca_spi.c
 * @brief		SPI access functions
 *
 * @attention	
 *
 * @author 		Rui
 */
#include <string.h>
#include "sys.h"
#include "delay.h"
#include "deca_spi.h"
#include "port.h"
//#include "deca_sleep.h"
#include "deca_device_api.h"

SPI_InitTypeDef SPI_InitStructure;

void dwm_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

	/*** CS pin ***/
    GPIO_InitStructure.GPIO_Pin = SPIx_CS;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPIx_CS_GPIO, &GPIO_InitStructure);

	/*** EXT_ON pin ***/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;							//Get high in active state after wake up
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*** Wakeup pin ***/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;						//SET high to wakeup dwm,
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*** RSTn pin ***/
    GPIO_InitStructure.GPIO_Pin = DW1000_RSTn;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;						//Low pulse to reset dwm,
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DW1000_RSTn_GPIO, &GPIO_InitStructure);

	/*** IRQn pin ***/
    GPIO_InitStructure.GPIO_Pin = DW1000_IRQ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;							//
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DW1000_IRQ_GPIO, &GPIO_InitStructure);

	
	GPIO_SetBits(GPIOA, GPIO_Pin_4);										//un-select chip
	GPIO_SetBits(GPIOB, GPIO_Pin_7);										//normol state ,no force wakeup to dwm
}

void dwm_spi_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/*** SPI pins ***/
    GPIO_InitStructure.GPIO_Pin = SPIx_SCK | SPIx_MISO | SPIx_MOSI;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPIx_GPIO, &GPIO_InitStructure);	
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;								//clk invalid in low
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;							//data valid at rising edge of clk
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;	//start to low spi rate
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPIx, &SPI_InitStructure);
 
	SPI_Cmd(SPIx, ENABLE); 													//enable spi
	dwm_gpio_init();
}
void dwm_set_spi_rate_low(void)
{
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;		//start to low spi rate
	SPI_Init(SPIx, &SPI_InitStructure);
}
void dwm_set_spi_rate_high(void)
{
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;		//start to low spi rate
	SPI_Init(SPIx, &SPI_InitStructure);
}
uint8 dwm_set_spi_rate(uint16 pre)
{
	if(IS_SPI_BAUDRATE_PRESCALER(pre)){										//valid prama
		SPI_InitStructure.SPI_BaudRatePrescaler = pre;
		SPI_Init(SPIx, &SPI_InitStructure);
		return DWT_SUCCESS;
	}
	else
		return DWT_ERROR;
}


/*** HAL SPI read/write interface ***/
uint16_t SPI_ReadWriteByte(uint16_t writeByte )
{
	u8 retry = 0;
	
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET) 	//wait for the TX empty flag
	{
		retry++;
		if(retry > 100)
			break;
	}		
	
	SPI_I2S_SendData(SPIx, writeByte); 								//write one byte to device

	retry = 0;	
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET) //wait for the RX not empty flag
	{
		retry++;
		if(retry > 100)
			break;
	}	  						    
	return SPI_I2S_ReceiveData(SPIx); 								//read one byte from device				    
}

/*** low level API for deca driver ***/
int writetospi(uint16 headerLength,const uint8 *headerBuffer,uint32 bodylength,const uint8 *bodyBuffer)
{
	int i=0;

    decaIrqStatus_t  stat ;

//    stat = decamutexon() ;
    SPIx_CS_LOW;

    for(i=0; i<headerLength; i++)
    {
    	SPIx->DR = headerBuffer[i];

    	while ((SPIx->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);

    	SPIx->DR ;
    }

    for(i=0; i<bodylength; i++)
    {
     	SPIx->DR = bodyBuffer[i];

    	while((SPIx->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);

		SPIx->DR ;
	}

//    for(i=0; i<headerLength; i++)
//		SPI_ReadWriteByte(headerBuffer[i]);		//write data header to device via spi

//    for(i=0; i<bodylength; i++)
//		SPI_ReadWriteByte(bodyBuffer[i]);		//write data body to device via spi

    SPIx_CS_HIGH;
//    decamutexoff(stat) ;

    return 0;
}

int readfromspi(uint16 headerLength,const uint8 *headerBuffer,uint32 readlength,uint8 *readBuffer)
{
	int i=0;

	decaIrqStatus_t  stat ;

	//    stat = decamutexon() ;
	SPIx_CS_LOW;

    for(i=0; i<headerLength; i++)
    {
    	SPIx->DR = headerBuffer[i];

     	while((SPIx->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);

     	readBuffer[0] = SPIx->DR ; // Dummy read as we write the header
    }

    for(i=0; i<readlength; i++)
    {
    	SPIx->DR = 0;  // Dummy write as we read the message body

    	while((SPIx->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);
 
	   	readBuffer[i] = SPIx->DR ;//port_SPIx_receive_data(); //this clears RXNE bit
    }

//	for(i=0; i < headerLength; i++)
//		SPI_ReadWriteByte(headerBuffer[i]);				//write data header to deice via spi
//	for(i=0; i < readlength; i++)
//		readBuffer[i] = SPI_ReadWriteByte(0);			//read data body frome device via spi

	SPIx_CS_HIGH;
	//    decamutexoff(stat) ;
	return 0;
}
