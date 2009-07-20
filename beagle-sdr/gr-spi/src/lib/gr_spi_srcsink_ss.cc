/* -*- c++ -*- */
/*
 * gr_spi_srcsink_ss.cc
 * George Schaertl <gschaertl@gmail.com>
 * 5 May 2009
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_spi_srcsink_ss.h>
#include <gr_io_signature.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <endian.h>

#if FAKE_IOCTL
int ioctl(int fd, int req, void* arg) {
	return 0;
}
#endif

gr_spi_srcsink_ss_sptr gr_make_spi_srcsink_ss ()
{
	return gr_spi_srcsink_ss_sptr (new gr_spi_srcsink_ss ());
}

static const int MIN_IN = 1;
static const int MAX_IN = 1;
static const int MIN_OUT = 1;
static const int MAX_OUT = 1;

// The kernel doesn't like more than 4096 bytes in one spi_transfer
static const int MAX_ITEMS = 2048;

gr_spi_srcsink_ss::gr_spi_srcsink_ss ()
	: gr_sync_block("spi_srcsink_ss",
	                gr_make_io_signature (MIN_IN, MAX_IN, sizeof (short)),
	                gr_make_io_signature (MIN_OUT, MAX_OUT, sizeof (short)))
{
	set_output_multiple(MAX_ITEMS); // Short spi_transfers are inefficient

	d_swap_bytes = (BYTE_ORDER != BIG_ENDIAN);
}

gr_spi_srcsink_ss::~gr_spi_srcsink_ss ()
{
}

bool gr_spi_srcsink_ss::open_spi (const std::string &device, unsigned int speed)
{
	int ret;
	d_spi_speed = speed;
	d_spi_bits = 16;
	d_spi_mode = 0;

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

bool gr_spi_srcsink_ss::close_spi()
{
	close(d_fd);
	return true;
}

void gr_spi_srcsink_ss::swap_bytes(char* stream, int len) {
	for (len-= len% 2; len ; len -= 2) {
		char     temp = stream[len-1];
		stream[len-1] = stream[len-2];
		stream[len-2] = temp;
	}
}

int gr_spi_srcsink_ss::work (int noutput_items,
          gr_vector_const_void_star &input_items,
          gr_vector_void_star &output_items)
{
	if (noutput_items > MAX_ITEMS) noutput_items = MAX_ITEMS;

	if (d_swap_bytes) swap_bytes(
		(char*) &input_items[0],
		noutput_items*sizeof(short));

	int ret;

	struct spi_ioc_transfer tr;
	tr.tx_buf = (unsigned long) &input_items[0],
	tr.rx_buf = (unsigned long) &output_items[0],
	tr.len = noutput_items * sizeof(short),
	tr.delay_usecs = 0,
	tr.speed_hz = d_spi_speed,
	tr.bits_per_word = d_spi_bits,

	ret = ioctl(d_fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret == 1) return -1;

        if (d_swap_bytes) swap_bytes(
		(char*) &output_items[0],
		noutput_items*sizeof(short));

	return noutput_items;
}
