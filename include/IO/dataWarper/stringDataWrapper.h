
class stringDataWrapper : public abstractDataWrapper {
// scalar data wrapper

	public:
	
	std::string* data = NULL;
	int num = 0;
	
	void* metadata = NULL;
	
	void setValue( int i, std::string val );
	void setMetadata( void* metadata_ );
	
	void exportData( FILE* file );
	void importData( FILE* file );
	
	stringDataWrapper(){};
	~stringDataWrapper();
	stringDataWrapper( int num, void* metadata_, char* dataName_ = "strings" );
	
	std::string operator [] ( int i );

};

// ==================== stringDataWrapper ==================== 

std::string stringDataWrapper::operator [] ( int i ){
// get an i-th item

	return data[i];

}

void stringDataWrapper::setMetadata( void* metadata_ ){
// set metadata

	metadata = metadata_;

}

void stringDataWrapper::setValue( int i, std::string val ){
// set a value to the i-th item

	if( i < 0 || i > num ){
		printf( "stringDataWrapper::setValue: Index %i out of bounds [0,%i]. No value set.\n", i, num );
	} else {
		data[i] = val;
	}

}

stringDataWrapper::~stringDataWrapper(){
// destructor

	delete [] data;

}

stringDataWrapper::stringDataWrapper( int num_, void* metadata_, char* dataName_ )
// constructor
: num( num_ ), metadata( metadata_ ), abstractDataWrapper( dataName_ )
{
	data = new(std::nothrow) std::string[num];
	if( data == NULL ){
		printf( "stringDataWrapper: Not enough space for %s.\n", dataName.c_str() );
	} else {
		for( int i = 0; i < num; i++ ) data[i] = "";
	}

}

void stringDataWrapper::exportData( FILE* file ){
// export scalar data to the file

	std::string time = ((std::string*) metadata)[0];

	fprintf( file, "%s\n", time.c_str() );
	for( int i = 0; i < num; i++ ){
		fprintf( file, "%s\n", data[i].c_str() );
	}		
}

void stringDataWrapper::importData( FILE* file ){
// import scalar data from a file
//TODO

}
