
#ifndef _SURF_PROCEDURES_
#define _SURF_PROCEDURES_

// SURFACES

class abstractSurf {
// parent class for all surfaces

	public:
	
	virtual void forceMolSurfSurf( int n, Vec3d* Rs, Vec3d* Fs ){};
	virtual void forceMolPerpSurf( int n, Vec3d * Rs, Vec3d * Fs ){};
	virtual Vec3d getHat(){};
	virtual void printSurf(){};

};

class surfVert : public abstractSurf {
// surf with vertical (and horizontal) action

	public:
	
	double z0	= 0;
	double zMin	= 0; 
	double Emin	= 0;
	Vec3d  hat;	// surface normal

	public:

	surfVert( char* filename );
	surfVert( surfVert* surf_ );
		
	void setSurfaceParameters( char* filename );
	void forceMolSurfSurf( int n, Vec3d* Rs, Vec3d* Fs );
	void forceMolPerpSurf( int n, Vec3d * Rs, Vec3d * Fs );
	Vec3d getHat();
	void printSurf();

};

// ==================== surfVert ==================== 

surfVert::surfVert( surfVert* surf_ )
// constructor
: z0( surf_->z0 ), zMin( surf_->zMin ), Emin( surf_->Emin ), hat( surf_->hat )
{
}

surfVert::surfVert( char* filename ){
// constructor
	
	setSurfaceParameters( filename );
	
}

void surfVert::printSurf(){
// print parameters

	printf( "z0, zMin, Emin = %lf %lf %lf\n", z0, zMin, Emin );
	printf( "hat = %lf %lf %lf\n", hat.x, hat.y, hat.z );

}

void surfVert::setSurfaceParameters( char* filename ){
// sets parameters of the surface

	char str[400];
	FILE* surffile = fopen( filename, "r" );
	if( surffile ){
		readLineComment( surffile, str );
		sscanf( str, "%lf", &z0 );

		readLineComment( surffile, str );
		sscanf( str, "%lf", &zMin );

		readLineComment( surffile, str );
		sscanf( str, "%lf", &Emin );

		readLineComment( surffile, str );
		sscanf( str, "%lf %lf %lf", &hat.x, &hat.y, &hat.z );

		fclose( surffile );
		
	} else {
		printf( "setSurfaceParameters: File %s cannot be opened. Default values used instead.\n", filename );
		hat.set( 0, 0, 1 );
		
	}

}

void surfVert::forceMolSurfSurf( int n, Vec3d * Rs, Vec3d * Fs ){
// vertical force

	forceMolSurf( z0, zMin, Emin, hat, n, Rs, Fs );

}

void surfVert::forceMolPerpSurf( int n, Vec3d * Rs, Vec3d * Fs ){
// horizontal force

	forceMolSurfPerp( n, Rs, Fs );

}

Vec3d surfVert::getHat(){
// get hat

	return hat;
	
}

#endif
