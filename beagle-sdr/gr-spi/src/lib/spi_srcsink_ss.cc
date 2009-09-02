/* -*- c++ -*- */
/*
 * spi_srcsink_ss.cc
 * George Schaertl <gschaertl@gmail.com>
 * 14 July 2009
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <spi_srcsink_ss.h>
#include <gr_io_signature.h>
#include <fcntl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <endian.h>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <cerrno>

#ifdef FAKE_IOCTL
int ioctl(int fd, int req, void* arg) {
	return 0;
}
#endif

spi_srcsink_ss_sptr spi_make_srcsink_ss (char* device, unsigned int speed)
{
	return spi_srcsink_ss_sptr (new spi_srcsink_ss (device, speed));
}

static const int MIN_IN = 1;
static const int MAX_IN = 1;
static const int MIN_OUT = 1;
static const int MAX_OUT = 1;

// The kernel doesn't like more than 4096 bytes in one spi_transfer
static const int MAX_ITEMS = 4096/sizeof(short);

spi_srcsink_ss::spi_srcsink_ss (char *device, unsigned int speed)
	: gr_sync_block("spi_srcsink_ss",
	                gr_make_io_signature (MIN_IN, MAX_IN, sizeof (short)),
	                gr_make_io_signature (MIN_OUT, MAX_OUT, sizeof (short)))
{
	set_output_multiple(MAX_ITEMS); // Short spi_transfers are inefficient

//	The BeagleBoard lies to us about this for some reason
//	d_swap_bytes = (BYTE_ORDER != BIG_ENDIAN);
	d_swap_bytes = false;

        int ret;
        d_spi_speed = speed;
        d_spi_bits = 16;
        d_spi_mode = 0;

        d_fd = open (device, O_RDWR);
        if (d_fd < 0) throw std::runtime_error("can't open device");
        
	ret = ioctl (d_fd, SPI_IOC_WR_MODE, &d_spi_mode);
        if (ret == -1) throw std::runtime_error("can't set spi mode");

        ret = ioctl (d_fd, SPI_IOC_WR_BITS_PER_WORD, &d_spi_bits);
        if (ret == -1) throw std::runtime_error("can't set spi bits per word");

        ret = ioctl (d_fd, SPI_IOC_WR_MAX_SPEED_HZ, &d_spi_speed);
        if (ret == -1) throw std::runtime_error("can't set spi max speed");

	std::cerr << "srcsink constructor exiting successfully" << std::endl;
}

spi_srcsink_ss::~spi_srcsink_ss ()
{
	close(d_fd);
}

void spi_srcsink_ss::swap_bytes(char* stream, int len) {
	for (len-= len% 2; len ; len -= 2) {
		char     temp = stream[len-1];
		stream[len-1] = stream[len-2];
		stream[len-2] = temp;
	}
}

int spi_srcsink_ss::work (int noutput_items,
          gr_vector_const_void_star &input_items,
          gr_vector_void_star &output_items)
{

	// input_items and output_items are vectors of pointers to vectors of
	// samples, one vector-of-samples for each input or output stream.
	// Forget this like I did and you get segfaults in strange places. )-:
	short* in  = (short *) input_items[0];
	short* out = (short *) output_items[0];	

	if (d_swap_bytes)
		swap_bytes( (char*) in, noutput_items*sizeof(short) );

/*
//	This *was* a good idea, but the ioctl() segfaults on the BeagleBoard.
	int n_xfers = noutput_items / MAX_ITEMS;
	struct spi_ioc_transfer *xfer = new struct spi_ioc_transfer[n_xfers];

	for (int i=0; i<n_xfers; i++) {
		int offset = i * MAX_ITEMS;
		xfer[i].tx_buf = (unsigned long) &(in[offset]);
		xfer[i].rx_buf = (unsigned long) &(out[offset]);
		xfer[i].len = MAX_ITEMS * sizeof(short);
		xfer[i].speed_hz = d_spi_speed;
		xfer[i].delay_usecs = 0;
		xfer[i].bits_per_word = d_spi_bits;
		xfer[i].cs_change = 0;
	}

	std::cerr << "Trying ioctl with n_xfers = " << n_xfers << std::endl;
	int ret = ioctl(d_fd, SPI_IOC_MESSAGE(n_xfers), xfer);
	if (ret == -1)
		throw std::runtime_error(
		 strcat("SPI transfer failed: ",strerror(errno)));

	delete xfer;
*/

	for(int offset = 0; offset < noutput_items; offset += MAX_ITEMS)
	{
		int n_items = MAX_ITEMS; // make sure we set_output_multiple()!

		struct spi_ioc_transfer tr;
		tr.tx_buf = (unsigned long) &(in[offset]);
		tr.rx_buf = (unsigned long) &(out[offset]);
		tr.len = n_items * sizeof(short);
		tr.delay_usecs = 0;
		tr.speed_hz = d_spi_speed;
		tr.bits_per_word = d_spi_bits;
		tr.cs_change = 0;

		int ret = ioctl(d_fd, SPI_IOC_MESSAGE(1), &tr);
		if (ret == -1) throw std::runtime_error(
			strcat("SPI transfer failed: ",strerror(errno)));
	}

	if (d_swap_bytes)
		swap_bytes( (char*) out, noutput_items*sizeof(short) );

	return noutput_items;
}
