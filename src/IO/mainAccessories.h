
#ifndef _MAIN_ACCESSORIES_
#define _MAIN_ACCESSORIES_

void showHelp(){
// print a list of keyboard shortcuts into a standard input

	printf( "\n-------------------- Help ----------------------\n\n" );
	printf( "Key shortcuts for graphical mode:\n" );

	printf( "\"F1\" \t\t\t print this help\n" );
	printf( "\"Esc\" \t\t\t quit the application\n" );	
	printf( "\"Space\" \t\t (un-)pause the relaxation\n" );		
	printf( "\"Enter\" \t\t skip one step of the molecule rasterization (active in a permanent aux. point mode)\n" );	
	printf( "\"e\" \t\t\t export molecules' rotation and position\n" );	
	printf( "\"Backspace\" \t\t reset the last atom movement\n" );	

	printf( "\"+\" \t\t\t zoom in\n" );
	printf( "\"-\" \t\t\t zoom out\n" );
	printf( "\"mouse wheel forward\" \t zoom in\n" );
	printf( "\"mouse wheel backward\" \t zoom out\n" );

	printf( "\"Arrow Right\" \t\t view along the x-axis\n" );
	printf( "\"Arrow Left\" \t\t view along the y-axis\n" );
	printf( "\"Arrow Up\" \t\t view along the z-axis\n" );
	printf( "\"Arrow Down\" \t\t view from aside\n" );

	printf( "\"Numerical Arrow Right\"  move scanning line to the right (in replay mode only)\n" );
	printf( "\"Numerical Arrow Left\" \t move scanning line to the left (in replay mode only)\n" );
	printf( "\"Numerical Arrow Up\" \t move scanning line up (in replay mode only)\n" );
	printf( "\"Numerical Arrow Down\" \t move scanning line down (in replay mode only)\n" );

	printf( "\"Left Ctrl\" \t\t while kept pressed one can choose (up to three) atoms in a (single) molecule (by clicks of the left mouse button)\n" );
	printf( "\"Left Shift\" \t\t while kept pressed one can move an atom in a molecule (by keeping pressed the left mouse button) or move an entire molecule (by keeping pressed the right mouse button)\n" );
	printf( "\"Left Ctrl + Left Shift\" while kept pressed one can make an auxiliary point to which additional attractive force is exhibited, the point is created by clicking and holding the right mouse button\n" );
	
	printf( "\"Left Mouse Button\" \t while kept pressed one can move a view; if the Left Ctrl or Left Shift is kept pressed other features occur, see above\n" );
	printf( "\"Right Mouse Button\" \t invokes a context menu; when Left Shift is kept pressed, the molecule is moved\n" );
	
	printf( "\n------------------------------------------------\n\n" );
}

void exportMetadata( const char* outputMetadataFileName, char* atomTypesFile, char* confFilesCommander, double minStep, int numOfMoleculeInstances, int glob_molToDraw_orig, int glob_atomToDraw_orig, Vec3d posProbe, Quat4d rotProbe, zSamplingMode zSampling, scanningType scanType, double xoffset, double yoffset, double zoffset, double xstep, double ystep, double zstep, int xdim, int ydim, int zdim, int glob_maxIters, double glob_dtmax, double glob_damping, int molOfInterest, int hours, int minutes, double seconds ){

	FILE* metadataFile = fopen( outputMetadataFileName, "w" );
	if( metadataFile == NULL ){
		printf( "exportMetadata: Cannot export metadata to the file %s. Halt.\n", outputMetadataFileName );	
		return;
	}
	
	printf( "exportMetadata: Exporting metadata into the file %s.\n", outputMetadataFileName );

	fprintf( metadataFile, "==== Configuration files ====\n" );
	
	fprintf( metadataFile, "atomTypesFile\t\t\t\t\t= %s\n", atomTypesFile );
	fprintf( metadataFile, "confFilesCommander\t\t\t\t= %s\n", confFilesCommander );
	fprintf( metadataFile, "minStep\t\t\t\t\t\t= %lf\n", minStep );
	
	fprintf( metadataFile, "==== System ====\n" );
	fprintf( metadataFile, "numOfMoleculeInstances\t\t\t\t= %i\n", numOfMoleculeInstances );
	
	fprintf( metadataFile, "==== Tip specification ====\n" );
	fprintf( metadataFile, "glob_molToDraw_orig, glob_atomToDraw_orig\t= %i %i\n", glob_molToDraw_orig, glob_atomToDraw_orig );
//	if( posProbe != NULL )
		fprintf( metadataFile, "posProbe\t\t\t\t\t= %lf %lf %lf\n", posProbe.x, posProbe.y, posProbe.z );
//	if( rotProbe != NULL )
		fprintf( metadataFile, "rotProbe\t\t\t\t\t= %lf %lf %lf %lf\n", rotProbe.x, rotProbe.y, rotProbe.z, rotProbe.w );

	fprintf( metadataFile, "==== Scanning geometry ====\n" );
	fprintf( metadataFile, "zSampling\t\t\t\t\t= %i\n", zSampling );
	fprintf( metadataFile, "scanType\t\t\t\t\t= %i\n", scanType );	
	fprintf( metadataFile, "xoffset, yoffset, zoffset\t\t\t= %lf %lf %lf\n", xoffset, yoffset, zoffset );
	fprintf( metadataFile, "xstep, ystep, zstep\t\t\t\t= %lf %lf %lf\n", xstep, ystep, zstep );
	fprintf( metadataFile, "xdim, ydim, zdim\t\t\t\t= %i %i %i\n", xdim, ydim, zdim );

	fprintf( metadataFile, "==== Relaxation ====\n" );
	fprintf( metadataFile, "glob_maxIters, glob_dtmax, glob_damping\t\t= %i %lf %lf\n", glob_maxIters, glob_dtmax, glob_damping );
	
	fprintf( metadataFile, "==== Trajectories ====\n" );
	fprintf( metadataFile, "molOfInterest\t\t\t\t\t= %i\n", molOfInterest );
	
	fprintf( metadataFile, "Loop lasted for %i hours, %i minutes and %lf seconds.\n", hours, minutes, seconds );
	
	fclose( metadataFile );
	
}

#endif

