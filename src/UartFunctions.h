#ifndef _UART_H_
#define _UART_H_

#include <Arduino.h>
#include <stdint.h>

#define RBUFF 40		// Max RS232 Buffer Size
#define MAXIMUM_DIGITS 6	// Max numerical digits to process
#define MAX_COMMAND_ENTRIES 6 

void ProcessSerial ( void );
u_int8_t ParseSerial ( char *str );
void ResetSerial (void);
void FlushSerialInput (void);

#endif // _UART_H_
