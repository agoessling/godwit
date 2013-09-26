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

void setup_clocks(void);

int main (void)
{
	board_init();
	setup_clocks();
	
	gpio_local_init();
	
	gpio_local_enable_pin_output_driver(AVR32_PIN_PA14);
	
	while(1){
#define INSERT_GPIO_LOCAL_TGL_GPIO_PIN(idx, pin) \
    gpio_local_tgl_gpio_pin(pin);
    MREPEAT(128, INSERT_GPIO_LOCAL_TGL_GPIO_PIN, AVR32_PIN_PA14)
#undef INSERT_GPIO_LOCAL_TGL_GPIO_PIN
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
