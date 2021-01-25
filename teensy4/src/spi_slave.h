/*
 * spi_slave.h
 *
 *  Created on: Jan 24, 2021
 *      Author: robin
 */

#ifndef SRC_SPI_SLAVE_H_
#define SRC_SPI_SLAVE_H_

#include <stddef.h>
#include <stdint.h>
#include <DMAChannel.h>

class SPISlave {
public:
	SPISlave() = delete;
	static void begin();
	static void rx(uint8_t* buf, size_t len);
	static void rx_isr(void (*isr)(uint8_t* buf, size_t len));
	static void tx(uint8_t* buf, size_t len);
	static void tx_isr(void (*isr)(uint8_t* buf, size_t len));
private:
	static void rx_isr();
	static void tx_isr();

	static DMAChannel _rx;
	static uint8_t *_rx_buf;
	static size_t _rx_len;
	static void (*_rx_isr)(uint8_t* buf, size_t len);

	static DMAChannel _tx;
	static uint8_t *_tx_buf;
	static size_t _tx_len;
	static void (*_tx_isr)(uint8_t* buf, size_t len);
};



#endif /* SRC_SPI_SLAVE_H_ */
