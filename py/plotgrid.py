#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt
import sys

filename = sys.argv[1]


def dtata_from_file( filename ):
	f      = open( filename, 'r' );
	header = f.readline(); 
	ndim = [ int( word ) for word in header.split() ]
	f.close()
	print ndim
	data = np.genfromtxt( filename, skip_header=1 )
	return data.reshape( ndim ), ndim

data, ndim = dtata_from_file( filename )
for i in range( ndim[2] ):
	imgname = filename+'_%03i.png' %i
	print imgname
	plt.figure( figsize=(5,5) )
	plt.imshow( -data[:,:,i], origin='image', cmap='gray', interpolation='nearest' )
	plt.savefig( imgname )
	
#plt.show()
