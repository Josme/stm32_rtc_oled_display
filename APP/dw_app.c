#include "dw_driver.h"
#include "oled.h"
#include "string.h"
#include "deca_spi.h"
#include "deca_device_api.h"
#include "deca_regs.h"
#include "usart.h"
#include "delay.h"
#include "dw_algorithm.h"
#include "ano_dt.h"

#define BLINK_FRAME_SN_IDX 1


char APP_NAME[30];
volatile uint32 status_reg;
static uint8 rx_buffer[FRAME_LEN_MAX];


void dwm_tx_demo(void)
{
	static uint8 tx_msg[] = {0xC5, 0, 'D', 'E', 'C', 'A', 'W', 'A', 'V', 'E', 0, 0};
	
//	dw_data.config.chan = 2;						/* Channel number. */
//	dw_data.config.prf = DWT_PRF_64M;				/* Pulse repetition frequency. */
//	dw_data.config.txPreambLength = DWT_PLEN_1024;	/* Preamble length. */
//	dw_data.config.rxPAC = DWT_PAC32;				/* Preamble acquisition chunk size. Used in RX only. */
//	dw_data.config.txCode = 9;						/* TX preamble code. Used in TX only. */
//	dw_data.config.rxCode = 9;						/* RX preamble code. Used in RX only. */
//	dw_data.config.nsSFD = 1;						/* Use non-standard SFD (Boolean) */
//	dw_data.config.dataRate = DWT_BR_6M8;			/* Data rate. */
//	dw_data.config.phrMode = DWT_PHRMODE_STD;		/* PHY header mode. */
//	dw_data.config.sfdTO = (1025 + 64 - 32);		/* SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
//	
//	dw_sw_init();
	
	strcpy(APP_NAME,"SIMPLE TX v1.2");
	OLED_ShowString(12,0,APP_NAME,12,1);
	
	dw_data.msg_sn_idx = 1;
	dw_data.fram_len = sizeof(tx_msg);
	
	while(1)
	{

		dwt_setsmarttxpower(1);
		
		while(1)
		{
			dwt_writetxdata(sizeof(tx_msg), tx_msg, 0); /* Zero offset in TX buffer. */
			dwt_writetxfctrl(sizeof(tx_msg), 0,0); /* Zero offset in TX buffer, no ranging. */

			dwt_starttx(DWT_START_TX_IMMEDIATE);

			OLED_ShowString(0,30,"sta:",12,1);
			OLED_ShowHex(33,30,status_reg,12);
			
			while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) & SYS_STATUS_TXFRS))
				;

			/* Clear TX frame sent event. */
			dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);

			delay_ms(10);

			tx_msg[BLINK_FRAME_SN_IDX]++;
		}
	}
}


void dwm_rx_demo(void)
{
	uint8 rx_cnt = 0;
	uint8 rx_err_cnt;
	uint8 rx_temp = 0;
	uint8 col = 0;
	uint8 line = 0;
	
//	dw_data.config.chan = 2;						/* Channel number. */
//	dw_data.config.prf = DWT_PRF_64M;				/* Pulse repetition frequency. */
//	dw_data.config.txPreambLength = DWT_PLEN_1024;	/* Preamble length. */
//	dw_data.config.rxPAC = DWT_PAC32;				/* Preamble acquisition chunk size. Used in RX only. */
//	dw_data.config.txCode = 9;						/* TX preamble code. Used in TX only. */
//	dw_data.config.rxCode = 9;						/* RX preamble code. Used in RX only. */
//	dw_data.config.nsSFD = 1;						/* Use non-standard SFD (Boolean) */
//	dw_data.config.dataRate = DWT_BR_6M8;			/* Data rate. */
//	dw_data.config.phrMode = DWT_PHRMODE_STD;		/* PHY header mode. */
//	dw_data.config.sfdTO = (1025 + 64 - 32);		/* SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
//	
//	dw_sw_init();
	
	strcpy(APP_NAME,"SIMPLE RX v1.2");
	OLED_ShowString(10,3,APP_NAME,12,1);

	dw_data.msg_sn_idx = 1;
	dw_data.fram_len = 12;
	
	while (1)
	{
		int i;

		for (i = 0 ; i < dw_data.fram_len; i++ )
		{
			rx_buffer[i] = 0;
		}

		dwt_rxenable(DWT_START_RX_IMMEDIATE);
		
		while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_ERR)))
			;

		if (status_reg & SYS_STATUS_RXFCG)
		{
			dw_data.fram_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFL_MASK_1023;
			if (dw_data.fram_len <= FRAME_LEN_MAX)
			{
				dwt_readrxdata(rx_buffer, dw_data.fram_len, 0);
				for(i = 0;i < dw_data.fram_len;i++){
					if(i > 5){
						line = i - 6;
						col = 1;
					}
					else{
						line = i;
						col = 0;
					}
					OLED_ShowHex(10 + 18*line,40 + 12*col,rx_buffer[i],12);
					rx_cnt++;
					if(rx_buffer[1] - rx_temp > 1)
						rx_err_cnt++;
					if(rx_cnt > 99){
						OLED_ShowNum(84,30,rx_err_cnt * 100 / rx_cnt,12,2,1);
						OLED_ShowChar(116,30,'%',12,1);
						rx_cnt = 0;
						rx_err_cnt = 0;
					}
					rx_temp = rx_buffer[1];
				}
			}
			dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);
		}
		else
			dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR);
	}	
}


