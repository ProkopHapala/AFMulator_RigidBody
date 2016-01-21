
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
