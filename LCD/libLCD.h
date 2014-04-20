#ifndef LIBLCD_H
#define LIBLCD_H 1
/**
 * Functions to interface with the 16x2 LCD screen on the 
 * Explorer 16 board over the PMP interface.
 * 
 * Author: Alex Stephens
 * Date: 2/5/2014
 */

#define LCD_WIDTH 16
#define LCD_HEIGHT 2
#define LCD_LENGTH 32

/**
 * Error codes - these character values are unused by CGRAM and CGROM so
 * alaising will not occur when using ROM code A00.
 * According to the datasheet the following ranges of values are not used:
 * 0x10 - 0x1F Error values
 * 0x80 - 0x9F Custom character codes (avoids aliasing with null)
 */
#define LCD_E_INVAL 0x10

#define IDXTOCODE(i) (i+0x80)
#define CODETOIDX(c) (c-0x80)

enum CharDim {
    LCD_5x7,
    LCD_5x10
};

enum LineWrap {
    LCD_SAMELINE,
    LCD_NEXTLINE,
    LCD_NOWRAP
};
#define LCD_CMDREG 0
#define LCD_DATAREG 1
#define PMDATA PMDIN1

/** Delay specified number of cycles @ 16us **/
#define LCD_DELAY(cycles) TMR1=0;do{}while(TMR1<(cycles))

/** LCD Command Register flags **/
#define LCD_STATUS_BUSY 0x80
#define LCD_STATUS_ADDR 0x7F

/** LCD Commands and execution time **/
// Clear the screen (1.64 ms)
#define LCD_CMD_CLEAR 0x01

// Place the cursor in the home position (1.64 ms)
#define LCD_CMD_HOME 0x2

// Set the entry mode to increment and not shift the display (40 us)
#define LCD_CMD_ENTRY 0x06

// Display on, Cursor display off, to not blink cursor (40 us)
#define LCD_CMD_DISPLAY 0x0C

// Cursor moves right instead of display shift (40 us)
#define LCD_CMD_CURSORCFG 0x14

// 8-bit data, 2 lines of characters, 5x7 font
#define LCD_CMD_FUNC 0x38

char readLCD(int addr);
void writeLCD(int addr, char c);

/**
 * Initialize the LCD module
 */
void initLCD(void);

/**
 * Clear the LCD screen
 */
void clearLCD(void);

/**
 * Put the cursor at the home position
 */
void homeLCD(void);

/**
 * Set the location of the cursor.
 * @param [in] row Row of the LCD screen to place the cursor at
 * @param [in] col Column of the LCD screen to place the cursor at
 * @return LCD_E_INVAL if (row,col) is out of range, the character under
 *         the cursor otherwise.
 */
char setCursorLCD(int row, int col);

/**
 * Display a character at the current cursor position, increments cursor's position
 * @param [in] c Character to display at the current cursor position
 * @return The character that was displayed
 */
char putLCD(char c);

/**
 * Display a character at a certain location on the LCD. Increments cursor's position
 * @param [in] c Character to display at the given row,col
 * @param [in] row Row of the LCD screen to place the character at
 * @param [in] col Column of the LCD screen to place the character at
 * @return LCD_E_INVAL if the provided row,col is not on the LCD. The character printed otherwise
 */
char putRCLCD(char c, int row, int col);

/**
 * Display a string on the LCD, starting at the cursor's position
 * @param [in] str null-terminated string to display on the LCD
 * @param [in] wrapStyle Specify how line wrapping will occur.
 * @return Zero if the string was truncated when displayed, non-zero if the
 *         entire string was displayed.
 */
int putsLCD(const char *str, enum LineWrap wrapStyle);

/**
 * Get the character directly under the cursor position. Does not change cursor position.
 * @return The character directly under the cursor
 */
char getLCD(void);

/**
 * Get the string from the cursor to the end of the display, or bufsize-1,
 * whichever comes first. Does not change the cursor's position.
 * @param [out] buf This buffer gets filled with the display's contents
 * @param [in] bufSize Total size of the buffer. Upto bufSize-1 characters
 *                     will be read into buf, with the last character
 *                     being reserved for '\0'.
 */
int getsnLCD(char *buf, int bufSize);

/**
 * Add a custom character to the character generation table.
 * @param charMap [in] map of the pixels which are on(1) and off(0) for the 
 *                     custom character. 1D array packing 2D data in row
 *                     major order.
 *                     Ex: 2x3 -> r1 r1 r1 r2 r2 r2
 * @param dimensions [in] Dimensions of the character map
 * @return The code to use when displaying this custom character
 */
char setCustomLCD(char *charMap, enum CharDim dimensions);

#endif // LIBLCD_H
