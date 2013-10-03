/*************************************************
 * FILE:    st7529_board.h
 * AUTHOR:  ADG
 * DESC:    ST7529 LCD Controller Driver
 *************************************************/

#ifndef _ST7529_BOARD_H_
#define _ST7529_BOARD_H_

/*************************************************
 * These items are required for the ST7529
 * driver and are specific to the application:
 *
 * ****** SYSTEM PARAMETERS ********
 * ST7529_FCPU
 * ST7529_PWRCTRL_BOOSTER
 * ST7529_PWRCTRL_PARAM0 
 * ST7529_VOLCTRL_PARAM0 
 * ST7529_VOLCTRL_PARAM1 
 * ST7529_DISCTRL_PARAM0 
 * ST7529_DISCTRL_PARAM1 
 * ST7529_DISCTRL_PARAM2 
 * ST7529_COMSCN_PARAM0  
 * ST7529_DATSDR_PARAM0  
 * ST7529_DATSDR_PARAM1  
 * ST7529_DATSDR_PARAM2  
 * ST7529_LASET_PARAM0   
 * ST7529_LASET_PARAM1   
 * ST7529_CASET_PARAM0   
 * ST7529_CASET_PARAM1   
 * ST7529_ANASET_PARAM0  
 * ST7529_ANASET_PARAM1  
 * ST7529_ANASET_PARAM2  
 *
 * ********* PIN MAPPING ***********
 * st7529_RST_DIR_IN()
 * st7529_RST_DIR_OUT()
 * st7529_RST_SET()
 * st7529_RST_CLR()
 * st7529_CS_DIR_IN()
 * st7529_CS_DIR_OUT()
 * st7529_CS_SET()
 * st7529_CS_CLR()
 * st7529_A0_DIR_IN()
 * st7529_A0_DIR_OUT()
 * st7529_A0_SET()
 * st7529_A0_CLR()
 * st7529_RD_DIR_IN()
 * st7529_RD_DIR_OUT()
 * st7529_RD_SET()
 * st7529_RD_CLR()
 * st7529_WR_DIR_IN()
 * st7529_WR_DIR_OUT()
 * st7529_WR_SET()
 * st7529_WR_CLR()
 * st7529_D07_DIR_IN()
 * st7529_D07_DIR_OUT()
 * st7529_D07_WRITE(uint8_t)
 * uint8_t st7529_D07_READ()
 *
 * ****** HELPER FUNCTIONS ******
 * st7529_delay_us(uint32_t)
 */

#include "cycle_counter.h"
#include "gpio.h"

#include "st7529_board_lut.h"
#include "user_board.h"

/***** SYSTEM PARAMETERS ****/
#define ST7529_FCPU     CPU_FREQ    // 25MHz

// These are selected based on the 128240D
// from DisplayTech
#define ST7529_PWRCTRL_BOOSTER  0x08    // Booster Must Be On First
#define ST7529_PWRCTRL_PARAM0   0x0B    // Reference, Follower, Booster On
#define ST7529_VOLCTRL_PARAM0   0x2A    // Vop = 15.5V
#define ST7529_VOLCTRL_PARAM1   0x04    // Vop = 15.5V
#define ST7529_DISCTRL_PARAM0   0x00    // No Dividing
#define ST7529_DISCTRL_PARAM1   0x23    // Duty = 1/144
#define ST7529_DISCTRL_PARAM2   0x00    // Not Inverted
#define ST7529_COMSCN_PARAM0    0x02    // 79 -> 0, 80 ->159
#define ST7529_DATSDR_PARAM0    0x04    // Scan in Line Direction
#define ST7529_DATSDR_PARAM1    0x00    // Normal pixel Order
#define ST7529_DATSDR_PARAM2    0x02    // 3B3P Mode
#define ST7529_LASET_PARAM0     0x00    // Start Line = 0
#define ST7529_LASET_PARAM1     0x9F    // End Line = 159
#define ST7529_CASET_PARAM0     0x00    // Start Column = 0
#define ST7529_CASET_PARAM1     0x54    // End Column = 84
#define ST7529_ANASET_PARAM0    0x01    // OSC = 17.3kHz
#define ST7529_ANASET_PARAM1    0x00    // Booster = 3kHz
#define ST7529_ANASET_PARAM2    0x02    // Bias = 1/12

/******* PIN MAPPING ********/
#define ST7529_RST_PIN      AVR32_PIN_PA02
#define ST7529_CS_PIN       AVR32_PIN_PA01
#define ST7529_A0_PIN       AVR32_PIN_PA18
#define ST7529_RD_PIN       AVR32_PIN_PA05
#define ST7529_WR_PIN       AVR32_PIN_PB11
#define ST7529_D0_PIN       AVR32_PIN_PA10
#define ST7529_D1_PIN       AVR32_PIN_PA12
#define ST7529_D2_PIN       AVR32_PIN_PA09
#define ST7529_D3_PIN       AVR32_PIN_PA08
#define ST7529_D4_PIN       AVR32_PIN_PA03
#define ST7529_D5_PIN       AVR32_PIN_PB12
#define ST7529_D6_PIN       AVR32_PIN_PA22
#define ST7529_D7_PIN       AVR32_PIN_PA00

