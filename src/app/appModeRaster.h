
class appModeRaster : public appModeTipMol {
// 2D scanning

	public:

	scanSpecification* scan  = NULL;
	fileManager* outputFiles = NULL;
	fileManager* outputPositMolFiles = NULL;
	fileManager* outputRotatMolFiles = NULL;
	
	void resetup( void );
	
	int convergStepLimit = 0;
	int* molOfInterest   = NULL;
	int numMolOfInterest = 0;

	appModeRaster(){};
	appModeRaster( int numOfMoleculeInstances, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics, abstractTip* tip, flagList *flags, scanSpecification* scan, fileManager* outputFiles, relaxParams* relaxParameters, int* molOfInterest_, int numMolOfInterest_, fileManager* outputPositMolFiles_ = NULL, fileManager* outputRotatMolFiles_ = NULL );
	
	void loop( int n );

};


// ================= appModeRaster procedures ================= 

void appModeRaster::resetup( void ){
// resetup the world geometry

	abstractAppMode::world->resetGeometry( appModeTipMol::geometryFile, appModeTipMol::posProbe, appModeTipMol::rotProbe );
	
}

appModeRaster::appModeRaster( int numOfMoleculeInstances, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics, abstractTip* tip, flagList *flags, scanSpecification* scan, fileManager* outputFiles_, relaxParams* relaxParameters, int* molOfInterest_, int numMolOfInterest_, fileManager* outputPositMolFiles_, fileManager* outputRotatMolFiles_ )
// constructor
: scan( scan ), outputFiles( outputFiles_ ), numMolOfInterest( numMolOfInterest_ ), outputPositMolFiles( outputPositMolFiles_ ), outputRotatMolFiles( outputRotatMolFiles_ ), appModeTipMol( numOfMoleculeInstances, moleculeFiles, typeList, geometryFile, surf, graphics, tip, flags )
{
	abstractAppMode::world->optimizer->dt       = relaxParameters->glob_dtmax;
	abstractAppMode::world->optimizer->damping  = relaxParameters->glob_damping;
	convergStepLimit = relaxParameters->glob_maxIters;

	molOfInterest = new int[numMolOfInterest];
	for( int i = 0; i < numMolOfInterest; i++ ) molOfInterest[i] = molOfInterest_[i];
	
	if( abstractAppMode::graphics != NULL ) abstractAppMode::graphics->drawBox( scan );

}

