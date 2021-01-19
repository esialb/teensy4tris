#include <Arduino.h>
#include <i2c_driver.h>
#include <imx_rt1060/imx_rt1060_i2c_driver.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <ssd1331.h>

SPISettings spi_tx(75000000, MSBFIRST, SPI_MODE3);
SSD1331 gfx(SPI, 8, 9, 10, spi_tx);

GFXcanvas16 gfx_buf(96, 64);

int frames = 0;

void setup() {
  Slave.listen(0x60);
  Slave.listen(0x64);
  Slave.listen(0x68);

  SPI.begin();
  gfx.begin();

  gfx_buf.setTextColor(gfx.Color565(255, 255, 255));

  gfx_buf.setRotation(2);

  delay(1);
}

void loop() {
	++frames;

	gfx_buf.fillScreen(0);
	gfx_buf.setCursor(0, 0);
	gfx_buf.print((frames * 1000) / millis());
	gfx_buf.print(" ");
	gfx_buf.print(frames);
	gfx.write(gfx_buf.getBuffer());
}
