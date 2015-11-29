
#ifndef _DATA_MANAGEMENT_
#define _DATA_MANAGEMENT_

#include <string>

// DATA MANAGEMENT

class abstractDataWrapper {
// parent class for data wrappers

	public:
	std::string dataName;
	
	const char* getDataName();
	virtual void setValue(){};
	virtual void setMetadata( void* metadata_ ){};
	
	virtual void exportData( FILE* file ){};
	virtual void importData( FILE* file ){};
	
	abstractDataWrapper(){};
	abstractDataWrapper( char* dataName_ );

};

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

class vectorDataManager : public vectorDataWrapper {
// manager of a set of vector data wrappers

	public:
	
	int numOfArrays = 0;
	vectorDataWrapper** data = NULL;
	
	void setValue( int i, int j, Vec3d val );

	int getNumOfArrays();

	~vectorDataManager();
	vectorDataManager(){};
	vectorDataManager( int num2 , int num , int* metadata_, char* dataName_ = "vectors" );
	
	vectorDataWrapper* operator [] ( int i );
	
};

class quaterDataManager : public quaterDataWrapper {
// manager of a set of quaternion data wrappers

	public:
	
	int numOfArrays = 0;
	quaterDataWrapper** data = NULL;
	
	void setValue( int i, int j, Quat4d val );

	int getNumOfArrays();

	~quaterDataManager();
	quaterDataManager(){};
	quaterDataManager( int num2 , int num , void* metadata_, char* dataName_ = "quaternions" );
	
	quaterDataWrapper* operator [] ( int i );
	
};

// ==================== abstractDataWrapper ==================== 


abstractDataWrapper::abstractDataWrapper( char* dataName_ )
// constructor
: dataName( dataName_ )
{
}

const char* abstractDataWrapper::getDataName(){
// get a data name

	return dataName.c_str();

}

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

// ==================== vectorDataManager ====================

vectorDataWrapper* vectorDataManager::operator [] ( int i ){
// get the i-th wrapper
	
	if( i < 0 || i > numOfArrays ){
		printf( "vectorDataManager::operator []: Index %i out of bounds [0,%i]. NULL returned.\n", i, numOfArrays );
		return NULL;
	} 
	
	return data[i];

}

void vectorDataManager::setValue( int i, int j, Vec3d val ){
// set value to the j-th item of the i-th wrapper

	if( i < 0 || i > numOfArrays ){
		printf( "vectorDataManager::setValue: Index %i out of bounds [0,%i]. No value set.\n", i, numOfArrays );
	} else {
		data[i]->setValue( j, val );
	}
}

vectorDataManager::~vectorDataManager(){
// destructor

	for( int i = 0; i < numOfArrays; i++ ){
		delete data[i];
	}
//	delete data;
}

vectorDataManager::vectorDataManager( int num2_ , int num , int* metadata_, char* dataName_ )
// constructor
: numOfArrays( num2_ )
{
	data = new vectorDataWrapper*[numOfArrays];
	for( int i = 0; i < numOfArrays; i++ ){
		data[i] = new vectorDataWrapper( num, metadata_, dataName_ );
	}
}

int vectorDataManager::getNumOfArrays(){
// get the number of wrappers contained in the manager

	printf( "numOfArrays = %i\n", numOfArrays );
	return numOfArrays;

}

// ==================== quaterDataManager ====================

quaterDataWrapper* quaterDataManager::operator [] ( int i ){
// get the i-th wrapper
	
	if( i < 0 || i > numOfArrays ){
		printf( "quaterDataManager::operator []: Index %i out of bounds [0,%i]. NULL returned.\n", i, numOfArrays );
		return NULL;
	} 
	
	return data[i];

}

void quaterDataManager::setValue( int i, int j, Quat4d val ){
// set value to the j-th item of the i-th wrapper

	if( i < 0 || i > numOfArrays ){
		printf( "quaterDataManager::setValue: Index %i out of bounds [0,%i]. No value set.\n", i, numOfArrays );
	} else {
		data[i]->setValue( j, val );
	}

}

quaterDataManager::~quaterDataManager(){
// destructor

	for( int i = 0; i < numOfArrays; i++ ){
		delete data[i];
	}
//	delete data;
}

quaterDataManager::quaterDataManager( int num2_ , int num , void* metadata_, char* dataName_ )
// constructor
: numOfArrays( num2_ )
{
	data = new quaterDataWrapper*[numOfArrays];
	for( int i = 0; i < numOfArrays; i++ ){
		data[i] = new quaterDataWrapper( num, metadata_, dataName_ );
	}

}

int quaterDataManager::getNumOfArrays(){
// get the number of wrappers contained in the manager

	printf( "numOfArrays = %i\n", numOfArrays );
	return numOfArrays;

}

#endif
