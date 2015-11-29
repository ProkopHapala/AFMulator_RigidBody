
#ifndef _PHYSICAL_SYSTEM_
#define _PHYSICAL_SYSTEM_

#include <string.h>
#include <stdio.h>

//#include "tipProcedures.h"

#define PI 3.1415926535

//class Screen; // forward declaration

class PhysicalSystem {
// a class encompassing all relevant molecules and pertaining methods

	public:

	// CONSTANTS AND VARIABLES
	
	// molecule geometry and forces
	
	int nmols;					// number of molecules in a system
	MoleculeType **molecules = NULL;		// molecules themselves
	Vec3d  *pos = NULL, *vpos = NULL, *fpos = NULL;	// positions of molecules, velocities and forces
	Quat4d *rot = NULL, *vrot = NULL, *frot = NULL;	// rotations of molecules, velocities and forces
	bool* notRigid = NULL;				// whether given molecule is subject to relaxation
	int nptmp;
	Vec3d  *Tps_i = NULL, *Tps_j = NULL;
	Vec3d  *fs_i  = NULL, *fs_j  = NULL;

	AtomTypes* atypeList = NULL;			// types of atoms present in molecules
	double *C6s = NULL, *C12s = NULL;		// coefficients for the Lennard-Jones potential

	OptimizerDerivs* optimizer = NULL;
	abstractTip* tip = NULL;
	abstractSurf* surf = NULL;
		
	// picked atom and molecule indices
	
	Vec3d*	atomToDrawOrigPos = NULL;		// position of a chosen atom at the moment of a mouse click, used to determine the plane in which the atom is moved by a mouse
	double distance;

	bool mouseTip = false;
		
	private:

	// picked atom and molecule indices and flags
	
	int	numPickedAtoms = 3;			// how many atoms form a complete selection
	int*	pickedAtoms = NULL;			// indices of atoms in a selection	
	int	pickedAtomCounter = 0;			// how many atoms are already picked to form a selection
	int	molToDrawPrev = -1;			// which molecule was chosen previously

	bool	atomHold = false;			// atomHold == true iff the mouse is holding an atom	
	int	atomToDraw = -1, molToDraw = -1;	// indices of atom and corresponding molecule which are to be picked by a mouse click
	bool	sysEvol = true;				// sysEvol == true iff the system undergoes evolution/relaxation

	bool	molHold  = false;			// molHold == true iff the mouse is holding a molecule
	float	atomscale	= 0.3;			// scale with which the atoms are rendered on the screen
	float	atomscaleEmph	= 0.7;			// scale with which the emphasized atoms are rendered on the screen
	
	// miscellaneous graphics
	
	GLuint normalID = 0;
	GLuint normID[3] = { 0, 0, 0 };
	int molToDrawNormal = -1;
	int* lines = NULL;

	public:
	
	// FUNCTIONS

	// initialization

	void initParams();
	void initTPoints();
	PhysicalSystem( char const* filename, int numOfMoleculeInstances, MoleculeType** molTypeList, bool raw, abstractTip* tip_, abstractSurf* surf_ );
	PhysicalSystem( fileWrapper* geometryFile, int numOfMoleculeInstances, MoleculeType** molTypeList, abstractTip* tip_, abstractSurf* surf_ );
	void makeFF();

	// destruction and reset
	
	~PhysicalSystem();
	void resetGeometry( char const* filename, Vec3d posProbe, Quat4d rotProbe, bool raw );
	void resetGeometry( fileWrapper* file, Vec3d posProbe, Quat4d rotProbe );

	// relaxation optimization 

	void forceFromPoints( int npoints, Vec3d* points, Vec3d* forces,  const Quat4d& q,  Vec3d& fp, Quat4d& fq );
	void forceFromPoints( Vec3d point, Vec3d force, const Quat4d& q, Vec3d& fp, Quat4d& fq );
	void forceFromPoints( int mol, int atom, Vec3d force );
	void forceFromPoints( Vec3d point, int mol, Vec3d force );
	
	void cleanPointForce( int npoints, Vec3d* forces );
	void rigidOptStep( double& pixelDataListItem );
	void forcesMolecules( bool surfaceForcesActive = true );
	void forcesTip( Vec3d& vec );
	void forcesTipAux( Vec3d centre, Vec3d Mvec, double mult, Vec3d force, GLuint id );

	// drawing and updating

	void draw();
	void drawAuxPoint();
	void update( bool& optimizingFlag, double& pixelDataListItem );
	void setSysEvol( bool sysEvol_ );
	bool getSysEvol();
	void mouseSetAuxPoint( float mouseA, float mouseB, Vec3d scCamRight, Vec3d scCamUp, Vec3d scCamDir );
	void adjustMolToTip();
	void chectTipMolAtom();
	
	// atom picking and moving
	
	void resetOneAtomMove();
	bool moveAtom( Vec3d systemCoords );
	bool moveMolecule( Vec3d systemCoords );
	bool rayPickAtomSet( const Vec3d& ray0, const Vec3d& hRay, bool emphasized );
	int  getMolToDraw();
	int  getAtomToDraw();
	void setAtomHold( bool atomHold_ );
	bool getAtomHold();
	void setMolHold( bool molHold_ );
	bool getMolHold();
	
	// selection of atoms
	
	bool addPickedAtom();
	bool addPickedAtomDrawLink();
	bool destroySelection();
	bool selectionChosen();	
			
	// additional actions
	
	void exportData();
	
	double distBetweenTwoAtoms( int indmol, int inda, int indb );
	Vec3d systemToMoleculeCoords( int mol, Vec3d systemCoords );
	Vec3d moleculeToSystemCoords( int mol, Vec3d systemCoords );
	void moleculeToSystemCoords( const Vec3d& pos, const Quat4d& rot, int npoints, Vec3d* points, Vec3d* Tpoints );
	Vec3d systemCoordsOfAtom( int mol, int atom );
	
	void setTemporaryAuxPoint( bool temporaryAuxPoint_ );
	bool getTemporaryAuxPoint();

	// molecule adjusting

	void adjustMolecule( char* end, double phi, Vec3d vec_pos, Vec3d vec_a, Vec3d vec_b );
	void adjustMoleculeSelection( char* end, Vec3d vec_pos, Vec3d vec_a, Vec3d vec_b );

	void setFromStdinAdjustMolecule();
	void setFromStdinAdjustMoleculeSelection();
	
	void adjustMoleculeSelectionXaxis();
	void adjustMoleculeSelectionYaxis();
	void adjustMoleculeSelectionZaxis();
	
	void adjustNormalAxis( Vec3d axis_one, Vec3d axis_two );
	void adjustNormalAxisMatrix( Vec3d axis_one, Vec3d axis_two );
	void rotateMolAxis( Vec3d a, Vec3d b );
	void rotateMolAxisNoGonio( Vec3d a, Vec3d b );	
	void adjustPosition( Vec3d vec_pos );

};

// ============== INITIALIZATION ==============

void PhysicalSystem::initParams(){
// optimization parameters

	int nparams = (3+4)*nmols; 
	optimizer   = new OptimizerFIRE( nparams, new double[nparams], new double[nparams], new double[nparams], NULL );
	//optimizer   = new OptimizerDerivs( nparams, new double[nparams], new double[nparams], new double[nparams], NULL );
	optimizer->dt = 0.05;
	optimizer->damping = 0.05;
	int irot = 3*nmols;
	pos   = (Vec3d* )&optimizer->pos[0];
	vpos  = (Vec3d* )&optimizer->vel[0];
	fpos  = (Vec3d* )&optimizer->force[0];
	rot   = (Quat4d*)&optimizer->pos[irot];
	vrot  = (Quat4d*)&optimizer->vel[irot]; 
	frot  = (Quat4d*)&optimizer->force[irot];
	
}

