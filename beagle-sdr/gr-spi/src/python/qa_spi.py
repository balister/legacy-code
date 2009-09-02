#!/usr/bin/env python

from gnuradio import gr, gr_unittest
import spi, time, sys

class qa_gr_spi (gr_unittest.TestCase):

	def setUp (self):
		self.fg = gr.top_block()

	def tearDown (self):
		del self.fg

	def test_001_srcsink_ss (self):
		sys.stderr.write("Creating null source\n")
		src = gr.null_source(gr.sizeof_short)
		sys.stderr.write("Creating throttle\n")
		thr = gr.throttle(gr.sizeof_short,1000)
		sys.stderr.write("Creating fake SPI device\n")
		dummy = spi.srcsink_ss("/dev/null",1000000)
		sys.stderr.write("Creating null sink\n")
		sink = gr.null_sink(gr.sizeof_short)
		sys.stderr.write("Connecting source to throttle\n")
		self.fg.connect(src,thr)
		sys.stderr.write("Connecting throttle to SPI\n")
		self.fg.connect(thr,dummy)
		sys.stderr.write("Connecting SPI to sink\n")
		self.fg.connect(dummy,sink)
		sys.stderr.write("Starting flow graph\n")
		self.fg.start()
                time.sleep(0.1)
		sys.stderr.write("Stopping flow graph\n")
		self.fg.stop()
		sys.stderr.write("Deleting blocks\n")
		del src
		del thr
		del dummy
		del sink
		sys.stderr.write("All done!\n")

if __name__ == '__main__':
	gr_unittest.main()
