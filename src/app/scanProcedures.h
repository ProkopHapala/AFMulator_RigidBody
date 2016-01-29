
#ifndef _SCAN_PROCEDURES_
#define _SCAN_PROCEDURES_

// SCANNING PROCEDURES

enum scanningType {			// which scanning mode to use
	SCANNING_NONE,
	SCANNING_NONE_VIEW,
	SCANNING_REPLAY,
	SCANNING_ONE,
	SCANNING_RASTER,
	SCANNING_SCRIPT
};

enum zSamplingMode {			// how to choose successive distances of a tip while being approach to the sample
	Z_SAMPLE_EQUIDIST,
	Z_SAMPLE_LINEAR,
	Z_SAMPLE_QUAD
};

enum moveReplayScanMode {		// where to move the scanning tip in the replay mode (SCANNING_REPLAY)
	REPLAY_MOVE_NONE,
	REPLAY_MOVE_LEFT,
	REPLAY_MOVE_RIGHT,
	REPLAY_MOVE_DOWN,
	REPLAY_MOVE_UP,
	REPLAY_MOVE_IN,
	REPLAY_MOVE_OUT
};

void linspace( int n, double x0, double dx, double * xs ){
	double x = x0;
	for( int i=0; i<n; i++){ xs[i] = x; x+=dx; }
}

/*
void printArray( int n, double * xs ){
	for( int i=0; i<n; i++){ printf( " %i %f \n", xs[i] ); }
}
*/

class scanSpecification {

	public:
	int     xdim    = 0, ydim    = 0, zdim    = 0;	// numbers of samples
	double  xstep   = 0, ystep   = 0, zstep   = 0; // scanning steps
	double  xoffset = 0, yoffset = 0, zoffset = 0;	// initial position of a scanning tip

	double *xs=NULL,*ys=NULL,*zs=NULL;

	double minStep;
	scanningType  scanType;			// which scanning mode to use
	zSamplingMode zSampling;		// how to choose successive distances of a tip while being approached to the sample
	scanSpecification(){};
	void createZSamplingSequenceLoc( scalarDataWrapper* zSteps ); // TODO: Deprecated

	void initLinear( ){
		printf( " scan dim   : %i %i %i \n", xdim,    ydim,    zdim    );
		printf( " scan step  : %f %f %f \n", xstep,   ystep,   zstep   );
		printf( " scan offset: %f %f %f \n", xoffset, yoffset, zoffset );
		xs = new double[xdim];
		ys = new double[ydim];
		zs = new double[zdim];
		linspace( xdim, xoffset, xstep, xs );
		linspace( ydim, yoffset, ystep, ys );
		linspace( zdim, zoffset, zstep, zs );
	}

	~scanSpecification(){
		if( xs=NULL ) delete xs;
		if( ys=NULL ) delete ys;
		if( zs=NULL ) delete zs;
	}
};

void scanSpecification::createZSamplingSequenceLoc( scalarDataWrapper* zSteps ){
// generates zSteps, which contains increments by which the tip should be approached to the sample in each step of z-scanning

	double auxstep, coef;
	switch( zSampling ){
		case Z_SAMPLE_EQUIDIST:
			printf( "createZSamplingSequence: Constant decrease.\n" );
			for( int i = 0; i < zdim ; i++ ){
				zSteps->setValue( i, zstep );
				printf( "zSteps[%i] = %lf\n", i, (*zSteps)[i] );
			}
			break;

		case Z_SAMPLE_LINEAR:
			printf( "createZSamplingSequence: Linear decrease.\n" );
			if( minStep < 1e-8 ){
				printf( "createZSamplingSequence: Minimum step set to zero. Default setting used.\n" );
				auxstep = 2*zstep / ( (double) (zdim - 1) );
				for( int i = 0; i < zdim ; i++ ){
					zSteps->setValue( i, (zdim - i - 1)*auxstep );
					printf( "zSteps[%i] = %lf\n", i, (*zSteps)[i] );
				}
			} else {
				auxstep = 2*(((double) zdim)*(zstep - minStep) + minStep) / ( (double) (zdim - 1)*(zdim - 2) );
				if( auxstep < 1e-8 )
					printf( "createZSamplingSequence: Linear interpolation degenerate. Succesive steps are equidistant of length %.3lf.\n", minStep );
				for( int i = 0; i < zdim ; i++ ){
					zSteps->setValue( i, minStep + (zdim - i - 2)*auxstep );
					printf( "zSteps[%i] = %lf\n", i, (*zSteps)[i] );
				}
			}
//			printf( "zdim, zstep, minStep = %i %lf %lf\n", zdim, zstep, minStep );
//			printf( "auxstep = %lf, (((double) zdim)*(zstep - minStep) + minStep) = %lf\n", auxstep, (17.0*(zstep - minStep) + minStep) );
			break;

		case Z_SAMPLE_QUAD:
			printf( "createZSamplingSequence: Quadratic decrease.\n" );
			coef = 0; // POPR. SI POHRAT S TIMTO PARAMETREM A POPR. DODAT I ABSOLUTNI CLEN DO KVADR. VYRAZU
			auxstep = ( zdim*zstep - coef*( zdim - 1 )) / ((double)( zdim - 1 )*( zdim - 1 ));
			for( int i = 0; i < zdim ; i++ ){
				zSteps->setValue( i, auxstep*( 2*zdim - 3 - 2*i ) );
				printf( "zSteps[%i] = %lf\n", i, (*zSteps)[i] );
			}
			break;
		default:
			printf( "createZSamplingSequence: Default equidistant decrease.\n" );
			for( int i = 0; i < zdim ; i++ ){
				zSteps->setValue( i, zstep );
				printf( "zSteps[%i] = %lf\n", i, (*zSteps)[i] );
			}
			break;

	}

}

#endif
