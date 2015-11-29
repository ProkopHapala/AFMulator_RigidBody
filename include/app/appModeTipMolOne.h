
class appModeTipMolOne : public appModeTipMol {
// only a single z-direction scanning 

	public:

	appModeTipMolOne(){};
	appModeTipMolOne( int numOfMoleculeInstances, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics, abstractTip* tip, flagList *flags );	
	
	double xpos = 3, ypos = -3, zpos = 12;
	
	void loop( int n );

};


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
