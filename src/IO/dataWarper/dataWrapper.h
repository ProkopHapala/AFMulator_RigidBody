
// DATA MANAGEMENT

#include <string>

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