void PhysicalSystem::initTPoints(){
// point temporary

	nptmp = 0; 
	for( int i = 0; i < nmols; i++ ){
		if( molecules[i]->natoms > nptmp ){
			nptmp = molecules[i]->natoms;
		}
	}
	Tps_i = new Vec3d[nptmp]; 
	Tps_j = new Vec3d[nptmp];
	fs_i  = new Vec3d[nptmp]; 
	fs_j  = new Vec3d[nptmp];
	atypeList = molecules[0]->typeList;
}

PhysicalSystem::PhysicalSystem( fileWrapper* geometryFile, int numOfMoleculeInstances, MoleculeType** molTypeList, abstractTip* tip_, abstractSurf* surf_ ){

	geometryFile->openRead();
	if( !geometryFile->isOpened() ){
		printf( "PhysicalSystem: File %s cannot be opened. Default physical system created.\n", geometryFile->getFileName() );
		nmols		= 0;
		molecules	= NULL;
		tip		= NULL;	
		surf		= NULL;
	} else {
		printf( "PhysicalSystem:\t\tLoading physical system from\t%s.\n", geometryFile->getFileName() );

		int itype, count, notRigidAux, nmolsAux;
		Mat3d M;
		char str[600];
		bool probeMoleculePresent = false;

		if( tip_ != NULL && tip_->probeMol != NULL ) probeMoleculePresent = true;

		geometryFile->readLineToString( str );		
		count = sscanf( str, " %i", &nmolsAux );
		( probeMoleculePresent ) ? nmols = nmolsAux + 1 : nmols = nmolsAux; // extra molecule is the probe molecule
		printf( "PhysicalSystem:\t\tNumber of molecules: nmols = %i \n", nmols );
	
		molecules = new MoleculeType*[nmols];
		notRigid = new bool[nmols];
		initParams();
	
		// molTypeList is a list of (numOfMoleculeInstances) lists

		for( int i = 0; i < nmolsAux; i++ ){
			geometryFile->readLineToString( str );
			if( geometryFile->isRaw() ){
				count = sscanf( str, " %i	%lf %lf %lf	%lf %lf %lf %lf	%i", \
						&itype,	&pos[i].x, &pos[i].y, &pos[i].z,	&rot[i].x, &rot[i].y, &rot[i].z, &rot[i].w,	&notRigidAux );
			} else {
				count = sscanf( str, " %i	%lf %lf %lf	 %lf %lf %lf     %lf %lf %lf	%i", \
						&itype,	&pos[i].x, &pos[i].y, &pos[i].z,	&M.ax, &M.ay, &M.az,    &M.bx, &M.by, &M.bz,	&notRigidAux );
				M.a.normalize();	
				M.b.add_mul( M.a, -M.a.dot( M.b ) );
				M.b.normalize();
				M.c.set_cross( M.a, M.b );
				rot[i].fromMatrix( M );
				
				if( count < 11 ) notRigidAux = true;
			}
				
			notRigid[i] = notRigidAux; // not used notRigid[i] in sscanf directly since in that case Valgrind issues an error
			if( itype - 1 > numOfMoleculeInstances ){
				printf( "PhysicalSystem: Too little molecule instances. Set to the maximum possible value. \n" );
				itype = numOfMoleculeInstances;
			}
			molecules[i] = molTypeList[itype - 1];

		}
		
		// probe molecule
		if( probeMoleculePresent ){
			molecules[nmols - 1] = molTypeList[numOfMoleculeInstances];
			pos[nmols - 1].set( tip_->probeMol->pos );
			rot[nmols - 1].set( tip_->probeMol->rot );
			notRigid[nmols - 1] = true;
		}
	
//		CHECK(8)
	
		geometryFile->close();
	
		tip = tip_;
		surf = surf_;
//		if( tip != NULL ) tip->printTip();
//		surf->printSurf();
	
//		CHECK(7)
		initTPoints();	
		makeFF();
		
	}
	
}

PhysicalSystem::PhysicalSystem( char const* filename, int numOfMoleculeInstances, MoleculeType** molTypeList, bool raw, abstractTip* tip_, abstractSurf* surf_ ){

	FILE* pFile = fopen( filename, "r" );
	if( pFile == NULL ){
		printf( "PhysicalSystem: File %s cannot be opened. Default physical system created.\n", filename );
		nmols		= 0;
		molecules	= NULL;
		tip		= NULL;	
		surf		= NULL;
	} else {
		printf( "PhysicalSystem:\t\tLoading physical system from\t%s.\n", filename );

		int itype, count, notRigidAux, nmolsAux;
		Mat3d M;
		char str[600];
		bool probeMoleculePresent = true;

		if( !tip_->probeMol != NULL ) probeMoleculePresent = false;

		readLineComment( pFile, str );
		count = sscanf( str, " %i", &nmolsAux );
		( probeMoleculePresent ) ? nmols = nmolsAux + 1 : nmols = nmolsAux; // extra molecule is the probe molecule
		printf( "PhysicalSystem:\t\tNumber of molecules: nmols = %i \n", nmols );
	
		molecules = new MoleculeType*[nmols];
		notRigid = new bool[nmols];
		initParams();
	
		// molTypeList is a list of (numOfMoleculeInstances) lists

		CHECK(3)

		for( int i = 0; i < nmolsAux; i++ ){
			readLineComment( pFile, str );
		
			if( raw ){
				count = sscanf( str, " %i	%lf %lf %lf	%lf %lf %lf %lf	%i", \
						&itype,	&pos[i].x, &pos[i].y, &pos[i].z,	&rot[i].x, &rot[i].y, &rot[i].z, &rot[i].w,	&notRigidAux );
			} else {
				count = sscanf( str, " %i	%lf %lf %lf	 %lf %lf %lf     %lf %lf %lf	%i", \
						&itype,	&pos[i].x, &pos[i].y, &pos[i].z,	&M.ax, &M.ay, &M.az,    &M.bx, &M.by, &M.bz,	&notRigidAux );
				M.a.normalize();	
				M.b.add_mul( M.a, -M.a.dot( M.b ) );
				M.b.normalize();
				M.c.set_cross( M.a, M.b );
				rot[i].fromMatrix( M );
				
				if( count < 11 ) notRigidAux = true;
			}
				
			notRigid[i] = notRigidAux; // not used notRigid[i] in sscanf directly since in that case Valgrind issues an error
			if( itype - 1 > numOfMoleculeInstances ){
				printf( "PhysicalSystem: Too little molecule instances. Set to the maximum possible value. \n" );
				itype = numOfMoleculeInstances;
			}
			molecules[i] = molTypeList[itype - 1];

		}
		
		// probe molecule
		if( probeMoleculePresent ){
			molecules[nmols - 1] = molTypeList[numOfMoleculeInstances];
			pos[nmols - 1].set( tip_->probeMol->pos );
			rot[nmols - 1].set( tip_->probeMol->rot );
			notRigid[nmols - 1] = true;
		}
	
		fclose( pFile );
	
		tip = tip_;
		surf = surf_;
		
		CHECK(5)
		
		if( tip != NULL ) tip->printTip();
//		surf->printSurf();
	
		CHECK(4)
		
		initTPoints();	
		makeFF();
		
	}
	
}

void PhysicalSystem::makeFF(){

	makeLJparams( atypeList->ntypes, atypeList->vdwRs, atypeList->vdwEs, C6s, C12s );
	
}

// ============== DESTRUCTION AND RESET ==============

PhysicalSystem::~PhysicalSystem(){
// destructor

	// molecules and atypelist are objects living outside world, so they cannot be deleted in the world destructor

	delete [] pos;
	delete [] vpos;
	delete [] fpos;
	// rot is deleted by calling delete [] pos; and similarly for vrot and frot

	delete [] Tps_i;
	delete [] Tps_j;
	delete [] fs_i;
	delete [] fs_j;

	delete [] C6s;
	delete [] C12s;

	delete optimizer;

	delete [] notRigid;

	delete atomToDrawOrigPos;
	delete [] pickedAtoms;
	delete [] lines;

}

