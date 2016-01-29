

void DEBUG_DELAY( int n, char* s ){
	SDL_Delay( 100 );
	printf( "DEBUG: %s \n", s );
}



class appModeRaster : public appModeTipMol {
	public:

	// constants
	const int maxFramesPerPoint = 10;

	// settings
	int  convergStepLimit = 0;
	int* molOfInterest    = NULL;
	int  numMolOfInterest = 0;
	scanSpecification* scan  = NULL;
	bool	saveMolOfInterestMovement = false;

	// output files
	fileManager* outputFiles = NULL;
	fileManager* outputPositMolFiles = NULL;
	fileManager* outputRotatMolFiles = NULL;
	scalarDataWrapper* pixelDataList;
	vectorDataWrapper* positDataList;
	quaterDataWrapper* rotatDataList;
	vectorDataManager* positMolDataList = NULL;
	quaterDataManager* rotatMolDataList = NULL;

	// loop variables
	int  ind,numOfNoncovergCases;
	bool loopEnd;

	// ======== Functions

	void resetup( void );
	void loop( int n );

	appModeRaster(){};
	appModeRaster(
		int                numOfMoleculeInstances,
		fileManager*       moleculeFiles,
		AtomTypes*         typeList,
		fileWrapper*       geometryFile,
		abstractSurf*      surf,
		graphInterface*    graphics,
		abstractTip*       tip,
		flagList *         flags,
		scanSpecification* scan,
		fileManager*       outputFiles,
		relaxParams*       relaxParameters,
		int*               molOfInterest_,
		int                numMolOfInterest_,
		fileManager*       outputPositMolFiles_ = NULL,
		fileManager*       outputRotatMolFiles_ = NULL
	);

	bool convergeTipPos(  double xpos, double ypos, double zpos ){
		double pixelDataListItem;
		bool optimizingFlag = true;
		bool skip           = false;
		bool stopFlag       = false;
		//DEBUG_DELAY( 100, " 1-1-1 " );
		if( graphics != NULL ) printf( " starting relaxation at point: (%3.3f,%3.3f,%3.3f)\n", xpos, ypos, zpos );
		world->tip->setPosition( xpos, ypos, zpos );
		//DEBUG_DELAY( 100, " 1-1-2 " );
		//for( int iframe = 0; iframe < maxFramesPerPoint; iframe++ ){
		while( true ){
			// handle input events
			//DEBUG_DELAY( 100, " 1-1-3 " );
			//printf( " %i %i %i \n", loopEnd, skip, stopFlag );
			//printf( " s \n"  );
			//SDL_Event event; SDL_PollEvent( &event ); // WTF? HOW THIS MAY CAUSE ERROR ?   with scanning steps ( 10 6 60 )
			if( graphics != NULL ){ graphics->inputHandling( loopEnd, skip, stopFlag ); }
			if( loopEnd || skip  ) return false;

			//DEBUG_DELAY( 100, " 1-1-4" );
			//if( graphics != NULL && world->tip != NULL ){	graphics->thisScreen->mouseSetAuxPoint( world ); }   // WTF IS THIS ?
			if( graphics != NULL ){ graphics->updateGraphics(); }
			//DEBUG_DELAY( 100, " 1-1-5 " );
			if( !stopFlag ){
				if( world->sysEvol ){
					world->update( optimizingFlag );
				}
				//DEBUG_DELAY( 100, " 1-1-6 " );
				if( !optimizingFlag ){
		//			printf( "convergStep = %i\n", convergStep );
					return true;
				}
				//DEBUG_DELAY( 100, " 1-1-7 " );
				if( world->optSteps > convergStepLimit ){
					if( !suppressOutput ){
		//				printf( "appModeRaster::loop: Relaxation in step %5i does not converge. Skipped.\n", ind );
						//printf( "appModeRaster::loop: step (ix,iy,iz) %5i %5i %5i Relaxation not converge in %5i. Skipped.\n", xind, yind, zind, convergStepLimit );
						printf( "Relaxation not converge in %i iterations, fmax = %f ( f2conv = %f )\n", convergStepLimit, world->fmax, world->fmaxConv );
					}
					//DEBUG_DELAY( 100, " 1-1-8 " );
		//			printf( "xind, yind, zind = %d %d %d\n", xind, yind, zind );
					numOfNoncovergCases++;
					return false;
				}
			//DEBUG_DELAY( 100, " 1-1-9 " );
			}
		} // iframe
		return false;
		// saving data to lists
		//DEBUG_DELAY( 100, " 1-1-10 " );
	}

};


// ================= appModeRaster procedures =================

void appModeRaster::resetup( void ){
// resetup the world geometry
	world->resetGeometry( geometryFile, posProbe, rotProbe );

}

