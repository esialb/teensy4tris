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
#include <TeensyThreads.h>

DMAMEM int16_t GPUSlave::i2c_receive_buf[256];

IMX_RT1060_I2CSlave &I2C_Slave = ::Slave;

volatile GPUSlave::State GPUSlave::state = GPUSlave::CMD_I2C_WAIT;

void GPUSlave::begin() {
	I2C_Slave.listen_range(0x60, 0x62);
	I2C_Slave.set_receive_buffer((uint8_t*)i2c_receive_buf, sizeof(i2c_receive_buf));
	I2C_Slave.after_receive(i2c_after_receive);
	I2C_Slave.before_transmit(i2c_before_transmit);
	I2C_Slave.after_transmit(i2c_after_transmit);

	SPISlave::begin();
	SPISlave::set_rx_callback(spi_after_receive);
	SPISlave::set_tx_callback(spi_after_transmit);
}

void GPUSlave::tick() {
	if (state == State::CMD_READY) {
		GPU::render();
		state = State::CMD_I2C_WAIT;
	}
}

void GPUSlave::i2c_after_receive(size_t len, uint16_t address) {
	if (state != State::CMD_I2C_WAIT) return;

	if (address == 0x60 && len == 6) {
		int16_t *buf = (int16_t*) i2c_receive_buf;
		int16_t id = buf[0];
		int16_t width = buf[1];
		int16_t height = buf[2];

		auto it = GPU::sprites.find(id);
		if (it != GPU::sprites.end()) {
			free(it->second.data);
		}
		GPU::sprites.erase(it, GPU::sprites.end());

		if (id != 0 && width != 0 && height != 0) {
			GPU::Sprite& sprite = GPU::sprites[id];
			sprite.id = id;
			sprite.width = width;
			sprite.height = height;
			size_t len = 2 * width * height;
			sprite.data = malloc(len);
			SPISlave::rx((uint8_t*)sprite.data, len);
			state = State::CMD_SPI_WAIT;
		} else {
			state = State::CMD_READY;
		}
	} else if(address == 0x61 && len == 8) {
		int16_t *buf = (int16_t*) i2c_receive_buf;
		int16_t id = buf[0];
		int16_t sprite_id = buf[1];
		int16_t x = buf[2];
		int16_t y = buf[3];

		GPU::xy_sprites0.erase(id);
		if (id != 0 && sprite_id != 0) {
			GPU::XYSprite& xy_sprite = GPU::xy_sprites0[id];
			xy_sprite.id = id;
			xy_sprite.sprite_id = sprite_id;
			xy_sprite.x = x;
			xy_sprite.y = y;
		}
		state = State::CMD_READY;
	} else if(address == 0x62 && len == 8) {
		int16_t *buf = (int16_t*) i2c_receive_buf;
		int16_t id = buf[0];
		int16_t sprite_id = buf[1];
		int16_t x = buf[2];
		int16_t y = buf[3];

		GPU::xy_sprites1.erase(id);
		if (id != 0 && sprite_id != 0) {
			GPU::XYSprite& xy_sprite = GPU::xy_sprites1[id];
			xy_sprite.id = id;
			xy_sprite.sprite_id = sprite_id;
			xy_sprite.x = x;
			xy_sprite.y = y;
		}
		state = State::CMD_READY;
	}
}

void GPUSlave::i2c_before_transmit(uint16_t address) {
}

void GPUSlave::i2c_after_transmit(uint16_t address) {

}

void GPUSlave::spi_after_receive(uint8_t *buf, size_t len) {
	if (state != State::CMD_SPI_WAIT) return;
	state = State::CMD_READY;
}

void GPUSlave::spi_after_transmit(uint8_t *buf, size_t len) {

}

