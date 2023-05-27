
#ifndef MY_ADC_H
#define	MY_ADC_H

#include <xc.h> // include processor files - each processor file is guarded.  

void init_adc_no_lib();
int read_adc_raw_no_lib(unsigned char channel);
float read_adc_voltage(unsigned char channel);
//void send_string_no_lib (unsigned char *p);


#endif	/* MY_ADC_H */

