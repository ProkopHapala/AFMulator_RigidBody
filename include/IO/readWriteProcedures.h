
#ifndef _READ_WRITE_PROCEDURES_
#define _READ_WRITE_PROCEDURES_

#include <string>

void readLineComment( FILE*& file, char* str, bool showComments = false ){
// reads lines in a file and checks whether it is a comment (beginning with '#'), white space or a meaningful value; it reads through successive lines until it reaches a line with a meaningful value, the output is stored in str
// assumes str is a long enough char array

	bool isComment = true;
	int i, len, lineLength = 400;
	char comment_char[] = "#";
	
	while( isComment ){
	
		if( fgets( str, lineLength, file ) != NULL ){
	
			// if the line is a comment or contains white characters only...
			len = strlen( str );
			for( i = 0; i < len; i++ ){
				if( !isspace( str[i] ) ){
					if( !strncmp( &str[i], comment_char, 1 ) ){
						if( showComments ) printf( "%s", str );
					} else {
						isComment = false;
					}
					break;
				}
			}

		} else {
		
			isComment = false;
			printf( "readLineComment: EOF.\n" );
			
		}

	}

}

// ALL THE FOLLOWING PROCEDURES WORK WELL, BUT ARE NOT USED IN THE MAIN CODE

//void exportReplayMetadata( FILE* file, void* data, void* metadata ){
//// export positions, orientations and relaxation-abilities for molecules in the system

//	std::string stringTime = ((std::string*) metadata)[0];
//	int num = strtol( ((std::string*) metadata)[1].c_str(), NULL, 10 );

//	fprintf( file, "%s\n", stringTime.c_str() );
//	for( int i = 0; i < num; i++ ){
//		fprintf( file, "%s\n", ((std::string*) data)[i].c_str() );
//	}

//}

//void exportGeometry( FILE* file, void* data, void* metadata ){
//// export positions, orientations and relaxation-abilities for molecules in the system

//	int nmols = ((int*) metadata)[0];

//	fprintf( file, "%i\n", nmols );
//	for( int i = 0; i < nmols; i++ ){
//		fprintf( file, "%i\t%lf %lf %lf\t", i + 1, ((double*) data)[8*i + 0], ((double*) data)[8*i + 1], ((double*) data)[8*i + 2] );
//		fprintf( file, "%lf %lf %lf %lf\t", ((double*) data)[8*i + 3], ((double*) data)[8*i + 4], ((double*) data)[8*i + 5], ((double*) data)[8*i + 6]  );
//		fprintf( file, "%.0lf\n", ((double*) data)[8*i + 7]  );
//	}

//}

//void exportQuaternions3D( FILE* file, void* data, void* metadata ){
//// export data to the file

//	int xdim = ((int*) metadata)[0];
//	int ydim = ((int*) metadata)[1];
//	int zdim = ((int*) metadata)[2];
//	int total = ((int*) metadata)[3];

//	fprintf( file, "%i %i %i\n", xdim, ydim, zdim );
//	for( int i = 0; i < total; i++ ){
//		fprintf( file, "%lf %lf %lf %lf\n", ((Quat4d*) data)[i].x, ((Quat4d*) data)[i].y, ((Quat4d*) data)[i].z, ((Quat4d*) data)[i].w );
//	}		
//}

//void importQuaternions3D( FILE* file, void* data, void* metadata ){
//// export data to the file

//	char str[400];
//	
//	readLineComment( file, str );
//	sscanf( str, "%i %i %i\n", &((int*) metadata)[0], &((int*) metadata)[1], &((int*) metadata)[2] );
//	int total = ((int*) metadata)[0]*((int*) metadata)[1]*((int*) metadata)[2];
//	for( int i = 0; i < total; i++ ){
//		readLineComment( file, str );
//		sscanf( str, "%lf %lf %lf %lf\n", &((Quat4d*) data)[i].x, &((Quat4d*) data)[i].y, &((Quat4d*) data)[i].z, &((Quat4d*) data)[i].w );	
//	}		
//}

//void exportVectors3D( FILE* file, void* data, void* metadata ){
//// export data to the file

//	int xdim = ((int*) metadata)[0];
//	int ydim = ((int*) metadata)[1];
//	int zdim = ((int*) metadata)[2];
//	int total = ((int*) metadata)[3];

