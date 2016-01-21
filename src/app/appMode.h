
#ifndef _APP_MODES_
#define _APP_MODES_

#include <string>

class abstractAppMode {
// parent class for all application modes

	public:

	bool delay = false;
	bool suppressOutput = false;

	PhysicalSystem* world = NULL;
	int numOfMoleculeInstances = 0;	// number of molecule instances, many molecules may be of the same instance
					// an instance defines specific atom positions within a given molecule 

	MoleculeType** molp = NULL;	// molecules
					// a molecule defines orientation and position of the molecule in the global system
					// two molecules might be of the same molecule instance, in which case a modification of one molecule affects accordingly the other molecule
					// two molecules may have the same topology, i.e. the type, but their atoms might be of slightly different mutual positions, i.e. different instances
					
					// 2 basic modes: each molecule has its own instance -- in such a case modification of a given molecule does not affects other molecules
					// 		  several molecules share the same instance -- in such a case modifications are also shared, but in this case less memory is used

	
	
	graphInterface* graphics = NULL;

	virtual void loop( int n ){};
	abstractAppMode(){};
	abstractAppMode( graphInterface* graphics_, int numOfMoleculeInstances_, bool delay_ = false, bool suppressOutput_ = false );
	abstractAppMode( graphInterface* graphics_, int numOfMoleculeInstances_, flagList *flags = NULL );
	virtual ~abstractAppMode();

};


// ================= abstractAppMode procedures ================= 

abstractAppMode::~abstractAppMode(){
// destructor
}

abstractAppMode::abstractAppMode( graphInterface* graphics_, int numOfMoleculeInstances_, flagList *flags )
// constructor
: numOfMoleculeInstances( numOfMoleculeInstances_ ), graphics( graphics_ )
{
	if( flags != NULL ){
		delay = flags->delay;
		suppressOutput = flags->suppressOutput;
	} else {
		delay = false;
		suppressOutput = false;
	}

}

abstractAppMode::abstractAppMode( graphInterface* graphics_, int numOfMoleculeInstances_, bool delay_, bool suppressOutput_ )
// constructor
: numOfMoleculeInstances( numOfMoleculeInstances_), graphics( graphics_), delay( delay_ ), suppressOutput( suppressOutput_ )
{
}


#endif
