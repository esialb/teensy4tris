/*
 * spi_slave.h
 *
 *  Created on: Jan 24, 2021
 *      Author: robin
 */

#ifndef SRC_IO_SPI_SLAVE_H_
#define SRC_IO_SPI_SLAVE_H_

#include <stddef.h>
#include <stdint.h>
#include <DMAChannel.h>
#include <functional>

class SPISlave {
public:
	SPISlave() = delete;

	static void begin();

	static void cancel_rx();
	static void rx(uint8_t* buf, size_t len);
	static void set_rx_callback(void (*isr)(uint8_t* buf, size_t len));

	static void cancel_tx();
	static void tx(uint8_t* buf, size_t len);
	static void set_tx_callback(void (*isr)(uint8_t* buf, size_t len));
private:
	static void rx_isr();
	static void tx_isr();

	static DMAChannel _rx;
	static uint8_t *_rx_buf;
	static size_t _rx_len;
	static void (*_rx_callback)(uint8_t* buf, size_t len);

	static DMAChannel _tx;
	static uint8_t *_tx_buf;
	static size_t _tx_len;
	static void (*_tx_callback)(uint8_t* buf, size_t len);
};

#endif /* SRC_IO_SPI_SLAVE_H_ */