void PhysicalSystem::resetGeometry( fileWrapper* file, Vec3d posProbe, Quat4d rotProbe ){
// resets positions and orientation of molecules in the system, input file is the same as is used in the constructor
// assumes there is a probe molecule in the system

	if( !file->openRead() ){
		printf( "resetGeometry: File %s cannot be opened. Default geometry used instead.\n", file->getFileName() );
		nmols		= 0;
		molecules	= NULL;
		tip		= NULL;	
		surf		= NULL;	
	} else {
//		printf( "resetGeometry: Reseting geometry based on values from %s.\n", file->getFileName() );

		int aux, aux2, count;
		Mat3d M;
		char str[400];

		file->readLineToString( str );
		
		count = sscanf( str, " %i", &aux );
		initParams();

		for( int i = 0; i < nmols - 1; i++ ){
			file->readLineToString( str );
			if( file->isRaw() ){
				count = sscanf( str, "%i	%lf %lf %lf	%lf %lf %lf %lf	%i", \
					&aux,	&pos[i].x, &pos[i].y, &pos[i].z,	&rot[i].x, &rot[i].y, &rot[i].z, &rot[i].w,	&aux2 );
			} else {
				sscanf( str, "%i	%lf %lf %lf     %lf %lf %lf     %lf %lf %lf	%i", \
					&aux, &pos[i].x, &pos[i].y, &pos[i].z,    &M.ax, &M.ay, &M.az,    &M.bx, &M.by, &M.bz,	&aux2 );
				M.a.normalize();	
				M.b.add_mul( M.a, -M.a.dot( M.b ) );
				M.b.normalize();
				M.c.set_cross( M.a, M.b );
				rot[i].fromMatrix( M );			
			}
		}

		// probe molecule
		pos[nmols - 1].set( posProbe );
		rot[nmols - 1].set( rotProbe );
	
		file->close();
	}	
}

void PhysicalSystem::resetGeometry( char const* filename, Vec3d posProbe, Quat4d rotProbe, bool raw ){
// resets positions and orientation of molecules in the system, input file is the same as is used in the constructor
// assumes there is a probe molecule in the system

	FILE* pFile = fopen( filename, "r" );
	if( pFile == NULL ){
		printf( "resetGeometry: File %s cannot be opened. Default geometry used instead.\n", filename );
		nmols		= 0;
		molecules	= NULL;
		tip		= NULL;	
		surf		= NULL;
	} else {
//		printf( "resetGeometry: Reseting geometry based on values from %s.\n", filename );

		int aux, aux2, count;
		Mat3d M;
		char str[400];

		readLineComment( pFile, str );
		count = sscanf( str, " %i", &aux );
		initParams();

		for( int i = 0; i < nmols - 1; i++ ){
			readLineComment( pFile, str );
			
			if( raw ){
				count = sscanf( str, "%i	%lf %lf %lf	%lf %lf %lf %lf	%i", \
					&aux,	&pos[i].x, &pos[i].y, &pos[i].z,	&rot[i].x, &rot[i].y, &rot[i].z, &rot[i].w,	&aux2 );
			} else {
				fscanf( pFile, "%i	%lf %lf %lf     %lf %lf %lf     %lf %lf %lf	%i", \
					&aux, &pos[i].x, &pos[i].y, &pos[i].z,    &M.ax, &M.ay, &M.az,    &M.bx, &M.by, &M.bz,	&aux2 );
				M.a.normalize();	
				M.b.add_mul( M.a, -M.a.dot( M.b ) );
				M.b.normalize();
				M.c.set_cross( M.a, M.b );
				rot[i].fromMatrix( M );			
			}
		}

		// probe molecule
		pos[nmols - 1].set( posProbe );
		rot[nmols - 1].set( rotProbe );
	
		fclose( pFile );
	}	
}
	
// ============== RELAXATION OPTIMIZATION ==============

void PhysicalSystem::forceFromPoints( int npoints, Vec3d * points, Vec3d * forces, const Quat4d& q, Vec3d& fp, Quat4d& fq ){

	for( int i = 0; i < npoints; i++ ){
		q .addForceFromPoint( points[i], forces[i], fq ); 
		fp.add( forces[i] );
	}
}

void PhysicalSystem::forceFromPoints( Vec3d point, Vec3d force, const Quat4d& q, Vec3d& fp, Quat4d& fq ){

	q .addForceFromPoint( point, force, fq );
	fp.add( force );
}

void PhysicalSystem::forceFromPoints( Vec3d point, int mol, Vec3d force ){

	rot[mol].addForceFromPoint( point, force, frot[mol] );
	fpos[mol].add( force );
}

void PhysicalSystem::forceFromPoints( int mol, int atom, Vec3d force ){

	rot[mol].addForceFromPoint( molecules[mol]->xyzs[atom], force, frot[mol] );
	fpos[mol].add( force );
}

void PhysicalSystem::cleanPointForce( int npoints, Vec3d * forces ){
// set forces to zero

	for( int i = 0; i < npoints; i++ ){
		forces[i].set( 0.0 );
	}

}

void PhysicalSystem::rigidOptStep( double& pixelDataListItem ){
// one step of the relaxation
	
	// initialize everything associated with forces to zero
	for( int i = 0; i < nmols; i++ ){ 
		fpos[i].set( 0.0 );			// set all "forces positions" to zero
		frot[i].set( 0.0, 0.0, 0.0, 0.0 );	// set all "forces rotations" to zero
		rot[i].normalize();			// keep quaternion normalized, otherwise unstable !!!
	}
	
	Vec3d vec;

	// calculate forces
	forcesMolecules();
	if( tip != NULL ){
		forcesTip( vec );
	}
	pixelDataListItem = vec.z;
	
	// in frot[i] left only its part which is perpendicular to rot[i]
	for( int i = 0; i < nmols; i++ ){ 
		double qfq = rot[i].dot( frot[i] );	// projection of frot[i] onto rot[i]
		frot[i].add_mul( rot[i], -qfq );	// from frot[i] subtract its part parallel to rot[i]
	} 
	 
	// update positions and velocities of molecules
	optimizer->move();
}

void PhysicalSystem::forcesMolecules( bool surfaceForcesActive ){
// calculate forces, no auxiliary point is taken into account
	
	// for all molecules...
	for( int i = 0; i < nmols; i++ ){
	
		// i-th molecule
		MoleculeType* moli = molecules[i];				// in moli store i-th molecule
		int npi = moli->natoms;						// npi is a number of atoms in i-th molecule
		moleculeToSystemCoords( pos[i], rot[i], npi, moli->xyzs, Tps_i );	// to Tps_i store system coordinates of i-th molecule's atom positions
		cleanPointForce( npi, fs_i );					// initialize fs_i to zeros
	
		// ...consider all preceding molecules
		for( int j = 0; j < i; j++ ){
		
			// j-th molecule
			MoleculeType* molj = molecules[j];				// in molj store j-th molecule
			int npj = molj->natoms;						// npj is a number of atoms in j-th molecule
			moleculeToSystemCoords( pos[j], rot[j], npj, molj->xyzs, Tps_j );	// to Tps_j store system coordinates of j-th molecule's atom positions
			cleanPointForce( npj, fs_j );					// initialize fs_j to zeros
						
			// calculate forces -- Lennard-Jones plus electrostatic
			// forces are stored in fs_i and fs_j for i-th and j-th molecule, respectively
			interMolForceLJE( 
				npi, moli->atypes, moli->Qs, Tps_i, fs_i, 
				npj, molj->atypes, molj->Qs, Tps_j, fs_j,
				atypeList->ntypes, C6s, C12s
			);
			
			// update orientation frot and forces fpos for j-th molecule
			if( notRigid[j] ) forceFromPoints( npj, molj->xyzs, fs_j, rot[j], fpos[j], frot[j] );
		}

		// calculate force between the i-th molecule and the surface
		if( surfaceForcesActive ){
			surf->forceMolSurfSurf( npi, Tps_i, fs_i ); // !!!
		}
		
		// update orientation rot and forces fpos for i-th molecule
		if( notRigid[i] ) forceFromPoints( npi, moli->xyzs, fs_i, rot[i], fpos[i], frot[i] );
	}
	
}

