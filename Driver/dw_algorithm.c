#include "dw_algorithm.h"

#define DATABUF_MAXLEN	50

typedef struct {
	uint8 data_buf_len;							//data buffer lenth for all algorithm
	float limit_max;							//max limit value for limit filter
	uint8 midVal_len;							//group lenth of middle value filter
	uint8 midavg_len;							//queue lenth of middle average filter
	
}algori_data_t;

float (*dw_data_proc_ptr)(float val);
algori_data_t algori_data;
float data_buf[DATABUF_MAXLEN] = {0};

/*** parameter initial for algorithm ***/
void algori_param_init(void)
{
	dw_data_proc_ptr = filter_midavg;
	algori_data.limit_max = 0.08;							//we assum that distance difference will not beyond 0.08m between twice measure.
	algori_data.midVal_len = 5;								//length of middle value filter data group
	algori_data.midavg_len = 50;
}

/*** set the current filter ***/
void algori_set_filter(ALGORITHM method)
{
	switch(method)
	{
		case LIMIT:		dw_data_proc_ptr = filter_limit;break;
		case MID_VAL:	dw_data_proc_ptr = filter_midval;break;
		case MID_AVG:	dw_data_proc_ptr = filter_midavg;break;
		
		default:dw_data_proc_ptr = filter_midavg;break;
	}
}

/*** 限幅 ***/
float filter_limit(float newval)
{
	static uint8 start_flag = 0;
	static uint16 invalid_cnt = 0;
	
	if(!start_flag){
		start_flag = 1;
		data_buf[0] = newval;
	}
	if(invalid_cnt > 100)
		data_buf[0] = newval;
	
	/*** invalid data ***/
	if((newval - data_buf[0]) > algori_data.limit_max || (data_buf[0] - newval) > algori_data.limit_max){
		invalid_cnt++;
		return data_buf[0];
	}
	else{
		data_buf[0] = newval;
		return newval;
	}
}

/*** 中位值 ***/
float filter_midval(float newval)
{
	static uint8 idx = 0;
	uint8 i = 0,j = 0;
	float temp;
	
	data_buf[idx++] = newval;
	
	if(idx >= (algori_data.midVal_len)){
		idx = 0;
		/*** 冒泡 ***/
		for(i = 0;i < algori_data.midVal_len;i++){
			for(j = 0;j < algori_data.midVal_len - 1 - j;j++){
				if(data_buf[j] > data_buf[j + 1]){
					temp = data_buf[j];
					data_buf[j] = data_buf[j+1];
					data_buf[j+1] = temp;
				}
			}
		}
		return data_buf[(algori_data.midVal_len - 1) / 2];
	}
}

/*** 滑动窗口平均值 ***/
float filter_midavg(float newval)
{
	static uint8 idx = 0;
	static uint8 min_idx = 0;
	static uint8 max_idx = 9;
	uint8 i = 0;
	float temp = 0;
	
	/*** queue idx ***/
	idx %= algori_data.midavg_len;						//range[0,9]
	
	if(newval < data_buf[min_idx])						//record the min value idx
		min_idx = idx;
	else if(newval > data_buf[max_idx])					//record the max value idx
		max_idx = idx;
	
	data_buf[idx++] = newval;							//storage the new data
	
	for(i = 0;i < algori_data.midavg_len;i++){
		temp += data_buf[i];							//
	}
	
	temp -= (data_buf[min_idx] + data_buf[max_idx]);	//minus the min & max value
	temp /= (algori_data.midavg_len - 2);				//caculate the average value
	
	return temp;
}




