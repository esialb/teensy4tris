/*
 * gpu.cpp
 *
 *  Created on: Jan 25, 2021
 *      Author: robin
 */

#include "gpu.h"
#include <SPI.h>

#include <avr/pgmspace.h>

SPISettings spi_tx(64000000, MSBFIRST, SPI_MODE3);
volatile bool gfx_writing = false;

std::map<int16_t, GPU::Sprite> GPU::sprites;
std::map<int16_t, GPU::XYSprite> GPU::xy_sprites;


SSD1331 GPU::gfx0(SPI1, 3, 4, 2, spi_tx, gfx_writing);
SSD1331 GPU::gfx1(SPI1, 6, 7, 5, spi_tx, gfx_writing);

GFXcanvas16 GPU::gfx0_buf(96, 64);
GFXcanvas16 GPU::gfx1_buf(96, 64);
