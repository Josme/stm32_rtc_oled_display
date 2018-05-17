/*! ----------------------------------------------------------------------------
 * @file	deca_spi.h
 * @brief	SPI access functions
 *
 * @attention
 *
 * @author Rui
 */

#ifndef _DECA_SPI_H_
#define _DECA_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "deca_types.h"
#include "sys.h"

#define DECA_MAX_SPI_HEADER_LENGTH      (3)                     // max number of bytes in header (for formating & sizing)

void dwm_spi_init(void);
void dwm_exti_init(uint8 level);
void dwm_set_spi_rate_low(void);
void dwm_set_spi_rate_high(void);
uint8 dwm_set_spi_rate(uint16 pre);
int openspi(void) ;
int closespi(void) ;
int writetospi( uint16 headerLength,
			   	    const uint8 *headerBuffer,
					uint32 bodylength,
					const uint8 *bodyBuffer
				  );

int readfromspi( uint16	headerLength,
			    	 const uint8 *headerBuffer,
					 uint32 readlength,
					 uint8 *readBuffer );

#ifdef __cplusplus
}
#endif

#endif
