#include "uart.h"
#include <xc.h>

static struct circ_buf g_uartBuf;
static int g_err = ALL_GOOD;

void _ISRFAST _U1RXInterrupt(void)
{
    if(U1STAbits.OERR) {
        g_err = BUF_FULL;
        U1STAbits.OERR = 0;
    }
    else {
        int status = 0;
        while(U1STAbits.URXDA) {
            append_fast(g_uartBuf, (U1RXREG)&0xFFu, &status);
            if(status < 0) {
                g_err = BUF_FULL;
            }
        }
    }
    IFS0bits.U1RXIF = 0;
    return;
}

void init_serial( void)
{
    g_uartBuf.size = BUF_LEN;
    g_uartBuf.begin = 0;
    g_uartBuf.end = 0;
    U1BRG   = BRATE;
    U1MODE  = U_ENABLE;
    U1STA   = U_TX;

    // Hold CTS low
    AD1PCFGbits.PCFG1 = 1; // Set RB1 as digital
    TRISBbits.TRISB1 = 0; // Set RB1 as output
    PORTBbits.RB1 = 0; // Set CTS low for the execution of this program

    // enable interrupts
    IFS0bits.U1RXIF = 0;
    IEC0bits.U1RXIE = 1;

    g_err = ALL_GOOD;
}


// send a character to the UART2 serial port
int write_serial(char c)
{
    while ( U1STAbits.UTXBF);   // wait while Tx buffer full
    U1TXREG = c;
    g_err = ALL_GOOD;
    return c;
}


// wait for a new character to arrive to the UART2 serial port
char read_serial( void)
{
    while(available_serial() == 0) {};
    int recved = pop(&g_uartBuf);
    if(recved < 0) {
        g_err = BUF_EMPTY;
        return (unsigned char)255;
    }
    g_err = ALL_GOOD;
    return (unsigned char)recved;
}

// Check to see how many bytes are available in the buffer
int available_serial(void)
{
    int len = length(&g_uartBuf);
    if(len == 0) {
        g_err = BUF_EMPTY;
    }
    else if(len == g_uartBuf.size) {
        g_err = BUF_FULL;
    }
    return len;
}

void writeBuf_serial(const char *buf, unsigned int bufLen)
{
    int i = 0;
    for(i = 0; i < bufLen; ++i) {
        write_serial(buf[bufLen]);
    }
    g_err = ALL_GOOD;
    return;
}

void writeStr_serial(const char *str)
{
    int i = 0;
    while(str[i] != '\0') {
        write_serial(str[i]);
        ++i;
    }
    write_serial('\r');
    write_serial('\n');
    return;
}