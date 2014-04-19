#include <config.h>
#include <circ_buf.h>

#include <uart.h>

int main(void)
{
    init_serial();

    writeStr_serial("Hello World!");
    while(1) {
        char recv = read_serial();
        write_serial(recv);
    }

    // Begin writing the real program here...

    // Step 1: Load last used settings from EEPROM
}
