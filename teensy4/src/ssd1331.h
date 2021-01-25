/***************************************************
  This is a library for the 0.96" 16-bit Color OLED with SSD1331 driver chip

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/684

  These displays use SPI to communicate, 4 or 5 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#ifndef SRC_IO_SSD1331_H_
#define SRC_IO_SSD1331_H_

#include <Adafruit_GFX.h>

#include <EventResponder.h>
#include <SPI.h>

class SSD1331 {
 public:
  SSD1331(SPIClass& spi,
		  	  	  int dc,
                   int rst,
				   int cs,
				   SPISettings spi_tx,
				   volatile bool &writing);

  void begin();

  void setAddrWindow(uint16_t x, uint16_t y, uint16_t w,
                                       uint16_t h);
  void write(const uint16_t* buf, size_t len=BYTES);
  void wait();

  static uint16_t Color565(uint8_t r, uint8_t g, uint8_t b);

  static const int16_t WIDTH = 96;
  static const int16_t HEIGHT = 64;
  static const size_t BYTES = WIDTH * HEIGHT * 2;

 private:


  uint8_t *tx_buf_;

  int dc_;
  int rst_;
  int cs_;
  SPIClass *spi_;
  SPISettings spi_tx_;

  EventResponder responder_;
  static void doneWriting(EventResponderRef responder);

  volatile bool &writing_;
};

#endif