#define ST7529_D07_PORTA_PINS   ((1<<ST7529_D0_PIN)|(1<<ST7529_D1_PIN)\
                                |(1<<ST7529_D2_PIN)|(1<<ST7529_D3_PIN)\
                                |(1<<ST7529_D4_PIN)|(1<<ST7529_D6_PIN)\
                                |(1<<ST7529_D7_PIN))

#define ST7529_D07_PORTB_PINS  (1<<(ST7529_D5_PIN-32))

#define st7529_RST_DIR_IN()     gpio_local_disable_pin_output_driver(\
                                    ST7529_RST_PIN)
#define st7529_RST_DIR_OUT()    gpio_local_enable_pin_output_driver(\
                                    ST7529_RST_PIN)
#define st7529_RST_SET()        gpio_local_set_gpio_pin(ST7529_RST_PIN)
#define st7529_RST_CLR()        gpio_local_clr_gpio_pin(ST7529_RST_PIN)
#define st7529_CS_DIR_IN()      gpio_local_disable_pin_output_driver(\
                                    ST7529_CS_PIN)
#define st7529_CS_DIR_OUT()     gpio_local_enable_pin_output_driver(\
                                    ST7529_CS_PIN)
#define st7529_CS_SET()         gpio_local_set_gpio_pin(ST7529_CS_PIN)
#define st7529_CS_CLR()         gpio_local_clr_gpio_pin(ST7529_CS_PIN)
#define st7529_A0_DIR_IN()      gpio_local_disable_pin_output_driver(\
                                    ST7529_A0_PIN)
#define st7529_A0_DIR_OUT()     gpio_local_enable_pin_output_driver(\
                                    ST7529_A0_PIN)
#define st7529_A0_SET()         gpio_local_set_gpio_pin(ST7529_A0_PIN)
#define st7529_A0_CLR()         gpio_local_clr_gpio_pin(ST7529_A0_PIN)
#define st7529_RD_DIR_IN()      gpio_local_disable_pin_output_driver(\
                                    ST7529_RD_PIN)
#define st7529_RD_DIR_OUT()     gpio_local_enable_pin_output_driver(\
                                    ST7529_RD_PIN)
#define st7529_RD_SET()         gpio_local_set_gpio_pin(ST7529_RD_PIN)
#define st7529_RD_CLR()         gpio_local_clr_gpio_pin(ST7529_RD_PIN)
#define st7529_WR_DIR_IN()      gpio_local_disable_pin_output_driver(\
                                    ST7529_WR_PIN)
#define st7529_WR_DIR_OUT()     gpio_local_enable_pin_output_driver(\
                                    ST7529_WR_PIN)
#define st7529_WR_SET()         gpio_local_set_gpio_pin(ST7529_WR_PIN)
#define st7529_WR_CLR()         gpio_local_clr_gpio_pin(ST7529_WR_PIN)

#define st7529_D07_DIR_IN()     AVR32_GPIO_LOCAL.port[0].oderc\
                                    =ST7529_D07_PORTA_PINS;\
                                AVR32_GPIO_LOCAL.port[1].oderc\
                                    =ST7529_D07_PORTB_PINS

#define st7529_D07_DIR_OUT()    AVR32_GPIO_LOCAL.port[0].oders\
                                    =ST7529_D07_PORTA_PINS;\
                                AVR32_GPIO_LOCAL.port[1].oders\
                                    =ST7529_D07_PORTB_PINS

// LCD OFFSETS
#define ST7529_XOFFSET	16
#define ST7529_YOFFSET	0

inline static void st7529_D07_WRITE(uint8_t data){
    // Clear Old Values
    AVR32_GPIO_LOCAL.port[0].ovrc = ST7529_D07_PORTA_PINS;
    AVR32_GPIO_LOCAL.port[1].ovrc = ST7529_D07_PORTB_PINS;

    // Set Value
    AVR32_GPIO_LOCAL.port[0].ovrs = st7529_pin_map_lut[data].porta;
    AVR32_GPIO_LOCAL.port[1].ovrs = st7529_pin_map_lut[data].portb;
}

inline static uint8_t st7529_D07_READ(void){
    uint8_t data = 0;
    
    data |= (gpio_local_get_pin_value(ST7529_D0_PIN)<<0);
    data |= (gpio_local_get_pin_value(ST7529_D1_PIN)<<1);
    data |= (gpio_local_get_pin_value(ST7529_D2_PIN)<<2);
    data |= (gpio_local_get_pin_value(ST7529_D3_PIN)<<3);
    data |= (gpio_local_get_pin_value(ST7529_D4_PIN)<<4);
    data |= (gpio_local_get_pin_value(ST7529_D5_PIN)<<5);
    data |= (gpio_local_get_pin_value(ST7529_D6_PIN)<<6);
    data |= (gpio_local_get_pin_value(ST7529_D7_PIN)<<7);

    return data;
}

/********* HELPER FUNCTIONS ***********/
#define st7529_delay_us(delay)      cpu_delay_us(delay, ST7529_FCPU)
#define st7529_delay_ms(delay)      cpu_delay_ms(delay, ST7529_FCPU)

inline static void st7529_delay_write(void){
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
}

#endif  // _ST7529_BOARD_H_
