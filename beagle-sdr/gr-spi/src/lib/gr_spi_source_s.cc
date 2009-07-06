/* -*- c++ -*- */
/*
 * gr_spi_source_s.cc
 * George Schaertl <gschaertl@gmail.com>
 * 5 May 2009
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_spi_source_s.h>
#include <gr_io_signature.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

gr_spi_source_s_sptr gr_make_spi_source_s ()
{
	return gr_spi_source_s_sptr (new gr_spi_source_s ());
}

static const int MIN_IN = 0;
static const int MAX_IN = 0;
static const int MIN_OUT = 1;
static const int MAX_OUT = 1;

gr_spi_source_s::gr_spi_source_s ()
	: gr_sync_block("spi_source_s",
	                gr_make_io_signature (MIN_IN, MAX_IN, sizeof (short)),
	                gr_make_io_signature (MIN_OUT, MAX_OUT, sizeof (short)))
{
}

gr_spi_source_s::~gr_spi_source_s ()
{
}

bool gr_spi_source_s::send_spi_command (const std::string &command)
{
	d_command_buffer.append(command);
	return true;
}

bool gr_spi_source_s::open_spi (const std::string &device, unsigned int speed)
{
	int ret;
	d_spi_speed = speed;
	d_spi_bits = 16;
	d_spi_mode = 0;
	d_command_buffer = "";

	d_fd = open (device.c_str(), O_RDWR);
	if (d_fd < 0) return false;

	ret = ioctl (d_fd, SPI_IOC_WR_MODE, &d_spi_mode);
	if (ret == -1) return false;

	ret = ioctl (d_fd, SPI_IOC_WR_BITS_PER_WORD, &d_spi_bits);
	if (ret == -1) return false;

	ret = ioctl (d_fd, SPI_IOC_WR_MAX_SPEED_HZ, &d_spi_speed);
	if (ret == -1) return false;

	return true;
}

bool gr_spi_source_s::close_spi()
{
	close(d_fd);
	return true;
}

int gr_spi_source_s::work (int noutput_items,
          gr_vector_const_void_star &input_items,
          gr_vector_void_star &output_items)
{
	short *out = (short *) output_items[0];
	int length = noutput_items * d_spi_bits / 8;
	int ret;

	unsigned char txbuf[length], rxbuf[length];

	for (int i = 0; i < length; i++) {
		if (d_command_buffer.empty()) break;
		txbuf[i]  = d_command_buffer[0];
	}

	struct spi_ioc_transfer tr;
	tr.tx_buf = (unsigned long) txbuf,
	tr.rx_buf = (unsigned long) rxbuf,
	tr.len = length,
	tr.delay_usecs = 0,
	tr.speed_hz = d_spi_speed,
	tr.bits_per_word = d_spi_bits,

	ret = ioctl(d_fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret == 1) return -1;

	for (int i = 0; i < noutput_items; i++) {
		out[i]  = rxbuf[i*2] << 8;
		out[i] &= 0x0fff;
		out[i] |= rxbuf[i*2 + 1];
	}

	return noutput_items;
}
