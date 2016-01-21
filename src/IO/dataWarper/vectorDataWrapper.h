
class vectorDataWrapper : public abstractDataWrapper {
// vector data wrapper

	public:
	
	
	Vec3d* data = NULL;
	int num = 0;
	
	int* metadata = NULL;
	
	void setValue( int i, Vec3d val );
	void setMetadata( int* metadata_ );
	
	void exportData( FILE* file );
	void importData( FILE* file );
	
	vectorDataWrapper(){};
	~vectorDataWrapper();
	vectorDataWrapper( int num, int* metadata_, char* dataName_ = "vectors" );
	
	Vec3d operator [] ( int i );

};


// ==================== vectorDataWrapper ====================

Vec3d vectorDataWrapper::operator [] ( int i ){
// get an i-th item

	return data[i];

}

void vectorDataWrapper::setMetadata( int* metadata_ ){
// set metadata

	metadata = metadata_;

}

void vectorDataWrapper::setValue( int i, Vec3d val ){
// set value to the i-th item

	if( i < 0 || i > num ){
		printf( "vectorDataWrapper::setValue: Index %i out of bounds [0,%i]. No value set.\n", i, num );
	} else {
		data[i].set( val );
	}

}

vectorDataWrapper::~vectorDataWrapper(){
// destructor

	delete [] data;

}

vectorDataWrapper::vectorDataWrapper( int num_, int* metadata_, char* dataName_ )
// constructor
: num( num_ ), metadata( metadata_ ), abstractDataWrapper( dataName_ )
{
	data = new(std::nothrow) Vec3d[num];
	if( data == NULL ){
		printf( "vectorDataWrapper: Not enough space for %s.\n", dataName.c_str() );
	} else {
		for( int i = 0; i < num; i++ ) data[i].set( 0, 0, 0 );
	}

}

void vectorDataWrapper::exportData( FILE* file ){
// export vector data to the file

	int xdim = metadata[0];
	int ydim = metadata[1];
	int zdim = metadata[2];

	fprintf( file, "%i %i %i\n", xdim, ydim, zdim );
	for( int i = 0; i < num; i++ ){
		fprintf( file, "%lf %lf %lf\n", data[i].x, data[i].y, data[i].z );
	}		
}

void vectorDataWrapper::importData( FILE* file ){
// import vector data from the file

	char str[400];

//	if( metadata != NULL ) delete metadata;
//	metadata = new int[3];
	if( metadata == NULL ) metadata = new int[3];

	readLineComment( file, str );
	sscanf( str, "%i %i %i\n", &metadata[0], &metadata[1], &metadata[2] );
	
	for( int i = 0; i < num; i++ ){
		readLineComment( file, str );
		sscanf( str, "%lf %lf %lf\n", &data[i].x, &data[i].y, &data[i].z );
	}		
}
