#
# Copyright 2004 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
# 

include $(top_srcdir)/Makefile.common

ourpythondir = $(grpythondir)
ourlibdir    = $(grpyexecdir)

INCLUDES = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS)

SWIGCPPPYTHONARGS = -noruntime -c++ -python $(PYTHON_CPPFLAGS) \
	-I$(swigincludedir) -I$(grincludedir)

ALL_IFILES = $(LOCAL_IFILES) $(NON_LOCAL_IFILES)             

NON_LOCAL_IFILES = $(GNURADIO_CORE_INCLUDEDIR)/swig/gnuradio.i

LOCAL_IFILES = gr_spi.i 

BUILT_SOURCES = gr_spi.cc gr_spi.py

ourpython_PYTHON = gr_spi.py

ourlib_LTLIBRARIES = _gr_spi.la

_gr_spi_la_SOURCES = gr_spi.cc gr_spi_source_s.cc gr_spi_srcsink_ss.cc

_gr_spi_la_LDFLAGS = -module -avoid-version

_gr_spi_la_LIBADD = $(PYTHON_LDFLAGS) -lstdc++

gr_spi.cc gr_spi.py: gr_spi.i $(ALL_IFILES)
	$(SWIG) $(SWIGCPPPYTHONARGS) -module gr_spi -o gr_spi.cc $<

grinclude_HEADERS = gr_spi_source_s.h gr_spi_srcsink_ss.h

swiginclude_HEADERS = $(LOCAL_IFILES)

MOSTLYCLEANFILES = $(BUILT_SOURCES) *.pyc
