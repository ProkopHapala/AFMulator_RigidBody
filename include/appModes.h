
#ifndef _APP_MODES_
#define _APP_MODES_

#include <string>

// APPLICATION FUNCTIONALITY MODES

class abstractAppMode {
// parent class for all application modes

	public:

	bool delay = false;
	bool suppressOutput = false;

	PhysicalSystem* world = NULL;
	int numOfMoleculeInstances = 0;	// number of molecule instances, many molecules may be of the same instance
					// an instance defines specific atom positions within a given molecule 

	MoleculeType** molp = NULL;	// molecules
					// a molecule defines orientation and position of the molecule in the global system
					// two molecules might be of the same molecule instance, in which case a modification of one molecule affects accordingly the other molecule
					// two molecules may have the same topology, i.e. the type, but their atoms might be of slightly different mutual positions, i.e. different instances
					
					// 2 basic modes: each molecule has its own instance -- in such a case modification of a given molecule does not affects other molecules
					// 		  several molecules share the same instance -- in such a case modifications are also shared, but in this case less memory is used

	
	
	graphInterface* graphics = NULL;

	virtual void loop( int n ){};
	abstractAppMode(){};
	abstractAppMode( graphInterface* graphics_, int numOfMoleculeInstances_, bool delay_ = false, bool suppressOutput_ = false );
	abstractAppMode( graphInterface* graphics_, int numOfMoleculeInstances_, flagList *flags = NULL );
	virtual ~abstractAppMode();

};

class appModeNoTip : public abstractAppMode {
// parent class for modes without a tip

	public:
	
	appModeNoTip(){};
	~appModeNoTip();
	appModeNoTip( int numOfMoleculeInstances_, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics_, flagList *flags = NULL );
	
	void setupApp( fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf );
	
};

class appModeView : public appModeNoTip {
// view mode

	public:
	
	appModeView(){};
	appModeView( int numOfMoleculeInstances, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics, flagList *flags = NULL );
	
	void loop( int n );
	
};

class appModeRelax : public appModeNoTip {
// relaxation mode

	public:
		
	appModeRelax(){};
	appModeRelax( int numOfMoleculeInstances_, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics, flagList* flags = NULL );	

	void loop( int n );
	
};

class appModeTip : public abstractAppMode {
// parent class for modes with a tip

	public:
	
	abstractTip* tip = NULL;

	appModeTip(){};
	appModeTip( graphInterface* graphics_, int numOfMoleculeInstances_, abstractTip* tip_, flagList *flags );
	~appModeTip();

};

class appModeTipMol : public appModeTip {
// parent class for modes with a tip to which the probe molecule is attached

	public:

	std::string geometryFileName;
	fileWrapper* geometryFile = NULL;

	appModeTipMol(){};
	appModeTipMol( int numOfMoleculeInstances, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics, abstractTip* tip, flagList *flags );
		
	int tipMol = -1;
	
	Vec3d posProbe;
	Quat4d rotProbe;
	
	void setupApp( fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, std::string probeMolFileName );
	
	~appModeTipMol();

};

class appModeTipMolOne : public appModeTipMol {
// only a single z-direction scanning 

	public:

	appModeTipMolOne(){};
	appModeTipMolOne( int numOfMoleculeInstances, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics, abstractTip* tip, flagList *flags );	
	
	double xpos = 3, ypos = -3, zpos = 12;
	
	void loop( int n );

};

class appModeRaster : public appModeTipMol {
// 2D scanning

	public:

	scanSpecification* scan = NULL;
	fileManager* outputFiles = NULL;
	fileManager* outputPositMolFiles = NULL;
	fileManager* outputRotatMolFiles = NULL;
	
	void resetup( void );
	
	int convergStepLimit = 0;
	int* molOfInterest = NULL;
	int numMolOfInterest = 0;

	appModeRaster(){};
	appModeRaster( int numOfMoleculeInstances, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics, abstractTip* tip, flagList *flags, scanSpecification* scan, fileManager* outputFiles, relaxParams* relaxParameters, int* molOfInterest_, int numMolOfInterest_, fileManager* outputPositMolFiles_ = NULL, fileManager* outputRotatMolFiles_ = NULL );
	
	void loop( int n );

};

class appModeReplay : public appModeTipMol {
// replay mode where no data is calculated but rather existing data is replayed

