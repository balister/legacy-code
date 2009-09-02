/* -*- c++ -*- */
/*
 * spi_srcsink_ss.h
 * George Schaertl <gschaertl@gmail.com>
 * 14 July 2009
 */

#ifndef INCLUDED_SPI_SRCSINK_SS_H
#define INCLUDED_SPI_SRCSINK_SS_H

#include <gr_sync_block.h>

class spi_srcsink_ss;

typedef boost::shared_ptr<spi_srcsink_ss> spi_srcsink_ss_sptr;

spi_srcsink_ss_sptr spi_make_srcsink_ss (char *device, unsigned int speed);

class spi_srcsink_ss : public gr_sync_block
{
	friend spi_srcsink_ss_sptr spi_make_srcsink_ss (
					char *device, unsigned int speed);
	
private:
	int d_fd;
	unsigned int d_spi_speed;
	unsigned char d_spi_bits;
	unsigned char d_spi_mode;
	bool d_swap_bytes;

	void swap_bytes(char* stream, int len);

protected:
	spi_srcsink_ss (char *device, unsigned int speed);

public:
	~spi_srcsink_ss ();

	int work (int noutput_items,
	          gr_vector_const_void_star &input_items,
	          gr_vector_void_star &output_items);

};

#endif /* INCLUDED_SPI_SRCSINK_SS_H */
