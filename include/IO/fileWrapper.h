
class fileWrapper {
// wrapper for external file handling
	
	public:
	
	enum dataTypeStruct {
	
		DATA_QUAT_GEOMETRY,
		DATA_VECT_GEOMETRY,
		DATA_NULL
	
	} dataType;
	
	FILE* file = NULL;
	std::string fileNameBase;	// the whole file name = fileNamePrefix + fileNameBase
	std::string fileNamePrefix = "";

	bool isRaw();
	bool openRead();
	bool isOpened();
	void close();
//	const char* getFileName();
	char* getFileName();
	void printFileName();
	const char* getFileNameBase();
	FILE* getFile();
	
	void setFileNameBase( const char* fileNameBase_ );
	
	void readLineToString( char* str );

	fileWrapper(){};
	fileWrapper( const char* fileNameBase_ );
	fileWrapper( char* fileNameBase_, bool raw );
	fileWrapper( std::string fileNamePrefix_, const char* fileNameBase_ );
	fileWrapper( const char* fileNamePrefix_, const char* fileNameBase_ );
	
	void importDataFromFile( const char functionName[500], abstractDataWrapper* dataArray );		
	void exportDataToFile( const char functionName[500], abstractDataWrapper* dataArray );
	
};


// ==================== fileWrapper ==================== 

void fileWrapper::printFileName(){
// print file name
	printf( "fileWrapper::printFileName: fileName = %s.\n", getFileName() );

}

void fileWrapper::setFileNameBase( const char* fileNameBase_ ){
// set file name

	fileNameBase.assign( fileNameBase_ );
	printf( "fileWrapper::setFileNameBase: fileNameBase = %s.\n", fileNameBase.c_str() );

}

bool fileWrapper::isRaw(){
// is the file format DATA_QUAT_GEOMETRY?

//	return ( dataType == DATA_QUAT_GEOMETRY ) ? true : false;
	return dataType == DATA_QUAT_GEOMETRY;

}

void fileWrapper::readLineToString( char* str ){
// assumes str is a long enough char array
// assumes file is opened

	readLineComment( file, str, false );

}

bool fileWrapper::openRead(){
// open the file for reading

	file = fopen( fileNameBase.c_str(), "r" );
	if( file == NULL ) return false;
	return true;

}

bool fileWrapper::isOpened(){
// if file opened?

//	return ( file != NULL ) ? true : false;
	return file != NULL;
	
}

void fileWrapper::close(){
// close the file

	fclose( file );
	file = NULL;

}

const char* fileWrapper::getFileNameBase(){
// get file base name

//	printf( "fileWrapper::getFileName: fileNameBase = %s\n", fileNameBase.c_str() );
	return fileNameBase.c_str();

}

char* fileWrapper::getFileName(){
// get file name

	char str[400];
	sprintf( str, "%s%s", fileNamePrefix.c_str(), fileNameBase.c_str() );
	return str;

}

FILE* fileWrapper::getFile(){
// get file

	return file;

}

fileWrapper::fileWrapper( std::string fileNamePrefix_, const char* fileNameBase_ )
// constructor
: fileNamePrefix( fileNamePrefix_ ), fileNameBase( fileNameBase_ )
{
}	

fileWrapper::fileWrapper( const char* fileNamePrefix_, const char* fileNameBase_ )
// constructor
: fileNamePrefix( fileNamePrefix_ ), fileNameBase( fileNameBase_ )
{
}	


fileWrapper::fileWrapper( const char* fileNameBase_ )
// constructor
: fileNameBase( fileNameBase_ )
{
}	

fileWrapper::fileWrapper( char* fileNameBase_, bool raw )
// constructor
: fileNameBase( fileNameBase_ )
{
	if( raw ){
		dataType = DATA_QUAT_GEOMETRY;
	} else {
		dataType = DATA_VECT_GEOMETRY;
	}
}	

void fileWrapper::importDataFromFile( const char functionName[500], abstractDataWrapper* dataArray ){
// wrapper for import functions, responsible for external file treatment
	
	char wholeFileName[800];
	sprintf( wholeFileName, "%s%s", fileNamePrefix.c_str(), fileNameBase.c_str() );
	
	file = fopen( wholeFileName, "r" );
	if( file == NULL ){
		printf( "%s: Specified input data file %s cannot be opened. No %s read.\n", functionName, wholeFileName, dataArray->getDataName() );
	} else {
		printf( "%s: Reading %s from the file %s.\n", functionName, dataArray->getDataName(), wholeFileName );
		dataArray->importData( file );
		fclose( file );
	}	
}

void fileWrapper::exportDataToFile( const char functionName[500], abstractDataWrapper* dataArray ){
// wrapper for export functions, responsible for external file treatment

	char wholeFileName[800];
	sprintf( wholeFileName, "%s%s", fileNamePrefix.c_str(), fileNameBase.c_str() );
	
	file = fopen( wholeFileName, "w" );
	if( file != NULL ){
		printf( "%s: Writing %s to the file %s.\n", functionName, dataArray->getDataName(), wholeFileName );
		dataArray->exportData( file );
		fclose( file );	
	} else {
		printf( "%s: Specified output data file %s cannot be opened. No data export.\n", functionName, wholeFileName );
	}

}
