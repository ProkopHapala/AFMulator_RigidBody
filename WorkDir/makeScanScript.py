#!/usr/bin/python
import numpy as np

xs = np.linspace( -20, 20, 10 )
ys = np.linspace( -12, 12, 6 )
zs = np.linspace(  22, 16, 6 )

zs = np.concatenate( (zs, zs[::-1]) )

fscript = open( "scan.script", 'w' )
for iy,y in enumerate( ys ):
	for ix,x in enumerate( xs ):
		fscript.write( "start_stroke; %3.3f %3.3f %3.3f\n" %(x,y,zs[0]) )
		for iz,z in enumerate( zs ):
			fscript.write( "set_tip_pos; %3.3f %3.3f %3.3f\n" %(x,y,z) )
fscript.close()


