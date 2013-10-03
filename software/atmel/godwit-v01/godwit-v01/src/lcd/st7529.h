/*************************************************
 * FILE:    st7529.h
 * AUTHOR:  ADG
 * DESC:    ST7529 LCD Controller Driver
 *************************************************/
#ifndef _ST7519_H_
#define _ST7519_H_

/* APPLICATION SPECIFIC INFO */

/* Target-specific information should be defined in
 * "st7529_board.h".  E.g. Pin mappings, delay functions,
 * and LCD parameters.
 */

/***** COMMANDS ******/

/* For simplicity/speed this library assumes the ST7529
 * idles with EXT=0, as most of the common commands 
 * utilize this setting.  This means that EXT=1 must be
 * set before and cleared after utilizing EXT=1 commands.
*/

// EXT = 0|1
//             COMMAND    HEX        FUNCTION             PARAMS
#define ST7529_EXTIN      0x30    // EXT=0 Set            NONE
#define ST7529_EXTOUT     0x31    // EXT=1 Set            NONE

// EXT = 0
//             COMMAND    HEX        FUNCTION             PARAMS
#define ST7529_DISON      0xAF    // Display On           NONE
#define ST7529_DISOFF     0xAE    // Display Off          NONE
#define ST7529_DISNOR     0xA6    // Normal Display       NONE
#define ST7529_DISINV     0xA7    // Inverse Display      NONE
#define ST7529_COMSCN     0xBB    // COM Scan Direction   1
#define ST7529_DISCTRL    0xCA    // Display Control      3
#define ST7529_SLPIN      0x95    // Sleep In             NONE
#define ST7529_SLPOUT     0x94    // Sleep Out            NONE
#define ST7529_LASET      0x75    // Line Address Set     2
#define ST7529_CASET      0x15    // Column Address Set   2
#define ST7529_DATSDR     0xBC    // Data Scan Direction  3
#define ST7529_RAMWR      0x5C    // Writing To Memory    DATA 
#define ST7529_RAMRD      0x5D    // Reading From Memory  DATA 
#define ST7529_PLTIN      0xA8    // Partial Display In   2
#define ST7529_PLTOUT     0xA9    // Partial Display Out  NONE
#define ST7529_RMWIN      0xE0    // Read, Modify, Write  NONE
#define ST7529_RMWOUT     0xEE    // RMW End              NONE
#define ST7529_ASCSET     0xAA    // Area Scroll Set      4
#define ST7529_SCSTART    0xAB    // Scroll Start Set     1
#define ST7529_OSCON      0xD1    // Internal OSC On      NONE
#define ST7529_OSCOFF     0xD2    // Internal OSC Off     NONE
#define ST7529_PWRCTRL    0x20    // Power Control        1
#define ST7529_VOLCTRL    0x81    // EC Control           2
#define ST7529_VOLUP      0xD6    // EC Increase By One   NONE
#define ST7529_VOLDOWN    0xD7    // EC Decrease By One   NONE
#define ST7529_RESERVED   0x82    // Not Used             NONE
#define ST7529_EPSRRD1    0x7C    // READ Register 1      NONE
#define ST7529_EPSRRD2    0x7D    // READ Register 2      NONE
#define ST7529_NOP        0x25    // NOP Instruction      NONE
#define ST7529_EPINT      0x07    // Initial Code         1

// EXT = 1
//             COMMAND    HEX        FUNCTION             PARAMS
#define ST7529_GRY1SET    0x20    // FRAME 1 Gray PWM Set 16
#define ST7529_GRY2SET    0x21    // FRAME 2 Gray PWM Set 16
#define ST7529_ANASET     0x32    // Analog Circuit Set   3
#define ST7529_SWINT      0x34    // Software Initial     NONE
#define ST7529_EPCTIN     0xCD    // Control EEPROM       1
#define ST7529_EPCOUT     0xCC    // Cancel EEPROM        NONE
#define ST7529_EPMWR      0xFC    // Write To EEPROM      NONE
#define ST7529_EPMRD      0xFD    // Read From EEPROM     NONE

/****** TYPEDEFS *******/

/* FUNCTION PROTOTYPES */

// Initializes ST7529
void st7529_init(void);
void st7529_put_5x7_text(uint8_t x, uint8_t y, char *str, uint32_t len);

#endif  // _ST7529_H_