	public:

	scanSpecification* scan = NULL;
	
	// following files are prefixed output* even though they are used for import, the prefix is retained due to keeping clear files origin in mode appModeRaster
	fileManager* outputFiles = NULL;
	fileManager* outputPositMolFiles = NULL;
	fileManager* outputRotatMolFiles = NULL;

	int* molOfInterest = NULL;
	int numMolOfInterest = 0;

	appModeReplay(){};
	appModeReplay( int numOfMoleculeInstances_, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics_, abstractTip* tip, flagList *flags, scanSpecification* scan, fileManager* outputFiles, int* molOfInterest_, int numMolOfInterest_, fileManager* outputPositMolFiles_ = NULL, fileManager* outputRotatMolFiles_ = NULL );
	
	void loop( int n );

};

// ================= abstractAppMode procedures ================= 

abstractAppMode::~abstractAppMode(){
// destructor
}

abstractAppMode::abstractAppMode( graphInterface* graphics_, int numOfMoleculeInstances_, flagList *flags )
// constructor
: numOfMoleculeInstances( numOfMoleculeInstances_ ), graphics( graphics_ )
{
	if( flags != NULL ){
		delay = flags->delay;
		suppressOutput = flags->suppressOutput;
	} else {
		delay = false;
		suppressOutput = false;
	}

}

abstractAppMode::abstractAppMode( graphInterface* graphics_, int numOfMoleculeInstances_, bool delay_, bool suppressOutput_ )
// constructor
: numOfMoleculeInstances( numOfMoleculeInstances_), graphics( graphics_), delay( delay_ ), suppressOutput( suppressOutput_ )
{
}

// ================= appModeNoTip procedures ================= 

appModeNoTip::appModeNoTip( int numOfMoleculeInstances_, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics_, flagList *flags )
// constructor
: abstractAppMode( graphics_, numOfMoleculeInstances_, flags )
{
	setupApp( moleculeFiles, typeList, geometryFile, surf );
}

void appModeNoTip::setupApp( fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf ){
// setting of all relevant parameters

	// lenght of an array listOfFileNames is assumed to be less than the maximum of an array listOfMoleculeInstances!!!

	abstractAppMode::molp = new MoleculeType*[abstractAppMode::numOfMoleculeInstances];
	for( int i = 0; i < abstractAppMode::numOfMoleculeInstances; i++ ){
		if( abstractAppMode::graphics != NULL ){
			abstractAppMode::molp[i] = new MoleculeType( (*moleculeFiles)[i]->getFileName(), typeList, 0.5, abstractAppMode::graphics->viewMolList );
		} else {
			abstractAppMode::molp[i] = new MoleculeType( (*moleculeFiles)[i]->getFileName(), typeList, 0.5, NULL );
		}
	}
	abstractAppMode::world = new PhysicalSystem( geometryFile, abstractAppMode::numOfMoleculeInstances, abstractAppMode::molp, NULL, surf );
	if( abstractAppMode::world == NULL )
		printf( "abstractAppMode: World cannot be allocated.\n" );
	if( abstractAppMode::graphics != NULL ) abstractAppMode::graphics->world = abstractAppMode::world;

}

appModeNoTip::~appModeNoTip(){
// destructor

	if( world != NULL ) delete world;
//	graphics = NULL; // graphics is nothing, but pointer to external graphics object
	if( molp != NULL ){
		for( int i = 0; i < numOfMoleculeInstances; i++ ){
			delete molp[i];
		}
		delete [] molp;
	}
	numOfMoleculeInstances = 0;
}

// ================= appModeView procedures ================= 

appModeView::appModeView( int numOfMoleculeInstances, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics, flagList *flags )
// constructor
: appModeNoTip( numOfMoleculeInstances, moleculeFiles, typeList, geometryFile, surf, graphics, flags )
{
}

void appModeView::loop( int n ){
// view loop

	bool loopEnd = false, loopContinue, stopFlag = false;

	if( abstractAppMode::world == NULL ){
		printf( "appModeView::loop: No world exists. Halt.\n" );
		return;
	}

	if( abstractAppMode::graphics != NULL ){
		for( int iframe = 0; iframe < n; iframe++ ){
			// handle input events
			abstractAppMode::graphics->inputHandling( loopEnd, loopContinue, stopFlag );
			if( loopEnd ) break;
			
			// update screens
			abstractAppMode::graphics->updateGraphics();
				
			SDL_Delay( 10 );
		}

	} else {
		printf( "appModeView::loop: No graphics exists. Halt.\n" );	
	}

}

