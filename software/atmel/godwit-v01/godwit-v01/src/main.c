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
#include "barometer/bmp180.h"

#define SENS_STATE_PRESSURE	0
#define SENS_STATE_MAG		1
#define SENS_STATE_TEMP		2

#define TICK_RATE	20

void setup_clocks(void);
void setup_gps_uart(void);
void setup_uart_dma(void);
void setup_sens_twi(void);
void setup_magnetometer(void);
void setup_barometer(void);
void setup_interrupts(void);
void setup_tick(void);

void twim_write_dma(volatile avr32_twim_t *twim, uint8_t dma_ch, uint8_t saddr, uint8_t *bytes, uint8_t len);
void twim_write_blocking(volatile avr32_twim_t *twim, uint8_t saddr, uint8_t *bytes, uint8_t len);
uint8_t twim_read_blocking(volatile avr32_twim_t *twim, uint8_t saddr, uint8_t raddr);
void sens_rx_start(void);

volatile unsigned int usart_timeout_flag = 0;
volatile char usart_rx_buffer[600];
volatile uc530_data_t gps_data;

volatile uint8_t sens_tx_buffer[2] = {0xF4, 0xB4};
volatile uint8_t sens_rx_buffer[6];
volatile uint32_t sens_rx_flag = 0;
volatile uint32_t sens_rx_state = SENS_STATE_PRESSURE;

volatile uint32_t tick_cnt = 0;
volatile uint32_t last_cnt = 0;

// GPS USART TIMEOUT INTERRUPT
ISR(usart_timeout_handler, AVR32_USART3_IRQ, 0){
	// Reset Timeout Flag
	(&AVR32_USART3)->cr |= AVR32_USART_CR_STTTO_MASK;
	
	usart_timeout_flag = 1;
	
	// Parse GPS Data
	uc530_parse_default(usart_rx_buffer,
						sizeof(usart_rx_buffer),
						&gps_data);
	
	// Reload DMA Address					
	pdca_load_channel(0,
					  (void *)usart_rx_buffer,
					  sizeof(usart_rx_buffer));
			
	// Deal With Tick Timer
	last_cnt = tick_cnt;
	tick_cnt = 0;
	AVR32_TC0.channel[0].ccr = AVR32_TC_CCR0_SWTRG_MASK;	// Reset Counter
	AVR32_TC0.channel[0].sr;	// Reset any interrupt flags
	AVR32_TC0.channel[0].ier = AVR32_TC_IER0_CPCS_MASK;	// Enable Compare Match Interrupt
}

ISR(sens_rx_complete_handler, AVR32_PDCA_IRQ_1, 0){
	// Disable Interrupt By Writing To TCR
	AVR32_PDCA.channel[1].tcr = sizeof(sens_rx_buffer);
	
	switch(sens_rx_state){
		case SENS_STATE_PRESSURE:
			// Start Next Calculation
			twim_write_dma(&AVR32_TWIM0, 2, 0x77, sens_tx_buffer, 2);
		
			// Do stuff with Pressure Data
			
			// Initiate Mag Transfer
			sens_rx_state = SENS_STATE_MAG;
			sens_rx_start();
			break;
			
		case SENS_STATE_MAG:
			// Do Stuff With Data
			
			sens_rx_state = SENS_STATE_PRESSURE;
			break;
			
		default:
			break;			
	}
}

ISR(tick_handler, AVR32_TC0_IRQ0, 0){
	// Clear Interrupt
	AVR32_TC0.channel[0].sr;
	
	tick_cnt++;
	
	if(tick_cnt < TICK_RATE-1){
		// Do Regular Things
	}
	else
		// Disable Interrupt
		AVR32_TC0.channel[0].idr = AVR32_TC_IDR0_CPCS_MASK;
}

