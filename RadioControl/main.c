#include <config.h>
#include <circ_buf.h>

#include <uart.h>

int main(void)
{
    init_serial();
    init_lcd();
    init_i2c1();

    writeStr_serial("Hello World!");
    while(1) {
        while(!U1STAbits.URXDA);
        char recv = U1RXREG;
        write_serial(recv);
    }

    // Begin writing the real program here...

    // Step 1: Load last used settings from EEPROM
}
