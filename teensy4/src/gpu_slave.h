/*
 * gpu_slave.h
 *
 *  Created on: Jan 25, 2021
 *      Author: robin
 */

#ifndef SRC_IO_GPU_SLAVE_H_
#define SRC_IO_GPU_SLAVE_H_

#include <stddef.h>
#include <stdint.h>
#include <avr/pgmspace.h>

class GPUSlave {
public:
	static void begin();

private:
	static void i2c_after_receive(size_t len, uint16_t address);
	static void i2c_before_transmit(uint16_t address);
	static void i2c_after_transmit(uint16_t address);
	static void spi_after_receive(uint8_t* buf, size_t len);
	static void spi_after_transmit(uint8_t* buf, size_t len);

	static uint8_t i2c_receive_buf[256];
	static uint8_t i2c_transmit_buf[256];
};

#endif /* SRC_IO_GPU_SLAVE_H_ */