int main (void)
{
	char text[50];
	
	board_init();
	setup_clocks();
	
	gpio_local_init();
	
	gpio_local_enable_pin_output_driver(AVR32_PIN_PA14);
	gpio_local_enable_pin_output_driver(AVR32_PIN_PA20);
	
	st7529_init();
	setup_gps_uart();
	setup_uart_dma();
	setup_sens_twi();
	
	setup_magnetometer();
	setup_barometer();
	
	setup_tick();
	
	setup_interrupts();
	
	while(1){
		if(usart_timeout_flag){
			usart_timeout_flag = 0;
			
			sprintf(text,"Time: %d:%d:%f",
					gps_data.hour,gps_data.minute,gps_data.second);
					
			st7529_put_5x7_text(0,0,text,17);
			
			sprintf(text,"Num Sats: %d", last_cnt);//gps_data.num_sats);
			
			st7529_put_5x7_text(0,9,text,12);
			
			sens_rx_start();
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

void setup_sens_twi(void){
	// Setup Pins
	gpio_enable_gpio_pin(AVR32_PIN_PA21);
	gpio_enable_gpio_pin(AVR32_PIN_PB05);
	
	const gpio_map_t TWIM_PIN_MAP = {
		{AVR32_TWIMS0_TWD_0_1_PIN, AVR32_TWIMS0_TWD_0_1_FUNCTION},
		{AVR32_TWIMS0_TWCK_0_3_PIN, AVR32_TWIMS0_TWCK_0_3_FUNCTION}
	};
	
	gpio_enable_module(TWIM_PIN_MAP, 2);
	
	// Setup TWI
	AVR32_TWIM0.cr = AVR32_TWIM_CR_MEN_MASK;	// Enable Master
	AVR32_TWIM0.scr = ~0UL;	// Clear all Status
	
	twim_set_speed(&AVR32_TWIM0, 200000UL, PBA_FREQ);	// Set to 200kHz
	
	// Setup DMA
	// RX
	static const pdca_channel_options_t PDCA_OPTIONS1 = {
		.pid = AVR32_PDCA_PID_TWIM0_RX,
		.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
		.addr = (void *)sens_rx_buffer,
		.size = 6,
		.r_addr = NULL,
		.r_size = 0
	};
	
	pdca_init_channel(1, &PDCA_OPTIONS1);
	pdca_enable_interrupt_transfer_complete(1);
	pdca_enable(1);
	
	// TX
	static const pdca_channel_options_t PDCA_OPTIONS2 = {
		.pid = AVR32_PDCA_PID_TWIM0_TX,
		.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
		.addr = (void *)sens_tx_buffer,
		.size = 0,
		.r_addr = NULL,
		.r_size = 0
	};
	
	pdca_init_channel(2, &PDCA_OPTIONS2);
	pdca_enable(2);
}

void twim_write_dma(volatile avr32_twim_t *twim, uint8_t dma_ch, uint8_t saddr, uint8_t *bytes, uint8_t len){
	// Load DMA
	pdca_load_channel(dma_ch, (void *)bytes, len);
	
	// Wait For TWIM Idle
	while(!(twim->sr & AVR32_TWIM_SR_IDLE_MASK)){
		cpu_relax();
	}
	
	twim->cmdr = (saddr << AVR32_TWIM_CMDR_SADR_OFFSET)	// Slave Address
	| (len << AVR32_TWIM_CMDR_NBYTES_OFFSET)	// Length
	| (AVR32_TWIM_CMDR_VALID_MASK)				// Valid Command
	| (AVR32_TWIM_CMDR_START_MASK)				// Start Transmission
	| (AVR32_TWIM_CMDR_STOP_MASK)				// Stop after NBYTES
	| (0 << AVR32_TWIM_CMDR_READ_OFFSET);		// Write Operation
}

void twim_write_blocking(volatile avr32_twim_t *twim, uint8_t saddr, uint8_t *bytes, uint8_t len){
	// Wait For TWIM Idle
	while(!(twim->sr & AVR32_TWIM_SR_IDLE_MASK)){
		cpu_relax();
	}
	
	twim->cmdr = (saddr << AVR32_TWIM_CMDR_SADR_OFFSET)	// Slave Address
			| (len << AVR32_TWIM_CMDR_NBYTES_OFFSET)	// Length
			| (AVR32_TWIM_CMDR_VALID_MASK)				// Valid Command
			| (AVR32_TWIM_CMDR_START_MASK)				// Start Transmission
			| (AVR32_TWIM_CMDR_STOP_MASK)				// Stop after NBYTES
			| (0 << AVR32_TWIM_CMDR_READ_OFFSET);		// Write Operation
	
	twim->thr = *bytes++;	// Load data
	
	// Transmit len-1 more bytes		
	for(uint32_t i=0; i<(len-1); i++){
		// Wait for TXRDY
		while(!(twim->sr & AVR32_TWIM_SR_TXRDY_MASK)){
			cpu_relax();
		}
		
		twim->thr = *bytes++;
	}
}

uint8_t twim_read_blocking(volatile avr32_twim_t *twim, uint8_t saddr, uint8_t raddr){
	// Wait For TWIM Idle
	while(!(twim->sr & AVR32_TWIM_SR_IDLE_MASK)){
		cpu_relax();
	}
	
	// Write Read Address
	twim->cmdr = (saddr << AVR32_TWIM_CMDR_SADR_OFFSET)	// Slave Address
	| (1 << AVR32_TWIM_CMDR_NBYTES_OFFSET)		// 1 Byte
	| (AVR32_TWIM_CMDR_VALID_MASK)				// Valid Command
	| (AVR32_TWIM_CMDR_START_MASK)				// Start Transmission
	| (0 << AVR32_TWIM_CMDR_READ_OFFSET);		// Write Operation
	
	// Read Byte
	twim->ncmdr = (saddr << AVR32_TWIM_CMDR_SADR_OFFSET)	// Slave Address
	| (1 << AVR32_TWIM_CMDR_NBYTES_OFFSET)		// 1 Byte
	| (AVR32_TWIM_CMDR_VALID_MASK)				// Valid Command
	| (AVR32_TWIM_CMDR_START_MASK)				// Start Transmission
	| (AVR32_TWIM_CMDR_STOP_MASK)				// Stop At Last Byte
	| (1 << AVR32_TWIM_CMDR_READ_OFFSET);		// Read Operation
		
	// Start Read Address Write
	twim->thr = raddr;
	
	// Wait For Read Byte
	while(!(twim->sr & AVR32_TWIM_SR_RXRDY_MASK)){
		cpu_relax();
	}
	
	return twim->rhr;
}

void setup_magnetometer(void){
	// Device Address = 0x0E
	// CTRL_REG1 = 0x10
	// ODR = 5Hz, OSR = 64, Enable => 0x51
	static const uint8_t bytes1[] = {0x10, 0x51};
	twim_write_blocking(&AVR32_TWIM0,0x0E,bytes1,2);
	
	// CTRL_REG2 = 0x11
	// AUTO_MRST_EN = 1 => 0x80
	static const uint8_t bytes2[] = {0x11, 0x80};
	twim_write_blocking(&AVR32_TWIM0,0x0E,bytes2,2);
}

void setup_barometer(void){
	// Disable DMA
	pdca_disable(1);
	
	uint32_t data = 0;
	volatile avr32_twim_t *twim = &AVR32_TWIM0;
	
	// Get Calibration Data	
	for(uint32_t i=0; i<11; i++){
		// Get MSB
		data = twim_read_blocking(twim, 0x77, (0xAA+2*i));
		// Get LSB
		data = (data<<8)|twim_read_blocking(twim, 0x77, 0xAA+2*i+1);
		
		*((uint16_t *)&bmp180_cal_data+i) = data;
	}
	
	pdca_enable(1);
}

void sens_rx_start(void){
	uint8_t length;
	uint8_t saddr;
	uint8_t raddr;
	
	switch(sens_rx_state){
		case SENS_STATE_PRESSURE:
		case SENS_STATE_TEMP:
			length = 3;
			saddr = 0x77;
			raddr = 0xF6;
			break;
			
		case SENS_STATE_MAG:
			length = 6;
			saddr = 0x0E;
			raddr = 0x01;
			break;
			
		default:
			return;
	}
	
	// Setup DMA
	pdca_load_channel(1,(void *)sens_rx_buffer,length);
	
	// Wait For TWIM Idle
	while(!(AVR32_TWIM0.sr & AVR32_TWIM_SR_IDLE_MASK)){
		cpu_relax();
	}
	
	// Read Address write
	AVR32_TWIM0.cmdr = (saddr << AVR32_TWIM_CMDR_SADR_OFFSET)	// Slave Address
	| (1 << AVR32_TWIM_CMDR_NBYTES_OFFSET)		// 1 Byte
	| (AVR32_TWIM_CMDR_VALID_MASK)				// Valid Command
	| (AVR32_TWIM_CMDR_START_MASK)				// Start Transmission
	| (0 << AVR32_TWIM_CMDR_READ_OFFSET);		// Write Operation
	
	// Read Bytes
	AVR32_TWIM0.ncmdr = (saddr << AVR32_TWIM_CMDR_SADR_OFFSET)	// Slave Address
	| (length << AVR32_TWIM_CMDR_NBYTES_OFFSET)		// Num Bytes
	| (AVR32_TWIM_CMDR_VALID_MASK)				// Valid Command
	| (AVR32_TWIM_CMDR_START_MASK)				// Start Transmission
	| (AVR32_TWIM_CMDR_STOP_MASK)				// Stop At Last Byte
	| (1 << AVR32_TWIM_CMDR_READ_OFFSET);		// Read Operation
	
	// Initial Read Address Write
	AVR32_TWIM0.thr = raddr;
}

void setup_tick(void){
	// TC Settings
	static const tc_waveform_opt_t waveform_opt = {
		.channel = 0,
		.bswtrg = TC_EVT_EFFECT_NOOP,
		.beevt = TC_EVT_EFFECT_NOOP,
		.bcpc = TC_EVT_EFFECT_NOOP,
		.bcpb = TC_EVT_EFFECT_NOOP,
		.aswtrg = TC_EVT_EFFECT_NOOP,
		.aeevt = TC_EVT_EFFECT_NOOP,
		.acpc = TC_EVT_EFFECT_NOOP,
		.acpa = TC_EVT_EFFECT_NOOP,
		.wavsel = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,	// Clear on Compare Match
		.enetrg = false,
		.eevt = 0,
		.eevtedg = TC_SEL_NO_EDGE,
		.cpcdis = false,
		.cpcstop = false,
		.burst = false,
		.clki = false,
		.tcclks = TC_CLOCK_SOURCE_TC5	// PBA/128
	};
	
	// Compare Match Interrupt Settings
	static const tc_interrupt_t tc_interrupt = {
		.etrgs = 0,
		.ldrbs = 0,
		.ldras = 0,
		.cpcs = 1,	// Compare Match Interrupt
		.cpbs = 0,
		.cpas = 0,
		.lovrs = 0,
		.covfs = 0
	};
	
	tc_init_waveform(&AVR32_TC0, &waveform_opt);
	// Achieve TICK_RATE ticks, but empty slot before GPS packet
	// for margin.
	tc_write_rc(&AVR32_TC0, 0, PBA_FREQ/128/(TICK_RATE+1));
	tc_configure_interrupts(&AVR32_TC0, 0, &tc_interrupt);
	
	tc_start(&AVR32_TC0, 0);
}

void setup_interrupts(void){
	Disable_global_interrupt();
	
	INTC_init_interrupts();
	INTC_register_interrupt(&usart_timeout_handler, AVR32_USART3_IRQ, AVR32_INTC_INT0);
	INTC_register_interrupt(&sens_rx_complete_handler, AVR32_PDCA_IRQ_1, AVR32_INTC_INT0);
	INTC_register_interrupt(&tick_handler, AVR32_TC0_IRQ0, AVR32_INTC_INT0);
	
	Enable_global_interrupt();
}