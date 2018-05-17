#ifndef DW_ALGORITHM_H
#define DW_ALGORITHM_H
#include "dw_app.h"

typedef enum filterAlgorithm{
	LIMIT,
	MID_VAL,
	MID_AVG,
}ALGORITHM;

extern float (*dw_data_proc_ptr)(float val);

void algori_param_init(void);
void algori_set_filter(ALGORITHM method);
float filter_limit(float newval);
float filter_midval(float newval);
float filter_midavg(float newval);


#endif