appModeRaster::appModeRaster(
	int numOfMoleculeInstances,
	fileManager* moleculeFiles,
	AtomTypes* typeList,
	fileWrapper* geometryFile,
	abstractSurf* surf,
	graphInterface* graphics,
	abstractTip* tip,
	flagList *flags,
	scanSpecification* scan,
	fileManager* outputFiles_,
	relaxParams* relaxParameters,
	int* molOfInterest_,
	int numMolOfInterest_,
	fileManager* outputPositMolFiles_,
	fileManager* outputRotatMolFiles_
):
	scan               ( scan ),
	outputFiles        ( outputFiles_ ),
	numMolOfInterest   ( numMolOfInterest_ ),
	outputPositMolFiles( outputPositMolFiles_ ),
	outputRotatMolFiles( outputRotatMolFiles_ ),
	appModeTipMol      ( numOfMoleculeInstances, moleculeFiles, typeList, geometryFile, surf, graphics, tip, flags )
{
	world->optimizer->dt       = relaxParameters->glob_dtmax;
	world->optimizer->damping  = relaxParameters->glob_damping;
	convergStepLimit = relaxParameters->glob_maxIters;
	molOfInterest = new int[numMolOfInterest];
	for( int i = 0; i < numMolOfInterest; i++ ) molOfInterest[i] = molOfInterest_[i];
	if( graphics != NULL ) graphics->drawBox( scan );
}


void appModeRaster::loop( int n ){
	if( world == NULL    ){ printf( "appModeRaster::loop: No world exists. Halt.\n" );	return;	}
	if( world->nmols < 2 ){ printf( "appModeRaster::loop: Trivial world. Halt.\n" );	return;	}
	if( outputPositMolFiles != NULL && outputRotatMolFiles != NULL ){
		saveMolOfInterestMovement = true;
		for( int i = 0; i < numMolOfInterest; i++ ){
			if( molOfInterest[i] > world->nmols - 1 ){
				printf( "appModeRaster::loop: molOfInterest[%i] too big. Set the highest valid value instead.\n", i );
				molOfInterest[i] = world->nmols - 2;
				// NEOSETRENO, KDYZ TO ORIZNE VIC NEZ JEDNU MOLEKULU TAKHLE, PAK SE UKLADA DO VICE SOUBORU TOTEZ
			}
		}
	}
	// allocation of data lists
	int    total        = (scan->xdim)*(scan->ydim)*(scan->zdim);
	int metadataList[]  = { scan->xdim, scan->ydim, scan->zdim, total };

	pixelDataList = new scalarDataWrapper( total, metadataList, "pixels" );
	positDataList = new vectorDataWrapper( total, metadataList, "trajectories" );
	rotatDataList = new quaterDataWrapper( total, metadataList, "rotations" );
	vectorDataManager* positMolDataList = NULL;
	quaterDataManager* rotatMolDataList = NULL;
	if( saveMolOfInterestMovement ){
		positMolDataList = new vectorDataManager( numMolOfInterest, total, metadataList, "mol. trajectories" );
		rotatMolDataList = new quaterDataManager( numMolOfInterest, total, metadataList, "mol. orientations" );
	}
	// generation of z-step sequence
	scalarDataWrapper* zSteps = new scalarDataWrapper( scan->zdim, metadataList, "z-steps" );

	//scan->createZSamplingSequenceLoc( zSteps );
	scan->zstep *= -1;
	scan->initLinear();
	//printf("scan->xs = "); printArray( scan->xdim, scan->xs ); printf("\n");
	//printf("scan->ys = "); printArray( scan->ydim, scan->ys ); printf("\n");
	//printf("scan->zs = "); printArray( scan->zdim, scan->zs ); printf("\n");

	// scanning loop

	world->perFrame = 1000;

	ind                 = 0;
	numOfNoncovergCases = 0;
	loopEnd             = false;
	printf( "\n=== CALCULATION LOOP INITIATED ===\n" );

	DEBUG_DELAY( 1000, " 1 " );

	for( int yind = scan->ydim - 1; yind >= 0; yind-- ){
	//for( int yind = 0; yind < scan->ydim ; yind++ ){
		//double ypos = yind*scan->ystep + scan->yoffset;
		double ypos = scan->ys[yind];
		for( int xind = 0; xind < scan->xdim; xind++ ){
			//double xpos = xind*scan->xstep + scan->xoffset;
			double xpos = scan->xs[xind];
			world->tip->setPosition( xpos, ypos, scan->zs[0] );
			world->adjustMolToTip();
			if( !suppressOutput ){	printf( "relaxing z-line (ix,iy) %5i %5i \n", xind, yind ); }
			for( int zind = 0; zind < scan->zdim ; zind++ ){
				double zpos = scan->zs[zind];
				//DEBUG_DELAY( 100, " 1-1 " );
				bool converged = convergeTipPos( xpos, ypos, zpos ); // most important part
				//CHECK( 2 );

				if( converged ){
					ind = (scan->ydim - 1 - yind)*scan->xdim*scan->zdim + xind*scan->zdim + zind;
					if( loopEnd ) break;
					pixelDataList->setValue( ind, world->fTip.z );
					positDataList->setValue( ind, world->pos[appModeTipMol::tipMol] );
					rotatDataList->setValue( ind, world->rot[appModeTipMol::tipMol] );
					if( saveMolOfInterestMovement ){
					for( int i = 0; i < numMolOfInterest; i++ ){
						positMolDataList->setValue( i, ind, world->pos[molOfInterest[i]] );
						rotatMolDataList->setValue( i, ind, world->rot[molOfInterest[i]] );
						}
					}
				}

				//CHECK( 3 );
			} // zind
			if( loopEnd ) break;
			//resetup();
		} // xind
		if( loopEnd ) break;
	} // yind

	DEBUG_DELAY( 1000, " 2 " );

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
	//delete zSteps;
	delete rotatDataList;

	if( saveMolOfInterestMovement ){
		delete positMolDataList;
		delete rotatMolDataList;
	}

}
