/* -*- c++ -*- */

%include "exception.i"
%import "gnuradio.i"
%include "std_string.i"

%{
#include "gnuradio_swig_bug_workaround.h"
#include "spi_srcsink_ss.h"
#include <stdexcept>
%}

/*
GR_SWIG_BLOCK_MAGIC(spi,source_s);

spi_source_s_sptr spi_make_source_s ();

class spi_source_s : public gr_sync_block
{
private:
	spi_source_s ();
public:
	bool open_spi (const std::string &device, unsigned int speed);
	bool close_spi ();
	bool send_spi_command (const std::string &command);
};
*/

GR_SWIG_BLOCK_MAGIC(spi,srcsink_ss);

spi_srcsink_ss_sptr spi_make_srcsink_ss (char *device, unsigned int speed);

class spi_srcsink_ss : public gr_sync_block
{
protected:
        spi_srcsink_ss (char *device, unsigned int speed);
public:
        ~spi_srcsink_ss ();
};
