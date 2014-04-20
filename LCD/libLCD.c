/**
 * Functions to interface with the 16x2 LCD screen on the 
 * Explorer 16 board over the PMP interface.
 * 
 * Author: Alex Stephens
 * Date: 2/5/2014
 */

#include "libLCD.h"
#include <p24FJ128GA010.h>

static char customMap[16];
static int nextCustomIdx = 0;

/**
 * Initialize the LCD module
 */
void initLCD(void)
{
    // Initialize the PMP port
    PMCON = 0x83BF;
    PMMODE = 0x03FF;
    PMAEN = 0x0001;

    // Initialize the LCD controller
    // Wait for its initialization sequence of ~30 ms to complete
    T1CON = 0x8030; // Fosc/2, prescale of 256
    LCD_DELAY(2000);

    PMADDR = LCD_CMDREG;
    PMDATA = LCD_CMD_FUNC;
    LCD_DELAY(3);

    PMDATA = LCD_CMD_DISPLAY;
    LCD_DELAY(3);

    PMDATA = LCD_CMD_CLEAR;
    LCD_DELAY(100);

    PMDATA = 0x06;  // increment cursor, no shift
    LCD_DELAY(100);
    
    // Initialize the custom character map
    int i = 0;
    for(i = 0; i < 16; ++i) {
        customMap[i] = LCD_E_INVAL;
    }
    nextCustomIdx = 0;
}

/**
 * Read an address contents from the LCD
 */
char readLCD(int addr)
{
    int dummy;
    while(PMMODEbits.BUSY);
    PMADDR = addr;
    dummy = PMDATA;
    while(PMMODEbits.BUSY);
    return (char)PMDATA;
}

/**
 * Write to an address contents from the LCD
 */
void writeLCD(int addr, char c)
{
    // Wait for the LCD controller to be free
    while(readLCD(LCD_CMDREG)&0x80); 
    // Wait for the PMP module to be free
    while(PMMODEbits.BUSY); 
    PMADDR = addr;
    PMDATA = c;
}

/**
 * Clear the LCD screen
 */
void clearLCD(void)
{
    writeLCD(LCD_CMDREG, LCD_CMD_CLEAR);
}

/**
 * Put the cursor at the home position
 */
void homeLCD(void)
{
    writeLCD(LCD_CMDREG, LCD_CMD_HOME);
}

/**
 * Set the location of the cursor.
 * @param [in] row Row of the LCD screen to place the cursor at
 * @param [in] col Column of the LCD screen to place the cursor at
 * @return LCD_E_INVAL if (row,col) is out of range, the character under
 *         the cursor otherwise.
 */
char setCursorLCD(int row, int col)
{
    if(row >= LCD_HEIGHT || row < 0)
        return LCD_E_INVAL;
    else if(col >= LCD_WIDTH || col < 0)
        return LCD_E_INVAL;
    // Otherwise, calculate the DDRAM address for the specified
    // row,col and set that address
    char setAddr = (row) ? 0x40 : 0x00;
    setAddr += col;
    writeLCD(LCD_CMDREG, 0x80 | (setAddr&0x7F));
    return getLCD();
}

/**
 * Display a character at the current cursor position
 * @param [in] c Character to display at the current cursor position
 * @return The character that was displayed
 */
char putLCD(char c)
{
    // Check to see if the character is in the characterMap
    unsigned char i = (unsigned char) c;
    if(i >= 0x80 && i < nextCustomIdx+0x80) {
        c = c-0x80;
    }
    writeLCD(LCD_DATAREG, c);
    return c;
}

/**
 * Display a character at a certain location on the LCD
 * @param [in] c Character to display at the given row,col
 * @param [in] row Row of the LCD screen to place the character at
 * @param [in] col Column of the LCD screen to place the character at
 * @return LCD_E_INVAL if the provided row,col is not on the LCD. The character printed otherwise
 */
char putRCLCD(char c, int row, int col)
{
    if(setCursorLCD(row, col) != LCD_E_INVAL) {
        putLCD(c); 
        return c;
    }
    else {
        return LCD_E_INVAL;
    }
}

