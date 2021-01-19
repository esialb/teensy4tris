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

#include "ssd1331.h"
#include "string.h"

#define gfx_swap(a, b) \
  {                    \
    uint16_t t = a;    \
    a = b;             \
    b = t;             \
  }

// Select one of these defines to set the pixel color order
#define SSD1331_COLORORDER_RGB
// #define SSD1331_COLORORDER_BGR

#if defined SSD1331_COLORORDER_RGB && defined SSD1331_COLORORDER_BGR
#error "RGB and BGR can not both be defined for SSD1331_COLORODER."
#endif

// Timing Delays
#define SSD1331_DELAYS_HWFILL (3)
#define SSD1331_DELAYS_HWLINE (1)

// SSD1331 Commands
#define SSD1331_CMD_DRAWLINE 0x21
#define SSD1331_CMD_DRAWRECT 0x22
#define SSD1331_CMD_FILL 0x26
#define SSD1331_CMD_SETCOLUMN 0x15
#define SSD1331_CMD_SETROW 0x75
#define SSD1331_CMD_CONTRASTA 0x81
#define SSD1331_CMD_CONTRASTB 0x82
#define SSD1331_CMD_CONTRASTC 0x83
#define SSD1331_CMD_MASTERCURRENT 0x87
#define SSD1331_CMD_SETREMAP 0xA0
#define SSD1331_CMD_STARTLINE 0xA1
#define SSD1331_CMD_DISPLAYOFFSET 0xA2
#define SSD1331_CMD_NORMALDISPLAY 0xA4
#define SSD1331_CMD_DISPLAYALLON 0xA5
#define SSD1331_CMD_DISPLAYALLOFF 0xA6
#define SSD1331_CMD_INVERTDISPLAY 0xA7
#define SSD1331_CMD_SETMULTIPLEX 0xA8
#define SSD1331_CMD_SETMASTER 0xAD
#define SSD1331_CMD_DISPLAYOFF 0xAE
#define SSD1331_CMD_DISPLAYON 0xAF
#define SSD1331_CMD_POWERMODE 0xB0
#define SSD1331_CMD_PRECHARGE 0xB1
#define SSD1331_CMD_CLOCKDIV 0xB3
#define SSD1331_CMD_PRECHARGEA 0x8A
#define SSD1331_CMD_PRECHARGEB 0x8B
#define SSD1331_CMD_PRECHARGEC 0x8C
#define SSD1331_CMD_PRECHARGELEVEL 0xBB
#define SSD1331_CMD_VCOMH 0xBE

void SSD1331::write(const uint16_t *buf, size_t len) {
	while (writing_)
		yield();
	memcpy(tx_buf_, buf, len);
	writing_ = true;
	digitalWrite(dc_, HIGH);
	spi_->beginTransaction(spi_tx_);
	digitalWrite(cs_, LOW);
	spi_->transfer(tx_buf_, nullptr, len, responder_);
}

void SSD1331::wait() {
	while (writing_)
		yield();
}

void SSD1331::doneWriting(EventResponderRef responder) {
	SSD1331 *gfx = (SSD1331*) responder.getContext();
	digitalWrite(gfx->cs_, HIGH);
	gfx->spi_->endTransaction();
	gfx->writing_ = false;
}

uint16_t SSD1331::Color565(uint8_t r, uint8_t g, uint8_t b) {
	uint16_t c;
	c = r >> 3;
	c <<= 6;
	c |= g >> 2;
	c <<= 5;
	c |= b >> 3;

	return c;
}

void SSD1331::setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {

	uint8_t x1 = x;
	uint8_t y1 = y;
	if (x1 > 95)
		x1 = 95;
	if (y1 > 63)
		y1 = 63;

	uint8_t x2 = (x + w - 1);
	uint8_t y2 = (y + h - 1);
	if (x2 > 95)
		x2 = 95;
	if (y2 > 63)
		y2 = 63;

	if (x1 > x2) {
		uint8_t t = x2;
		x2 = x1;
		x1 = t;
	}
	if (y1 > y2) {
		uint8_t t = y2;
		y2 = y1;
		y1 = t;
	}

	digitalWrite(dc_, LOW);
	digitalWrite(cs_, LOW);
	spi_->transfer(0x15); // Column addr set
	spi_->transfer(x1);
	spi_->transfer(x2);

	spi_->transfer(0x75); // Column addr set
	spi_->transfer(y1);
	spi_->transfer(y2);
	digitalWrite(cs_, HIGH);
}

