#ifndef __DATATRANS_H__
#define __DATATRANS_H__
#include "sys.h"

extern u8 data_to_send[50];


void DT_SendRCData(void);
void DT_SendData(u8 *dataToSend, u8 len);
void DT_SendDFData(void);





#endif
