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

GPUSlave::State volatile GPUSlave::state = GPUSlave::CMD_I2C_WAIT;
uint16_t GPUSlave::i2c_receive_address;
DMAMEM uint8_t GPUSlave::i2c_receive_buf[256];
uint8_t *GPUSlave::spi_receive_buf = nullptr;

IMX_RT1060_I2CSlave &I2C_Slave = ::Slave;

void GPUSlave::begin() {
	I2C_Slave.listen_range(0x60, 0x63);
	I2C_Slave.set_receive_buffer(i2c_receive_buf, sizeof(i2c_receive_buf));
	I2C_Slave.after_receive(i2c_after_receive);

	SPISlave::begin();
	SPISlave::set_rx_callback(spi_after_receive);
}

void GPUSlave::tick() {
	if (state == State::CMD_READY) {
		GPU::render();
		state = State::CMD_I2C_WAIT;
	}
}

void GPUSlave::i2c_after_receive(size_t len, uint16_t address) {
	if (state != State::CMD_I2C_WAIT)
		return;

	i2c_receive_address = address;

	if (address == 0x60 && len == 6) {
		int16_t *buf = (int16_t*) i2c_receive_buf;
		int16_t id = buf[0];
		int16_t width = buf[1];
		int16_t height = buf[2];

		auto it = GPU::sprites.find(id);
		if (it != GPU::sprites.end()) {
			free(it->second.data);
			GPU::sprites.erase(id);
		}

		if (id != 0 && width != 0 && height != 0) {
			size_t datalen = sizeof(uint16_t) * width * height;
			spi_receive_buf = (uint8_t*) malloc(datalen);
			SPISlave::rx(spi_receive_buf, datalen);
			state = State::CMD_SPI_WAIT;
		} else {
			state = State::CMD_READY;
		}
	} else if (address == 0x61 && len == 6) {
		int16_t *buf = (int16_t*) i2c_receive_buf;
		int16_t id = buf[0];
		int16_t width = buf[1];
		int16_t height = buf[2];

		auto it = GPU::sprites.find(id);
		if (it == GPU::sprites.end())
			return;

		GPU::Sprite &sprite = it->second;
		if (sprite.mask != nullptr) {
			free(sprite.mask);
			sprite.mask = nullptr;
		}

		if (sprite.width != width || sprite.height != height) return;

		size_t datalen = sizeof(uint16_t) * sprite.width * sprite.height;
		spi_receive_buf = (uint8_t*) malloc(datalen);
		SPISlave::rx(spi_receive_buf, datalen);
		state = State::CMD_SPI_WAIT;

	} else if (address == 0x62 && len == 8) {
		int16_t *buf = (int16_t*) i2c_receive_buf;
		int16_t id = buf[0];
		int16_t sprite_id = buf[1];
		int16_t x = buf[2];
		int16_t y = buf[3];

		GPU::xy_sprites0.erase(id);
		if (id != 0 && sprite_id != 0) {
			GPU::XYSprite &xy_sprite = GPU::xy_sprites0[id];
			xy_sprite.id = id;
			xy_sprite.sprite_id = sprite_id;
			xy_sprite.x = x;
			xy_sprite.y = y;
		}
		state = State::CMD_READY;
	} else if (address == 0x63 && len == 8) {
		int16_t *buf = (int16_t*) i2c_receive_buf;
		int16_t id = buf[0];
		int16_t sprite_id = buf[1];
		int16_t x = buf[2];
		int16_t y = buf[3];

		GPU::xy_sprites1.erase(id);
		if (id != 0 && sprite_id != 0) {
			GPU::XYSprite &xy_sprite = GPU::xy_sprites1[id];
			xy_sprite.id = id;
			xy_sprite.sprite_id = sprite_id;
			xy_sprite.x = x;
			xy_sprite.y = y;
		}
		state = State::CMD_READY;
	}
}

void GPUSlave::spi_after_receive(uint8_t *spi_buf, size_t len) {
	if (state != State::CMD_SPI_WAIT)
		return;

	if (i2c_receive_address == 0x60) {
		int16_t *i2c_buf = (int16_t*) i2c_receive_buf;
		int16_t id = i2c_buf[0];
		int16_t width = i2c_buf[1];
		int16_t height = i2c_buf[2];

		GPU::Sprite &sprite = GPU::sprites[id];
		sprite.id = id;
		sprite.width = width;
		sprite.height = height;
		sprite.data = (uint16_t*) spi_receive_buf;

		spi_receive_buf = nullptr;
		state = State::CMD_READY;
	} else if (i2c_receive_address == 0x61) {
		int16_t *i2c_buf = (int16_t*) i2c_receive_buf;
		int16_t id = i2c_buf[0];

		GPU::Sprite &sprite = GPU::sprites[id];
		sprite.mask = (uint16_t*) spi_receive_buf;

		spi_receive_buf = nullptr;
		state = State::CMD_READY;
	}
}