void SSD1331::begin() {
	digitalWrite(rst_, HIGH);
	delay(1);
	digitalWrite(rst_, LOW);
	delay(1);
	digitalWrite(rst_, HIGH);
	delay(10);

	pinMode(dc_, OUTPUT);
	pinMode(cs_, OUTPUT);

	digitalWrite(dc_, LOW);
	digitalWrite(cs_, LOW);
	// Initialization Sequence
	spi_->transfer(SSD1331_CMD_DISPLAYOFF);  // 0xAE
	spi_->transfer(SSD1331_CMD_SETREMAP);    // 0xA0
#if defined SSD1331_COLORORDER_RGB
	spi_->transfer(0x72);  // RGB Color
#else
  spi_->transfer(0x76);  // BGR Color
#endif
	spi_->transfer(SSD1331_CMD_STARTLINE);  // 0xA1
	spi_->transfer(0x0);
	spi_->transfer(SSD1331_CMD_DISPLAYOFFSET);  // 0xA2
	spi_->transfer(0x0);
	spi_->transfer(SSD1331_CMD_NORMALDISPLAY);  // 0xA4
	spi_->transfer(SSD1331_CMD_SETMULTIPLEX);   // 0xA8
	spi_->transfer(0x3F);                       // 0x3F 1/64 duty
	spi_->transfer(SSD1331_CMD_SETMASTER);      // 0xAD
	spi_->transfer(0x8E);
	spi_->transfer(SSD1331_CMD_POWERMODE);  // 0xB0
	spi_->transfer(0x0B);
	spi_->transfer(SSD1331_CMD_PRECHARGE);  // 0xB1
	spi_->transfer(0x31);
	spi_->transfer(SSD1331_CMD_CLOCKDIV);  // 0xB3
	spi_->transfer(0xF0);  // 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio
						   // (A[3:0]+1 = 1..16)
	spi_->transfer(SSD1331_CMD_PRECHARGEA);  // 0x8A
	spi_->transfer(0x64);
	spi_->transfer(SSD1331_CMD_PRECHARGEB);  // 0x8B
	spi_->transfer(0x78);
	spi_->transfer(SSD1331_CMD_PRECHARGEA);  // 0x8C
	spi_->transfer(0x64);
	spi_->transfer(SSD1331_CMD_PRECHARGELEVEL);  // 0xBB
	spi_->transfer(0x3A);
	spi_->transfer(SSD1331_CMD_VCOMH);  // 0xBE
	spi_->transfer(0x3E);
	spi_->transfer(SSD1331_CMD_MASTERCURRENT);  // 0x87
	spi_->transfer(0x06);
	spi_->transfer(SSD1331_CMD_CONTRASTA);  // 0x81
	spi_->transfer(0x91);
	spi_->transfer(SSD1331_CMD_CONTRASTB);  // 0x82
	spi_->transfer(0x50);
	spi_->transfer(SSD1331_CMD_CONTRASTC);  // 0x83
	spi_->transfer(0x7D);
	spi_->transfer(SSD1331_CMD_DISPLAYON);  //--turn on oled panel
	digitalWrite(cs_, HIGH);

	setAddrWindow(0, 0, 96, 64);
}

/********************************* low level pin initialization */

SSD1331::SSD1331(SPIClass &spi, int dc, int rst, int cs, SPISettings spi_tx) :
		spi_(&spi), dc_(dc), rst_(rst), cs_(cs), spi_tx_(spi_tx) {
	responder_.attachImmediate(doneWriting);
	responder_.setContext(this);
}
