

// only a single z-direction scanning
class appModeTipMolOne : public appModeTipMol, public InputHandler {
	public:

	appModeTipMolOne(){};
	appModeTipMolOne( int numOfMoleculeInstances, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics, abstractTip* tip, flagList *flags );

	bool restart = true;
	double xpos = 0, ypos = 0, zpos = 20;

	void loop( int n );
	virtual void handleInput( const SDL_Event& event ); // overides InputHandler::handleInput


};


// ================= appModeTipMolOne procedures =================

appModeTipMolOne::appModeTipMolOne( int numOfMoleculeInstances, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics, abstractTip* tip, flagList *flags )
// constructor
: appModeTipMol( numOfMoleculeInstances, moleculeFiles, typeList, geometryFile, surf, graphics, tip, flags )
{
	if( graphics != NULL){
		graphics->userInputHandler = this;
	}
}

void appModeTipMolOne::handleInput( const SDL_Event& event ){
	//printf( " inside appModeReplay::handleInput \n " );
	double dstep = 0.1;
	switch( event.type ){
		case SDL_KEYDOWN:
			switch( event.key.keysym.sym ){
				case SDLK_KP_4: xpos-=dstep; restart=true; break;
				case SDLK_KP_6: xpos+=dstep; restart=true; break;
				case SDLK_KP_2: ypos-=dstep; restart=true; break;
				case SDLK_KP_8: ypos+=dstep; restart=true; break;
				case SDLK_KP_9: zpos+=dstep; restart=true; break;
				case SDLK_KP_3: zpos-=dstep; restart=true; break;
			};
			if( restart ) printf( " tip(x,y,z) %3.3f %3.3f %3.3f \n ", xpos, ypos, zpos );
			break;
	};
};

void appModeTipMolOne::loop( int n ){
// one z-axis scanning loop

	bool   optimizingFlag  = true;
	bool   loopEnd         = false, loopContinue, stopFlag = false;
	int    convergStep      = 0;
	int    convergStepLimit = n;
	double pixelDataListItem;

	if( world == NULL ){
		printf( "appModeTipMolOne::loop: No world exists. Halt.\n" );
		return;
	}

	world->tip->setPosition( xpos, ypos, zpos );
	world->adjustMolToTip();

	for( int iframe = 0; iframe < n; iframe++ ){

		if( graphics != NULL ){
			graphics->updateGraphics();
			graphics->inputHandling( loopEnd, loopContinue, stopFlag );
			if( loopEnd ) break;
		}

		if( restart ){
			world->tip->setPosition( xpos, ypos, zpos );
			//world->adjustMolToTip();
			optimizingFlag = true;
			restart        = false;
		};

		// update world
		if( world->getSysEvol() ){
			if( graphics != NULL && world->tip != NULL ){
				graphics->thisScreen->mouseSetAuxPoint( world );
			}
			world->update( optimizingFlag, pixelDataListItem );
//			printf( "loopSingleRelaxationPerm: step %i\n", convergStep );
			convergStep++;
		}

		if( convergStep > convergStepLimit ){
			if( !suppressOutput )
				printf( "appModeTipMolOne::loop: Relaxation does not converge. Skipped.\n" );
			break;
		}

		if( graphics != NULL && (delay || stopFlag) ) SDL_Delay( 10 );

	}
}