//	fprintf( file, "%i %i %i\n", xdim, ydim, zdim );
//	for( int i = 0; i < total; i++ ){
//		fprintf( file, "%lf %lf %lf\n", ((Vec3d*) data)[i].x, ((Vec3d*) data)[i].y, ((Vec3d*) data)[i].z );
//	}		
//}

//void importVectors3D( FILE* file, void* data, void* metadata ){
//// export data to the file

//	char str[400];
//	
//	readLineComment( file, str );
//	sscanf( str, "%i %i %i\n", &((int*) metadata)[0], &((int*) metadata)[1], &((int*) metadata)[2] );
//	int total = ((int*) metadata)[0]*((int*) metadata)[1]*((int*) metadata)[2];
//	for( int i = 0; i < total; i++ ){
//		readLineComment( file, str );
//		sscanf( str, "%lf %lf %lf\n", &((Vec3d*) data)[i].x, &((Vec3d*) data)[i].y, &((Vec3d*) data)[i].z );
//	}
//	
//}

//void exportScalars3D( FILE* file, void* data, void* metadata ){
//// export data to the file

//	int xdim = ((int*) metadata)[0];
//	int ydim = ((int*) metadata)[1];
//	int zdim = ((int*) metadata)[2];
//	int total = ((int*) metadata)[3];

//	fprintf( file, "%i %i %i\n", xdim, ydim, zdim );
//	for( int i = 0; i < total; i++ ){
//		fprintf( file, "%lf\n", ((double*) data)[i] );
//	}		
//}

//void exportScalars1D( FILE* file, void* data, void* metadata ){
//// export data to the file

//	int zdim = ((int*) metadata)[2]; // !!!

//	fprintf( file, "%i\n", zdim );
//	for( int i = 0; i < zdim; i++ ){
//		fprintf( file, "%lf\n", ((double*) data)[i] );
//	}		
//}

//void exportDataToFile( const char functionName[500], const char outputFileNameDefault[500], const char outputFileName[500], const char dataName[500], void (*funExportBare)( FILE*, void*, void* ), void* data, void* metadata ){
////void exportDataToFile( const char* functionName, const char* outputFileNameDefault, const char* outputFileName, const char* dataName, void (*funExportBare)( FILE*, void*, void* ), void* data, void* metadata ){
//// wrapper for export functions, responsible for external file treatment

//	char outputFileNameLocal[500];
//	strcpy( outputFileNameLocal, outputFileNameDefault );
//	
//	FILE* outputFile;
//	if( !strcmp( outputFileName, "" ) ){
//		printf( "%s: No output data file for %s specified. Default value used instead.\n", functionName, dataName );
//		outputFile = fopen( outputFileNameLocal, "w" );
//	} else {
//		outputFile = fopen( outputFileName, "w" );
//		if( !outputFile ){
//			printf( "%s: Specified output data file %s cannot be opened. Default value %s used instead.\n", functionName, outputFileName, outputFileNameLocal );
//			outputFile = fopen( outputFileNameLocal, "w" );
//		} else {
//			strcpy( outputFileNameLocal, outputFileName );
//		}
//	}
//	if( outputFile ){
//		printf( "%s: Writing %s to the file %s.\n", functionName, dataName, outputFileNameLocal );
//		
//		(*funExportBare)( outputFile, data, metadata );

//		fclose( outputFile );	
//	} else {
//		printf( "%s: Sorry, even the default file cannot be opened. No data export.\n", functionName );
//	}
//	
//}

//void importDataFromFile( char functionName[500], const char inputFileName[500], char dataName[500], void (*funImportBare)( FILE*, void*, void* ), void* data, void* metadata ){
//// wrapper for import functions, responsible for external file treatment

//	FILE* inputFile = fopen( inputFileName, "r" );
//	if( inputFile == NULL ){
//		printf( "%s: Specified input data file %s cannot be opened. No %s read.\n", functionName, inputFileName, dataName );
//	} else {
//		printf( "%s: Reading %s from the file %s.\n", functionName, dataName, inputFileName );
//		
//		(*funImportBare)( inputFile, data, metadata );

//		fclose( inputFile );
//	}	
//}

#endif
