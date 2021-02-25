#include "stdint.h"


#ifndef MISC_
#define MISC_


#define COUNTER_MIN_VALUE           0
#define COUNTER_MAX_VALUE           255

typedef struct count_up_down
{         
	uint32_t 				 counter_min_value;
	uint32_t				 counter_max_value;
	uint32_t				 counter;
	uint8_t 				 count_up;
	
} count_up_down_t;




 void uint32_to_uint8(uint32_t input,  uint8_t* output);
 uint16_t bytes_to_uint16(uint8_t* input);
 uint32_t bytes_to_uint32(uint8_t* input);
 short bytes_to_int16(uint8_t* input);
 int bytes_to_int32(uint8_t* input);
 void countUpandDown(count_up_down_t parameters);
 int strlen0(const char * str);
 void reverse(char s[], int len);
 char* itoa(int num, char* str, int base);
void print_hex(uint32_t value);
void debug_string(const char* str);
void debug_print(const char* str, uint32_t value);
void simple_uart_putstring(const uint8_t * str);








#endif

