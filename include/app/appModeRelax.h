
class appModeRelax : public appModeNoTip {
// relaxation mode

	public:
		
	appModeRelax(){};
	appModeRelax( int numOfMoleculeInstances_, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics, flagList* flags = NULL );	

	void loop( int n );
	
};


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
