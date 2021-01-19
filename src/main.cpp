#include <Arduino.h>
#include <i2c_driver.h>
#include <imx_rt1060/imx_rt1060_i2c_driver.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <ssd1331.h>

SPISettings spi_tx(50000000, MSBFIRST, SPI_MODE3);

volatile bool gfx_writing = false;

SSD1331 gfx0(SPI1, 3, 4, 2, spi_tx, gfx_writing);
SSD1331 gfx1(SPI1, 6, 7, 5, spi_tx, gfx_writing);

GFXcanvas16 gfx_buf(96, 64);

int frames = 0;

void setup() {
	Slave.listen_range(0x60, 0x64);

	SPI1.begin();
	gfx0.begin();
	gfx1.begin();

	gfx_buf.setTextColor(SSD1331::Color565(255, 255, 255));

	gfx_buf.setRotation(2);

	delay(1);

	Serial5.begin(115200);
	Serial5.print("hello\r\n");

}

void loop() {
	++frames;

	gfx_buf.fillScreen(0);
	gfx_buf.setCursor(0, 0);
	gfx_buf.print((frames * 1000) / millis());
	gfx_buf.print(" ");
	gfx_buf.print(frames);
	gfx0.write(gfx_buf.getBuffer());
	gfx1.write(gfx_buf.getBuffer());
}
