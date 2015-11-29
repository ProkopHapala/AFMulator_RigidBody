#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <math.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_ttf.h>

// !!!
#define CHECK(NUM) printf( "CHECK_POINT %i\n", NUM );
//#define CHECK(NUM)
// !!!

#include "include/terminalAccessories.h"
#include "include/Vec3.h"
#include "include/Mat3.h"
#include "include/quaternion.h"
#include "include/raytrace.h"

#include "include/readWriteProcedures.h"
#include "include/dataManagement.h"

#include "include/scanProcedures.h"

#include "include/settingProcedures.h"
#include "include/drawMath.h"
#include "include/drawUtils.h"

#include "include/fileManagement.h"

#include "include/atomTypes.h"
#include "include/moleculeType.h"

#include "include/forceField.h"
#include "include/surfProcedures.h"
#include "include/tipProcedures.h"

#include "include/optimizerDerivs.h"
#include "include/physicalSystem.h"
#include "include/buttons.h"

#include "include/mainAccessories.h"
#include "include/Screen.h"
#include "include/graphicalInterface.h"

#include "include/appModes.h"


// ===============================
// ===== GLOBAL VARIABLES
// ===============================

AtomTypes* typeList	= NULL;
tipOnePoint* tip	= NULL;
abstractSurf* surf	= NULL;
flagList *flags = new flagList;

char scanningParams[400];
char moleculeParams[400];
char tipParams[400];

char inputDataPrefix[]      = "InputData/";
char outputDataPrefix[]     = "OutputData/";
char pixelDataFileName[400] = "pixelData.out";
char zStepDataFileName[400] = "zstepData.out";
char positDataFileName[400] = "positData.out";
char rotatDataFileName[400] = "rotatData.out";
char metadataFileName[400]  = "metadata.out";
char atomTypesFile[400]     = "";
char geometryFileName[400]  = "";
bool raw_data;

char surfSettingFile[400]        = "InputData/surfParams";
char confFilesCommander[400]     = "InputData/confFilesCommander";
char outputPositMolFilesPrefix[] = "OutputData/positMolData";
char outputRotatMolFilesPrefix[] = "OutputData/rotatMolData";
	

int numOfMoleculeTypes;			// number of molecule types, many molecule instances may be of the same type
					// a type defines number of atoms, their elements and topology of the molecule
int numOfMoleculeInstances;		// number of molecule instances, many molecules may be of the same instance
					// an instance defines specific atom positions within a given molecule 
	
// SCANNING
	
scanSpecification* scan = new scanSpecification();
relaxParams* relaxParameters = new relaxParams;
graphInterface* graphics = NULL;
abstractAppMode* app = NULL;
	
int* molOfInterest;
int numMolOfInterest;

// ===============================
// ===== FUNCTIONS
// ===============================

// deallocation in: atomTypes.h, (button.cpp), ListKeeper.h, moleculeType.h, (optimizerDerivs.h), physicalSystem.h, screen.h
void terminateApp(){
// terminate the application
	
	if( graphics != NULL ){
		delete graphics;
	}
	delete typeList;
	delete tip;
	delete surf;
	delete flags;
	delete scan;
	delete relaxParameters;
	
//	delete app;
	printf( "\n=== APPLICATION TERMINATED SUCCESSFULLY ===\n" );

}

