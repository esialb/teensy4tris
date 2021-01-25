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

#include <TeensyThreads.h>

class GPU {
public:
	GPU() = delete;

	struct Sprite {
		int16_t id;
		int16_t width;
		int16_t height;
		uint16_t* data;
		uint16_t* mask;
	};

	struct XYSprite {
		int16_t id;
		int16_t sprite_id;
		int16_t x;
		int16_t y;
	};

	static std::map<int16_t, Sprite> sprites;
	static std::map<int16_t, XYSprite> xy_sprites0;
	static std::map<int16_t, XYSprite> xy_sprites1;

	static SSD1331 gfx0;
	static SSD1331 gfx1;

	static GFXcanvas16 gfx0_buf;
	static GFXcanvas16 gfx1_buf;

	static void begin();
	static void render();
};

#endif /* SRC_GFX_GPU_H_ */
