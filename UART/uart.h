/* 
 * File:   uart.h
 * Author: alex
 *
 * Created on March 29, 2014, 1:13 PM
 */

#ifndef UART_H
#define	UART_H 1

#ifdef	__cplusplus
extern "C" {
#endif

#include <circ_buf.h>

#define ALL_GOOD 0
#define BUF_EMPTY 1
#define BUF_FULL 2

// timing and baud rate calculations
#define BRATE    34         // 115200 baud (BREGH=1)
#define U_ENABLE 0x8008     // enable the UART peripheral, 8N1
#define U_TX     0x0400     // enable transmission, rx interrupts every byte

/**
 * Initialize UART2 module
 */
void init_serial(void);

/**
 * Get the least recently received byte from the buffer
 * @return Least recently received byte
 */
char read_serial(void);

/**
 * Check to see how many bytes are available to read in
 * @return Number of available bytes
 */
int available_serial(void);

/**
 * Write a byte out of the UART2 port
 * @param c Byte to send out
 * @return The byte that was sent
 */
int write_serial(char c);

/**
 * Write the contents of a buffer out UART2
 * @param buf Buffer to send
 * @param bufLen Number of bytes to send in the buffer
 */
void writeBuf_serial(const char *buf, unsigned int bufLen);

/**
 * Send a null terminated string out this serial port.
 * An EOL sequence will be sent automatically.
 * @param [in] str Null terminated string
 */
void writeStr_serial(const char *str);

#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

