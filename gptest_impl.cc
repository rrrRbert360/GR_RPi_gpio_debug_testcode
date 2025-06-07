/* -*- c++ -*- */
/*
 * Copyright 2025 MyName. test debug version. Read gpio input pin 17 and output the status to pin 27.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "gptest_impl.h"
#include <gnuradio/io_signature.h>

#include <gpiod.hpp>
#ifndef	CONSUMER
#define	CONSUMER	"Consumer"
#endif

const char* chipname_B = "gpiochip0";
const int gpio_write_pin = 27;  										// GPIO27 LED
const int gpio_read_pin = 17;  											// GPIO17 EXT_CLK_input

gpiod_chip* global_chip_B = nullptr;
gpiod_line* global_write_line_B = nullptr;
gpiod_line* global_read_line_B = nullptr;

namespace gr {
namespace customModule {

using output_type = int;
gptest::sptr gptest::make() { return gnuradio::make_block_sptr<gptest_impl>(); }


/*
 * The private constructor
 */
gptest_impl::gptest_impl()
    : gr::sync_block("gptest",
                     gr::io_signature::make(0, 0, 0),
                     gr::io_signature::make(
                         1 /* min outputs */, 1 /*max outputs */, sizeof(output_type)))
{
}

/*
 * Our virtual destructor.
 */
gptest_impl::~gptest_impl() {}

bool gptest_impl::start() {
std::cout << "OOT module started" << std::endl;
    gpiod_chip* chip = gpiod_chip_open_by_name(chipname_B);				// Open GPIO chip
    if (!chip) {
		std::cout << "Error opening chip" << std::endl;
    }
	else {
		std::cout << "Succesfully opened chip" << std::endl;
	}	
    gpiod_line* write_line = gpiod_chip_get_line(chip, gpio_write_pin);	// Initialize output line
    if (!write_line) {
		std::cout << "Error setting output line" << std::endl; 
        gpiod_chip_close(chip);
    }
	else {
		std::cout << "Succesfully set output line" << std::endl;
	}
    if (gpiod_line_request_output(write_line, CONSUMER, 0) < 0) {		// Request output mode
		std::cout << "Request line as output failed\n" << std::endl; 
        gpiod_line_release(write_line);
        gpiod_chip_close(chip);
    }
	else {
		std::cout << "Succesfully requested line as output" << std::endl;
	}
    gpiod_line* read_line = gpiod_chip_get_line(chip, gpio_read_pin);	// Initialize input line 
    if (!read_line) {
		std::cout << "Error setting input line" << std::endl; 
        gpiod_chip_close(chip);
    }
	else {
		std::cout << "Succesfully set input line" << std::endl;
	}
	if (gpiod_line_request_input(read_line, CONSUMER) < 0) {			// Request input mode
		std::cout << "Request line as input failed\n" << std::endl; 
        gpiod_line_release(read_line);
        gpiod_chip_close(chip);
    }
	else {
		std::cout << "Succesfully requested line as input" << std::endl;
	}
    int ret = gpiod_line_set_value(write_line, 1);						// Set LED on
	if (ret < 0) {
	std::cout << "Set line output failed\n" << std::endl; 
	}
	else {
		std::cout << "Succesfully set lineoutput" << std::endl;
	}	
	global_chip_B = chip;							// Assign the value of `chip` to the global variable
	global_write_line_B = write_line;				// Assign the value of `write_line` to the global variable
	global_read_line_B = read_line;				// Assign the value of `read_line` to the global variable	
return true; // Return true if initialization is successful
}


bool gptest_impl::stop() {
std::cout << "OOT module stopped, see you next time!" << std::endl; 
	gpiod_chip* chip = global_chip_B;
	gpiod_line* write_line = global_write_line_B;
	gpiod_line* read_line = global_read_line_B;	
	 
	int ret = gpiod_line_set_value(write_line, 0);						// Set LED off
    if (ret < 0) {
		perror("Set line output failed\n");
	}
	std::cout << "Start to release output line" << std::endl; 			// Cleanup output line
	gpiod_line_release(write_line);	
	std::cout << "Outpunt line released " << std::endl; 

	std::cout << "Start to release input line" << std::endl; 			// Cleanup input line
	gpiod_line_release(read_line);	
	std::cout << "Input line released " << std::endl; 

	std::cout << "Start to close chip line" << std::endl; 	
	gpiod_chip_close(chip);
	std::cout << "Chip closed" << std::endl; 	
return true; // Return true if de-initialization is successful
}

int gptest_impl::work(int noutput_items,
                      gr_vector_const_void_star& input_items,
                      gr_vector_void_star& output_items)
{
	gpiod_line* write_line = global_write_line_B;
	gpiod_line* read_line = global_read_line_B;	

    auto out = static_cast<output_type*>(output_items[0]);



    for (int index = 0; index < noutput_items; index++) {				// Here the action the the module takes place !
 
    int state = gpiod_line_get_value(read_line);
	int ret = gpiod_line_set_value(write_line, state);
		if (ret < 0) {
			std::cout << "Set line output failed" << std::endl; 
		}
	if (state == 0) { out[index] = 0x00; }
    else{ out[index] = 0xFF; }

    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace customModule */
} /* namespace gr */
