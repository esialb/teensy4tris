#include <stdint.h>
#include <Arduino.h>
#include <DMAChannel.h>
#include <SPI.h>
#include "spi_slave.h"

DMAChannel SPISlave::_rx(false);
uint8_t *SPISlave::_rx_buf;
size_t SPISlave::_rx_len;
void (*SPISlave::_rx_isr)(uint8_t *buf, size_t len) = nullptr;

DMAChannel SPISlave::_tx(false);
uint8_t *SPISlave::_tx_buf;
size_t SPISlave::_tx_len;
void (*SPISlave::_tx_isr)(uint8_t *buf, size_t len) = nullptr;

void SPISlave::rx_isr() {
	_rx.clearInterrupt();
	asm volatile ("dsb");
	arm_dcache_delete(_rx_buf, _rx_len);
	if (_rx_isr) _rx_isr(_rx_buf, _rx_len);
}

void SPISlave::rx(uint8_t *buf, size_t len) {
	_rx.destinationBuffer(_rx_buf = buf, _rx_len = len);
	_rx.disableOnCompletion();
	_rx.enable();
}

void SPISlave::rx_isr(void (*isr)(uint8_t *buf, size_t len)) {
	_rx_isr = isr;
}

void SPISlave::tx_isr() {
	_tx.clearInterrupt();
	asm volatile ("dsb");
	arm_dcache_delete(_tx_buf, _tx_len);
	if (_tx_isr) _tx_isr(_tx_buf, _tx_len);
}

void SPISlave::tx(uint8_t *buf, size_t len) {
	_tx.sourceBuffer(_tx_buf = buf, _tx_len = len);
	_tx.disableOnCompletion();
	_tx.enable();
}

void SPISlave::tx_isr(void (*isr)(uint8_t *buf, size_t len)) {
	_tx_isr = isr;
}

void SPISlave::begin() {
	SPI.begin();
	SPI.setCS(10);

	LPSPI4_CR &= ~LPSPI_CR_MEN; //Disable SPI Module!
	LPSPI4_CR = LPSPI_CR_RST; //Master Logic reset! (Control Register => Software Reset)
	LPSPI4_CR &= ~LPSPI_CR_RST; //Master Logic reset! (Control Register => Software Reset)
	LPSPI4_TCR = LPSPI_TCR_FRAMESZ(7); //8Bit Mode
	LPSPI4_DER = LPSPI_DER_RDDE | LPSPI_DER_TDDE; //TX,RX DMA Request Enable
	LPSPI4_CR |= LPSPI_CR_MEN; //Enable SPI Module!

	_rx.begin(true);
	_rx.source((uint8_t&) LPSPI4_RDR);
	_rx.triggerAtHardwareEvent(DMAMUX_SOURCE_LPSPI4_RX);
	_rx.attachInterrupt(SPISlave::rx_isr);
	_rx.interruptAtCompletion(); //TCD->CSR |= DMA_TCD_CSR_INTMAJOR;

	_tx.begin(true);
	_tx.destination((uint8_t&) LPSPI4_TDR);
	_tx.triggerAtHardwareEvent(DMAMUX_SOURCE_LPSPI4_TX);
	_tx.attachInterrupt(SPISlave::tx_isr);
	_tx.interruptAtCompletion(); //TCD->CSR |= DMA_TCD_CSR_INTMAJOR;
}
