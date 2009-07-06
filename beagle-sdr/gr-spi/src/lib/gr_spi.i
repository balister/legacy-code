/* -*- c++ -*- */

%include "exception.i"
%import "gnuradio.i"

%{
#include "gnuradio_swig_bug_workaround.h"
#include "gr_spi_source_s.h"
#include "gr_spi_srcsink_ss.h"
#include <stdexcept>
%}

GR_SWIG_BLOCK_MAGIC(gr,spi_source_s);

gr_spi_source_s_sptr gr_make_spi_source_s ();

class gr_spi_source_s : public gr_sync_block
{
private:
	gr_spi_source_s ();
public:
	bool open_spi (const std::string &device, unsigned int speed);
	bool close_spi ();
	bool send_spi_command (const std::string &command);
};

GR_SWIG_BLOCK_MAGIC(gr,spi_srcsink_ss);

gr_spi_srcsink_ss_sptr gr_make_spi_srcsink_ss ();

class gr_spi_srcsink_ss : public gr_sync_block
{
private:
        gr_spi_srcsink_ss ();
public:
        bool open_spi (const std::string &device, unsigned int speed);
        bool close_spi ();
};