void PhysicalSystem::forcesTipAux( Vec3d centre, Vec3d Mvec, double mult, Vec3d force, GLuint id ){
	
	Vec3d Mcaux, vecaux, vecaux2;
	
	Mcaux.set( centre ); // !!!
	Mcaux.add_mul( Mvec, mult ); // !!!
	vecaux = systemToMoleculeCoords( tip->probeMol->mol, Mcaux );
	vecaux2.set_mul( force, 1.0 / mult ); // !!!
//	drawCoordAxesRot( Mcaux, vecaux2, vec_zero, vec_zero, id, 5 );
	forceFromPoints( vecaux, tip->probeMol->mol, vecaux2 );

}

void PhysicalSystem::forcesTip( Vec3d& vec ){
// calculate forces between the tip and a single atom atomToDraw_ in molecule molToDraw_, tip_sys are system coordinates of the tip

	double mult = 1;		
	Vec3d sysCoordAtom;
	Vec3d centre, force;

	molToDrawNormal = tip->probeMol->mol;	
	
	sysCoordAtom = systemCoordsOfAtom( tip->probeMol->mol, tip->probeMol->atom );	
	centre = moleculeToSystemCoords( tip->probeMol->mol, tip->probeMol->centreMol );

	// forces to auxiliary point
	force = tip->getForceRad( sysCoordAtom, vec );
	forceFromPoints( tip->probeMol->mol, tip->probeMol->atom, force );	
	
	// draw axes
	Vec3d vec_zero;
	vec_zero.set( 0, 0, 0 );
	drawCoordAxesRot( centre, force, vec_zero, vec_zero, normID[1], 5 );

//	printf( "tip->tip->probeMol->mol = %i, tip->probeMol->atom = %i\n", tip->tip->probeMol->mol, tip->probeMol->atom );

	if( molecules[tip->probeMol->mol]->natoms > 1 ){
	
		Vec3d vec2, vec3, force2, force3;
		tip->getForceOrient( rot[tip->probeMol->mol], force2, force3, vec2, vec3 );
		
		forcesTipAux( centre, vec2,  mult, force2, 0 );
		forcesTipAux( centre, vec2, -mult, force2, 0 );

		forcesTipAux( centre, vec3,  mult, force3, 0 );
		forcesTipAux( centre, vec3, -mult, force3, 0 );
	}
}

// ============== DRAWING AND UPDATING ==============

void PhysicalSystem::draw(){
// draw the system

	Uint32 atomColor;
	Mat3d rotmat;
	float glMat[4*4];
				
	// for all molecules in the system...
	for( int i = 0; i < nmols; i++ ){
					
		// if the molecule is to be drawn...
		if( molecules[i]->viewlist > 0 ){
					
			// rotmat stores the rotation of a molecule w.r.t. the system
			rot[i].toMatrix( rotmat );			
			toGLMat( pos[i], rotmat, glMat );
			
			glPushMatrix();
			glMultMatrixf( glMat );
	
			// draw the molecule
			glCallList( molecules[i]->viewlist );
		
			// draw lines connecting chosen atoms
			if( pickedAtomCounter > 1 && molToDraw == i && molToDrawNormal == i ){
			
				glPushAttrib( GL_LINE_BIT );
				glDisable( GL_LIGHTING );
				glLineWidth( 5 );
				for( int j = 0; j < pickedAtomCounter - 1; j++ ){
					glCallList( lines[j] );
				}
				glEnable( GL_LIGHTING );
				glPopAttrib();
				
			}

			// if an atom is picked, draw it
			if( atomHold && molToDraw == i ){			
				atomColor = molecules[i]->typeList->colors[ molecules[i]->atypes[atomToDraw] ];
				molecules[i]->drawAtom( atomToDraw, 5, atomscaleEmph, atomColor );
			}
			
			// if the selection of atoms exists, draw it
			if( pickedAtoms != NULL && molToDrawPrev == i ){
				for( int j = 0; j < pickedAtomCounter; j++ ){
					atomColor = molecules[molToDrawPrev]->typeList->colors[ molecules[molToDrawPrev]->atypes[pickedAtoms[j]] ];
					molecules[molToDrawPrev]->drawAtom( pickedAtoms[j], 5, atomscaleEmph, atomColor );
				}
			}
			
			glPopMatrix();
			
//			// draw the normal, if it exists
			glDisable( GL_LIGHTING );
//			printf( "molToDrawNormal = %i\n", molToDrawNormal );
			if( molToDrawNormal == i && normalID != 0 ){
				glCallList( normalID );
			}
			if( molToDrawNormal == i && tip != NULL ){
				for( int i = 0; i < 3; i++ ){
					if( normID[i] ){
						glCallList( normID[i] );
					}
				}
			}

			glEnable( GL_LIGHTING );
			
		}
	}
	
	// draw permanent auxiliary point			
	if( tip != NULL && tip->probeMol != NULL ){
		Vec3d pos1;
		pos1 = moleculeToSystemCoords( tip->probeMol->mol, molecules[tip->probeMol->mol]->xyzs[tip->probeMol->atom] );
		double rad = 0.5*(molecules[tip->probeMol->mol]->typeList->vdwRs[molecules[tip->probeMol->mol]->atypes[tip->probeMol->atom]]);
		
		tip->renderTip( pos1, rad );
		tip->drawTip();
	}

}

void PhysicalSystem::update( bool& optimizingFlag, double& pixelDataListItem ){
// update the relaxation

	if( optimizingFlag ){

		double fmax;
		int perFrame = 1;
		
		for( int i = 0; i < perFrame; i++ ){
		
			rigidOptStep( pixelDataListItem );
			fmax = optimizer->getFmaxAbs();
			
//			printf( "fmax = %f\n", fmax );
			
			if( fmax < 1e-4 ){
				optimizingFlag = false;
//				printf( "Converged in %i !!!, Fmax = %f \n", optimizer->stepsDone, fmax );
				break;
			}
		}
//		printf( " %i fmax %f\n",  optimizer->stepsDone, fmax );
	}
}

void PhysicalSystem::setSysEvol( bool sysEvol_ ){
// set sysEvol

	sysEvol = sysEvol_;
	
}

bool PhysicalSystem::getSysEvol(){
// get sysEvol

	return sysEvol;
	
}
	
void PhysicalSystem::mouseSetAuxPoint( float mouseA, float mouseB, Vec3d scCamRight, Vec3d scCamUp, Vec3d scCamDir ){
// move a picked molecule where the mouse is

	Vec3d atomSystemCoords, atomMolCoords, atomInSystemPos;
	float z_coord;

	if( !mouseTip ) return;

	if( molToDraw == -1 ){
		printf( "mouseSetAuxPoint: Invalid molToDraw.\n" );
		return;
	}

	// in atomInSystemPos store the original position of the picked atom expressed in system coordinates
	atomInSystemPos = moleculeToSystemCoords( molToDraw, *atomToDrawOrigPos );
 
	// get the z-coordinate of atomInSystemPos in camera coordinates
	z_coord = scCamDir.x*atomInSystemPos.x + scCamDir.y*atomInSystemPos.y + scCamDir.z*atomInSystemPos.z;
	
	// in atomSystemCoords store a new position of the moved atom expressed in system coordinates
	// z-coordinate is chosen in such a way that moving of the molecule by mouse is done in a plane parallel to the screen
	atomSystemCoords.set_lincomb( mouseA, mouseB, z_coord, scCamRight, scCamUp, scCamDir );

	// update an auxiliary point position	
	if( tip != NULL ){
		tip->setPosition( atomSystemCoords );
		if( tip->probeMol != NULL ){
			tip->probeMol->atom = atomToDraw;
			tip->probeMol->mol = molToDraw;
		}

	}
	
}

