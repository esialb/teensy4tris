/*
 * gpu_slave.cpp
 *
 *  Created on: Jan 25, 2021
 *      Author: robin
 */

#include "gpu_slave.h"

#include <i2c_driver.h>
#include <imx_rt1060/imx_rt1060_i2c_driver.h>
#include <spi_slave.h>
#include <SPI.h>
#include "ssd1331.h"
#include "gpu.h"

DMAMEM uint8_t GPUSlave::i2c_receive_buf[256];
DMAMEM uint8_t GPUSlave::i2c_transmit_buf[256];

IMX_RT1060_I2CSlave &I2C_Slave = ::Slave;


void GPUSlave::begin() {
	I2C_Slave.listen_range(0x60, 0x62);
	I2C_Slave.set_receive_buffer(i2c_receive_buf, sizeof(i2c_receive_buf));
	I2C_Slave.set_transmit_buffer(i2c_transmit_buf, sizeof(i2c_transmit_buf));
	I2C_Slave.after_receive(i2c_after_receive);
	I2C_Slave.before_transmit(i2c_before_transmit);
	I2C_Slave.after_transmit(i2c_after_transmit);

	SPISlave::begin();
	SPISlave::set_rx_callback(spi_after_receive);
	SPISlave::set_tx_callback(spi_after_transmit);
}

void GPUSlave::i2c_after_receive(size_t len, uint16_t address) {
	if (address == 0x60) {
		int16_t *buf = (int16_t*) i2c_receive_buf;
	}
}

void GPUSlave::i2c_before_transmit(uint16_t address) {
}

void GPUSlave::i2c_after_transmit(uint16_t address) {

}

void GPUSlave::spi_after_receive(uint8_t *buf, size_t len) {

}

void GPUSlave::spi_after_transmit(uint8_t *buf, size_t len) {

}

