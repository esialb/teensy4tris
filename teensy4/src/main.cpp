#include <Arduino.h>
#include <i2c_driver.h>
#include <imx_rt1060/imx_rt1060_i2c_driver.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include "ssd1331.h"
#include "spi_slave.h"

SPISettings spi_tx(64000000, MSBFIRST, SPI_MODE3);

volatile bool gfx_writing = false;
SSD1331 gfx0(SPI1, 3, 4, 2, spi_tx, gfx_writing);
SSD1331 gfx1(SPI1, 6, 7, 5, spi_tx, gfx_writing);

GFXcanvas16 gfx_buf(96, 64);

int frames = 0;

volatile bool rx_started = false;
volatile bool rx_finished = false;

DMAMEM uint8_t i2c_rx_buf[256];
size_t spi_rx_len;
DMAMEM uint8_t spi_rx_buf[256];
size_t spi_tx_len;
DMAMEM uint8_t spi_tx_buf[256];

void i2c_after_receive(size_t len, uint16_t address) {
	rx_started = true;
	spi_rx_len = i2c_rx_buf[0];
	SPI_Slave::rx(spi_rx_buf, spi_rx_len);
}

void spi_after_receive(uint8_t* buf, size_t len) {
	rx_finished = true;
}

void setup() {
	Slave.listen_range(0x60, 0x60);
	Slave.set_receive_buffer(i2c_rx_buf, sizeof(i2c_rx_buf));
	Slave.after_receive(i2c_after_receive);

	SPI_Slave::begin();
	SPI_Slave::set_rx_isr(spi_after_receive);

	SPI1.begin();
	gfx0.begin();
	gfx1.begin();

	gfx_buf.setTextColor(SSD1331::Color565(255, 255, 255));
	gfx_buf.setRotation(2);
	gfx_buf.fillScreen(0);
	gfx_buf.setCursor(0, 0);
	gfx_buf.print("rx: waiting");
	gfx0.write(gfx_buf.getBuffer());
	gfx1.write(gfx_buf.getBuffer());

}

void loop() {
	yield();
	if (rx_started) {
		gfx_buf.fillScreen(0);
		gfx_buf.setCursor(0, 0);
		gfx_buf.print("rx: ");
		gfx_buf.print(i2c_rx_buf[0]);
		gfx0.write(gfx_buf.getBuffer());
		gfx1.write(gfx_buf.getBuffer());
		rx_started = false;
	}
	if (rx_finished) {
		gfx_buf.fillScreen(0);
		gfx_buf.setCursor(0, 0);
		gfx_buf.print("rx: ok\n");
		for (uint8_t i = 0; i < spi_rx_len; i++) {
			gfx_buf.print((char) spi_rx_buf[i]);
		}
		gfx0.write(gfx_buf.getBuffer());
		gfx1.write(gfx_buf.getBuffer());
		rx_finished = false;
		memcpy(spi_tx_buf, spi_rx_buf, spi_rx_len);
		spi_tx_len = spi_rx_len;
		SPI_Slave::tx(spi_tx_buf, spi_tx_len);
	}
}
