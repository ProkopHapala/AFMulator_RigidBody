
class appModeView : public appModeNoTip {
// view mode

	public:
	
	appModeView(){};
	appModeView( int numOfMoleculeInstances, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, flagList *flags = NULL );
	
	void loop( int n );
	
};


// ================= appModeView procedures ================= 

appModeView::appModeView( int numOfMoleculeInstances, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, flagList *flags )
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

