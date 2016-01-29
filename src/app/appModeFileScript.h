


// For lua
// #include <lua.hpp>
// LFLAGS = -I/usr/include/lua5.2 -L/usr/include/lua5.2 -llua5.2 -ldl  -lm -lSDL2_ttf -lSDL2_image -lSDL2



class appModeFileScript : public appModeTipMol {
	public:

	// constants
	const int maxFramesPerPoint = 10;

	FILE* pScript;
	FILE* pOut;

	// settings
	scanSpecification* scan  = NULL;  // FIXME : this dependence should be removed !!!!
	int  convergStepLimit    = 0;
	int* molOfInterest       = NULL;
	int  numMolOfInterest    = 0;
	bool saveMolOfInterestMovement = false;

	// loop variables
	int  ind,numOfNoncovergCases;
	bool loopEnd;

	// ======== Functions

	void resetup( void );
	void loop( int n );

	appModeFileScript(){};
	appModeFileScript(
		int                numOfMoleculeInstances,
		fileManager*       moleculeFiles,
		AtomTypes*         typeList,
		fileWrapper*       geometryFile,
		abstractSurf*      surf,
		graphInterface*    graphics,
		abstractTip*       tip,
		flagList *         flags,
		scanSpecification* scan,
		relaxParams*       relaxParameters,
		int*               molOfInterest_,
		int                numMolOfInterest_
	);


	void command_tipPos     ( char* line );
	void command_startStroke( char* line );


	bool convergeTipPos(  double xpos, double ypos, double zpos ){
		double pixelDataListItem;
		bool optimizingFlag = true;
		bool skip           = false;
		bool stopFlag       = false;

		//if( graphics != NULL ) printf( " starting relaxation at point: (%3.3f,%3.3f,%3.3f)\n", xpos, ypos, zpos );
		world->tip->setPosition( xpos, ypos, zpos );

		//for( int iframe = 0; iframe < maxFramesPerPoint; iframe++ ){
		while( true ){
			// handle input events
			if( graphics != NULL )	{ graphics->inputHandling( loopEnd, skip, stopFlag ); }
			if( loopEnd || skip  ) return false;
			//if( graphics != NULL && world->tip != NULL ){	graphics->thisScreen->mouseSetAuxPoint( world ); }   // WTF IS THIS ?
			if( graphics != NULL ){ graphics->updateGraphics(); }
			if( !stopFlag ){
				if( world->sysEvol ){
					world->update( optimizingFlag );
				}
				if( !optimizingFlag ){
					//printf( "convergStep = %i\n", convergStep );
					return true;
				}
				if( world->optSteps > convergStepLimit ){
					if( !suppressOutput ){
						//printf( "appModeFileScript::loop: Relaxation in step %5i does not converge. Skipped.\n", ind );
						//printf( "appModeFileScript::loop: step (ix,iy,iz) %5i %5i %5i Relaxation not converge in %5i. Skipped.\n", xind, yind, zind, convergStepLimit );
						printf( "Relaxation not converge in %i iterations, fmax = %f ( f2conv = %f )\n", convergStepLimit, world->fmax, world->fmaxConv );
					}
					//printf( "xind, yind, zind = %d %d %d\n", xind, yind, zind );
					numOfNoncovergCases++;
					return false;
				}
			}
		} // iframe
		return false;
		// saving data to lists
	}

};


// ================= appModeFileScript procedures =================

void appModeFileScript::resetup( void ){
// resetup the world geometry
	world->resetGeometry( geometryFile, posProbe, rotProbe );
}

appModeFileScript::appModeFileScript(
	int                numOfMoleculeInstances,
	fileManager*       moleculeFiles,
	AtomTypes*         typeList,
	fileWrapper*       geometryFile,
	abstractSurf*      surf,
	graphInterface*    graphics,
	abstractTip*       tip,
	flagList *         flags,
	scanSpecification* scan,
	relaxParams*       relaxParameters,
	int*               molOfInterest_,
	int                numMolOfInterest_
):
	scan            ( scan ),
	numMolOfInterest( numMolOfInterest_ ),
	appModeTipMol   ( numOfMoleculeInstances, moleculeFiles, typeList, geometryFile, surf, graphics, tip, flags )
{
	world->optimizer->dt       = relaxParameters->glob_dtmax;
	world->optimizer->damping  = relaxParameters->glob_damping;
	convergStepLimit = relaxParameters->glob_maxIters;
	printf( "===========  convergStepLimit %i \n", convergStepLimit );
	molOfInterest = new int[numMolOfInterest];
	for( int i = 0; i < numMolOfInterest; i++ ) molOfInterest[i] = molOfInterest_[i];
	if( graphics != NULL ) graphics->drawBox( scan );
}

