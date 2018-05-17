/*! ----------------------------------------------------------------------------
 * @file	port.h
 * @brief	HW specific definitions and functions for portability
 *
 * @attention
 *
 * @author Rui
 */


#ifndef PORT_H_
#define PORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"


/*** dwm spi pins ***/	
#define SPIx						SPI1
#define SPIx_GPIO					GPIOA
#define SPIx_CS						GPIO_Pin_4
#define SPIx_CS_GPIO				GPIOA
#define SPIx_SCK					GPIO_Pin_5
#define SPIx_MISO					GPIO_Pin_6
#define SPIx_MOSI					GPIO_Pin_7
/*** dwm rst pin ***/
#define DW1000_RSTn					GPIO_Pin_8
#define DW1000_RSTn_GPIO			GPIOB
/*** dwm irq pin ***/
#define DW1000_IRQ                  GPIO_Pin_9
#define DW1000_IRQ_GPIO             GPIOB
#define DW1000_IRQ_EXTI             EXTI_Line9
#define DW1000_IRQ_EXTI_PORT        GPIO_PortSourceGPIOB
#define DW1000_IRQ_EXTI_PIN         GPIO_PinSource9
#define DW1000_IRQ_EXTI_IRQn        EXTI9_5_IRQn
#define DW1000_IRQ_EXTI_USEIRQ      ENABLE
#define DW1000_IRQ_EXTI_NOIRQ       DISABLE

#define SPIx_CS_LOW					GPIO_ResetBits(SPIx_CS_GPIO, SPIx_CS)
#define SPIx_CS_HIGH				GPIO_SetBits(SPIx_CS_GPIO, SPIx_CS)

ITStatus EXTI_GetITEnStatus(uint32_t x);

#define port_GetEXT_IRQStatus()             EXTI_GetITEnStatus(DW1000_IRQ_EXTI_IRQn)
#define port_DisableEXT_IRQ()               NVIC_DisableIRQ(DW1000_IRQ_EXTI_IRQn)
#define port_EnableEXT_IRQ()                NVIC_EnableIRQ(DW1000_IRQ_EXTI_IRQn)
#define port_CheckEXT_IRQ()                 GPIO_ReadInputDataBit(DW1000_IRQ_GPIO, DW1000_IRQ)
int NVIC_DisableDECAIRQ(void);

int is_IRQ_enabled(void);


/* DW1000 IRQ (EXTI9_5_IRQ) handler type. */
typedef void (*port_deca_isr_t)(void);

/* DW1000 IRQ handler declaration. */
extern port_deca_isr_t port_deca_isr;

void peripherals_init (void);

void port_set_deca_isr(port_deca_isr_t deca_isr);

void SPI_ChangeRate(uint16_t scalingfactor);
void spi_set_rate_low (void);
void spi_set_rate_high (void);

//unsigned long portGetTickCnt(void);

//#define portGetTickCount() 			portGetTickCnt()

void reset_DW1000(void);
void setup_DW1000RSTnIRQ(int enable);

#ifdef __cplusplus
}
#endif

#endif /* PORT_H_ */
