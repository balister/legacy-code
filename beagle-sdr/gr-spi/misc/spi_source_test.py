#!/usr/bin/env python

from gnuradio import gr, gr_spi
import sys

class spi_test (gr.top_block):

	def build_graph(self):
		self.src = gr_spi.spi_source_s()
		sys.stdout.write("Source initialized\n")
		self.src.open_spi("/dev/spidev4.0",10000000) #,4,False,False)
		sys.stdout.write("SPI device open\n")		
		out = gr.file_sink(gr.sizeof_short,"output.bin")
		sys.stdout.write("Sink initialized\n")
		self.connect(self.src,out)
		sys.stdout.write("Graph connected\n")
	def __init__(self):
		gr.top_block.__init__(self)
		self.build_graph()

def main():
	app = spi_test()
	raw_input('Initialized. Press any key to start:')	
	app.start()
	sys.stdout.write("App started.\n")
	cmds = str(0xdeadbeefbaadf00df00fc7c8)
	app.src.send_spi_command(cmds)
	raw_input('Press any key to exit:')
	app.src.close_spi()
	app.stop()
	exit()

if __name__ == '__main__':
	main()
