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
	static void set_rx(uint8_t* buf, size_t len);
	static void set_isr(void (*isr)(uint8_t* buf, size_t len));
private:
	static void rx_isr();

	static DMAChannel _rx;
	static uint8_t *_buf;
	static size_t _len;
	static void (*_isr)(uint8_t* buf, size_t len);
};



#endif /* SRC_SPI_SLAVE_H_ */