void dwm_ss_twr_init(void)
{
	static double tof;
	static double distance;
	char dist_str[16] = {0};
	static uint8 tx_poll_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'W', 'A', 'V', 'E', 0xE0, 0, 0};
	static uint8 rx_resp_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'V', 'E', 'W', 'A', 0xE1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	dw_data.config.chan = 2;						/* Channel number. */
	dw_data.config.prf = DWT_PRF_64M;				/* Pulse repetition frequency. */
	dw_data.config.txPreambLength = DWT_PLEN_128;	/* Preamble length. */
	dw_data.config.rxPAC = DWT_PAC8;				/* Preamble acquisition chunk size. Used in RX only. */
	dw_data.config.txCode = 9;						/* TX preamble code. Used in TX only. */
	dw_data.config.rxCode = 9;						/* RX preamble code. Used in RX only. */
	dw_data.config.nsSFD = 0;						/* Use standard SFD (Boolean) */
	dw_data.config.dataRate = DWT_BR_6M8;			/* Data rate. */
	dw_data.config.phrMode = DWT_PHRMODE_STD;		/* PHY header mode. */
	dw_data.config.sfdTO = (129 + 8 - 8);			/* SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
	
	dw_sw_init();
		
	/*** parameter initial for algorithm ***/
	algori_param_init();
	algori_set_filter(MID_AVG);	
		
	strcpy(APP_NAME,"SS TWR INIT");
	OLED_Clear();
	OLED_ShowString(6,3,APP_NAME,12,1);
	
	dw_data.msg_sn_idx = 2;
	dw_data.fram_len = 12;
	
    dwt_setrxaftertxdelay(dw_data.rx_after_tx_delay);
    dwt_setrxtimeout(dw_data.wait_resp_timeout);
	dwt_setsmarttxpower(1);

    while (1)
    {
        tx_poll_msg[dw_data.msg_sn_idx] = dw_data.frame_seq_num;
        dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);
        dwt_writetxdata(sizeof(tx_poll_msg), tx_poll_msg, 0); /* Zero offset in TX buffer. */
        dwt_writetxfctrl(sizeof(tx_poll_msg), 0, 1); /* Zero offset in TX buffer, ranging. */

        dwt_starttx(DWT_START_TX_IMMEDIATE | DWT_RESPONSE_EXPECTED);

		/*** wait for response message ***/
        while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR)))
        { };

        dw_data.frame_seq_num++;

        if (status_reg & SYS_STATUS_RXFCG)
        {
            dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);

            dw_data.fram_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFLEN_MASK;
            if (dw_data.fram_len <= FRAME_LEN_MAX)
            {
                dwt_readrxdata(rx_buffer, dw_data.fram_len, 0);
            }
            rx_buffer[dw_data.msg_sn_idx] = 0;
			
            if (memcmp(rx_buffer, rx_resp_msg, (dw_data.fram_len - 10)) == 0)
            {
                int32 rtd_init, rtd_resp;
				
                dw_data.tx_pool_ts = get_tx_timestamp_u64();
                dw_data.tx_resp_ts = get_rx_timestamp_u64();

                resp_msg_get_ts(&rx_buffer[dw_data.rx_pool_ts_idx], &(dw_data.rx_pool_ts));
                resp_msg_get_ts(&rx_buffer[dw_data.rx_resp_ts_idx], &(dw_data.rx_resp_ts));

                rtd_init = dw_data.tx_resp_ts - dw_data.tx_pool_ts;
                rtd_resp = dw_data.rx_resp_ts - dw_data.rx_pool_ts;

                tof = ((rtd_init - rtd_resp) / 2.0) * DWT_TIME_UNITS;
                distance = tof * SPEED_OF_LIGHT;
			
				/*** data filter ***/
				distance = filter_midavg(distance);
				
				OLED_ShowString(24,30,"distance:",12,1);
				sprintf(&dist_str[0],"%3.2fm",distance);
				OLED_ShowString(76,48,"  ",12,1);
				OLED_ShowString(36,48,dist_str,12,1);
				/*** upload distance data ***/
				ANO_DT_Send_Distance(distance);
            }
        }
        else
        {
            dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);

            dwt_rxreset();
        }
        delay_ms(dw_data.ranging_interval_ms);
    }
}