void PhysicalSystem::chectTipMolAtom(){
// set a permanent auxiliary point

	if( tip == NULL || tip->probeMol == NULL ){
		printf( "chectTipMolAtom: Invalid tip. Halt.\n" );
		return;
	} 

	if( tip->probeMol->mol >= nmols ){
		printf( "chectTipMolAtom: Probe molecule index too big.\n" );
		tip->probeMol->mol = nmols - 1;
	}
	
	if( tip->probeMol->atom >= molecules[tip->probeMol->mol]->natoms ){
		printf( "chectTipMolAtom: Probe atom index too big.\n" );
		tip->probeMol->atom = molecules[tip->probeMol->mol]->natoms - 1;
	}

}

void PhysicalSystem::adjustMolToTip(){
// move aux_molToDraw_orig so as its aux_atomToDraw_orig-th atom lies in auxiliary point aux_force_point

	if( tip == NULL || tip->probeMol == NULL ){
		printf( "adjustMolToTip: No tip. Halt.\n" );
		return;
	} 
	if( tip->probeMol->mol == -1 ){
		printf( "adjustMolToTip: Invalid mol.\n" );
		return;
	}
	if( tip->probeMol->atom == -1 ){
		printf( "adjustMolToTip: Invalid atom.\n" );
		return;
	}

	Vec3d atomPos, vec_diff;
	atomPos = moleculeToSystemCoords( tip->probeMol->mol, molecules[tip->probeMol->mol]->xyzs[tip->probeMol->atom] );
	vec_diff.set_sub( tip->pos, atomPos );
	pos[tip->probeMol->mol].add( vec_diff );

}

// ============== ATOM PICKING AND MOVING ==============

void PhysicalSystem::resetOneAtomMove(){						
// go one step backward, i.e. reset the last moving of an atom
		
	if( atomToDrawOrigPos != NULL && molToDraw != -1 && atomToDraw != -1 ){
		(molecules[molToDraw]->xyzs[atomToDraw]).set( *atomToDrawOrigPos );
		molecules[molToDraw]->redrawMol();
	}
}

bool PhysicalSystem::moveAtom( Vec3d systemCoords ){
// if a picked atom is moved, set the new position of the atom
			
	if( !atomHold ){
		return false;
	}
		
	Vec3d atomCoords;
	atomCoords = systemToMoleculeCoords( molToDraw, systemCoords );
		
	molecules[molToDraw]->xyzs[atomToDraw].set( atomCoords );
	molecules[molToDraw]->redrawMol();
	return true;
}

bool PhysicalSystem::moveMolecule( Vec3d systemCoords ){
// if a picked molecule is moved, set the new position of the molecule
			
	if( !molHold ){
		return false;
	}
		
	Vec3d molCoords;
	 	
	pos[molToDraw].set( molCoords );
	molecules[molToDraw]->redrawMol();
	return true;
}

bool PhysicalSystem::rayPickAtomSet( const Vec3d& ray0, const Vec3d& hRay, bool emphasized ){
// if a mouse is clicked over the atom, choose this atom

	int atomToDrawAux = -1; // in order to keep drawing a ball when no atom is selected, but mouse clicked
	bool found = false;
	Vec3d* xyzs_aux;
	float atomscaleLoc;
	float distMin = INFINITY, dist;
	
	// loop over all molecules in the system
	for( int i = 0; i < nmols; i++ ){
	
		// in xyzs_aux store positions of atoms in the molecule expressed in the system coordinates
		xyzs_aux = new Vec3d[molecules[i]->natoms];
		moleculeToSystemCoords( pos[i], rot[i], molecules[i]->natoms, molecules[i]->xyzs, xyzs_aux );

		// find an atom over which the mouse is clicked
//		atomToDrawAux = molecules[i]->rayPickAtom_sphereAlt( ray0, hRay, xyzs_aux, atomscaleLoc, dist );
		atomToDrawAux = molecules[i]->rayPickAtom_sphereAlt( ray0, hRay, xyzs_aux, atomscale, atomscaleEmph, pickedAtomCounter, pickedAtoms, dist );
		
		// if the mouse is clicked over an actual atom...
		if( dist < distMin ){
			distMin = dist;		
			// store the clicked atom and molecule indices
			atomToDraw = atomToDrawAux;
			molToDraw = i;
		}
		
		// atom positions in system coords are no longer needed
		delete [] xyzs_aux;
	}
	
	// if no atom has been chosen by mouse...
	if( distMin == INFINITY ){
		molToDraw  = -1;
		atomToDraw = -1;	
	} else {			
		// store position of the clicked atom at the moment of the clicking
		if( atomToDrawOrigPos == NULL){
			atomToDrawOrigPos = new Vec3d;
		}
		atomToDrawOrigPos->set( molecules[molToDraw]->xyzs[atomToDraw] );
	
		// set flag
		found = true;		
	}
		
	printf( "rayPickAtomSet: Picked atom no. %i in molecule no. %i.\n", atomToDraw, molToDraw );
		
	return found;
}

int PhysicalSystem::getMolToDraw(){
// returns an index of a chosen molecule
	
	return molToDraw;

}

int PhysicalSystem::getAtomToDraw(){
// returns an index of a chosen atom

	return atomToDraw;
	
}

void PhysicalSystem::setAtomHold( bool atomHold_ ){
// set atomHold

	atomHold = atomHold_;

}

bool PhysicalSystem::getAtomHold(){
// return atomHold

	return atomHold;
	
}

void PhysicalSystem::setMolHold( bool molHold_ ){
// set molHold

	molHold = molHold_;

}

bool PhysicalSystem::getMolHold(){
// return molHold

	return molHold;
	
}

// ============== SELECTION OF ATOMS ==============

bool PhysicalSystem::addPickedAtom(){
// if an atom is chosen, add it to the selection
	
	if( pickedAtomCounter == 0 ){
		molToDrawPrev = molToDraw;
		pickedAtoms = new int[numPickedAtoms];
		pickedAtoms[pickedAtomCounter] = atomToDraw;
		pickedAtomCounter++;
		return true;
	}
	
	if( (molToDrawPrev != molToDraw) || (pickedAtomCounter > numPickedAtoms - 1) ){
		pickedAtomCounter = 0;
		molToDrawPrev = molToDraw;
		pickedAtoms[pickedAtomCounter] = atomToDraw;
		pickedAtomCounter++;
		return false;
	}
	
	pickedAtoms[pickedAtomCounter] = atomToDraw;
	pickedAtomCounter++;
	return true;
}

double PhysicalSystem::distBetweenTwoAtoms( int indmol, int inda, int indb ){
// return a distance between inda-th atom and indb-th atom of the indmol-th molecule

	Vec3d diff;	
	diff.set_sub( molecules[indmol]->xyzs[inda], molecules[indmol]->xyzs[indb] );
	diff = moleculeToSystemCoords( indmol, diff );

	return diff.norm();
}

