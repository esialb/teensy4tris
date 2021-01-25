/*
 * gpu.h
 *
 *  Created on: Jan 25, 2021
 *      Author: robin
 */

#ifndef SRC_GFX_GPU_H_
#define SRC_GFX_GPU_H_

#include <stdint.h>

#include <map>

#include <avr/pgmspace.h>
#include <Adafruit_GFX.h>
#include "ssd1331.h"

class GPU {
public:
	GPU() = delete;

	struct Sprite {
		int16_t id;
		int16_t width;
		int16_t height;
		uint8_t* data;
	};

	struct XYSprite {
		int16_t id;
		int16_t x;
		int16_t y;
	};

	static std::map<int16_t, Sprite> sprites;
	static std::map<int16_t, XYSprite> xy_sprites;

	static SSD1331 gfx0;
	static SSD1331 gfx1;

	static GFXcanvas16 gfx0_buf;
	static GFXcanvas16 gfx1_buf;
};

#endif /* SRC_GFX_GPU_H_ */