/**
 * Display a string on the LCD, starting at the cursor's position
 * @param [in] str null-terminated string to display on the LCD
 * @param [in] wrapStyle Specify how line wrapping will occur.
 * @return Zero if the string was truncated when displayed, non-zero if the
 *         entire string was displayed.
 */
int putsLCD(const char *str, enum LineWrap wrapStyle)
{
    char addr = readLCD(LCD_CMDREG) & 0x7F;
    int charsWritten = 0;
    while(*str != '\0') {
        putLCD(*str++);
        ++charsWritten;
        ++addr;
        if(addr > 0xF && addr < 0x40) {
            switch(wrapStyle) {
                case LCD_SAMELINE:
                    addr = 0x00;
                    break;
                case LCD_NEXTLINE:
                    addr = 0x40;
                    break;
                case LCD_NOWRAP:
                    return charsWritten;
                    break;
            }
            // set the DDRAM address to 0x40
            writeLCD(LCD_CMDREG, addr | 0x80);
        }
        else if(addr > 0x4F) {
            switch(wrapStyle) {
                case LCD_SAMELINE:
                    addr = 0x40;
                    break;
                case LCD_NEXTLINE:
                    addr = 0x00;
                    break;
                case LCD_NOWRAP:
                    return charsWritten;
                    break;
            }
            writeLCD(LCD_CMDREG, addr | 0x80);
        }
    }
    return charsWritten;
}

/**
 * Get the character directly under the cursor position
 * @return The character directly under the cursor
 */
char getLCD(void)
{
    return readLCD(LCD_DATAREG);
}

/**
 * Get the string from the cursor to the end of the display, or bufsize-1,
 * whichever comes first.
 * @param [out] buf This buffer gets filled with the display's contents
 * @param [in] bufSize Total size of the buffer. Upto bufSize-1 characters
 *                     will be read into buf, with the last character
 *                     being reserved for '\0'.
 */
int getsnLCD(char *buf, int bufSize)
{
     char addr = readLCD(LCD_CMDREG) & 0x7F;
    // compare the string length against row,col
    char row = (addr < 0x40) ? 0 : 1;
    char col = (addr & 0xF);

    int count = 0, r = row, c = col;
    do {
        buf[count++] = getLCD();
        ++col;
        if(col >= 16) {
            col = 0;
            if(row)
                break;
            row = 1;
        }
        setCursorLCD(r, c);
    } while(count < bufSize-1);
    buf[count] = '\0';
    // return the cursor to its original position
    writeLCD(LCD_CMDREG, addr | 0x80);
    return count;
}

/**
 * Add a custom character to the character generation table.
 * @param charMap [in] map of the pixels which are on(1) and off(0) for the 
 *                     custom character. 1D array packing 2D data in row
 *                     major order.
 *                     Ex: 2x3 -> r1 r1 r1 r2 r2 r2
 * @param dimensions [in] Dimensions of the character map
 * @return The code to use when displaying this custom character
 */
char setCustomLCD(char *charMap, enum CharDim dimensions)
{
    int numRows;
    int padRows;
    switch(dimensions) {
        case LCD_5x7:
            numRows = 7;
            padRows = numRows+1;
            break;
        case LCD_5x10:
            numRows = 10;
            padRows = numRows+6;
            break;
    }
    // Store the DDRAM address
    char addr = readLCD(LCD_CMDREG) & 0x7F;

    // Set the address to point to the CGRAM at the address corresponding to
    // nextCustomIdx
    writeLCD(LCD_CMDREG, ((nextCustomIdx*8)&0x3F) | 0x40);
    
    // write the character data to CGRAM
    int i;
    for(i = 0; i < numRows; ++i) {
        writeLCD(LCD_DATAREG, charMap[i]&0x1F);
    }
    for(i=numRows; i < padRows; ++i) {
        writeLCD(LCD_DATAREG, 0x0);
    }
    customMap[nextCustomIdx] = IDXTOCODE(nextCustomIdx);
    ++nextCustomIdx;

    // Restore the LCD controller to write to the previous DDRAM address
    writeLCD(LCD_CMDREG, (addr&0x7F)|0x80);
    return customMap[nextCustomIdx-1];
}

