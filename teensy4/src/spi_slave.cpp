#include <stdint.h>
#include <Arduino.h>
#include <DMAChannel.h>
#include <SPI.h>
#include "spi_slave.h"

DMAChannel SPISlave::_rx(false);
uint8_t *SPISlave::_buf;
size_t SPISlave::_len;
void (*SPISlave::_isr)(uint8_t* buf, size_t len);

void SPISlave::rx_isr() {
  _rx.clearInterrupt();
  _rx.disable();
  asm volatile ("dsb");
  arm_dcache_delete(_buf, _len);
  _isr(_buf, _len);
}

void SPISlave::set_rx(uint8_t* buf, size_t len) {
	_rx.destinationBuffer(_buf = buf, _len = len);
	_rx.enable();
}

void SPISlave::set_isr(void (*isr)(uint8_t* buf, size_t len)) {
	_isr = isr;
}

void SPISlave::begin() {
	  SPI.begin();
	  SPI.setCS(10);

	  LPSPI4_CR &= ~LPSPI_CR_MEN; //Disable SPI Module!
	  LPSPI4_CR = LPSPI_CR_RST; //Master Logic reset! (Control Register => Software Reset)
	  LPSPI4_CR &=  ~LPSPI_CR_RST; //Master Logic reset! (Control Register => Software Reset)
	  LPSPI4_TCR = LPSPI_TCR_FRAMESZ(7); //8Bit Mode
	  LPSPI4_DER = LPSPI_DER_RDDE; //RX DMA Request Enable
	  LPSPI4_CR |= LPSPI_CR_MEN; //Enable SPI Module!

	  _rx.begin(true);
	  _rx.source((uint16_t &) LPSPI4_RDR);
	  _rx.triggerAtHardwareEvent(DMAMUX_SOURCE_LPSPI4_RX);
	  _rx.attachInterrupt(SPISlave::rx_isr);
	  _rx.interruptAtCompletion(); //TCD->CSR |= DMA_TCD_CSR_INTMAJOR;
}