void dwm_ss_twr_resp(void)
{
	static uint64 resp_tx_ts;
	static uint8 rx_poll_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'W', 'A', 'V', 'E', 0xE0, 0, 0};
	static uint8 tx_resp_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'V', 'E', 'W', 'A', 0xE1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	
	dw_data.config.chan = 2;						/* Channel number. */
	dw_data.config.prf = DWT_PRF_64M;				/* Pulse repetition frequency. */
	dw_data.config.txPreambLength = DWT_PLEN_128;	/* Preamble length. */
	dw_data.config.rxPAC = DWT_PAC8;				/* Preamble acquisition chunk size. Used in RX only. */
	dw_data.config.txCode = 9;						/* TX preamble code. Used in TX only. */
	dw_data.config.rxCode = 9;						/* RX preamble code. Used in RX only. */
	dw_data.config.nsSFD = 0;						/* Use standard SFD (Boolean) */
	dw_data.config.dataRate = DWT_BR_6M8;			/* Data rate. */
	dw_data.config.phrMode = DWT_PHRMODE_STD;		/* PHY header mode. */
	dw_data.config.sfdTO = (129 + 8 - 8);			/* SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
	
	dw_sw_init();
	
	strcpy(APP_NAME,"SS TWR RESP");
	OLED_Clear();
	OLED_ShowString(0, 0,APP_NAME,12,1);
		
	dw_data.rx_resp_delay_uus = 330;
	dwt_setsmarttxpower(1);

    while (1)
    {
        dwt_rxenable(DWT_START_RX_IMMEDIATE);

		/*** wait for pool message ***/
        while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_ERR)))
        { };

        if (status_reg & SYS_STATUS_RXFCG)
        {
            dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);

            dw_data.fram_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFL_MASK_1023;
            if (dw_data.fram_len <= FRAME_LEN_MAX)
            {
                dwt_readrxdata(rx_buffer, dw_data.fram_len, 0);
            }
			
            rx_buffer[dw_data.msg_sn_idx] = 0;
            if (memcmp(rx_buffer, rx_poll_msg, dw_data.fram_len - 2) == 0)
            {
                uint32 resp_tx_time;
				uint64 delayed_tx_time;
				uint8 system_time[5];
				uint64 sys_time_u64 = 0;
				char oled_str[20];
                int ret;

                dw_data.rx_pool_ts = get_rx_timestamp_u64();											//get rx_pool timestamp
				
				delayed_tx_time = dw_data.rx_pool_ts + (dw_data.rx_resp_delay_uus * UUS_TO_DWT_TIME);	//caculate the delayed transmit timestamp

                resp_tx_time = delayed_tx_time >> 8;
                dwt_setdelayedtrxtime(resp_tx_time);													//set the delayed transmit timestamp

                resp_tx_ts = (((uint64)(resp_tx_time & 0xFFFFFFFEUL)) << 8) + dw_data.txantennaDelay;	//caculate the rx_resp timestamp

                resp_msg_set_ts(&tx_resp_msg[dw_data.rx_pool_ts_idx], dw_data.rx_pool_ts);				//write the rx_pool timestamp to response message
                resp_msg_set_ts(&tx_resp_msg[dw_data.rx_resp_ts_idx], resp_tx_ts);						//write the rx_resp timestamp to response message

                tx_resp_msg[dw_data.msg_sn_idx] = dw_data.frame_seq_num;								//set the frame number
                dwt_writetxdata(sizeof(tx_resp_msg), tx_resp_msg, 0);									//write response message to tx buffer
                dwt_writetxfctrl(sizeof(tx_resp_msg), 0, 1);
								
                ret = dwt_starttx(DWT_START_TX_DELAYED);												//set the delayed TX
				
				dwt_readsystime(system_time);
				sys_time_u64 = system_time[4];
				sys_time_u64 <<= 32;
				sys_time_u64 += system_time[0] + (((uint16)system_time[1]) << 8) + (((uint32)system_time[2]) << 16) + (((uint32)system_time[3]) << 24);
				
				sprintf((char*)&oled_str[0],"ts:0x%llX",dw_data.rx_pool_ts);
				OLED_ShowString(0,24,oled_str,12,1);
				
				sprintf((char*)&oled_str[0],"dl:0x%llX",delayed_tx_time);
				OLED_ShowString(0,36,oled_str,12,1);
				
				sprintf((char*)&oled_str[0],"tm:0x%llX",sys_time_u64);
				OLED_ShowString(0,48,oled_str,12,1);

                if (ret == DWT_SUCCESS)
                {
					OLED_ShowString(0,12,"Delayed tx ok !",12,1);
					
                    while (!(dwt_read32bitreg(SYS_STATUS_ID) & SYS_STATUS_TXFRS))
                    { };

                    dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);

                    dw_data.frame_seq_num++;
                }
				else
					OLED_ShowString(0,12,"Delayed tx fail",12,1);
            }
        }
        else
        {
            dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR);
            dwt_rxreset();
        }
	}

}

