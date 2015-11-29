
#ifndef _SETTING_PROCEDURES_
#define _SETTING_PROCEDURES_

void setScanningParams( const char* filename, zSamplingMode& zSampling,
	scanningType& scanType, double& minStep, double& xoffset, double& yoffset, double& zoffset,
	double& xstep, double& ystep, double& zstep, int& xdim, int& ydim, int& zdim,
	int& glob_maxIters, double& glob_dtmax, double& glob_damping, int*& molOfInterest, int& numMolOfInterest
	){
// set parameters pertaining to scanning
	
	char auxStr[400], str[400];
	int count;
	double xdimAux, ydimAux, zdimAux;
	double xstepAux, ystepAux, zstepAux;		
	zSampling = Z_SAMPLE_EQUIDIST;
	
	enum { SCAN_STEP_NUM, SCAN_STEP_DIM } inputType;
	
	FILE* scanFile = fopen( filename, "r" );
	if( scanFile ){
		printf( "setScanningParams:\tValues loaded from\t\t%s.\n", filename );

		readLineComment( scanFile, str );
		count = sscanf( str, " %399s", &auxStr );		
		if(	   !strcmp( auxStr, "SCANNING_NONE" )	   ){ scanType = SCANNING_NONE;
		} else if( !strcmp( auxStr, "SCANNING_NONE_VIEW" ) ){ scanType = SCANNING_NONE_VIEW;
		} else if( !strcmp( auxStr, "SCANNING_RASTER" )    ){ scanType = SCANNING_RASTER;
		} else if( !strcmp( auxStr, "SCANNING_REPLAY" )    ){ scanType = SCANNING_REPLAY;
		} else if( !strcmp( auxStr, "SCANNING_ONE" ) 	   ){ scanType = SCANNING_ONE;
		} else {
			scanType = SCANNING_NONE;
		}
		printf( "setScanningParams:\tscanType = %i.\n", scanType );
	
		if( scanType == SCANNING_RASTER || scanType == SCANNING_REPLAY ){

			readLineComment( scanFile, str );
			count = sscanf( str, "%lf %lf %lf", &xdimAux, &ydimAux, &zdimAux );
			
			readLineComment( scanFile, str );
			count = sscanf( str, "%lf %lf %lf", &xstepAux, &ystepAux, &zstepAux );

			readLineComment( scanFile, str );
			count = sscanf( str, " %399s", &auxStr );
			if(	   !strcmp( auxStr, "number" ) ){ inputType = SCAN_STEP_NUM;	// number of steps
			} else if( !strcmp( auxStr, "length" ) ){ inputType = SCAN_STEP_DIM;	// lengths of each step
			} else {
				inputType = SCAN_STEP_NUM;
				printf( "setScanningParams: Default step format used.\n" );
			}
	
			readLineComment( scanFile, str );
			count = sscanf( str, " %399s", &auxStr );
//			printf( "%s\n", str ); // !!!
			if(	   !strcmp( auxStr, "Z_SAMPLE_EQUIDIST" )	){ zSampling = Z_SAMPLE_EQUIDIST;
			} else if( !strcmp( auxStr, "Z_SAMPLE_LINEAR" )		){ zSampling = Z_SAMPLE_LINEAR;
			} else if( !strcmp( auxStr, "Z_SAMPLE_QUAD" )		){ zSampling = Z_SAMPLE_QUAD;
			} else {
				zSampling = Z_SAMPLE_EQUIDIST;
			}
			readLineComment( scanFile, str );
			count = sscanf( str, " %lf", &minStep );
			
			if( zSampling != Z_SAMPLE_EQUIDIST ) printf( "setScanningParams:\tNon-equidistant z-spacing used.\n" );
			printf( "setScanningParams:\tzSampling = %i.\n", zSampling );
						
			readLineComment( scanFile, str );
			count = sscanf( str, "%lf %lf %lf", &xoffset, &yoffset, &zoffset );
				
			switch( inputType ){
				case SCAN_STEP_DIM:
					printf( "setScanningParams:\tInput data specified by definite dimensions of scanning steps.\n" );
		
					xdim = (int) floor( xdimAux / ( double ) xstepAux );
					ydim = (int) floor( ydimAux / ( double ) ystepAux );
					zdim = (int) floor( zdimAux / ( double ) zstepAux );	
					break;
				case SCAN_STEP_NUM:
					printf( "setScanningParams:\tInput data specified by numbers of scanning samples in each dimension.\n" );
										
					xdim = (int) xstepAux;
					ydim = (int) ystepAux;
					zdim = (int) zstepAux;
					break;
				default:
					printf( "setScanningParams:\tInvalid input data.\n" );
					break;
			}
				
			xstep = xdimAux / (double) xdim;
			ystep = ydimAux / (double) ydim;
			zstep = zdimAux / (double) zdim;
					
			printf( "setScanningParams:\tNumber of samples in each dimension: xdim, ydim, zdim = %i %i %i\n", xdim, ydim, zdim );
			printf( "setScanningParams:\tLengths of steps in each dimension: xstep, ystep, zstep = %.3lf %.3lf %.3lf\n", xstep, ystep, zstep );
			printf( "setScanningParams:\tMinimum length of z-step: minStep = %.3lf\n", minStep );
			
			readLineComment( scanFile, str );
			count = sscanf( str, "%lf", &glob_dtmax );

			readLineComment( scanFile, str );
			count = sscanf( str, "%lf", &glob_damping );

			readLineComment( scanFile, str );
			count = sscanf( str, "%i", &glob_maxIters );

			readLineComment( scanFile, str );
			count = sscanf( str, "%i", &numMolOfInterest );

			molOfInterest = new int[numMolOfInterest];
			
			for( int i = 0; i < numMolOfInterest; i++ ){
				readLineComment( scanFile, str );
				count = sscanf( str, "%i", &molOfInterest[i] );
			}
		
		}
	
		fclose( scanFile );

	} else {
		printf( "setScanningParams:\tFile %s cannot be opened. Default values used instead.\n", filename );
		
		scanType = SCANNING_NONE;
		
		xdim = 1; ydim = 1; zdim = 1;
		xstep = 0.2; ystep = 0.2; zstep = 0.2;
		xoffset = -12; yoffset = -13; zoffset = 13;

	}

}