bool PhysicalSystem::addPickedAtomDrawLink(){
// if an atom is chosen, add it to the selection
// returns true iff there are at least two atoms in the selection, simultaneously calculates the distance between the first atom and the current atom in the selection
	
	if( pickedAtomCounter == 0 ){
		pickedAtoms = new int[numPickedAtoms];
		lines 	    = new int[numPickedAtoms - 1];
		
		molToDrawPrev   = molToDraw;
		molToDrawNormal = molToDraw;
		pickedAtoms[pickedAtomCounter] = atomToDraw;
		pickedAtomCounter++;
		
//		return true;
		return false;
	}
	
	if( (molToDrawPrev != molToDraw) || (pickedAtomCounter >= numPickedAtoms) ){
		
		for( int i = 0; i < pickedAtomCounter - 1; i++){
			glDeleteLists( lines[i], 1 );
		}		
		pickedAtomCounter = 0;
		
		molToDrawPrev = molToDraw;
		molToDrawNormal = molToDraw;
		pickedAtoms[pickedAtomCounter] = atomToDraw;
		pickedAtomCounter++;
		
		return false;
	}
	
	pickedAtoms[pickedAtomCounter] = atomToDraw;
	
	distance = distBetweenTwoAtoms( molToDraw, pickedAtoms[pickedAtomCounter], pickedAtoms[0] ); // !!!
	
	lines[pickedAtomCounter - 1] = glGenLists( 1 );
//	printf( "pickedAtomCounter = %i\n", pickedAtomCounter );

//	glPushAttrib( GL_LINE_BIT );
//	glLineWidth( 3 );

	float color[] = { 0, 0, 0 }; // black
	
	switch( pickedAtomCounter ){
		case 1: // yellow
			color[0] = 1;
			color[1] = 1;
			break;
		case 2: // magenta
			color[0] = 1;
			color[2] = 1;
			break;
		case 3: // cyan
			color[1] = 1;
			color[2] = 1;
			break;
		default:
			break;
	
	}
	
	glNewList( lines[pickedAtomCounter - 1], GL_COMPILE );
	glBegin( GL_LINES );
		glColor3fv( color );
		glLineWidth( 10 );
		glVertex3f( 
			molecules[molToDraw]->xyzs[pickedAtoms[0]].x,
			molecules[molToDraw]->xyzs[pickedAtoms[0]].y,
			molecules[molToDraw]->xyzs[pickedAtoms[0]].z
		 );
		glVertex3f( 
			molecules[molToDraw]->xyzs[pickedAtoms[pickedAtomCounter]].x,
			molecules[molToDraw]->xyzs[pickedAtoms[pickedAtomCounter]].y,
			molecules[molToDraw]->xyzs[pickedAtoms[pickedAtomCounter]].z
		 );

	glEnd();
	glEndList();
	
//	glPopAttrib();
	
	pickedAtomCounter++;
	return true;
}

bool PhysicalSystem::destroySelection(){
// destroy selection of atoms
		
	if( pickedAtoms == NULL ){
		return false;
	}
	
	delete [] pickedAtoms;
	pickedAtoms = NULL;
	
	if( lines == NULL ){
		return false;
	}
	
	delete [] lines;
	lines = NULL;
	
	pickedAtomCounter = 0;
	molToDrawPrev = -1;
	return true;
}

bool PhysicalSystem::selectionChosen(){
// is selection complete and chosen by mouse?

	int i;

	if( pickedAtomCounter < numPickedAtoms ){
		return false;
	}
	
	if( molToDrawPrev != molToDraw ){
		return false;
	}

	for( i = 0; i < numPickedAtoms; i++ ){
		if( pickedAtoms[i] == atomToDraw ){
			break;
		}
	}
	
	if( i == numPickedAtoms ){
		return false;
	}
	
	return true;
}

// ============== ADDITIONAL ACTIONS ==============

void PhysicalSystem::exportData(){
// export data about molecules to the file
	
	char filename[] = "exportedData.dat";	

	FILE* file = fopen( filename, "w" );
	if( file == NULL ){
		printf( "exportData: File cannot be opened. No data export.\n" );
		return;
	}
	
	printf( "exportData: Data export into the file %s.", filename );
	
	fprintf( file, "%i\n", nmols );
	
	for( int i = 0; i < nmols; i++ ){
		fprintf( file, "%i\t%lf %lf %lf\t", i + 1, pos[i].x, pos[i].y, pos[i].z );
		fprintf( file, "%lf %lf %lf %lf\t", rot[i].x, rot[i].y, rot[i].z, rot[i].w );
		fprintf( file, "%i\n", notRigid[i] );
	}	
		
	fclose( file );
}

Vec3d PhysicalSystem::systemToMoleculeCoords( int mol, Vec3d systemCoords ){
// takes vector of system coordinates and express the same point in a molecule coordinates corresponding to molecule mol

	Mat3d mat;
	Vec3d molCoordsAux, molCoords;		
		
	rot[mol].toMatrix( mat );		
	
	molCoordsAux.set_sub( systemCoords, pos[mol] );	
	mat.dot_to( molCoordsAux, molCoords );
	
	return molCoords;

}

Vec3d PhysicalSystem::moleculeToSystemCoords( int mol, Vec3d molCoords ){
// express molecule coordinates stored in molCoords into system coordinates which are returned
// molCoords are coordinates in mol-th molecule coordinate system

	Mat3d mat;
	Vec3d systemCoordsAux, systemCoords;
	
	rot[mol].toMatrix( mat );
	
	mat.dot_to_T( molCoords, systemCoordsAux );
	systemCoords.set_add( pos[mol], systemCoordsAux ); 
	
	return systemCoords;
	
}

Vec3d PhysicalSystem::systemCoordsOfAtom( int mol, int atom ){
// express molecule coordinates stored in molCoords into system coordinates which are returned
// molCoords are coordinates in mol-th molecule coordinate system

	return moleculeToSystemCoords( mol, molecules[mol]->xyzs[atom] );
	
}

void PhysicalSystem::moleculeToSystemCoords( const Vec3d& pos, const Quat4d& rot, int npoints, Vec3d* points, Vec3d* Tpoints ){
// express molecule coordinates stored in points into system coordinates which are stored into Tpoints
// pos and rot are a position and an orientation of a given molecule w.r.t. the system, respectively
// npoints is a number of points

	Mat3d T;
	rot.toMatrix( T );
	
	for( int i = 0; i < npoints; i++ ){ 
		Vec3d Tp;
		T.dot_to_T( points[i], Tp ); 
		Tpoints[i].set_add( pos, Tp ); 
	}
	
}

// ============== MOLECULE ADJUSTING ==============

void PhysicalSystem::setFromStdinAdjustMolecule(){
// adjustMolecule procedure with user interface

	double x, y, z, phi;
	char end[40];
	Vec3d vec_pos, vec_a, vec_b;
	
	printf( "Molecule position specification by coordinate system: (y/i/n)? (i = yes, but use incrementation) " );
	scanf( "%s", end );
	if( !strcmp( end, "n" ) || !strcmp( end, "N" ) ) return;
		
	printf( "Specify the position of the molecule: " );
	scanf( "%lf %lf %lf", &x, &y, &z );
	vec_pos.set( x, y, z );	

	printf( "Specify the first axis: " );
	scanf( "%lf %lf %lf", &x, &y, &z );
	vec_a.set( x, y, z );

	printf( "Specify the second axis: " );
	scanf( "%lf %lf %lf", &x, &y, &z );
	vec_b.set( x, y, z );
	
	printf( "Specify the angle in degrees: " );
	scanf( "%lf", &phi );
	
	phi = phi * (2*PI) / 360.0 ; // from degrees to radians
	
//	printf( "phi = %f\n", phi );
	
	adjustMolecule( end, phi, vec_pos, vec_a, vec_b );
	
}

