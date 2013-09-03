/*************************************************
 * FILE:    st7529.c
 * AUTHOR:  ADG
 * DESC:    ST7529 LCD Controller Driver
 *************************************************/

#include "st7529.h"
#include "st7529_board.h"

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

    st7529_RST_SET();       // Release reset
    st7529_delay_us(20000);

    st7529_CS_CLR();

    st7529_write_cmd(ST7529_EXTIN);
    st7529_write_cmd(ST7529_SLPOUT);
    st7529_write_cmd(ST7529_OSCON);
    st7529_write_cmd(ST7529_PWRCTRL);
    st7529_write_data(ST7529_PWRCTRL_BOOSTER);  // Booster First
    st7529_delay_us(1000);
    st7529_write_cmd(ST7529_PWRCTRL);
    st7529_write_data(ST7529_PWRCTRL_PARAM0);
    st7529_write_cmd(ST7529_VOLCTRL);
    st7529_write_data(ST7529_VOLCTRL_PARAM0);
    st7529_write_data(ST7529_VOLCTRL_PARAM1);
    st7529_write_cmd(ST7529_DISCTRL);
    st7529_write_data(ST7529_DISCTRL_PARAM0);
    st7529_write_data(ST7529_DISCTRL_PARAM1);
    st7529_write_data(ST7529_DISCTRL_PARAM2);
    st7529_write_cmd(ST7529_DISNOR);
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
    st7529_write_cmd(ST7529_DISON);

    st7529_CS_SET();
}

static inline void st7529_write_cmd(uint8_t data){
    st7529_A0_CLR();
    st7529_WR_CLR();

    st7529_D07_WRITE(data);

    st7529_WR_SET(); 
    st7529_A0_SET();
}

static inline void st7529_write_data(uint8_t data){
    st7529_WR_CLR();

    st7529_D07_WRITE(data);

    st7529_WR_SET(); 
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

