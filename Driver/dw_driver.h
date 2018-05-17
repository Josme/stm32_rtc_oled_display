#ifndef __DW_DRV_H
#define __DW_DRV_H
#include "sys.h"
#include "deca_device_api.h"

/****************************************
*
*	Pin define:
*		SPI_CSn:	PA4
*		SPI_SCK:	PA5
*		SPI_MISO:	PA6
*		SPI_MOSI:	PA7
*
*		EXT_ON:		PB6
*		WAKEUP:		PB7
*		RSTn:		PB8
*		IRQn:		PB9
*
****************************************/

#define DWM_CS			PAout(4)
#define DWM_EXTON_IN	PBin(6)
#define DWM_WAKEUP		PBout(7)
#define DWM_RSTn		PBout(8)
#define DWM_IRQn		PBout(9)

#define SPI_CLK			PAout(5)
#define SPI_MISO		PAin(6)
#define SPI_MOSI		PAout(7)

typedef unsigned long long uint64;
/*** avaliable modes for dw node ***/
typedef enum dwModes {
	TX,
	RX,
	SS_TWR_TX,
	SS_TWR_RX,
	DS_TWR_TX,
	DS_TWR_RX,
	TAG,
	ANCHOR
}DW_MODE;

/*** dw status ***/
typedef enum dwStatus {
	DW_INIT,
	DW_TXE_WAIT,
	DW_TXPOLL_WAIT_SEND,
	DW_TX_WAIT_RESP,
	DW_TXRESP_WAIT_SEND,
	
	DW_RXE_WAIT,
	DW_RX_WAIT_DATA,
	
	DW_SLEEP_DONE
}DW_STATE;

/*** all data for dw node ***/
typedef struct {
	DW_MODE mode;
	DW_STATE currtState;
	
/*** configurations ***/
	dwt_config_t config;
	dwt_txconfig_t txConfig;
	uint16 txantennaDelay;
	uint16 rxantennaDelay;
	uint8 loaducode;
	
	uint16 fram_len;
	
	uint16 rx_after_tx_delay;				//set rx mode after tx poll delay in Uus
	uint16 wait_resp_timeout;				//wait for response message timeout for pool tx node
	uint16 rx_resp_delay_uus;					//time from rx_pool to rx_resp
	uint8 msg_sn_idx;
	uint8 frame_seq_num;
	uint16 ranging_interval_ms;
	
/*** system time delay for delayed transmit ***/
	uint64 initReplyDelay;
	uint64 finalReplyDelay;
	uint64 respReplyDelay;
	int16  finalReplyDelay_ms;
	
/*** timestamps ***/
	uint64 tx_pool_ts;						//timestamp when send the pool message
	uint64 tx_resp_ts;						//timestamp when recieved the response messfage
	uint64 rx_pool_ts;						//timestamp when recieved the pool message
	uint64 rx_resp_ts;						//timestamp when send the response message
	uint8 rx_pool_ts_idx;
	uint8 rx_resp_ts_idx;
	
}dw_data_t;


#define FRAME_LEN_MAX 1023
#define SPEED_OF_LIGHT 299702547
#define UUS_TO_DWT_TIME 65536
#define RESP_MSG_TS_LEN 4

extern dw_data_t dw_data;


void dw_init(void);
void dw_hw_init(void);
int  dw_sw_init(void);
void dw_param_init(void);
void dwm_GPIOInit(void);
void resp_msg_get_ts(uint8 *ts_field, uint64 *ts);
uint8 dw_get_systime_ms(void);
uint64 get_rx_timestamp_u64(void);
uint64 get_tx_timestamp_u64(void);
void resp_msg_set_ts(uint8 *ts_field, const uint64 ts);
void dwm_rx_demo(void);
void dwm_tx_demo(void);
void dwm_ss_twr_init(void);
void dwm_ss_twr_resp(void);



#endif
