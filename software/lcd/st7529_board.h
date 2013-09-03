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
 * st7529_D07_WRITE(uint8_t)
 * uint8_t st7529_D07_READ()
 *
 * ****** HELPER FUNCTIONS ******
 * st7529_delay_us(uint32_t)
 */

#include "gpio.h"

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

#define st7529_RST_DIR_IN()     gpio_local_disable_pin_output_driver(\
                                    ST7529_RST_PIN)
#define st7529_RST_DIR_OUT()    gpio_local_enable_pin_output_driver(\
                                    ST7529_RST_PIN)
#define st7529_RST_SET()        gpio_local_set_gpio_pin(ST7529_RST_PIN)
#define st7529_RST_CLR()        gpio_local_clr_gpio_pin(ST7529_RST_PIN)
#define st7529_CS_DIR_IN()     gpio_local_disable_pin_output_driver(\
                                    ST7529_CS_PIN)
#define st7529_CS_DIR_OUT()    gpio_local_enable_pin_output_driver(\
                                    ST7529_CS_PIN)
#define st7529_CS_SET()        gpio_local_set_gpio_pin(ST7529_CS_PIN)
#define st7529_CS_CLR()        gpio_local_clr_gpio_pin(ST7529_CS_PIN)
#define st7529_A0_DIR_IN()     gpio_local_disable_pin_output_driver(\
                                    ST7529_A0_PIN)
#define st7529_A0_DIR_OUT()    gpio_local_enable_pin_output_driver(\
                                    ST7529_A0_PIN)
#define st7529_A0_SET()        gpio_local_set_gpio_pin(ST7529_A0_PIN)
#define st7529_A0_CLR()        gpio_local_clr_gpio_pin(ST7529_A0_PIN)
#define st7529_RD_DIR_IN()     gpio_local_disable_pin_output_driver(\
                                    ST7529_RD_PIN)
#define st7529_RD_DIR_OUT()    gpio_local_enable_pin_output_driver(\
                                    ST7529_RD_PIN)
#define st7529_RD_SET()        gpio_local_set_gpio_pin(ST7529_RD_PIN)
#define st7529_RD_CLR()        gpio_local_clr_gpio_pin(ST7529_RD_PIN)
#define st7529_WR_DIR_IN()     gpio_local_disable_pin_output_driver(\
                                    ST7529_WR_PIN)
#define st7529_WR_DIR_OUT()    gpio_local_enable_pin_output_driver(\
                                    ST7529_WR_PIN)
#define st7529_WR_SET()        gpio_local_set_gpio_pin(ST7529_WR_PIN)
#define st7529_WR_CLR()        gpio_local_clr_gpio_pin(ST7529_WR_PIN)

#endif  // _ST7529_BOARD_H_