/*
appModeRaster::appModeRaster(
	int numOfMoleculeInstances,
	fileManager* moleculeFiles,
	AtomTypes* typeList,
	fileWrapper* geometryFile,
	abstractSurf* surf,
	graphInterface* graphics,
	abstractTip* tip,
	flagList *flags,
	scanSpecification* scan,
	fileManager* outputFiles_,
	relaxParams* relaxParameters,
	int* molOfInterest_,
	int numMolOfInterest_,
	fileManager* outputPositMolFiles_,
	fileManager* outputRotatMolFiles_
):
	scan               ( scan ),
	outputFiles        ( outputFiles_ ),
	numMolOfInterest   ( numMolOfInterest_ ),
	outputPositMolFiles( outputPositMolFiles_ ),
	outputRotatMolFiles( outputRotatMolFiles_ ),
	appModeTipMol      ( numOfMoleculeInstances, moleculeFiles, typeList, geometryFile, surf, graphics, tip, flags )
{
	world->optimizer->dt       = relaxParameters->glob_dtmax;
	world->optimizer->damping  = relaxParameters->glob_damping;
	convergStepLimit = relaxParameters->glob_maxIters;
	molOfInterest = new int[numMolOfInterest];
	for( int i = 0; i < numMolOfInterest; i++ ) molOfInterest[i] = molOfInterest_[i];
	if( graphics != NULL ) graphics->drawBox( scan );
}
*/

void appModeFileScript::command_tipPos( char * line ){
	double x,y,z;
	sscanf( line, "%lf %lf %lf", &x, &y, &z );
	if( graphics != NULL ) printf( "   relaxing tip pos %f %f %f\n", x, y, z );
	bool converged = convergeTipPos( x, y, z );
	Vec3d tippos = world->tip->pos;
	Vec3d pppos  = world->systemCoordsOfAtom( world->tip->probeMol->mol, world->tip->probeMol->atom  );
	fprintf( pOut, "%f %f %f    %f %f %f    %f %f %f    %f %f %f\n", x,y,z,
			tippos.x,      tippos.y,      tippos.z,
			world->fTip.x, world->fTip.y, world->fTip.z,
			pppos.x,       pppos.y,       pppos.z
	);
}

void appModeFileScript::command_startStroke( char * line ){
	double x,y,z;
	sscanf( line, "%lf %lf %lf", &x, &y, &z );
	printf( "   starting stroke at %f %f %f \n", x, y, z );
	world->tip->setPosition( x, y, z );
	world->adjustMolToTip();
}

void appModeFileScript::loop( int n ){
	if( world == NULL    ){ printf( "appModeRaster::loop: No world exists. Halt.\n" );	return;	}
	if( world->nmols < 2 ){ printf( "appModeRaster::loop: Trivial world. Halt.\n" );	return;	}

	world->perFrame = 1000;

	int total = 0;

	char * script_name =  "scan.script";
	char * fout_name   =  "data.out";
	printf( " script ile:  %s  outfile:  %s \n", script_name, fout_name );
	pScript = fopen( script_name, "r" );
	pOut    = fopen( fout_name, "w" );
	int  iline = 0;
	char line[1024];
	while ( fgets ( line, sizeof(line), pScript ) ) {
		//printf( "%s[%06i]:  %s", fname, iline, line );
		iline++;
		total++;
		char* rest = strchr( line, ';' );
		int cmdlen = (int)(rest-line);
		if      ( 0==strncmp( "set_tip_pos",  line, cmdlen ) ){ command_tipPos     ( rest+1 ); }
		else if ( 0==strncmp( "start_stroke", line, cmdlen ) ){ command_startStroke( rest+1 ); }
	}
	fclose( pScript );
	fclose( pOut );

	printf( "\n=== SCANNING SUMMARY ===\n" );
	printf( "appModeRaster::loop: There were %i cases out of %i (%i) when the relaxation did not converge.\n", numOfNoncovergCases, ind + 1, total );


}