// ================= appModeRelax procedures ================= 

appModeRelax::appModeRelax( int numOfMoleculeInstances, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics, flagList* flags  )
// constructor
: appModeNoTip( numOfMoleculeInstances, moleculeFiles, typeList, geometryFile, surf, graphics, flags )
{
}

void appModeRelax::loop( int n ){
// relax loop

	bool optimizingFlag = true;
	bool loopEnd = false, loopContinue, stopFlag = false;
	int convergStep = 0;
	int convergStepLimit = n;
	double pixelDataListItem; // just due to canonical argument list of update function
			
	if( abstractAppMode::world == NULL ){
		printf( "appModeRelax::loop: No world exists. Halt.\n" );
		return;
	}
	
	for( int iframe = 0; iframe < n; iframe++ ){

		if( abstractAppMode::graphics != NULL ){
			// update screens
			abstractAppMode::graphics->updateGraphics();
			
			// handle input events
			abstractAppMode::graphics->inputHandling( loopEnd, loopContinue, stopFlag );
			if( loopEnd ) break;
		}
		
		// update world			
		if( abstractAppMode::world->getSysEvol() ){
			if( abstractAppMode::graphics != NULL && abstractAppMode::world->tip != NULL ){
				abstractAppMode::graphics->thisScreen->mouseSetAuxPoint( abstractAppMode::world );
			}
			
			abstractAppMode::world->update( optimizingFlag, pixelDataListItem );
//			printf( "loopSingleRelaxation: step %i\n", convergStep );
			convergStep++;
		}
		
		if( convergStep > convergStepLimit ){
			if( !abstractAppMode::suppressOutput )
				printf( "appModeRelax::loop: Relaxation does not converge. Skipped.\n" );
			break;
		}
		
		if( abstractAppMode::graphics != NULL && (abstractAppMode::delay || stopFlag || !optimizingFlag) ) SDL_Delay( 10 );

	}

}

// ================= appModeTip procedures ================= 

appModeTip::~appModeTip(){
// destructor

	delete tip;

}

appModeTip::appModeTip( graphInterface* graphics_, int numOfMoleculeInstances_, abstractTip* tip_, flagList *flags )
// constructor
: tip( tip_ ), abstractAppMode( graphics_, numOfMoleculeInstances_, flags )
{
}


// ================= appModeTipMol procedures ================= 

appModeTipMol::appModeTipMol( int numOfMoleculeInstances, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile_, abstractSurf* surf, graphInterface* graphics, abstractTip* tip, flagList *flags )
// constructor
: geometryFile( geometryFile_ ), posProbe( tip->probeMol->pos ), rotProbe( tip->probeMol->rot ), appModeTip( graphics, numOfMoleculeInstances, tip, flags )
{
	setupApp( moleculeFiles, typeList, geometryFile, surf, tip->probeMol->fileName );
}

appModeTipMol::~appModeTipMol(){
// destructor

	if( abstractAppMode::world != NULL ) delete world;
//	abstractAppMode::graphics = NULL; // graphics is nothing, but a pointer to external graphics object
	if( abstractAppMode::molp != NULL ){
		for( int i = 0; i < abstractAppMode::numOfMoleculeInstances; i++ ){
			delete abstractAppMode::molp[i];
		}
		delete abstractAppMode::molp[abstractAppMode::numOfMoleculeInstances]; // delete also a probe molecule
		delete [] abstractAppMode::molp;
	}
	abstractAppMode::numOfMoleculeInstances = 0;

}

