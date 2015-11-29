
class fileManager : fileWrapper {
// class embracing more files

	public:
	
	int numOfFiles = 0;
	fileWrapper** listOfFiles;
	std::string fileNamePrefix = "";
	
	fileManager(){};
	fileManager( int n );
	fileManager( int n, std::string fileNamePrefix_ );
	fileManager( int n, const char* fileNamePrefix_ );
	fileManager( int n, std::string* listOfFileNames, int* listOfMoleculeInstances );	
	
	void exportDataToFiles( const char functionName[500], vectorDataManager* dataArray );
	void exportDataToFiles( const char functionName[500], quaterDataManager* dataArray );

	void importDataFromFiles( const char functionName[500], vectorDataManager* dataArray );
	void importDataFromFiles( const char functionName[500], quaterDataManager* dataArray );
	
	fileWrapper* getFile( int i );
	fileWrapper* operator [] ( int i );

};


// ==================== fileManager ==================== 

void fileManager::importDataFromFiles( const char functionName[500], quaterDataManager* dataArray ){
// import quaternion data from multiple files

	for( int i = 0; i < numOfFiles; i++ ){
		listOfFiles[i]->importDataFromFile( functionName, (*dataArray)[i] );
	}

}

void fileManager::importDataFromFiles( const char functionName[500], vectorDataManager* dataArray ){
// import vector data from multiple files

	for( int i = 0; i < numOfFiles; i++ ){
		listOfFiles[i]->importDataFromFile( functionName, (*dataArray)[i] );
	}

}

void fileManager::exportDataToFiles( const char functionName[500], vectorDataManager* dataArray ){
// export vector data to multiple files

	for( int i = 0; i < numOfFiles; i++ ){
		listOfFiles[i]->exportDataToFile( functionName, (*dataArray)[i] );
	}

}

void fileManager::exportDataToFiles( const char functionName[500], quaterDataManager* dataArray ){
// export quaternion data to multiple files

	for( int i = 0; i < numOfFiles; i++ ){
		listOfFiles[i]->exportDataToFile( functionName, (*dataArray)[i] );
	}

}

fileWrapper* fileManager::getFile( int i ){
// get i-th file

	if( i > numOfFiles ){
		printf( "fileManager::getFile: Index out of bounds. Index > number of files.\n" );
		return NULL;
	}
	if( i < 0 ){
		printf( "fileManager::getFile: Index out of bounds. Index < 0.\n" );
		return NULL;
	}
	return listOfFiles[i];

}

fileWrapper* fileManager::operator [] ( int i ){
// get i-th file

	if( i > numOfFiles ){
		printf( "fileManager::operator []: Index out of bounds. Index > number of files.\n" );
		return NULL;
	}
	if( i < 0 ){
		printf( "fileManager::operator []: Index out of bounds. Index < 0.\n" );
		return NULL;
	}
	return listOfFiles[i];

}

fileManager::fileManager( int n )
// constructor
: numOfFiles( n )
{
	listOfFiles = new fileWrapper*[numOfFiles];
	for( int i = 0; i < numOfFiles; i++ ){
		listOfFiles[i] = new fileWrapper();
	}

	
}

fileManager::fileManager( int n, std::string fileNamePrefix_ )
// constructor
: numOfFiles( n ), fileNamePrefix( fileNamePrefix_ )
{
	listOfFiles = new fileWrapper*[numOfFiles];
	char str[400];

	for( int i = 0; i < numOfFiles; i++ ){
		sprintf( str, "%i.dat", i );
		listOfFiles[i] = new fileWrapper( fileNamePrefix, str );
	}
}

fileManager::fileManager( int n, const char* fileNamePrefix_ )
// constructor
: numOfFiles( n ), fileNamePrefix( fileNamePrefix_ )
{
	listOfFiles = new fileWrapper*[numOfFiles];
	char str[400];

	for( int i = 0; i < numOfFiles; i++ ){
		sprintf( str, "%i.dat", i );
		listOfFiles[i] = new fileWrapper( fileNamePrefix, str );
	}
}

fileManager::fileManager( int n, std::string* listOfFileNames, int* listOfMoleculeInstances )
// constructor
: numOfFiles( n )
{

	CHECK(numOfFiles)
	// lenght of an array listOfFileNames is assumed to be less than the maximum of an array listOfMoleculeInstances!!!
	listOfFiles = new fileWrapper*[numOfFiles];
	for( int i = 0; i < numOfFiles; i++ ){
//		listOfFiles->setFileNameBase( listOfFileNames[listOfMoleculeInstances[i]].c_str() );
		listOfFiles[i] = new fileWrapper( listOfFileNames[listOfMoleculeInstances[i]].c_str() );
		listOfFiles[i]->printFileName();
	}
	
}
