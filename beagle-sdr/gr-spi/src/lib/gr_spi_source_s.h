/* -*- c++ -* */
/*
 * gr_spi_source_s.h
 * George Schaertl <gschaertl@gmail.com>
 * 5 May 2009
 */

#ifndef INCLUDED_GR_SPI_SOURCE_S_H
#define INCLUDED_GR_SPI_SOURCE_S_H

#include <gr_sync_block.h>

class gr_spi_source_s;

typedef boost::shared_ptr<gr_spi_source_s> gr_spi_source_s_sptr;

gr_spi_source_s_sptr gr_make_spi_source_s ();

class gr_spi_source_s : public gr_sync_block
{
	int d_fd;
	unsigned int d_spi_speed;
	unsigned char d_spi_bits;
	unsigned char d_spi_mode;
	std::string d_command_buffer;

private:
	friend gr_spi_source_s_sptr gr_make_spi_source_s ();
	gr_spi_source_s ();

public:
	~gr_spi_source_s ();

	int work (int noutput_items,
	          gr_vector_const_void_star &input_items,
	          gr_vector_void_star &output_items);

	bool open_spi (const std::string &device,
	               unsigned int speed);

	bool close_spi ();

	bool send_spi_command (const std::string &command);
};

#endif /* INCLUDED_GR_SPI_SOURCE_S_H */
