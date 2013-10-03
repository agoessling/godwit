/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>
#include <stdio.h>
#include "lcd/st7529.h"
#include "gps/uc530.h"

void setup_clocks(void);
void setup_gps_uart(void);
void setup_uart_dma(void);

volatile unsigned int usart_timeout_flag = 0;
volatile char usart_rx_buffer[600];
volatile uc530_data_t data;

ISR(usart_timeout_handler, AVR32_USART3_IRQ, 0){
	gpio_local_set_gpio_pin(AVR32_PIN_PA14);
	
	(&AVR32_USART3)->cr |= AVR32_USART_CR_STTTO_MASK;
	
	usart_timeout_flag = 1;
	
	uc530_parse_default(usart_rx_buffer,
						sizeof(usart_rx_buffer),
						&data);
						
	pdca_load_channel(0,
					  (void *)usart_rx_buffer,
					  sizeof(usart_rx_buffer));
					  
	gpio_local_clr_gpio_pin(AVR32_PIN_PA14);
}

int main (void)
{
	char text[50];
	
	board_init();
	setup_clocks();
	
	gpio_local_init();
	st7529_init();
	setup_gps_uart();
	setup_uart_dma();
	
	gpio_local_enable_pin_output_driver(AVR32_PIN_PA14);
	
	Disable_global_interrupt();
	
	INTC_init_interrupts();
	INTC_register_interrupt(&usart_timeout_handler, AVR32_USART3_IRQ, AVR32_INTC_INT0);
	
	Enable_global_interrupt();
	
	while(1){
		if(usart_timeout_flag){
			usart_timeout_flag = 0;
			
			sprintf(text,"Time: %d:%d:%f",
					data.hour,data.minute,data.second);
					
			st7529_put_5x7_text(0,0,text,17);
			
			sprintf(text,"Num Sats: %d", data.num_sats);
			
			st7529_put_5x7_text(0,9,text,12);
		}
	}
}

void setup_clocks(void){
	scif_disable_rc32out();
	
	pcl_freq_param_t pcl_params;
	scif_gclk_opt_t gclk_opts;
	
	pcl_params.main_clk_src = PCL_MC_DFLL0;
	pcl_params.cpu_f = CPU_FREQ;
	pcl_params.pba_f = CPU_FREQ;
	pcl_params.pbb_f = CPU_FREQ;
	pcl_params.pbc_f = CPU_FREQ;
	pcl_params.dfll_f = DFLL_FREQ;
	
	gclk_opts.clock_source = SCIF_GCCTRL_SLOWCLOCK;
	gclk_opts.diven = false;
	
	pcl_params.pextra_params = &gclk_opts;
	
	pcl_configure_clocks(&pcl_params);
}

void setup_gps_uart(void){
	static const gpio_map_t USART_GPIO_MAP =
	{
		{AVR32_USART3_RXD_0_1_PIN, AVR32_USART3_RXD_0_1_FUNCTION},
		{AVR32_USART3_TXD_0_1_PIN, AVR32_USART3_TXD_0_1_FUNCTION}
	};
	
	static const usart_options_t USART_OPTIONS =
	{
		.baudrate		= 9600,
		.charlength		= 8,
		.paritytype		= USART_NO_PARITY,
		.stopbits		= USART_1_STOPBIT,
		.channelmode	= USART_NORMAL_CHMODE
	};
	
	gpio_enable_module(USART_GPIO_MAP, 2);
	
	usart_init_rs232(&AVR32_USART3, &USART_OPTIONS, CPU_FREQ);
	
	(&AVR32_USART3)->ier = AVR32_USART_IER_TIMEOUT_MASK;
	(&AVR32_USART3)->rtor = 100;
	(&AVR32_USART3)->cr |= AVR32_USART_CR_STTTO_MASK;
}

void setup_uart_dma(void){
	static const pdca_channel_options_t PDCA_OPTIONS = {
		.pid = AVR32_PDCA_PID_USART3_RX,
		.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
		.addr = (void *)usart_rx_buffer,
		.size = sizeof(usart_rx_buffer),
		.r_addr = NULL,
		.r_size = 0
	};
	
	pdca_init_channel(0, &PDCA_OPTIONS);
	pdca_enable(0);
}