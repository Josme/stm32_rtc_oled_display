#include "dw_driver.h"
#include "delay.h"
#include "stdio.h"
#include "sys.h"
#include "oled.h"
#include "deca_spi.h"
#include "deca_regs.h"
#include "string.h"
#include "usart.h"


dw_data_t dw_data;


/*** all initial process entry ***/
void dw_init(void)
{
	dw_hw_init();
	dw_param_init();
	if(dw_sw_init() == DWT_ERROR)
	{
		OLED_ShowString(30,12,"INIT FAILED",12,1);
		while (1)
			;
	}
	else
		OLED_ShowString(30,12,"Config OK",12,1);
}

/*** Low level initial process ***/
void dw_hw_init(void)
{	
	dwm_spi_init();
		
	DWM_RSTn = 0;
	delay_ms(100);
	DWM_RSTn = 1;
	delay_ms(100);
}

/*** upper level initial process ***/
int dw_sw_init(void)
{
	dwm_set_spi_rate_low();
	if (dwt_initialise(dw_data.loaducode) == DWT_ERROR){
		return DWT_ERROR;
	}
	dwm_set_spi_rate_high();

	dwt_configure(&(dw_data.config));
	
	dwt_setleds(0x03);										//enable tx/rx led

    dwt_settxantennadelay(dw_data.txantennaDelay);			//set antenna delay
	dwt_setrxantennadelay(dw_data.rxantennaDelay);
	
	return DWT_SUCCESS;
}

/*** initial parameter set first ***/
void dw_param_init(void)
{
	dw_data.txantennaDelay = 16440;
	dw_data.rxantennaDelay = 16440;
	
	dw_data.loaducode = 1;
	dw_data.rx_after_tx_delay = 140;
	dw_data.wait_resp_timeout = 210;
	dw_data.msg_sn_idx = 2;
	dw_data.rx_pool_ts_idx = 10;
	dw_data.rx_resp_ts_idx = 14;
	
	dw_data.fram_len = 12;
	dw_data.ranging_interval_ms = 10;
	dw_data.rx_resp_delay_uus = 330;
	
	dw_data.config.chan = 2;						/* Channel number. */
	dw_data.config.prf = DWT_PRF_64M;				/* Pulse repetition frequency. */
	dw_data.config.txPreambLength = DWT_PLEN_1024;	/* Preamble length. */
	dw_data.config.rxPAC = DWT_PAC32;				/* Preamble acquisition chunk size. Used in RX only. */
	dw_data.config.txCode = 9;						/* TX preamble code. Used in TX only. */
	dw_data.config.rxCode = 9;						/* RX preamble code. Used in RX only. */
	dw_data.config.nsSFD = 1;						/* Use non-standard SFD (Boolean) */
	dw_data.config.dataRate = DWT_BR_6M8;			/* Data rate. */
	dw_data.config.phrMode = DWT_PHRMODE_STD;		/* PHY header mode. */
	dw_data.config.sfdTO = (1025 + 64 - 32);		/* SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
}

/*** return the current systime in ms ***/
uint8 dw_get_systime_ms(void)
{
	//freq = 63.8976Ghz
	//1.003ms = time * 256 * 256 * 256
	return (dwt_readsystimestamphi32() >> 24) / 67;
}

/*** get the single variable of tx timestamp in uint64 ***/
uint64 get_tx_timestamp_u64(void)
{
    uint8 ts_tab[5];
    uint64 ts = 0;
    int i;
    dwt_readtxtimestamp(ts_tab);
    for (i = 4; i >= 0; i--)
    {
        ts <<= 8;
        ts |= ts_tab[i];
    }
    return ts;
}

/*** get the single variable of rx timestamp in uint64 ***/
uint64 get_rx_timestamp_u64(void)
{
    uint8 ts_tab[5];
    uint64 ts = 0;
    int i;
    dwt_readrxtimestamp(ts_tab);
    for (i = 4; i >= 0; i--)
    {
        ts <<= 8;
        ts |= ts_tab[i];
    }
    return ts;
}
/*** get timestamp form the response message ***/
void resp_msg_get_ts(uint8 *ts_field, uint64 *ts)
{
    int i;
    *ts = 0;
    for (i = 0; i < RESP_MSG_TS_LEN; i++)
    {
        *ts += ts_field[i] << (i * 8);
    }
}

/*** write the response timestamp to the response message ***/
void resp_msg_set_ts(uint8 *ts_field, const uint64 ts)
{
    int i;
    for (i = 0; i < RESP_MSG_TS_LEN; i++)
    {
        ts_field[i] = (ts >> (i * 8)) & 0xFF;
    }
}




