
class appModeTipMol : public appModeTip { // parent class for modes with a tip to which the probe molecule is attached
	public:
	Vec3d posProbe;
	Quat4d rotProbe;
	int tipMol = -1;
	std::string geometryFileName;
	fileWrapper* geometryFile = NULL;
	// ==== functions
	void setupApp( fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, std::string probeMolFileName );
	// ==== construnctor & destructor
	appModeTipMol(){};
	appModeTipMol( int numOfMoleculeInstances, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, graphInterface* graphics, abstractTip* tip, flagList *flags );
	~appModeTipMol();
};

// ================= appModeTipMol procedures =================

appModeTipMol::appModeTipMol( int numOfMoleculeInstances, fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile_, abstractSurf* surf, graphInterface* graphics, abstractTip* tip, flagList *flags )
: geometryFile( geometryFile_ ), posProbe( tip->probeMol->pos ), rotProbe( tip->probeMol->rot ), appModeTip( graphics, numOfMoleculeInstances, tip, flags ){
	setupApp( moleculeFiles, typeList, geometryFile, surf, tip->probeMol->fileName );
}

appModeTipMol::~appModeTipMol(){  // destructor
	if( abstractAppMode::world != NULL ) delete world;
//	abstractAppMode::graphics = NULL; // graphics is nothing, but a pointer to external graphics object
	if( abstractAppMode::molp != NULL ){
		for( int i = 0; i < abstractAppMode::numOfMoleculeInstances; i++ ){
			delete abstractAppMode::molp[i];
		}
		delete abstractAppMode::molp[abstractAppMode::numOfMoleculeInstances]; // delete also a probe molecule
		delete [] abstractAppMode::molp;
	}
	abstractAppMode::numOfMoleculeInstances = 0;
}

// setting up all relevant parameters
// lenght of an array listOfFileNames is assumed to be less than the maximum of an array listOfMoleculeInstances!!!
void appModeTipMol::setupApp( fileManager* moleculeFiles, AtomTypes* typeList, fileWrapper* geometryFile, abstractSurf* surf, std::string probeMolFileName ){
	abstractAppMode::molp = new MoleculeType*[abstractAppMode::numOfMoleculeInstances + 1];
	for( int i = 0; i < abstractAppMode::numOfMoleculeInstances; i++ ){
		if( abstractAppMode::graphics != NULL ){
			abstractAppMode::molp[i] = new MoleculeType( (*moleculeFiles)[i]->getFileName(), typeList, 0.5, abstractAppMode::graphics->viewMolList );
		} else {
			abstractAppMode::molp[i] = new MoleculeType( (*moleculeFiles)[i]->getFileName(), typeList, 0.5, NULL );
		}
	}
	if( graphics != NULL ){
		abstractAppMode::molp[numOfMoleculeInstances] = new MoleculeType( probeMolFileName.c_str(), typeList, 0.5, abstractAppMode::graphics->viewMolList ); // probe particle
	} else {
		abstractAppMode::molp[numOfMoleculeInstances] = new MoleculeType( probeMolFileName.c_str(), typeList, 0.5, NULL ); // probe particle
	}
	geometryFileName.assign( geometryFile->getFileName() );

	abstractAppMode::world = new PhysicalSystemEditor( geometryFile, abstractAppMode::numOfMoleculeInstances, abstractAppMode::molp, appModeTip::tip, surf );
	if( abstractAppMode::world == NULL )
		printf( "abstractAppMode: World cannot be allocated.\n" );
	if( abstractAppMode::graphics != NULL ) abstractAppMode::graphics->world = abstractAppMode::world;
	tipMol = abstractAppMode::world->nmols - 1;
	appModeTip::tip->probeMol->mol = tipMol;
	abstractAppMode::world->chectTipMolAtom();
}