void appModeTipMol::setupApp( fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, std::string probeMolFileName ){
// setting up all relevant parameters

	// lenght of an array listOfFileNames is assumed to be less than the maximum of an array listOfMoleculeInstances!!!

	abstractAppMode::molp = new MoleculeType*[abstractAppMode::numOfMoleculeInstances + 1];
	for( int i = 0; i < abstractAppMode::numOfMoleculeInstances; i++ ){
		if( abstractAppMode::graphics != NULL ){
			abstractAppMode::molp[i] = new MoleculeType( (*moleculeFiles)[i]->getFileName(), typeList, 0.5, abstractAppMode::graphics->viewMolList );
		} else {
			abstractAppMode::molp[i] = new MoleculeType( (*moleculeFiles)[i]->getFileName(), typeList, 0.5, NULL );
		}
	}
	if( graphics != NULL ){
		abstractAppMode::molp[numOfMoleculeInstances] = new MoleculeType( probeMolFileName.c_str(), typeList, 0.5, abstractAppMode::graphics->viewMolList ); // probe particle
	} else {
		abstractAppMode::molp[numOfMoleculeInstances] = new MoleculeType( probeMolFileName.c_str(), typeList, 0.5, NULL ); // probe particle
	}
	geometryFileName.assign( geometryFile->getFileName() );
	
	abstractAppMode::world = new PhysicalSystem( geometryFile, abstractAppMode::numOfMoleculeInstances, abstractAppMode::molp, appModeTip::tip, surf );
	if( abstractAppMode::world == NULL )
		printf( "abstractAppMode: World cannot be allocated.\n" );
	if( abstractAppMode::graphics != NULL ) abstractAppMode::graphics->world = abstractAppMode::world;
	tipMol = abstractAppMode::world->nmols - 1;
	appModeTip::tip->probeMol->mol = tipMol;
	abstractAppMode::world->chectTipMolAtom();

}

// ================= appModeTipMolOne procedures ================= 

appModeTipMolOne::appModeTipMolOne( int numOfMoleculeInstances, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics, abstractTip* tip, flagList *flags )	
// constructor
: appModeTipMol( numOfMoleculeInstances, moleculeFiles, typeList, geometryFile, surf, graphics, tip, flags )
{
}

void appModeTipMolOne::loop( int n ){
// one z-axis scanning loop
	
	bool optimizingFlag = true;
	bool loopEnd = false, loopContinue, stopFlag = false;
	int convergStep = 0;
	int convergStepLimit = n;
	double pixelDataListItem;
			
	if( abstractAppMode::world == NULL ){
		printf( "appModeTipMolOne::loop: No world exists. Halt.\n" );
		return;
	}
	
	abstractAppMode::world->tip->setPosition( xpos, ypos, zpos );
	abstractAppMode::world->adjustMolToTip();
	
	for( int iframe = 0; iframe < n; iframe++ ){
	
		if( abstractAppMode::graphics != NULL ){
			// update screens			
			abstractAppMode::graphics->updateGraphics();
			
			// handle input events
			abstractAppMode::graphics->inputHandling( loopEnd, loopContinue, stopFlag );
			if( loopEnd ) break;
		}
		
		// update world			
		if( abstractAppMode::world->getSysEvol() ){
			if( abstractAppMode::graphics != NULL && abstractAppMode::world->tip != NULL ){
				abstractAppMode::graphics->thisScreen->mouseSetAuxPoint( abstractAppMode::world );
			}
			abstractAppMode::world->update( optimizingFlag, pixelDataListItem );
//			printf( "loopSingleRelaxationPerm: step %i\n", convergStep );
			convergStep++;
		}
		
		if( convergStep > convergStepLimit ){
			if( !abstractAppMode::suppressOutput )
				printf( "appModeTipMolOne::loop: Relaxation does not converge. Skipped.\n" );
			break;
		}
		
		if( abstractAppMode::graphics != NULL && (abstractAppMode::delay || stopFlag) ) SDL_Delay( 10 );

	}
}

// ================= appModeRaster procedures ================= 

void appModeRaster::resetup( void ){
// resetup the world geometry

	abstractAppMode::world->resetGeometry( appModeTipMol::geometryFile, appModeTipMol::posProbe, appModeTipMol::rotProbe );
	
}

appModeRaster::appModeRaster( int numOfMoleculeInstances, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics, abstractTip* tip, flagList *flags, scanSpecification* scan, fileManager* outputFiles_, relaxParams* relaxParameters, int* molOfInterest_, int numMolOfInterest_, fileManager* outputPositMolFiles_, fileManager* outputRotatMolFiles_ )
// constructor
: scan( scan ), outputFiles( outputFiles_ ), numMolOfInterest( numMolOfInterest_ ), outputPositMolFiles( outputPositMolFiles_ ), outputRotatMolFiles( outputRotatMolFiles_ ), appModeTipMol( numOfMoleculeInstances, moleculeFiles, typeList, geometryFile, surf, graphics, tip, flags )
{
	abstractAppMode::world->optimizer->dt       = relaxParameters->glob_dtmax;
	abstractAppMode::world->optimizer->damping  = relaxParameters->glob_damping;
	convergStepLimit = relaxParameters->glob_maxIters;

	molOfInterest = new int[numMolOfInterest];
	for( int i = 0; i < numMolOfInterest; i++ ) molOfInterest[i] = molOfInterest_[i];
	
	if( abstractAppMode::graphics != NULL ) abstractAppMode::graphics->drawBox( scan );

}

