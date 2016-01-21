
class scalarDataWrapper : public abstractDataWrapper {
// scalar data wrapper

	public:
	
	double* data = NULL;
	int num = 0;
	
	void* metadata = NULL;
	
	void setValue( int i, double val );
	void setMetadata( void* metadata_ );
	
	void exportData( FILE* file );
	void importData( FILE* file );
	
	scalarDataWrapper(){};
	~scalarDataWrapper();
	scalarDataWrapper( int num, void* metadata_, char* dataName_ = "scalars" );
	
	double operator [] ( int i );

};


// ==================== scalarDataWrapper ==================== 

double scalarDataWrapper::operator [] ( int i ){
// get an i-th item

	return data[i];

}

void scalarDataWrapper::setMetadata( void* metadata_ ){
// set metadata

	metadata = metadata_;

}

void scalarDataWrapper::setValue( int i, double val ){
// set a value to the i-th item

	if( i < 0 || i > num ){
		printf( "scalarDataWrapper::setValue: Index %i out of bounds [0,%i]. No value set.\n", i, num );
	} else {
		data[i] = val;
	}

}

scalarDataWrapper::~scalarDataWrapper(){
// destructor

	delete [] data;

}

scalarDataWrapper::scalarDataWrapper( int num_, void* metadata_, char* dataName_ )
// constructor
: num( num_ ), metadata( metadata_ ), abstractDataWrapper( dataName_ )
{
	data = new(std::nothrow) double[num];
	if( data == NULL ){
		printf( "scalarDataWrapper: Not enough space for %s.\n", dataName.c_str() );
	} else {
		for( int i = 0; i < num; i++ ) data[i] = 0;
	}

}

void scalarDataWrapper::exportData( FILE* file ){
// export scalar data to the file

	int xdim = ((int*) metadata)[0];
	int ydim = ((int*) metadata)[1];
	int zdim = ((int*) metadata)[2];
	
	int total = ((int*) metadata)[3];

	fprintf( file, "%i %i %i\n", xdim, ydim, zdim );
	for( int i = 0; i < num; i++ ){
		fprintf( file, "%lf\n", data[i] );
	}		
}

void scalarDataWrapper::importData( FILE* file ){
// import scalar data from a file
//TODO

}
