

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

// ==================== function implementation

quaterDataWrapper* quaterDataManager::operator [] ( int i ){

// get the i-th wrapper
	
	if( i < 0 || i > numOfArrays ){
		printf( "quaterDataManager::operator []: Index %i out of bounds [0,%i]. NULL returned.\n", i, numOfArrays );
		return NULL;
	} 
	
	//return data[i];

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
