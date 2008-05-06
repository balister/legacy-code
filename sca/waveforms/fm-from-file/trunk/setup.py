#! /usr/bin/env python

from distutils.core import setup
import sys

install_location = '/sdr'

if len(sys.argv) != 2:
        sys.exit(1)

sys.argv.append('--install-lib='+install_location)

setup(name='WFMMono', description='WFMMono',data_files=[(install_location+'/waveforms/WFMMono',['WFMMono.sad.xml', 'WFMMono_DAS.xml'])])