void PhysicalSystem::adjustMolecule( char* end, double phi, Vec3d vec_pos, Vec3d vec_a, Vec3d vec_b ){
// based on user input adjust position and rotation of the chosen molecule
// molecule coordinate system is used to adjust the molecule appropriately

	printf( "adjustMolecule()\n" );
	if( molToDraw == -1 ){
		printf( "adjustMolecule: No molecule chosen.\n" );
		return;
	}

	Vec3d normal;
	Quat4d auxq;
	
	// set position of the molecule
	if( !strcmp( end, "i" ) ){
		// relative setting -- increment
		pos[molToDraw].add( vec_pos );
	} else {
		// absolute setting
		pos[molToDraw].set( vec_pos );
	}
	
	// plane normal, the plane is specified by vec_a and vec_b
	vec_a.normalize();
	vec_b.normalize();
	normal.set_cross( vec_a, vec_b );
	
	// if a normal is a zero vector, do nothing
	if( normal.norm2() < 1e-4 ){	
		printf( "adjustMolecule: Invalid axes. Original orientation setting used.\n" );
		return;		
	} else {
		// set rotation of the molecule
		normal.normalize();
		auxq.fromAngleAxis( phi, normal );
		
		if( !strcmp( end, "i" ) ){
			// relative setting -- increment
			rot[molToDraw].qmul( auxq );		
		} else {
			// absolute setting
			rot[molToDraw].set( auxq );
		}
		
		printf( "adjustMolecule: New orientation set.\n" );
//		printf( "%f %f %f %f\n", rot[molToDraw].x, rot[molToDraw].y, rot[molToDraw].z, rot[molToDraw].w );
	}

}

void PhysicalSystem::setFromStdinAdjustMoleculeSelection(){
// adjustMoleculeSelection procedure with user interface

	double x, y, z;
	char end[10];
	Vec3d vec_pos, vec_a, vec_b;

	printf( "Molecule position specification by coordinate system: (y/i/n)?\n (i = yes, but use incrementation for position) " );
	scanf( "%s", end );
	if( !strcmp( end, "n" ) || !strcmp( end, "N" ) ) return;
		
	printf( "Specify the first axis:  " );
	scanf( "%lf %lf %lf", &x, &y, &z );
	vec_a.set( x, y, z );

	printf( "Specify the second axis: " );
	scanf( "%lf %lf %lf", &x, &y, &z );
	vec_b.set( x, y, z );

	printf( "Specify the position of the molecule: " );
	scanf( "%lf %lf %lf", &x, &y, &z );
	vec_pos.set( x, y, z );	

	adjustMoleculeSelection( end, vec_pos, vec_a, vec_b );

}

void PhysicalSystem::adjustMoleculeSelection( char* end, Vec3d vec_pos, Vec3d vec_a, Vec3d vec_b ){
// orientate molecule molToDraw such that it is perpendicular to vec_a and aligned with vec_b
// for details see docstrings for adjustNormalAxis and adjustPosition

	Vec3d vec_pos_aux;

	printf( "adjustMoleculeSelection()\n" );
	if( molToDraw == -1 ){
		printf( "adjustMoleculeSelection: No molecule chosen.\n" );
		return;
	}
			
	if( pickedAtoms == NULL ){
		printf( "adjustMoleculeSelection: No selection of atoms.\n" );
		return;
	}
	
	if( pickedAtomCounter < 3 ){
		printf( "adjustMoleculeSelection: Selection of atoms is incomplete.\n" );
		return;
	}	

//	moleculeToSystemCoords( pos[molToDraw], rot[molToDraw], 1, &molecules[molToDraw]->xyzs[pickedAtoms[0]], &vec_pos_aux );	
	vec_pos_aux = moleculeToSystemCoords( molToDraw, molecules[molToDraw]->xyzs[pickedAtoms[0]] );	
	adjustNormalAxis( vec_a , vec_b );
	
	// if increment mode...
	if( !strcmp( end, "i" ) ){
		vec_pos.add( vec_pos_aux );
	}	
	
	adjustPosition( vec_pos );

}

void PhysicalSystem::adjustPosition( Vec3d vec_pos ){
// assumes that at least one atom is selected
// moves molecule molToDraw so that position of the selected atom coincides with vec_pos

	Vec3d vec_diff, vec_pos_aux;

	// express the first picked atom position in a system coordinates
//	moleculeToSystemCoords( pos[molToDraw], rot[molToDraw], 1, &molecules[molToDraw]->xyzs[pickedAtoms[0]], &vec_pos_aux );
	vec_pos_aux = moleculeToSystemCoords( molToDraw, molecules[molToDraw]->xyzs[pickedAtoms[0]] );
	
	// find translation vector between the position of the atom and the specified position
	vec_diff.set_sub( vec_pos, vec_pos_aux );

	// set new position of the molecule
	pos[molToDraw].add( vec_diff );

}

void PhysicalSystem::adjustMoleculeSelectionXaxis(){
// orientate molecule molToDraw such that it is perpendicular to x-axis
// for details see docstring for adjustNormalAxis

	printf( "adjustMoleculeSelectionXaxis()\n" );
	if( molToDraw == -1 ){
		printf( "adjustMoleculeSelectionXaxis: No molecule chosen.\n" );
		return;
	}
			
	if( pickedAtoms == NULL ){
		printf( "adjustMoleculeSelectionXaxis: No selection of atoms.\n" );
		return;
	}
	
	if( pickedAtomCounter < 3 ){
		printf( "adjustMoleculeSelectionXaxis: Selection of atoms is incomplete.\n" );
		return;
	}	
	
	Vec3d xAxis, yAxis;
	
	xAxis.set( 1, 0, 0 ); // x-axis
	yAxis.set( 0, 1, 0 ); // y-axis

//	adjustNormalAxis( xAxis, yAxis );
	adjustNormalAxisMatrix( xAxis, yAxis );

}

void PhysicalSystem::adjustMoleculeSelectionYaxis(){
// orientate molecule molToDraw such that it is perpendicular to y-axis
// for details see docstring for adjustNormalAxis

	printf( "adjustMoleculeSelectionYaxis()\n" );
	if( molToDraw == -1 ){
		printf( "adjustMoleculeSelectionYaxis: No molecule chosen.\n" );
		return;
	}
			
	if( pickedAtoms == NULL ){
		printf( "adjustMoleculeSelectionYaxis: No selection of atoms.\n" );
		return;
	}
	
	if( pickedAtomCounter < 3 ){
		printf( "adjustMoleculeSelectionYaxis: Selection of atoms is incomplete.\n" );
		return;
	}	
	
	Vec3d yAxis, zAxis;
	
	yAxis.set( 0, 1, 0 ); // y-axis
	zAxis.set( 0, 0, 1 ); // z-axis

//	adjustNormalAxis( yAxis, zAxis );
	adjustNormalAxisMatrix( yAxis, zAxis );

}

void PhysicalSystem::adjustMoleculeSelectionZaxis(){
// orientate molecule molToDraw such that it is perpendicular to y-axis
// for details see docstring for adjustNormalAxis

	printf( "adjustMoleculeSelectionZaxis()\n" );
	if( molToDraw == -1 ){
		printf( "adjustMoleculeSelectionZaxis: No molecule chosen.\n" );
		return;
	}
			
	if( pickedAtoms == NULL ){
		printf( "adjustMoleculeSelectionZaxis: No selection of atoms.\n" );
		return;
	}
	
	if( pickedAtomCounter < 3 ){
		printf( "adjustMoleculeSelectionZaxis: Selection of atoms is incomplete.\n" );
		return;
	}
	
	Vec3d xAxis, zAxis;
	
	xAxis.set( 1, 0, 0 ); // x-axis
	zAxis.set( 0, 0, 1 ); // z-axis

//	adjustNormalAxis( zAxis, xAxis );
	adjustNormalAxisMatrix( zAxis, xAxis );

}

