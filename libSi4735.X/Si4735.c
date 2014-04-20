#include "Si4735.h"
#include <i2c.h>

#define I2CBRG_400KHZ 37

/**
 * Initialize the first SI4735 module
 */
void initRadio(void)
{
    OpenI2C1(I2C_ON &
             I2C_IDLE_STOP &
             I2C_CLK_HLD &
             I2C_7BIT_ADD &
             I2C_SLW_EN &
             I2C_GCALL_DIS &
             I2C_STR_DIS &
             I2C_ACK &
             I2C_ACK_EN
             , I2CBRG_400KHZ);
}

