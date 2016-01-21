
class quaterDataWrapper : public abstractDataWrapper {
// quaternion data wrapper

	public:

	Quat4d* data = NULL;
	int num = 0;
	
	void* metadata = NULL;
	
	void setValue( int i, Quat4d val );
	void setMetadata( void* metadata_ );
	
	void exportData( FILE* file );
	void importData( FILE* file );
	
	quaterDataWrapper(){};
	~quaterDataWrapper();
	quaterDataWrapper( int num, void* metadata_, char* dataName_ = "quaternions" );
	
	Quat4d operator [] ( int i );

};

// ==================== quaterDataWrapper ====================

Quat4d quaterDataWrapper::operator [] ( int i ){
// get the i-th value

	return data[i];

}

void quaterDataWrapper::setMetadata( void* metadata_ ){
// set metadata

	metadata = metadata_;

}

void quaterDataWrapper::setValue( int i, Quat4d val ){
// set value to the i-th item

	if( i < 0 || i > num ){
		printf( "quaterDataWrapper::setValue: Index %i out of bounds [0,%i]. No value set.\n", i, num );
	} else {
		data[i].set( val );
	}

}

quaterDataWrapper::~quaterDataWrapper(){
// destructor

	delete [] data;

}

quaterDataWrapper::quaterDataWrapper( int num_, void* metadata_, char* dataName_ )
// constructor
: num( num_ ), metadata( metadata_ ), abstractDataWrapper( dataName_ )
{
	data = new(std::nothrow) Quat4d[num];
	if( data == NULL ){
		printf( "quaterDataWrapper: Not enough space for %s.\n", dataName.c_str() );
	} else {
		for( int i = 0; i < num; i++ ) data[i].setOne();
	}

}

void quaterDataWrapper::exportData( FILE* file ){
// export quaternion data to the file

	int xdim = ((int*) metadata)[0];
	int ydim = ((int*) metadata)[1];
	int zdim = ((int*) metadata)[2];
	
	int total = ((int*) metadata)[3];

	fprintf( file, "%i %i %i\n", xdim, ydim, zdim );
	for( int i = 0; i < num; i++ ){
		fprintf( file, "%lf %lf %lf %lf\n", data[i].x, data[i].y, data[i].z, data[i].w );
	}		
}

void quaterDataWrapper::importData( FILE* file ){
// import quaternion data from the file

	char str[400];

//	if( metadata != NULL ) delete metadata;
//	metadata = new int[3];
	if( metadata == NULL ) metadata = new int[3];

	readLineComment( file, str );
	sscanf( str, "%i %i %i\n", &((int*) metadata)[0], &((int*) metadata)[1], &((int*) metadata)[2] );
	
	for( int i = 0; i < num; i++ ){
		readLineComment( file, str );
		sscanf( str, "%lf %lf %lf %lf\n", &data[i].x, &data[i].y, &data[i].z, &data[i].w );
	}	
}


