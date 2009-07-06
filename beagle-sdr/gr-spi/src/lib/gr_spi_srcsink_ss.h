/* -*- c++ -*- */
/*
 * gr_spi_srcsink_ss.h
 * George Schaertl <gschaertl@gmail.com>
 * 5 May 2009
 */

#ifndef INCLUDED_GR_SPI_SRCSINK_SS_H
#define INCLUDED_GR_SPI_SRCSINK_SS_H

#include <gr_sync_block.h>

class gr_spi_srcsink_ss;

typedef boost::shared_ptr<gr_spi_srcsink_ss> gr_spi_srcsink_ss_sptr;

gr_spi_srcsink_ss_sptr gr_make_spi_srcsink_ss ();

class gr_spi_srcsink_ss : public gr_sync_block
{
	int d_fd;
	unsigned int d_spi_speed;
	unsigned char d_spi_bits;
	unsigned char d_spi_mode;
	bool d_swap_bytes;

private:
	friend gr_spi_srcsink_ss_sptr gr_make_spi_srcsink_ss ();
	gr_spi_srcsink_ss ();
	void swap_bytes(char* stream, int len);

public:
	~gr_spi_srcsink_ss ();

	int work (int noutput_items,
	          gr_vector_const_void_star &input_items,
	          gr_vector_void_star &output_items);

	bool open_spi (const std::string &device,
	               unsigned int speed);

	bool close_spi ();

};

#endif /* INCLUDED_GR_SPI_SOURCE_S_H */
