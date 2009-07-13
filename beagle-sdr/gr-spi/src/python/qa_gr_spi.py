#!/usr/bin/env python

from gnuradio import gr, gr_unittest
import gr_spi, time

class qa_gr_spi (gr_unittest.TestCase):

	def setUp (self):
		self.fg = gr.top_block()

	def tearDown (self):
		self.fg = None

	def test_001_srcsink_ss (self):
		src = gr.null_source(gr.sizeof_short)
		spi = gr_spi.spi_srcsink_ss()
		spi.open_spi("/dev/null",1000000)
		sink = gr.null_sink(gr.sizeof_short)
		self.fg.connect(src,0,spi,0)
		self.fg.connect(spi,sink)
		self.fg.start()
		time.sleep(0.5)
		self.fg.stop()
		spi.close_spi()

if __name__ == '__main__':
	gr_unittest.main()