void appModeRaster::loop( int n ){
// 2D scanning loop
			
	if( abstractAppMode::world == NULL    ){ printf( "appModeRaster::loop: No world exists. Halt.\n" );	return;	}
	if( abstractAppMode::world->nmols < 2 ){ printf( "appModeRaster::loop: Trivial world. Halt.\n" );	return;	}
		
	bool	saveMolOfInterestMovement = false;
	if( outputPositMolFiles != NULL && outputRotatMolFiles != NULL ){
		saveMolOfInterestMovement = true;	
		for( int i = 0; i < numMolOfInterest; i++ ){
			if( molOfInterest[i] > abstractAppMode::world->nmols - 1 ){
				printf( "appModeRaster::loop: molOfInterest[%i] too big. Set the highest valid value instead.\n", i );
				molOfInterest[i] = abstractAppMode::world->nmols - 2;
				// NEOSETRENO, KDYZ TO ORIZNE VIC NEZ JEDNU MOLEKULU TAKHLE, PAK SE UKLADA DO VICE SOUBORU TOTEZ
			}
		}
	}
	
	// allocation of data lists
	int	    total      = (scan->xdim)*(scan->ydim)*(scan->zdim);
	int metadataList[] = { scan->xdim, scan->ydim, scan->zdim, total };
	
	scalarDataWrapper* pixelDataList = new scalarDataWrapper( total, metadataList, "pixels" );
	vectorDataWrapper* positDataList = new vectorDataWrapper( total, metadataList, "trajectories" );
	quaterDataWrapper* rotatDataList = new quaterDataWrapper( total, metadataList, "rotations" );
	
	vectorDataManager* positMolDataList = NULL;
	quaterDataManager* rotatMolDataList = NULL;
	if( saveMolOfInterestMovement ){
		positMolDataList = new vectorDataManager( numMolOfInterest, total, metadataList, "mol. trajectories" ); 
		rotatMolDataList = new quaterDataManager( numMolOfInterest, total, metadataList, "mol. orientations" );
	}
	
	// generation of z-step sequence
	scalarDataWrapper* zSteps = new scalarDataWrapper( scan->zdim, metadataList, "z-steps" );
	scan->createZSamplingSequenceLoc( zSteps );
			
	// scanning loop

	int  ind                 = 0;
	int	 numOfNoncovergCases = 0;
	bool loopEnd             = false;
	printf( "\n=== CALCULATION LOOP INITIATED ===\n" );
	for( int yind = scan->ydim - 1; yind >= 0; yind-- ){
	//for( int yind = 0; yind < scan->ydim ; yind++ ){
	
		double ypos = yind*scan->ystep + scan->yoffset;
		for( int xind = 0; xind < scan->xdim; xind++ ){

			double xpos = xind*scan->xstep + scan->xoffset;

			double zpos = scan->zoffset;
			abstractAppMode::world->tip->setPosition( xpos, ypos, zpos );
			abstractAppMode::world->adjustMolToTip();

			if( !abstractAppMode::suppressOutput ){	printf( "relaxing z-line (ix,iy) %5i %5i \n", xind, yind ); }

			for( int zind = 0; zind < scan->zdim ; zind++ ){
				

				
				int convergStep = 0;
				ind = (scan->ydim - 1 - yind)*scan->xdim*scan->zdim + xind*scan->zdim + zind;

/*						
				if( !abstractAppMode::suppressOutput ){		
					if( abstractAppMode::graphics != NULL ){
						if( !(ind % 200) ) printf( "index %7i out of %i reached\n", ind, total );
					} else {
						if( !(ind % 1000) ) printf( "index %7i out of %i reached\n", ind, total );
					}
				}
*/


				double pixelDataListItem;
				bool optimizingFlag = true;
				bool loopContinue   = false;
				bool stopFlag       = false;

				for( int iframe = 0; iframe < n; iframe++ ){
	
					// handle input events
					if( abstractAppMode::graphics != NULL )
						abstractAppMode::graphics->inputHandling( loopEnd, loopContinue, stopFlag );
					if( loopEnd || loopContinue ) break;
		
					// update world			
					if( abstractAppMode::world->getSysEvol() ){
						if( abstractAppMode::graphics != NULL && abstractAppMode::world->tip != NULL ){
							abstractAppMode::graphics->thisScreen->mouseSetAuxPoint( abstractAppMode::world );
						}				
						abstractAppMode::world->update( optimizingFlag, pixelDataListItem );				
						convergStep++;
					}
				
					// update screens
					if( abstractAppMode::graphics != NULL )
						abstractAppMode::graphics->updateGraphics();

					// others
					if( !optimizingFlag ){
//						printf( "convergStep = %i\n", convergStep );	
						break;
					}
					if( convergStep > convergStepLimit ){
						if( !abstractAppMode::suppressOutput ){
//							printf( "appModeRaster::loop: Relaxation in step %5i does not converge. Skipped.\n", ind );
							printf( "appModeRaster::loop: step (ix,iy,iz) %5i %5i %5i Relaxation not converge in %5i. Skipped.\n", xind, yind, zind, convergStepLimit );
						}
//						printf( "xind, yind, zind = %d %d %d\n", xind, yind, zind );
						numOfNoncovergCases++;
						break;
					}
					
					if( abstractAppMode::delay || stopFlag ){ SDL_Delay( 20 ); }

				} // iframe

				if( loopEnd ) break;
						
				// saving data to lists			
				pixelDataList->setValue( ind, pixelDataListItem );
				positDataList->setValue( ind, abstractAppMode::world->pos[appModeTipMol::tipMol] );
				rotatDataList->setValue( ind, abstractAppMode::world->rot[appModeTipMol::tipMol] );
				
				if( saveMolOfInterestMovement ){
					for( int i = 0; i < numMolOfInterest; i++ ){
						positMolDataList->setValue( i, ind, abstractAppMode::world->pos[molOfInterest[i]] );
						rotatMolDataList->setValue( i, ind, abstractAppMode::world->rot[molOfInterest[i]] );
					}
				}
	
				// readjust the tip
				zpos -= (*zSteps)[zind]; // movement downwards!!!
				abstractAppMode::world->tip->setPosition( xpos, ypos, zpos );
				
			} // zind
			if( loopEnd ) break;
			resetup();
		} // xind
		if( loopEnd ) break;
	} // yind

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
	delete zSteps;
	delete rotatDataList;

	if( saveMolOfInterestMovement ){
		delete positMolDataList;
		delete rotatMolDataList;
	}
	
}
