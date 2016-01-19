
#ifndef _APP_MODES_
#define _APP_MODES_

#include <string>

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


	static const int nScreensMax;
	Screen* thisScreen = NULL;
	Screen** screens   = NULL;
	Uint32* screensID  = NULL;
	
	static float viewMolList[4];			// parameters for molecules' look, KDYZTAK DODELAT
	
	SDL_Event event;
	bool scanningBoxFlag;


	// ========== functions
	
	// moved from graphInterface
	void drawBox( scanSpecification* scan );
	void setActiveScreen( Uint32 windowID );
	virtual void setupGraphics( bool scanningBoxFlag_ );
	virtual void updateGraphics();
	//virtual void inputHandling( bool& loopEnd, bool& loopContinue, bool& stopFlag );
	virtual void inputHandling( bool& loopEnd, bool& loopContinue, bool& stopFlag, moveReplayScanMode& moveReplayScan );
	

	virtual void loop( int n ){};
	abstractAppMode(){};
	//abstractAppMode( graphInterface* graphics_, int numOfMoleculeInstances_, bool delay_ = false, bool suppressOutput_ = false );
	//abstractAppMode( graphInterface* graphics_, int numOfMoleculeInstances_, flagList *flags = NULL );
	abstractAppMode( int numOfMoleculeInstances_, flagList *flags = NULL );
	virtual ~abstractAppMode();

};



// ================== Implementation of functions

const int abstractAppMode::nScreensMax = 1;
float     abstractAppMode::viewMolList[] = { 5, 4, 1, 0.1 };

void abstractAppMode::updateGraphics(){
	for( int i = 0; i < nScreensMax; i++ ) {
		if( screens[i] != NULL ) {
			screens[i]->updateScreen( world );
			screens[i]->drawScreen( world );
		}
	}

}


void abstractAppMode::drawBox( scanSpecification* scan ){
	if( scanningBoxFlag ){
		Vec3d origin, end;
		origin.set( 0, 0, 0 );
		end.set( 10, 0, 0 );
		for( int i = 0; i < nScreensMax; i++ ){
			screens[i]->renderScanningBox( scan );
			screens[i]->renderMeasuringScale( origin, end, screens[i]->measuringScalePerm );
		}
	}
}

void abstractAppMode::setActiveScreen( Uint32 windowID ){
	for( int i = 0; i < nScreensMax; i++ ){
		if( screensID[i] == windowID ){
			thisScreen = screens[i];
//			printf( "screensID[i] = %i, setActiveScreen: i = %i\n", screensID[i], i );
			break;
		}
	}
	
}

void abstractAppMode::inputHandling( bool& loopEnd, bool& loopContinue, bool& stopFlag, moveReplayScanMode& moveReplayScan ){
	while( SDL_PollEvent( &event ) ){
		//thisScreen->inputHandlingBare( event, world, loopEnd );
		switch( event.type ){
			case SDL_KEYDOWN:
				switch( event.key.keysym.sym ){
					case SDLK_ESCAPE:
						loopEnd = true;
						break;
					case SDLK_SPACE:
						world->setSysEvol( stopFlag );	
						stopFlag = !stopFlag;
						printf( stopFlag ? "inputHandling: Relaxation paused.\n" : "inputHandling: Relaxation unpaused.\n" );
						break;
					case SDLK_F1:
						showHelp();
						break;
					case SDLK_RETURN:
					case SDLK_KP_ENTER:
						loopContinue = true;
						break;
					case SDLK_e:
						world->exportData();
						break;	
						
					// following used in replay mode

					case SDLK_KP_4:
						moveReplayScan = REPLAY_MOVE_LEFT;
						break;
					case SDLK_KP_6:
						moveReplayScan = REPLAY_MOVE_RIGHT;
						break;
					case SDLK_KP_2:
						moveReplayScan = REPLAY_MOVE_DOWN;
						break;
					case SDLK_KP_8:
						moveReplayScan = REPLAY_MOVE_UP;
						break;
					case SDLK_KP_5:
						moveReplayScan = REPLAY_MOVE_OUT;
						break;
				}				
				break;
			case SDL_QUIT:
				loopEnd = true;
				break;
			case SDL_WINDOWEVENT: // if( graphicalMode ){... ???
				if( event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED ){
	            			SDL_Log( "Window %d gained focus.", event.window.windowID );
//					thisScreen = screens[event.window.windowID];
					setActiveScreen( event.window.windowID );
				}
				break;
		}
//		if( graphicalMode && event.window.windowID == thisScreen->screenID ){
			thisScreen->inputHandlingBare( event, world, loopEnd );
//		}
	}
	
	// following used in replay mode
	
//	const Uint8* keyboardState = SDL_GetKeyboardState( NULL );

//	if( keyboardState[SDL_SCANCODE_KP_2] ) moveReplayScan = REPLAY_MOVE_DOWN;
//	if( keyboardState[SDL_SCANCODE_KP_4] ) moveReplayScan = REPLAY_MOVE_LEFT;
//	if( keyboardState[SDL_SCANCODE_KP_6] ) moveReplayScan = REPLAY_MOVE_RIGHT;
//	if( keyboardState[SDL_SCANCODE_KP_8] ) moveReplayScan = REPLAY_MOVE_UP;
		
}

// ================= abstractAppMode procedures ================= 

void abstractAppMode::setupGraphics( bool scanningBoxFlag_ ){

	//world = world_;
	scanningBoxFlag = scanningBoxFlag_;

	screens = new Screen*[nScreensMax];
	screensID = new Uint32[nScreensMax];

	int sid;
	SDL_Init( SDL_INIT_VIDEO );
	SDL_GL_SetAttribute( SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1 );

	printf( "\n=== GRAPHICAL MODE SELECTED ===\n" );
	printf( "Number of screens: %i\n", nScreensMax );
	for( int i = 0; i < nScreensMax; i++ ){ // PROBLEM: VSTUP JDE DO SPRAVNEHO OKNA, ALE OVLIVNENA JE JINA OBRAZOVKA!!!
		screens[i] = new Screen( sid, 800, 600 );
		screensID[i] = sid;
//			printf( "sid = %i\n", sid );
	}
//		setActiveScreen( screensID[0] );
	setActiveScreen( sid );

}


abstractAppMode::abstractAppMode( int numOfMoleculeInstances_, flagList *flags ){
	numOfMoleculeInstances = numOfMoleculeInstances_;
	if( flags != NULL ){
		if( flagList->graphicalMode ) setupGraphics( flags->scanningBoxFlag );
		delay = flags->delay;
		suppressOutput = flags->suppressOutput;
	} else {
		delay = false;
		suppressOutput = false;
	}
}



/*
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
*/


abstractAppMode::~abstractAppMode(){
	for( int i = 0; i < nScreensMax; i++ ){
		if( screens[i] != NULL ) delete screens[i];
	}
	thisScreen = NULL;
	SDL_Quit();
}


#endif
