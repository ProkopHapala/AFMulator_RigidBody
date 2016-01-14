
class appModeNoTip : public abstractAppMode {
// parent class for modes without a tip

	public:
	
	appModeNoTip(){};
	~appModeNoTip();
	appModeNoTip( int numOfMoleculeInstances_, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics_, flagList *flags = NULL );
	
	void setupApp( fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf );
	
};


// ================= appModeNoTip procedures ================= 

appModeNoTip::appModeNoTip( int numOfMoleculeInstances_, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics_, flagList *flags )
// constructor
: abstractAppMode( graphics_, numOfMoleculeInstances_, flags )
{
	setupApp( moleculeFiles, typeList, geometryFile, surf );
}

void appModeNoTip::setupApp( fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf ){
// setting of all relevant parameters

	// lenght of an array listOfFileNames is assumed to be less than the maximum of an array listOfMoleculeInstances!!!

	printf( ">>> Initializing: appModeNoTip  " );
	abstractAppMode::molp = new MoleculeType*[abstractAppMode::numOfMoleculeInstances];
	for( int i = 0; i < abstractAppMode::numOfMoleculeInstances; i++ ){
		printf( " initialize MoleculeType from \n", (*moleculeFiles)[i]->getFileName() );
		if( abstractAppMode::graphics != NULL ){
			abstractAppMode::molp[i] = new MoleculeType( (*moleculeFiles)[i]->getFileName(), typeList, 0.5, abstractAppMode::graphics->viewMolList );
		} else {
			abstractAppMode::molp[i] = new MoleculeType( (*moleculeFiles)[i]->getFileName(), typeList, 0.5, NULL );
		}
	}
	abstractAppMode::world = new PhysicalSystem( geometryFile, abstractAppMode::numOfMoleculeInstances, abstractAppMode::molp, NULL, surf );
	if( abstractAppMode::world == NULL )
		printf( "abstractAppMode: World cannot be allocated.\n" );
	if( abstractAppMode::graphics != NULL ) abstractAppMode::graphics->world = abstractAppMode::world;

}

appModeNoTip::~appModeNoTip(){
// destructor

	if( world != NULL ) delete world;
//	graphics = NULL; // graphics is nothing, but pointer to external graphics object
	if( molp != NULL ){
		for( int i = 0; i < numOfMoleculeInstances; i++ ){
			delete molp[i];
		}
		delete [] molp;
	}
	numOfMoleculeInstances = 0;
}
