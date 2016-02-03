

// only a single z-direction scanning
class appModeTipMolOne : public appModeTipMol, public InputHandler {
	public:
	bool   tipMoved        = false;
	bool   optimizingFlag  = true;
	double xpos = 0, ypos = 0, zpos = 20;
	int    convergStep      = 0;

	appModeTipMolOne(){};
	appModeTipMolOne( int numOfMoleculeInstances, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics, abstractTip* tip, flagList *flags );

	void loop( int n );
	virtual void handleInput( const SDL_Event& event ); // overides InputHandler::handleInput
};


// ================= appModeTipMolOne procedures =================

appModeTipMolOne::appModeTipMolOne( int numOfMoleculeInstances, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics, abstractTip* tip, flagList *flags )
// constructor
: appModeTipMol( numOfMoleculeInstances, moleculeFiles, typeList, geometryFile, surf, graphics, tip, flags ){
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
				case SDLK_KP_4: xpos-=dstep; tipMoved=true; break;
				case SDLK_KP_6: xpos+=dstep; tipMoved=true; break;
				case SDLK_KP_2: ypos-=dstep; tipMoved=true; break;
				case SDLK_KP_8: ypos+=dstep; tipMoved=true; break;
				case SDLK_KP_9: zpos+=dstep; tipMoved=true; break;
				case SDLK_KP_3: zpos-=dstep; tipMoved=true; break;
			};
			if( tipMoved ) printf( " tip(x,y,z) %3.3f %3.3f %3.3f \n ", xpos, ypos, zpos );
			break;
	};
};

void appModeTipMolOne::loop( int n ){  // one z-axis scanning loop
	bool   loopEnd         = false, loopContinue, stopFlag = false;
	int    convergStepLimit = n;
	double pixelDataListItem;
	if( world == NULL ){
		printf( "appModeTipMolOne::loop: No world exists. Halt.\n" );
		return;
	}
	world->perFrame = 100;
	world->tip->setPosition( xpos, ypos, zpos );
	world->adjustMolToTip();
	world->optSteps = 0;
	for( int iframe = 0; iframe < n; iframe++ ){
		if( graphics != NULL ){
			graphics->updateGraphics();
			graphics->inputHandling( loopEnd, loopContinue, stopFlag );
			if( loopEnd ) break;
		}
		if( tipMoved ){
			world->tip->setPosition( xpos, ypos, zpos );
			//world->adjustMolToTip();
			if(  optimizingFlag ){
				printf( " user-tip-move interupted relaxation at step %i  ( dt %e damping %e ) \n", world->optSteps, ((OptimizerFIRE*)world->optimizer)->dt_var, ((OptimizerFIRE*)world->optimizer)->damp_var  );
			}else{
				optimizingFlag = true;
			}
			world->optSteps = 0;
			tipMoved    = false;
		}
		// update world
		//if( world->getSysEvol() ){
		if( world->sysEvol ){
			if( ( graphics != NULL ) && ( world->tip != NULL ) ){   graphics->thisScreen->mouseSetAuxPoint( world );  }
			if( optimizingFlag ){
				world->update( optimizingFlag );
				pixelDataListItem = world->fTip.z;
				if( !optimizingFlag ){
					printf( " converged in %i iterations ( dt %e damping %e ) \n", world->optSteps, ((OptimizerFIRE*)world->optimizer)->dt_var, ((OptimizerFIRE*)world->optimizer)->damp_var  );
				}
			}
//			printf( "loopSingleRelaxationPerm: step %i\n", convergStep );
			convergStep++;
		}
		if( convergStep > convergStepLimit ){
			if( !suppressOutput ){ printf( "appModeTipMolOne::loop: Relaxation does not converge. Skipped.\n" ); }
			break;
		}
		//if( graphics != NULL && (delay || stopFlag) ) SDL_Delay( 10 );
	}
}