void appModeRaster::loop( int n ){
// 2D scanning loop
			
	if( abstractAppMode::world == NULL ){
		printf( "appModeRaster::loop: No world exists. Halt.\n" );
		return;
	}
	
	if( abstractAppMode::world->nmols < 2 ){
		printf( "appModeRaster::loop: Trivial world. Halt.\n" );
		return;
	}
		
	// declarations and initialization
	double	xpos, ypos, zpos, pixelDataListItem;
	int	ind, convergStep, numOfNoncovergCases = 0;
	bool	optimizingFlag = true, loopContinue = false;
	bool	loopEnd = false, stopFlag = false;
	int	total = (scan->xdim)*(scan->ydim)*(scan->zdim);
	
	bool	saveMolOfInterestMovement = false;
	if( outputPositMolFiles != NULL && outputRotatMolFiles != NULL ){
		saveMolOfInterestMovement = true;	
		for( int i = 0; i < numMolOfInterest; i++ ){
			if( molOfInterest[i] > abstractAppMode::world->nmols - 1 ){
				printf( "appModeRaster::loop: molOfInterest[%i] too big. Set the highest valid value instead.\n", i );
				molOfInterest[i] = abstractAppMode::world->nmols - 2;
				// NEOSETRENO, KDYZ TO ORIZNE VIC NEZ JEDNU MOLEKULU TAKHLE, PAK SE UKLADA DO VICE SOUBORU TOTEZ
			}
		}
	}
	
	// allocation of data lists
	int metadataList[] = { scan->xdim, scan->ydim, scan->zdim, total };
	
	scalarDataWrapper* pixelDataList = new scalarDataWrapper( total, metadataList, "pixels" );
	vectorDataWrapper* positDataList = new vectorDataWrapper( total, metadataList, "trajectories" );
	quaterDataWrapper* rotatDataList = new quaterDataWrapper( total, metadataList, "rotations" );
	
	vectorDataManager* positMolDataList = NULL;
	quaterDataManager* rotatMolDataList = NULL;
	if( saveMolOfInterestMovement ){
		positMolDataList = new vectorDataManager( numMolOfInterest, total, metadataList, "mol. trajectories" );
		rotatMolDataList = new quaterDataManager( numMolOfInterest, total, metadataList, "mol. orientations" );
	}
	
	// generation of z-step sequence
	scalarDataWrapper* zSteps = new scalarDataWrapper( scan->zdim, metadataList, "z-steps" );
	scan->createZSamplingSequenceLoc( zSteps );
			
	// scanning loop
	printf( "\n=== CALCULATION LOOP INITIATED ===\n" );
	for( int yind = scan->ydim - 1; yind >= 0 ; yind-- ){
	
		ypos = yind*scan->ystep + scan->yoffset;
		for( int xind = 0; xind < scan->xdim; xind++ ){

			xpos = xind*scan->xstep + scan->xoffset;

			zpos = scan->zoffset;
			abstractAppMode::world->tip->setPosition( xpos, ypos, zpos );
			abstractAppMode::world->adjustMolToTip();

			for( int zind = 0; zind < scan->zdim ; zind++ ){
				
				optimizingFlag = true;
				loopContinue = false;
				
				convergStep = 0;
				ind = (scan->ydim - 1 - yind)*scan->xdim*scan->zdim + xind*scan->zdim + zind;
						
				if( !abstractAppMode::suppressOutput ){		
					if( abstractAppMode::graphics != NULL ){
						if( !(ind % 200) ) printf( "index %7i out of %i reached\n", ind, total );
					} else {
						if( !(ind % 1000) ) printf( "index %7i out of %i reached\n", ind, total );
					}
				}
				
				for( int iframe = 0; iframe < n; iframe++ ){
	
					// handle input events
					if( abstractAppMode::graphics != NULL )
						abstractAppMode::graphics->inputHandling( loopEnd, loopContinue, stopFlag );
					if( loopEnd || loopContinue ) break;
		
					// update world			
					if( abstractAppMode::world->getSysEvol() ){
						if( abstractAppMode::graphics != NULL && abstractAppMode::world->tip != NULL ){
							abstractAppMode::graphics->thisScreen->mouseSetAuxPoint( abstractAppMode::world );
						}				
						abstractAppMode::world->update( optimizingFlag, pixelDataListItem );				
						convergStep++;
					}
				
					// update screens
					if( abstractAppMode::graphics != NULL )
						abstractAppMode::graphics->updateGraphics();

					// others
					if( !optimizingFlag ){
//						printf( "convergStep = %i\n", convergStep );	
						break;
					}
					if( convergStep > convergStepLimit ){
						if( !abstractAppMode::suppressOutput )
							printf( "appModeRaster::loop: Relaxation in step %5i does not converge. Skipped.\n", ind );
//						printf( "xind, yind, zind = %d %d %d\n", xind, yind, zind );
						numOfNoncovergCases++;
						break;
					}
					
					if( abstractAppMode::delay || stopFlag ){
						SDL_Delay( 20 );
					}

				}
				if( loopEnd ) break;
						
				// saving data to lists			
				pixelDataList->setValue( ind, pixelDataListItem );
				positDataList->setValue( ind, abstractAppMode::world->pos[appModeTipMol::tipMol] );
				rotatDataList->setValue( ind, abstractAppMode::world->rot[appModeTipMol::tipMol] );
				
				if( saveMolOfInterestMovement ){
					for( int i = 0; i < numMolOfInterest; i++ ){
						positMolDataList->setValue( i, ind, abstractAppMode::world->pos[molOfInterest[i]] );
						rotatMolDataList->setValue( i, ind, abstractAppMode::world->rot[molOfInterest[i]] );
					}
				}
	
				// readjust the tip
				zpos -= (*zSteps)[zind]; // movement downwards!!!
				abstractAppMode::world->tip->setPosition( xpos, ypos, zpos );
				
			}
			if( loopEnd ) break;
			resetup();
		}
		if( loopEnd ) break;
	}

	printf( "\n=== SCANNING SUMMARY ===\n" );		
	if( loopEnd ) printf( "appModeRaster::loop: Last index calculated is %i out of %i.\n", ind, total );
	printf( "appModeRaster::loop: There were %i cases out of %i (%i) when the relaxation did not converge.\n", numOfNoncovergCases, ind + 1, total );
	
	printf( "\n=== SAVING DATA ===\n" );	
	// export positDataList, pixelDataList, zSteps, and rotatDataList respectively
	outputFiles->getFile( 0 )->exportDataToFile( "appModeRaster::loop", positDataList );
	outputFiles->getFile( 1 )->exportDataToFile( "appModeRaster::loop", pixelDataList );
	outputFiles->getFile( 2 )->exportDataToFile( "appModeRaster::loop", zSteps );
	outputFiles->getFile( 3 )->exportDataToFile( "appModeRaster::loop", rotatDataList );

	// export trajectories and orientations of the molecule of interest
	if( saveMolOfInterestMovement ){
		outputPositMolFiles->exportDataToFiles( "appModeRaster::loop", positMolDataList );
		outputRotatMolFiles->exportDataToFiles( "appModeRaster::loop", rotatMolDataList );
	}
	
	// deallocation
	delete pixelDataList;
	delete positDataList;
	delete zSteps;
	delete rotatDataList;

	if( saveMolOfInterestMovement ){
		delete positMolDataList;
		delete rotatMolDataList;
	}
	
}

