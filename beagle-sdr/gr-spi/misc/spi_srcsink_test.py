#!/usr/bin/env python

from gnuradio import gr, gr_spi
import sys

class spi_test (gr.top_block):

	def build_graph(self,device,speed):
		self.src = gr.sig_source_s(2,gr.GR_CONST_WAVE,1,0,1)
		self.sink = gr.null_sink(gr.sizeof_short)
		self.spi = gr_spi.gr_spi_srcsink_ss()
		sys.stdout.write("Blocks created\n")
		self.spi.open_spi(device,speed)
		sys.stdout.write("SPI device open\n")
		self.connect(self.src,self.spi)
		sys.stdout.write("Source connected\n")
		self.connect(self.spi,self.sink)
		sys.stdout.write("Sink connected\n")
	def __init__(self,device,speed):
		gr.top_block.__init__(self)
		self.build_graph(device,speed)

def main(argv):
	argc = len(argv)
	if argc != 3 :
		sys.stderr.write('Usage: ' + argv[0] +
				 ' <device-name> <speed>\n')
		sys.exit(1)

	device = argv[1]
	speed = int(argv[2])

	app = spi_test(device,speed)
	app.dump()
	raw_input('Press any key to start:')	
	app.start()
	sys.stdout.write("App started.\n")
	raw_input('Press any key to exit:')
	app.spi.close_spi()
	app.stop()
	exit()

if __name__ == '__main__':
	main(sys.argv)
