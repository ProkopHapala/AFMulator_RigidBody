
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