// ================= appModeReplay procedures ================= 

appModeReplay::appModeReplay( int numOfMoleculeInstances_, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics_, abstractTip* tip, flagList *flags, scanSpecification* scan, fileManager* outputFiles_, int* molOfInterest_, int numMolOfInterest_, fileManager* outputPositMolFiles_, fileManager* outputRotatMolFiles_ )
// constructor
: scan( scan ), outputFiles( outputFiles_ ), numMolOfInterest( numMolOfInterest_ ), outputPositMolFiles( outputPositMolFiles_ ), outputRotatMolFiles( outputRotatMolFiles_ ), appModeTipMol( numOfMoleculeInstances_, moleculeFiles, typeList, geometryFile, surf, graphics_, tip, flags )
{
	molOfInterest = new int[numMolOfInterest];
	for( int i = 0; i < numMolOfInterest; i++ ) molOfInterest[i] = molOfInterest_[i];
	
	if( abstractAppMode::graphics != NULL ) abstractAppMode::graphics->drawBox( scan );
}

void appModeReplay::loop( int n ){
// replay loop

	// if something is wrong, halt
	if( abstractAppMode::world == NULL ){
		printf( "appModeReplay::loop: No world exists. Halt.\n" );
		return;
	}
	
	if( abstractAppMode::world->nmols < 2 ){
		printf( "appModeReplay::loop: Trivial world. Halt.\n" );
		return;
	}
	
	if( abstractAppMode::graphics == NULL ){
		printf( "appModeReplay::loop: Nothing to show. Halt.\n" );
		return;
	}
	
	bool loopEnd = false, stopFlag = false, loopContinue = false;
	double	xpos, ypos, zpos;
	int	total = (scan->xdim)*(scan->ydim)*(scan->zdim);
	
	bool	showMolOfInterestMovement = false;
	if( outputPositMolFiles != NULL && outputRotatMolFiles != NULL ){
		showMolOfInterestMovement = true;	
		for( int i = 0; i < numMolOfInterest; i++ ){
			if( molOfInterest[i] > abstractAppMode::world->nmols - 1 ){
				printf( "appModeRaster::loop: molOfInterest[%i] too big. Set the highest valid value instead.\n", i );
				molOfInterest[i] = abstractAppMode::world->nmols - 2;
				// NEOSETRENO, KDYZ TO ORIZNE VIC NEZ JEDNU MOLEKULU TAKHLE, PAK SE UKLADA DO VICE SOUBORU TOTEZ
			}
		}
	}

	// import data:
	int metadataList[4] = { 0, 0, 0, 0 };
	int numOfStrings = 2;
	
	vectorDataWrapper* positDataList = new vectorDataWrapper( total, metadataList, "trajectories" );
	quaterDataWrapper* rotatDataList = new quaterDataWrapper( total, metadataList, "rotations" );
	
	vectorDataManager* positMolDataList = NULL;
	quaterDataManager* rotatMolDataList = NULL;
	if( showMolOfInterestMovement ){
		positMolDataList = new vectorDataManager( numMolOfInterest, total, metadataList, "mol. trajectories" );
		rotatMolDataList = new quaterDataManager( numMolOfInterest, total, metadataList, "mol. rotations" );
	}

	// generation of z-step sequence
	scalarDataWrapper* zSteps = new scalarDataWrapper( scan->zdim, metadataList, "z-steps" );
	scan->createZSamplingSequenceLoc( zSteps );
	
	// import positDataList and rotatDataList
	(*outputFiles)[0]->importDataFromFile( "appModeReplay::loop", positDataList );
	(*outputFiles)[3]->importDataFromFile( "appModeReplay::loop", rotatDataList ); // 3 !!!

	// import trajectories and orientations of the molecule of interest
	if( showMolOfInterestMovement ){
		numOfStrings += 2*numMolOfInterest;	
		outputPositMolFiles->importDataFromFiles( "appModeReplay::loop", positMolDataList );
		outputRotatMolFiles->importDataFromFiles( "appModeReplay::loop", rotatMolDataList );
	}	
	
	moveReplayScanMode moveReplayScan = REPLAY_MOVE_NONE;
	
	int xind = 0;
	int yind = scan->ydim - 1;
	int zind = 0;
	int ind = 0;

	xpos = scan->xoffset;
	ypos = yind*scan->ystep + scan->yoffset;
	zpos = scan->zoffset;
	
	// export metadata
	time_t rawtime;
	std::string stringTime;
	
  	time( &rawtime );
	stringTime.assign( asctime( localtime( &rawtime ) ) );

	stringDataWrapper* metadataStrings = new stringDataWrapper( numOfStrings, &stringTime, "metadata" );
	
	// export names of files containing trajectories and orientations of the molecule of interest
	if( showMolOfInterestMovement ){
		for( int i = 0; i < numMolOfInterest; i++ ){
			metadataStrings->setValue( 2*i + 0, (*outputPositMolFiles)[i]->getFileName() );
			metadataStrings->setValue( 2*i + 1, (*outputRotatMolFiles)[i]->getFileName() );
		}
	}
	metadataStrings->setValue( numOfStrings - 2, (*outputFiles)[0]->getFileName() );
	metadataStrings->setValue( numOfStrings - 1, (*outputFiles)[3]->getFileName() );

	fileWrapper* metadataFile = new fileWrapper( "OutputData/replayConfigFile" );
	metadataFile->exportDataToFile( "appModeReplay::loop", metadataStrings );
	
	// replay loop	
	printf( "\n=== REPLAY MODE LAUNCHED ===\n" );
	
	while( true ){
	
		loopContinue = false;
		moveReplayScan = REPLAY_MOVE_NONE;
		ind = (scan->ydim - 1 - yind)*scan->xdim*scan->zdim + xind*scan->zdim + zind;
				
		abstractAppMode::world->tip->setPosition( xpos, ypos, zpos );
		abstractAppMode::world->pos[tipMol].set( (*positDataList)[ind] );
		abstractAppMode::world->rot[tipMol].set( (*rotatDataList)[ind] );
		
		if( showMolOfInterestMovement ){
			for( int i = 0; i < numMolOfInterest; i++ ){
				abstractAppMode::world->pos[molOfInterest[i]].set( (*(*positMolDataList)[i])[ind] );
				abstractAppMode::world->rot[molOfInterest[i]].set( (*(*rotatMolDataList)[i])[ind] );
			}
		}

		// handle input events
		if( abstractAppMode::graphics != NULL )
			abstractAppMode::graphics->inputHandling( loopEnd, loopContinue, stopFlag, moveReplayScan );
		if( loopEnd ) break;
		if( loopContinue ){
			zpos -= (*zSteps)[zind];
			zind++;
			if( zind > scan->zdim - 2 ){
				zpos = scan->zoffset;
				zind = 0;
			}
			continue;
		}
		
		switch( moveReplayScan ){
		
			case REPLAY_MOVE_LEFT:
				xpos -= scan->xstep;
				xind--;
				if( xpos < scan->xoffset ){
					xpos += scan->xstep;
					xind++;
				}
				break;

			case REPLAY_MOVE_RIGHT:
				xpos += scan->xstep;
				xind++;
				if( ((xpos - scan->xoffset) / (double) scan->xstep) > scan->xdim - 1 ){
					xpos -= scan->xstep;
					xind--;
				}
				break;

			case REPLAY_MOVE_DOWN:
				ypos -= scan->ystep;
				yind--;
				if( ypos < scan->yoffset ){
					ypos += scan->ystep;
					yind++;
				}
				break;

			case REPLAY_MOVE_UP:
				ypos += scan->ystep;
				yind++;
				if( ((ypos - scan->yoffset) / (double) scan->ystep) > scan->ydim - 1 ){
					ypos -= scan->ystep;
					yind--;
				}
				break;

			case REPLAY_MOVE_OUT:
				zpos += (*zSteps)[zind];
				zind--;
				if( zind < 0 ){
					zind = 0;
					zpos = scan->zoffset;
//					zind++;
//					zpos -= (*zSteps)[zind];
				}
				break;
				
//			case REPLAY_MOVE_NONE:
//			default:
		}
		
		// update screens
		for( int i = 0; i < abstractAppMode::graphics->nScreensMax; i++ ) {
			if( abstractAppMode::graphics->screens[i] != NULL ) {
				abstractAppMode::graphics->screens[i]->updateScreen( abstractAppMode::world );
				abstractAppMode::graphics->screens[i]->drawScreen( abstractAppMode::world );
			}
		}

		SDL_Delay( 100 );
		
		if( abstractAppMode::delay || stopFlag ){
			SDL_Delay( 50 );
		}

		if( !stopFlag ){
			zpos -= (*zSteps)[zind];
			zind++;
			if( zind > scan->zdim - 2 ){
				zpos = scan->zoffset;
				zind = 0;
			}
		}

	}	
	
	// deallocation
	delete positDataList;
	delete zSteps;
	delete rotatDataList;
	delete metadataFile;
	
	if( showMolOfInterestMovement ){
		delete positMolDataList;
		delete rotatMolDataList;		
	}

}


#endif