void setupApp( bool& loopEnd ){
// setup the application

	printf( "setupApp: === APPLICATION SETUP... ===\n" );

	std::string* listOfFileNames = new std::string[1];
	int* listOfMoleculeInstances = new int[1];
	std::string probeMolFileName;
	
	// set parameters from external files

	setScanningParams( scanningParams, scan, relaxParameters, molOfInterest, numMolOfInterest );
	setMoleculeParams( moleculeParams, listOfMoleculeInstances, listOfFileNames,
		numOfMoleculeInstances, numOfMoleculeTypes, geometryFileName, raw_data );
	
	typeList = new AtomTypes( atomTypesFile );
	surf	 = new surfVert( surfSettingFile );
		
	Vec3d surf_hat_inv;
	surf_hat_inv.set_mul( surf->getHat(), -1 );
	tip	 = new tipOnePointMol( tipParams, surf_hat_inv );
			
	// allocate molecules and create the world
	
	fileWrapper* geometryFile  = new fileWrapper( geometryFileName, raw_data );
	fileManager* moleculeFiles = new fileManager( numOfMoleculeInstances, listOfFileNames, listOfMoleculeInstances );
	
	fileManager* outputFiles = NULL;
	outputFiles = new fileManager( 4, outputDataPrefix );
	outputFiles->getFile( 0 )->setFileNameBase( positDataFileName );
	outputFiles->getFile( 1 )->setFileNameBase( pixelDataFileName );
	outputFiles->getFile( 2 )->setFileNameBase( zStepDataFileName );
	outputFiles->getFile( 3 )->setFileNameBase( rotatDataFileName );
	
	fileManager* outputPositMolFiles = new fileManager( numMolOfInterest, outputPositMolFilesPrefix );
	fileManager* outputRotatMolFiles = new fileManager( numMolOfInterest, outputRotatMolFilesPrefix );
//	CHECK(753)
	
	switch( scan->scanType ){
		case SCANNING_NONE_VIEW:
			app = new appModeView( numOfMoleculeInstances, moleculeFiles, typeList, geometryFile, surf, graphics );
			break;
		case SCANNING_NONE:
			app = new appModeRelax( numOfMoleculeInstances, moleculeFiles, typeList, geometryFile, surf, graphics, flags );
			break;
		case SCANNING_ONE:
			app = new appModeTipMolOne( numOfMoleculeInstances, moleculeFiles, typeList, geometryFile, surf, graphics, tip, flags );
			break;
		case SCANNING_RASTER:
			app = new appModeRaster( numOfMoleculeInstances, moleculeFiles, typeList, geometryFile, surf, graphics, tip, flags, 
				scan, outputFiles, relaxParameters, molOfInterest, numMolOfInterest, outputPositMolFiles, outputRotatMolFiles );
			break;
		case SCANNING_REPLAY:
			app = new appModeReplay( numOfMoleculeInstances, moleculeFiles, typeList, geometryFile, surf, graphics, tip, flags, 
				scan, outputFiles, molOfInterest, numMolOfInterest, outputPositMolFiles, outputRotatMolFiles );
			break;
	}
	
//	CHECK(754)
	
	if( app == NULL ) loopEnd = true;
	
	delete [] listOfMoleculeInstances;
	delete [] listOfFileNames;
	
	printf( "setupApp: === ...SETUP IS FINISHED. ===\n" );
}

void getTime( clock_t startTime, int& hours, int& minutes, double& seconds ){
// return time between startTime and call of this function

	clock_t finishTime = clock();
	double execTime = (double) (finishTime - startTime) / CLOCKS_PER_SEC;
	int execTimeInt = ( (int) floor( execTime ) );

	hours = execTimeInt / 3600;
	minutes = ( execTimeInt % 3600 ) / 60;
	seconds = execTime - 60*minutes - 3600*hours;

}

int main( int argc, char *argv[] ){

	bool loopEnd = false;
	clock_t startTime;
	
	// handle arguments from the command line
	handleTerminalInput( argc, argv, inputDataPrefix, outputDataPrefix,
		flags->graphicalMode, flags->delay, flags->suppressOutput, loopEnd, confFilesCommander,
		pixelDataFileName, zStepDataFileName, positDataFileName, metadataFileName );
	if( loopEnd ) exit( 1 );
	
	atexit( terminateApp );

	// if in graphical mode, create screens
	if( flags->graphicalMode ){
		printf( "\n=== GRAPHICAL MODE SELECTED ===\n" );
		graphics = new graphInterface();
	} else {
		printf( "\n=== NON-GRAPHICAL MODE SELECTED ===\n" );
		graphics = NULL;
	}
			
	// setup application
	setConfFiles( confFilesCommander, atomTypesFile, scanningParams, moleculeParams, tipParams );
	setupApp( loopEnd );
	if( loopEnd ) exit( 1 );
		
	startTime = clock();

	// loop itself	
	app->loop( 1e6 );

	// calculate the time for which the main loop has been executed
	int minutes, hours;
	double seconds;
		
	getTime( startTime, hours, minutes, seconds );
	printf( "\nLoop lasted for %i hours, %i minutes and %lf seconds.\n", hours, minutes, seconds );

	// terminate
//	exportMetadata( metadataFileName );
//	exportMetadata( metadataFileName, atomTypesFile, confFilesCommander, minStep, numOfMoleculeInstances, glob_molToDraw_orig, glob_atomToDraw_orig, posProbe, rotProbe, zSampling, scanType, xoffset, yoffset, zoffset, xstep, ystep, zstep, xdim, ydim, zdim, glob_maxIters, glob_dtmax, glob_damping, molOfInterest, hours, minutes, seconds );
	exit( 0 );

}

// ==========================================================
// ===== Export this functions to Dynamic library for Python
// ==========================================================

extern "C"{



}


