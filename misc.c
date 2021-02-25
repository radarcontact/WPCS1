
#include "misc.h"
#include "stdint.h"
#include <stdbool.h>
#include "app_uart.h"

void simple_uart_putstring(const uint8_t * str)
{
    uint_fast8_t i  = 0;
    uint8_t      ch = str[i++];

    while (ch != '\0')
    {
        app_uart_put(ch);
        ch = str[i++];
    }
}


void debug_string(const char* str)
{


        simple_uart_putstring((const uint8_t *)"\n\r" );
        simple_uart_putstring((const uint8_t *)str );


}

void debug_print(const char* str, uint32_t value)
{
        char buffer[10];


        itoa(value, buffer, 16);
 simple_uart_putstring((const uint8_t *)"\n\r" );
        simple_uart_putstring((const uint8_t *)str);
        simple_uart_putstring((const uint8_t *)buffer );



}



void print_hex(uint32_t value)
{
    char buffer[10];
        itoa(value, buffer, 16);
    simple_uart_putstring((const uint8_t *)"\n\rHex: 0x");
    simple_uart_putstring((const uint8_t *)buffer );
    simple_uart_putstring((const uint8_t *)"\n");


}




 void uint32_to_uint8(uint32_t input,  uint8_t* output)
{
    //This places least significant byte in array position 0
    output[3] = input & 0xFF;
    output[2] = (input>>8) & 0xFF;
    output[1] = (input>>16) & 0xFF;
    output[0] = (input>>24) & 0xFF;


}

 void uint16_to_bytes(uint16_t input,  uint8_t* output)
{
    //This places least significant byte in array position 0
    output[0] = input & 0xFF;
    output[1] = (input>>8) & 0xFF;



}






uint16_t bytes_to_uint16(uint8_t* input)
{
    //This assumes least significant byte in array position 0
    uint16_t output;

    output = input[1];
    output = output | (input[0]<<8);


    return output;


}








uint32_t bytes_to_uint32(uint8_t* input)
{
    //This assumes least significant byte in array position 0
    uint32_t output;

    output = input[3];
    output = output | (input[2]<<8);
    output = output | (input[1]<<16);
    output = output | (input[0]<<24); 


    return output;

}





short bytes_to_int16(uint8_t* input)
{
    //This assumes least significant byte in array position 0
    short output;

    output = input[1];
    output = output | (input[0]<<8);


    return output;

}





int bytes_to_int32(uint8_t* input)
{
    //This assumes least significant byte in array position 0
    int output;

    output = input[3];
    output = output | (input[2]<<8);
    output = output | (input[1]<<16);
    output = output | (input[0]<<24); 


    return output;

}







void countUpandDown(count_up_down_t parameters)
{


    if(parameters.count_up == 1)
    {
        parameters.counter++; 
        if(parameters.counter == parameters.counter_max_value )
            parameters.count_up =0;   
    }
    else if(parameters.count_up == 0)
    {
        parameters.counter--;
        if(parameters.counter == parameters.counter_min_value )
            parameters.count_up =1; 
    }


}






int strlen0(const char * str) //implemntation of strlen0 from FreeBSD 6.2. Copied/pasted from wikipedia in order to avoid including the whole string.h library
//standard C function
{
    const char * s = str;
    for (; *s; ++s);
    return(s - str);
}

void reverse(char s[], int len)   //standard C function (utilize cplusplus.com or man for more information)
{
    int c, i, j;
    
    for (i = 0, j = len-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}


/// Implementation of itoa()
char* itoa(int num, char* str, int base)
{
    int i = 0;
    bool isNegative = false;
 
    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
 
    // In standard itoa(), negative numbers are handled only with 
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = true;
        num = -num;
    }
 
    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
 
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
 
    str[i] = '\0'; // Append string terminator
 
    // Reverse the string
    reverse(str, i);
 
    return str;
}





