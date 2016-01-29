
class appModeRelax : public appModeNoTip {  // relaxation mode
	public:
	// ===== functions
	void loop( int n );
	// ==== construnctor & destructor
	appModeRelax(){};
	appModeRelax( int numOfMoleculeInstances_, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics, flagList* flags = NULL );
};

// ================= appModeRelax procedures =================

appModeRelax::appModeRelax( int numOfMoleculeInstances, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics, flagList* flags  )
// constructor
: appModeNoTip( numOfMoleculeInstances, moleculeFiles, typeList, geometryFile, surf, graphics, flags ){}

void appModeRelax::loop( int n ){  // relax loop
	bool optimizingFlag = true;
	bool loopEnd = false, loopContinue, stopFlag = false;
	int convergStep = 0;
	int convergStepLimit = n;
	double pixelDataListItem; // just due to canonical argument list of update function
	if( world == NULL ){
		printf( "appModeRelax::loop: No world exists. Halt.\n" );
		return;
	}
	for( int iframe = 0; iframe < n; iframe++ ){
		if( graphics != NULL ){
			// update screens
			graphics->updateGraphics();
			// handle input events
			graphics->inputHandling( loopEnd, loopContinue, stopFlag );
			if( loopEnd ) break;
		}
		// update world
		//if( world->getSysEvol() ){
		if( world->sysEvol ){
			if( graphics != NULL && world->tip != NULL ){
				graphics->thisScreen->mouseSetAuxPoint( world );
			}
			//world->update( optimizingFlag, pixelDataListItem );
			world->update( optimizingFlag );
			pixelDataListItem = world->fTip.z;
//			printf( "loopSingleRelaxation: step %i\n", convergStep );
			convergStep++;
		}
		if( convergStep > convergStepLimit ){
			if( !suppressOutput )
				printf( "appModeRelax::loop: Relaxation does not converge. Skipped.\n" );
			break;
		}
		if( graphics != NULL && (delay || stopFlag || !optimizingFlag) ) SDL_Delay( 10 );
	}
}