void setScanningParams( const char* filename, scanSpecification*& scan,
	relaxParams* relaxParameters, int*& molOfInterest, int& numMolOfInterest
	){

	setScanningParams( filename, scan->zSampling,
	scan->scanType, scan->minStep, scan->xoffset, scan->yoffset, scan->zoffset,
	scan->xstep, scan->ystep, scan->zstep, scan->xdim, scan->ydim, scan->zdim,
	relaxParameters->glob_maxIters, relaxParameters->glob_dtmax, relaxParameters->glob_damping, molOfInterest, numMolOfInterest
	);

}

void setMoleculeParams( const char* filename, int*& listOfMoleculeInstances, std::string*& listOfFileNames,
	int& numOfMoleculeInstances, int& numOfMoleculeTypes, char* world_data, bool& raw_data
	){
// set parameters pertaining to molecule geometry
	
	char raw_data_str[40], fileNameLoc[400], str[400];
	int count, molInst;
	
	FILE* molFile = fopen( filename, "r" );
	if( molFile ){
		printf( "setMoleculeParams:\tValues loaded from\t\t%s.\n", filename );
		
		readLineComment( molFile, str );
		count = sscanf( str, "%i", &numOfMoleculeTypes );
		if( numOfMoleculeTypes > 1 ) delete [] listOfFileNames; // ???
		listOfFileNames = new std::string[numOfMoleculeTypes];
		for( int i = 0; i < numOfMoleculeTypes; i++ ){
			readLineComment( molFile, str );
			count = sscanf( str, "%399s", &fileNameLoc );			
			listOfFileNames[i].assign( fileNameLoc );
//			printf( "listOfFileNames[%i] = %s\n", i, listOfFileNames[i].c_str() );
		}
		
		readLineComment( molFile, str );
		count = sscanf( str, "%i", &numOfMoleculeInstances );
//		printf( "numOfMoleculeInstances = %d\n", numOfMoleculeInstances );
		if( numOfMoleculeInstances > 1 ) delete [] listOfMoleculeInstances; // ???
		listOfMoleculeInstances = new int[numOfMoleculeInstances];
		for( int i = 0; i < numOfMoleculeInstances; i++ ){
			readLineComment( molFile, str );
			count = sscanf( str, "%i", &molInst );
			if( molInst > numOfMoleculeTypes ){
				printf( "setMoleculeParams: Out of bounds!!!\n" );
			}
			listOfMoleculeInstances[i] = molInst;
//			printf( "listOfMoleculeInstances[%i] = %i\n", i, listOfMoleculeInstances[i] );
		}

		readLineComment( molFile, str );
		count = sscanf( str, "%399s", world_data );
		
		readLineComment( molFile, str );
		count = sscanf( str, "%39s", &raw_data_str );
		raw_data = false;
		if( count > 0 && !strcmp( raw_data_str, "raw" ) ){
			raw_data = true;
		}

		fclose( molFile );
	
	} else {
		printf( "setMoleculeParams:\tFile %s cannot be opened. Default values used instead.\n", filename );
		
		numOfMoleculeTypes = 1;
		numOfMoleculeInstances = 5;
		
		if( numOfMoleculeTypes > 1 ) delete [] listOfFileNames; // ???
		if( numOfMoleculeInstances > 1 ) delete [] listOfMoleculeInstances; // ???
		
		listOfFileNames = new std::string[numOfMoleculeTypes];
		listOfFileNames[0].assign( "InputData/PTCDA.bas" );
		
		listOfMoleculeInstances = new int[numOfMoleculeInstances];
		for( int i = 0; i < numOfMoleculeInstances; i++ ){
			listOfMoleculeInstances[i] = 0;
		}
		
	}

}

