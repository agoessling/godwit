/*************************************************
 * FILE:    st7529.c
 * AUTHOR:  ADG
 * DESC:    ST7529 LCD Controller Driver
 *************************************************/

#include <cycle_counter.h>
#include "st7529.h"
#include "st7529_board.h"
#include "st7529_5x7_font.h"

/*** LOCAL PROTOTYPES ***/
static inline void st7529_write_cmd(uint8_t);
static inline void st7529_write_data(uint8_t);
static inline uint8_t st7529_read_data(void);

// Initializes ST7529
void st7529_init(void){

    // Setup GPIO
    st7529_RST_DIR_OUT();   // Hold in reset
    st7529_RST_CLR();

    st7529_CS_DIR_OUT();
    st7529_CS_SET();
    st7529_A0_DIR_OUT();
    st7529_A0_SET();        // Idles High
    st7529_RD_DIR_OUT();
    st7529_RD_SET();
    st7529_WR_DIR_OUT();
    st7529_WR_SET();

    st7529_D07_DIR_OUT();   // Idles as output
	
	st7529_CS_CLR();
	st7529_delay_ms(1);
	
    st7529_RST_SET();       // Release reset
	st7529_delay_ms(10);
	
    st7529_write_cmd(ST7529_EXTIN);
    st7529_write_cmd(ST7529_SLPOUT);
    st7529_write_cmd(ST7529_OSCON);
    st7529_write_cmd(ST7529_PWRCTRL);
    st7529_write_data(ST7529_PWRCTRL_BOOSTER);  // Booster First
	st7529_delay_ms(100);
    st7529_write_cmd(ST7529_PWRCTRL);
    st7529_write_data(ST7529_PWRCTRL_PARAM0);
    st7529_write_cmd(ST7529_VOLCTRL);
    st7529_write_data(ST7529_VOLCTRL_PARAM0);
    st7529_write_data(ST7529_VOLCTRL_PARAM1);
    st7529_write_cmd(ST7529_DISCTRL);
    st7529_write_data(ST7529_DISCTRL_PARAM0);
    st7529_write_data(ST7529_DISCTRL_PARAM1);
    st7529_write_data(ST7529_DISCTRL_PARAM2);
    st7529_write_cmd(ST7529_DISINV);
    st7529_write_cmd(ST7529_COMSCN);
    st7529_write_data(ST7529_COMSCN_PARAM0);
    st7529_write_cmd(ST7529_DATSDR);
    st7529_write_data(ST7529_DATSDR_PARAM0);
    st7529_write_data(ST7529_DATSDR_PARAM1);
    st7529_write_data(ST7529_DATSDR_PARAM2);
    st7529_write_cmd(ST7529_LASET);
    st7529_write_data(ST7529_LASET_PARAM0);
    st7529_write_data(ST7529_LASET_PARAM1);
    st7529_write_cmd(ST7529_CASET);
    st7529_write_data(ST7529_CASET_PARAM0);
    st7529_write_data(ST7529_CASET_PARAM1);

    st7529_write_cmd(ST7529_EXTOUT);
    st7529_write_cmd(ST7529_ANASET);
    st7529_write_data(ST7529_ANASET_PARAM0);
    st7529_write_data(ST7529_ANASET_PARAM1);
    st7529_write_data(ST7529_ANASET_PARAM2);
    st7529_write_cmd(ST7529_SWINT);

    st7529_write_cmd(ST7529_EXTIN);
	
	st7529_write_cmd(ST7529_RAMWR);
	
	// Clear Display
	for(int i=0; i<41000; i++){
		st7529_write_data(0x00);
	}
	
	st7529_write_cmd(ST7529_DISON);
	
    st7529_CS_SET();
}

static inline void st7529_write_cmd(uint8_t data){
    st7529_A0_CLR();
    st7529_WR_CLR();
	
    st7529_D07_WRITE(data);
	
	st7529_delay_write();
	
    st7529_WR_SET();
    st7529_A0_SET();
	
	st7529_delay_write();
}

static inline void st7529_write_data(uint8_t data){
    st7529_WR_CLR();
	
    st7529_D07_WRITE(data);
	
	st7529_delay_write();

    st7529_WR_SET(); 
	
	st7529_delay_write();
}

static inline uint8_t st7529_read_data(void){
    uint8_t data;

    st7529_D07_DIR_IN();
    st7529_RD_CLR();

    data = st7529_D07_READ();

    st7529_RD_SET();
    st7529_D07_DIR_OUT();

    return data;
}

// This function draws 5x7 pixel text at the specified location
// With vertical orientation
// x: x lower left coordinate
// y: y lower left coordinate (must be a start block [0-84])
// str: character string to display (doesn't have to be null terminated)
// len: length of character string (not including null termination)
// NOTES:
// To avoid reads, this function assumes two rows of cleared pixels above
// each character.

void st7529_put_5x7_text(uint8_t x, uint8_t y, char *str, uint32_t len){

    st7529_CS_CLR();
	
	x = x + ST7529_XOFFSET;
	y = y + ST7529_YOFFSET;
	
    // Set Start and End Lines/Columns
    uint32_t end_line = x+6*len-1;
    uint32_t end_col = y/3+2;

    st7529_write_cmd(ST7529_LASET);
    st7529_write_data(x);
    st7529_write_data(end_line);

    st7529_write_cmd(ST7529_CASET);
    st7529_write_data(y/3);
    st7529_write_data(end_col);

    // Write Text Data
    st7529_write_cmd(ST7529_RAMWR);

    uint32_t iline;  
    uint8_t char_byte;

    for(iline=0; iline<6*len; iline++){
        if((iline % 6) == 5){
            st7529_write_data(0x00);
            st7529_write_data(0x00);
            st7529_write_data(0x00);
        }
        else{
            char_byte = st7529_5x7_font[(*(str+iline/6)-0x20)*5+iline%6];

            if(char_byte & (1<<6))
                st7529_write_data(0xFF);
            else
                st7529_write_data(0x00);

            if(char_byte & (1<<5))
                st7529_write_data(0xFF);
            else
                st7529_write_data(0x00);

            if(char_byte & (1<<4))
                st7529_write_data(0xFF);
            else
                st7529_write_data(0x00);
        }
    }

    for(iline=0; iline<6*len; iline++){
        if((iline % 6) == 5){
            st7529_write_data(0x00);
            st7529_write_data(0x00);
            st7529_write_data(0x00);
        }
        else{
            char_byte = st7529_5x7_font[(*(str+iline/6)-0x20)*5+iline%6];

            if(char_byte & (1<<3))
                st7529_write_data(0xFF);
            else
                st7529_write_data(0x00);

            if(char_byte & (1<<2))
                st7529_write_data(0xFF);
            else
                st7529_write_data(0x00);

            if(char_byte & (1<<1))
                st7529_write_data(0xFF);
            else
                st7529_write_data(0x00);
        }
    }

    for(iline=0; iline<6*len; iline++){
        if((iline % 6) == 5){
            st7529_write_data(0x00);
            st7529_write_data(0x00);
            st7529_write_data(0x00);
        }
        else{
            char_byte = st7529_5x7_font[(*(str+iline/6)-0x20)*5+iline%6];

            if(char_byte & (1<<0))
                st7529_write_data(0xFF);
            else
                st7529_write_data(0x00);

            st7529_write_data(0x00);
            st7529_write_data(0x00);
        }
    }

    st7529_CS_SET();
}