void PhysicalSystem::adjustNormalAxis( Vec3d axis_one, Vec3d axis_two ){
// assumes there is a selection of (at least) three atoms pickedAtoms; their positions define a plane
// adjustNormalAxis orientates molecule molToDraw such that the plane is perpendicular to axis_one and first two atoms lie in axis_two

	Vec3d normal_sys, vec_10_sys, vec_20_sys, axis_two_proj;
	Vec3d pos_0_mol, pos_1_mol, pos_2_mol;
	Vec3d pos_0_sys, pos_1_sys, pos_2_sys;
	
	// get positions of (first three) atoms in a selection
	pos_0_mol.set( molecules[molToDraw]->xyzs[pickedAtoms[0]] );
	pos_1_mol.set( molecules[molToDraw]->xyzs[pickedAtoms[1]] );
	pos_2_mol.set( molecules[molToDraw]->xyzs[pickedAtoms[2]] );

	// express these positions in a system coordinates
	pos_0_sys = moleculeToSystemCoords( molToDraw, pos_0_mol );
	pos_1_sys = moleculeToSystemCoords( molToDraw, pos_1_mol );
	pos_2_sys = moleculeToSystemCoords( molToDraw, pos_2_mol );
	
	// get vectors connecting the positions
	vec_10_sys.set_sub( pos_1_sys, pos_0_sys );
	vec_20_sys.set_sub( pos_2_sys, pos_0_sys );
	
	vec_10_sys.normalize();
	vec_20_sys.normalize();
	axis_one.normalize();
	axis_two.normalize();

	// normal to the plane defined by selection
	normal_sys.set_cross( vec_10_sys, vec_20_sys );
	if( normal_sys.norm() < 1e-4 ){
		printf( "adjustNormalAxis: Atoms selected are aligned in the same line. No rotation performed\n" );
		return;
	}

	// orientate the plane so that it is perpendicular to axis_one
//	rotateMolAxis( normal_sys, axis_one );
	rotateMolAxisNoGonio( normal_sys, axis_one );
	
	// update vec_10_sys according to new orientation
	pos_0_sys = moleculeToSystemCoords( molToDraw, pos_0_mol );	
	pos_1_sys = moleculeToSystemCoords( molToDraw, pos_1_mol );
	vec_10_sys.set_sub( pos_1_sys, pos_0_sys );
	
	// orientate the molecule so that vec_10_sys is parallel to axis_two
	// since axis_two does not have to lie in a plane defined by axis_one, we project it onto this plane
	axis_two_proj.findProjectionOntoPlane( axis_two, axis_one );
	if( axis_two_proj.norm() < 1e-4 ){
		printf( "adjustNormalAxis: The two axes are parallel with each other. Final rotation not performed.\n" );
		return;
	}
	axis_two_proj.normalize();
		
//	rotateMolAxis( vec_10_sys, axis_two_proj );
	rotateMolAxisNoGonio( vec_10_sys, axis_two_proj );

}

void PhysicalSystem::adjustNormalAxisMatrix( Vec3d axis_one, Vec3d axis_two ){
// assumes there is a selection of (at least) three atoms pickedAtoms; their positions define a plane
// adjustNormalAxis orientates molecule molToDraw such that the plane is perpendicular to axis_one and first two atoms lie in axis_two
// unlike adjustNormalAxis this procedure avoids using angle approach at all and uses matrices instead

	Vec3d normal_sys, vec_10_sys, vec_20_sys, axis_two_proj, binormal_sys, binormal_axes;
	Vec3d pos_0_mol, pos_1_mol, pos_2_mol;
	Vec3d pos_0_sys, pos_1_sys, pos_2_sys;
	Mat3d mat_sys, mat_axes, mat;
	Quat4d qrot;
	
	// SELECTION
	
	// get positions of (first three) atoms in a selection
	pos_0_mol.set( molecules[molToDraw]->xyzs[pickedAtoms[0]] );
	pos_1_mol.set( molecules[molToDraw]->xyzs[pickedAtoms[1]] );
	pos_2_mol.set( molecules[molToDraw]->xyzs[pickedAtoms[2]] );

	// express these positions in a system coordinates
	pos_0_sys = moleculeToSystemCoords( molToDraw, pos_0_mol );
	pos_1_sys = moleculeToSystemCoords( molToDraw, pos_1_mol );
	pos_2_sys = moleculeToSystemCoords( molToDraw, pos_2_mol );

	// get vectors connecting the positions
	vec_10_sys.set_sub( pos_1_sys, pos_0_sys );
	vec_20_sys.set_sub( pos_2_sys, pos_0_sys );
	
	vec_10_sys.normalize();
	vec_20_sys.normalize();	
	
	// normal to the plane defined by selection
	normal_sys.set_cross( vec_10_sys, vec_20_sys );
	if( normal_sys.norm() < 1e-4 ){
		printf( "adjustNormalAxisMatrix: Atoms selected are aligned in the same line. No rotation performed\n" );
		return;
	}
	normal_sys.normalize();
	
	// binormal for selection
	binormal_sys.set_cross( normal_sys, vec_10_sys );
	binormal_sys.normalize();
	
	// normal_sys, vec_10_sys, binormal_sys form the first matrix
	mat_sys.set( normal_sys, vec_10_sys, binormal_sys );
	
	// AXES
	
	axis_one.normalize();
	axis_two.normalize();
	
	// since axis_two does not have to lie in a plane defined by axis_one, we project it onto this plane
	axis_two_proj.findProjectionOntoPlane( axis_two, axis_one );
	if( axis_two_proj.norm() < 1e-4 ){
		printf( "adjustNormalAxis: The two axes are parallel with each other. Final rotation not performed.\n" );
		return;
	}
	axis_two_proj.normalize();

	// binormal for axes
	binormal_axes.set_cross( axis_one, axis_two_proj );
	binormal_axes.normalize();

	// axis_one, axis_two_proj, binormal_axes form the second matrix
	mat_axes.set( axis_one, axis_two_proj, binormal_axes );

	// ROTATION

	mat.set_mmul_TN( mat_sys, mat_axes );

	qrot.fromMatrix( mat );
	qrot.normalize();	
//	qrot.mul( -1 );
	rot[molToDraw].qmul( qrot );
	rot[molToDraw].normalize();
	
}

void PhysicalSystem::rotateMolAxis( Vec3d a, Vec3d b ){
// suppose vector a is fixed in a molecule coord. system and vector b is fixed in the global coord. system
// rotateMolAxis rotates molecule molToDraw so that vector a is rotated into vector b

	float phi, scal_prod;
	Quat4d auxq;
	Vec3d rot_normal;
	
	a.normalize();
	b.normalize();

	scal_prod = a.dot( b );
	if( scal_prod >  1 ) scal_prod =  1.;
	if( scal_prod < -1 ) scal_prod = -1.;
//	printf( "scal_prod = %f\n", scal_prod );

	if( scal_prod > 1 - 1e-5 ){
		printf( "rotateMolAxis: No extra rotation needed.\n" );
	} else {
		phi = acos( scal_prod );
		
		rot_normal.set_cross( a, b );
//		rot_normal.normalize();
		auxq.fromAngleAxis( -phi, rot_normal ); // minus phi !!!
		
		rot[molToDraw].qmul( auxq );
	}

}

void PhysicalSystem::rotateMolAxisNoGonio( Vec3d a, Vec3d b ){
// suppose vector a is fixed in a molecule coord. system and vector b is fixed in the global coord. system
// rotateMolAxisNoGonio rotates molecule molToDraw so that vector a is rotated into vector b
// unlike rotateMolAxis this procedure avoids using goniometric functions as much as possible

	float phi, scal_prod;
	Quat4d auxq;
	Vec3d rot_normal;
	
	a.normalize();
	b.normalize();

	scal_prod = a.dot( b );
	if( scal_prod >  1 ) scal_prod =  1.;
	if( scal_prod < -1 ) scal_prod = -1.;
//	printf( "scal_prod = %f\n", scal_prod );

	if( scal_prod > 1 - 1e-5 ){
		printf( "rotateMolAxisNoGonio: No extra rotation needed.\n" );
	} else {		
		rot_normal.set_cross( a, b );
//		rot_normal.normalize();
		auxq.fromAngleAxisNoGonio( scal_prod, rot_normal ); // minus phi !!!
		
		rot[molToDraw].qmul( auxq );
	}

}

#endif