void setConfFiles( const char* confFilesCommander, char* atomTypesFile, char* scanningParams, char* moleculeParams, char* tipParams ){
// set names to various configuration files used by the application

	char str[400];
	int count;
	
	FILE* confFilesCommanderFile = fopen( confFilesCommander, "r" );
	if( confFilesCommanderFile == NULL ){
		printf( "setConfFiles: File %s cannot be opened. Default setting used instead.\n", confFilesCommander );
		strcpy( atomTypesFile, "InputData/atomTypes.dat" );
		strcpy( scanningParams, "InputData/scanningParams" );
		strcpy( moleculeParams, "InputData/moleculeParams" );
		strcpy( tipParams, "InputData/tipParams" );

	} else {
		readLineComment( confFilesCommanderFile, str );
		count = sscanf( str, "%399s", atomTypesFile );

		readLineComment( confFilesCommanderFile, str );
		count = sscanf( str, "%399s", scanningParams );

		readLineComment( confFilesCommanderFile, str );
		count = sscanf( str, "%399s", moleculeParams );

		readLineComment( confFilesCommanderFile, str );
		count = sscanf( str, "%399s", tipParams );

		fclose( confFilesCommanderFile );
	}
		
//	printf( "setConfFiles: Value atomTypesFile set to\t%s.\n", atomTypesFile );
//	printf( "setConfFiles: Value scanningParams set to\t%s.\n", scanningParams );
//	printf( "setConfFiles: Value moleculeParams set to\t%s.\n", moleculeParams );
//	printf( "setConfFiles: Value tipParams set to\t\t%s.\n", tipParams );

}

#endif
