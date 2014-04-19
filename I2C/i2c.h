/* 
 * File:   i2c.h
 * Author: alex
 *
 * Created on April 11, 2014, 9:24 PM
 */

#ifndef I2C_H
#define	I2C_H

#include "circ_buf.h"

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * Initialize the first I2C module
 */
void init_i2c1(void);

/**
 * Write a 16-bit value out the I2C1 port
 * @param word double byte to send
 * @return The word sent
 */
int writeWord_i2c1(int word);

/**
 * Write a byte to the I2C1 port
 * @param byte Byte to send
 * @return the byte sent
 */
unsigned char writeByte_i2c1(unsigned char byte);

/**
 * Read a 16-bit word from the I2C1 port
 * @return The least recently received word that hasn't been read yet
 */
int readWord_i2c1(void);

/**
 * Read a single byte from the I2C1 port
 * @return The least recently received byte from the port
 */
unsigned char readByte_i2c1(void);

#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */

