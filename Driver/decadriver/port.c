/*! ----------------------------------------------------------------------------
 * @file	port.c
 * @brief	HW specific definitions and functions for portability
 *
 * @attention
 *
 * @author Rui
 */
#include "sleep.h"
#include "port.h"
#include "sys.h"

EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
port_deca_isr_t port_deca_isr;


int NVIC_DisableDECAIRQ(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	/* Configure EXTI line */
	EXTI_InitStructure.EXTI_Line = DW1000_IRQ_EXTI;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	//MPW3 IRQ polarity is high by default
	EXTI_InitStructure.EXTI_LineCmd = DW1000_IRQ_EXTI_NOIRQ;
	EXTI_Init(&EXTI_InitStructure);

	return 0;
}

/********************************  
*
*	Config MCU exit
*	1 -> Rinsing edge to trig
*	0 -> Falling edge to trig
*
********************************/
void dwm_exti_init(u8 edge)
{
	GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	/*** IRQn pin ***/
    GPIO_InitStructure.GPIO_Pin = DW1000_IRQ;
	if(edge)
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;							//Rinsing edge to trig, normally in low level
	else
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;							//Falling edge to trig, normally in high level
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(DW1000_IRQ_GPIO, &GPIO_InitStructure);

	/*** GPIO EXIT Init ***/
  	GPIO_EXTILineConfig(DW1000_IRQ_EXTI_PORT,DW1000_IRQ_EXTI_PIN);

  	EXTI_InitStructure.EXTI_Line = DW1000_IRQ_EXTI;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	if(edge)
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;					//Rinsing edge to trig,
	else
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;					//Falling edge to trig,
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	
	
  	NVIC_InitStructure.NVIC_IRQChannel = DW1000_IRQ_EXTI_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure); 
}

/**
  * @brief  Checks whether the specified EXTI line is enabled or not.
  * @param  EXTI_Line: specifies the EXTI line to check.
  *   This parameter can be:
  *     @arg EXTI_Linex: External interrupt line x where x(0..19)
  * @retval The "enable" state of EXTI_Line (SET or RESET).
  */
ITStatus EXTI_GetITEnStatus(uint32_t EXTI_Line)
{
  ITStatus bitstatus = RESET;
  uint32_t enablestatus = 0;
  /* Check the parameters */
  assert_param(IS_GET_EXTI_LINE(EXTI_Line));

  enablestatus =  EXTI->IMR & EXTI_Line;
  if (enablestatus != (uint32_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  return bitstatus;
}

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn port_set_deca_isr()
 *
 * @brief This function is used to install the handling function for DW1000 IRQ.
 *
 * NOTE:
 *   - As EXTI9_5_IRQHandler does not check that port_deca_isr is not null, the user application must ensure that a
 *     proper handler is set by calling this function before any DW1000 IRQ occurs!
 *   - This function makes sure the DW1000 IRQ line is deactivated while the handler is installed.
 *
 * @param deca_isr function pointer to DW1000 interrupt handler to install
 *
 * @return none
 */

void port_set_deca_isr(port_deca_isr_t deca_isr)
{
    /* Check DW1000 IRQ activation status. */
    ITStatus en = port_GetEXT_IRQStatus();

    /* If needed, deactivate DW1000 IRQ during the installation of the new handler. */
    if (en)
    {
        port_DisableEXT_IRQ();
    }
    port_deca_isr = deca_isr;
    if (en)
    {
        port_EnableEXT_IRQ();
    }
}

//void EXTI9_5_IRQHandler()
//{
//	port_set_deca_isr(spi_set_rate_low);
//	port_deca_isr();
//	
//}

void SPI_ChangeRate(uint16_t scalingfactor)
{
	uint16_t tmpreg = 0;

	/* Get the SPIx CR1 value */
	tmpreg = SPIx->CR1;

	/*clear the scaling bits*/
	tmpreg &= 0xFFC7;

	/*set the scaling bits*/
	tmpreg |= scalingfactor;

	/* Write to SPIx CR1 */
	SPIx->CR1 = tmpreg;
}

void spi_set_rate_low (void)
{
    SPI_ChangeRate(SPI_BaudRatePrescaler_32);
}

void spi_set_rate_high (void)
{
    SPI_ChangeRate(SPI_BaudRatePrescaler_4);
}

int is_IRQ_enabled(void)
{
	return ((   NVIC->ISER[((uint32_t)(DW1000_IRQ_EXTI_IRQn) >> 5)]
	           & (uint32_t)0x01 << (DW1000_IRQ_EXTI_IRQn & (uint8_t)0x1F)  ) ? 1 : 0) ;
}
