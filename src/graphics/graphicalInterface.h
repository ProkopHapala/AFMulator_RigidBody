
#ifndef _GRAPHICAL_INTERFACE_
#define _GRAPHICAL_INTERFACE_

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_ttf.h>

class InputHandler{
	public:
	virtual void handleInput( const SDL_Event& event ) = 0;
};

class graphInterface : Screen {
	public:
	static const int nScreensMax;
	Screen* thisScreen = NULL;
	Screen** screens = NULL;
	Uint32* screensID = NULL;
	static float viewMolList[4];			// parameters for molecules' look, KDYZTAK DODELAT
	SDL_Event event;
	bool scanningBoxFlag;
	PhysicalSystemEditor* world;
	int delay = 20;
	InputHandler * userInputHandler = NULL;

	// ===== functions
	void drawBox( scanSpecification* scan );
	void setActiveScreen( Uint32 windowID );
	void inputHandling( bool& loopEnd, bool& loopContinue, bool& stopFlag );
	void inputHandling( bool& loopEnd, bool& loopContinue, bool& stopFlag, moveReplayScanMode& moveReplayScan );
	void updateGraphics();

	// ==== construnctor & destructor
	graphInterface( graphInterface* graphics );
	graphInterface( bool scanningBoxFlag = true );
	graphInterface( PhysicalSystemEditor* world_, bool scanningBoxFlag_ = true );
	~graphInterface();
};

const int graphInterface::nScreensMax = 1;
float graphInterface::viewMolList[] = { 5, 4, 1, 0.1 };

void graphInterface::updateGraphics(){
	for( int i = 0; i < nScreensMax; i++ ) {
		if( screens[i] != NULL ) {
			screens[i]->updateScreen( world );
			screens[i]->drawScreen( world );
		}
	}
	//if( delay || stopFlag ){ SDL_Delay( 20 ); }
	SDL_Delay( delay );
}

graphInterface::graphInterface( graphInterface* graphics ){
	//nScreensMax = graphics->nScreensMax;
	scanningBoxFlag = graphics->scanningBoxFlag;
	world     = graphics->world;	// KDYZTAK PREDELAT
	screens   = new Screen*[nScreensMax];
	screensID = new Uint32[nScreensMax];
	int sid;
	SDL_Init( SDL_INIT_VIDEO );
	SDL_GL_SetAttribute( SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1 );
	//printf( "\n=== GRAPHICAL MODE SELECTED ===\n" );
	printf( "Number of screens: %i\n", nScreensMax );
	for( int i = 0; i < nScreensMax; i++ ){ // PROBLEM: VSTUP JDE DO SPRAVNEHO OKNA, ALE OVLIVNENA JE JINA OBRAZOVKA!!!
		screens[i]   = new Screen( sid, 800, 600 );
		screensID[i] = sid;
		//printf( "sid = %i\n", sid );
	}
	//setActiveScreen( screensID[0] );
	setActiveScreen( sid );
}

graphInterface::graphInterface( bool scanningBoxFlag_ ){
	scanningBoxFlag  = scanningBoxFlag_;
	screens          = new Screen*[nScreensMax];
	screensID        = new Uint32[nScreensMax];
	int sid;
	SDL_Init( SDL_INIT_VIDEO );
	SDL_GL_SetAttribute( SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1 );

	//printf( "\n=== GRAPHICAL MODE SELECTED ===\n" );
	printf( "Number of screens: %i\n", nScreensMax );
	for( int i = 0; i < nScreensMax; i++ ){ // PROBLEM: VSTUP JDE DO SPRAVNEHO OKNA, ALE OVLIVNENA JE JINA OBRAZOVKA!!!
		screens[i]   = new Screen( sid, 800, 600 );
		screensID[i] = sid;
		//printf( "sid = %i\n", sid );
	}
	//setActiveScreen( screensID[0] );
	setActiveScreen( sid );

}

graphInterface::graphInterface( PhysicalSystemEditor* world_, bool scanningBoxFlag_ ){
	world           = world_;
	scanningBoxFlag = scanningBoxFlag_;
	screens         = new Screen*[nScreensMax];
	screensID       = new Uint32[nScreensMax];
	int sid;
	SDL_Init( SDL_INIT_VIDEO );
	SDL_GL_SetAttribute( SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1 );
	printf( "\n=== GRAPHICAL MODE SELECTED ===\n" );
	printf( "Number of screens: %i\n", nScreensMax );
	for( int i = 0; i < nScreensMax; i++ ){ // PROBLEM: VSTUP JDE DO SPRAVNEHO OKNA, ALE OVLIVNENA JE JINA OBRAZOVKA!!!
		screens[i] = new Screen( sid, 800, 600 );
		screensID[i] = sid;
		//printf( "sid = %i\n", sid );
	}
	//setActiveScreen( screensID[0] );
	setActiveScreen( sid );

}

void graphInterface::drawBox( scanSpecification* scan ){
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

graphInterface::~graphInterface(){
	for( int i = 0; i < nScreensMax; i++ ){	if( screens[i] != NULL ) delete screens[i];	}
	thisScreen = NULL;
	SDL_Quit();
}

void graphInterface::setActiveScreen( Uint32 windowID ){
	for( int i = 0; i < nScreensMax; i++ ){
		if( screensID[i] == windowID ){
			thisScreen = screens[i];
			//printf( "screensID[i] = %i, setActiveScreen: i = %i\n", screensID[i], i );
			break;
		}
	}
}

void graphInterface::inputHandling( bool& loopEnd, bool& skip, bool& stopFlag ){ // handle the input events
	while( SDL_PollEvent( &event ) ){
		//thisScreen->inputHandlingBare( event, world, loopEnd );
		switch( event.type ){
			case SDL_KEYDOWN:
				switch( event.key.keysym.sym ){
					case SDLK_ESCAPE:		loopEnd = true;			break;
					case SDLK_F1:			showHelp();				break;
					case SDLK_KP_ENTER:		skip = true;			break;
					case SDLK_e:			world->exportData();	break;
					case SDLK_SPACE:
						//world->setSysEvol( stopFlag );
						world->sysEvol = stopFlag ;
						stopFlag = !stopFlag;
						printf( stopFlag ? "inputHandling: Relaxation paused.\n" : "inputHandling: Relaxation unpaused.\n" );
						break;
				}
				break;
			case SDL_QUIT:	loopEnd = true;	break;
			case SDL_WINDOWEVENT: // if( graphicalMode ){... ???
				if( event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED ){
					//SDL_Log( "Window %d gained focus.", event.window.windowID );
					//thisScreen = screens[event.window.windowID];
					setActiveScreen( event.window.windowID );
				}
				break;
		}
		if( userInputHandler != NULL ){ userInputHandler->handleInput( event );	}
		//if( graphicalMode && event.window.windowID == thisScreen->screenID ){
			thisScreen->inputHandlingBare( event, world, loopEnd );
		//}
	}
}

#endif
