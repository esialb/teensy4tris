/*
 * gpu.cpp
 *
 *  Created on: Jan 25, 2021
 *      Author: robin
 */

#include "gpu.h"
#include <SPI.h>

#include <avr/pgmspace.h>
#include <TeensyThreads.h>
#include "gpu_slave.h"

SPISettings spi_tx(64000000, MSBFIRST, SPI_MODE3);
volatile bool gfx_writing = false;

DMAMEM std::map<int16_t, GPU::Sprite> GPU::sprites;
std::map<int16_t, GPU::XYSprite> GPU::xy_sprites0;
std::map<int16_t, GPU::XYSprite> GPU::xy_sprites1;

SSD1331 GPU::gfx0(SPI1, 3, 4, 2, spi_tx, gfx_writing);
SSD1331 GPU::gfx1(SPI1, 6, 7, 5, spi_tx, gfx_writing);

GFXcanvas16 GPU::gfx0_buf(96, 64);
GFXcanvas16 GPU::gfx1_buf(96, 64);

void GPU::begin() {
	SPI1.begin();
	gfx0.begin();
	gfx1.begin();
	gfx0_buf.setRotation(2);
	gfx1_buf.setRotation(2);
	gfx0.write(gfx0_buf.getBuffer());
	gfx1.write(gfx1_buf.getBuffer());
}

void GPU::render() {
	gfx0_buf.fillScreen(0);
	gfx1_buf.fillScreen(0);
	for (auto it = xy_sprites0.begin(); it != xy_sprites0.end(); ++it) {
		auto &xy_sprite = it->second;
		auto sprite_it = sprites.find(xy_sprite.sprite_id);
		if (sprite_it == sprites.end())
			continue;
		auto &sprite = sprite_it->second;
		gfx0_buf.drawRGBBitmap(xy_sprite.x, xy_sprite.y, sprite.data,
				sprite.width, sprite.height);
	}
	for (auto it = xy_sprites1.begin(); it != xy_sprites1.end(); ++it) {
		auto &xy_sprite = it->second;
		auto sprite_it = sprites.find(xy_sprite.sprite_id);
		if (sprite_it == sprites.end())
			continue;
		auto &sprite = sprite_it->second;
		gfx1_buf.drawRGBBitmap(xy_sprite.x, xy_sprite.y, sprite.data,
				sprite.width, sprite.height);
	}
	gfx0.write(gfx0_buf.getBuffer());
	gfx1.write(gfx1_buf.getBuffer());
}
