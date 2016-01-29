
#ifndef _PHYSICAL_SYSTEM_
#define _PHYSICAL_SYSTEM_

#include <string.h>
#include <stdio.h>

class PhysicalSystem {
	public:

	int nmols;										// number of molecules in a system
	MoleculeType **molecules  = NULL;				// molecules themselves
	Vec3d  *pos = NULL, *vpos = NULL, *fpos = NULL;	// positions of molecules, velocities and forces
	Quat4d *rot = NULL, *vrot = NULL, *frot = NULL;	// rotations of molecules, velocities and forces
	bool* notRigid = NULL;							// whether given molecule is subject to relaxation
	int nptmp;
	Vec3d  *Tps_i = NULL, *Tps_j = NULL;
	Vec3d  *fs_i  = NULL, *fs_j  = NULL;

	AtomTypes* atypeList = NULL;			// types of atoms present in molecules
	double *C6s = NULL, *C12s = NULL;		// coefficients for the Lennard-Jones potential

	OptimizerDerivs* optimizer = NULL;
	abstractTip*     tip       = NULL;
	abstractSurf*    surf      = NULL;

	double fmaxConv = 1e-4;
	Vec3d fTip;

	bool   sysEvol = true;				    // sysEvol == true iff the system undergoes evolution/relaxation
	int    perFrame = 10;
	int    optSteps = 0;
	double fmax;

	// ============= FUNCTIONS

	// setup
	void initParams();
	void initTPoints();

	void resetGeometry         ( char const* filename, Vec3d posProbe, Quat4d rotProbe, bool raw );
	void resetGeometry         ( fileWrapper* file, Vec3d posProbe, Quat4d rotProbe );

	// geometry utilities
	double distBetweenTwoAtoms   ( int indmol, int inda, int indb );
	Vec3d  systemToMoleculeCoords( int mol, Vec3d systemCoords );
	Vec3d  moleculeToSystemCoords( int mol, Vec3d systemCoords );
	void   moleculeToSystemCoords( const Vec3d& pos, const Quat4d& rot, int npoints, Vec3d* points, Vec3d* Tpoints );
	Vec3d  systemCoordsOfAtom    ( int mol, int atom );
	void   adjustMolToTip();

	// relaxation optimization
	void forceFromPoints( int npoints, Vec3d* points, Vec3d* forces,  const Quat4d& q,  Vec3d& fp, Quat4d& fq );
	void forceFromPoints( const Vec3d& point, const Vec3d& force, const Quat4d& q, Vec3d& fp, Quat4d& fq );
	void forceFromPoints( const Vec3d& point, const Vec3d& force, int mol );
	void forceFromPoints( int mol, int atom,  const Vec3d& force );
	void forcesMolecules( bool surfaceForcesActive = true );
	void forcesTip( Vec3d& vec );
	void forcesTipAux( Vec3d centre, Vec3d Mvec, double mult, Vec3d force, GLuint id );
	void cleanPointForce( int npoints, Vec3d* forces );
	//void rigidOptStep( double& pixelDataListItem );
	void rigidOptStep( );
	void update( bool& optimizingFlag );

	//void setSysEvol( bool sysEvol_ );
	//bool getSysEvol();

	// additional actions
	void exportData     ();
	void chectTipMolAtom();

	// ==== inline functions
	void makeFF(){ makeLJparams( atypeList->ntypes, atypeList->vdwRs, atypeList->vdwEs, C6s, C12s ); }

	// ==== construnctor & destructor
	PhysicalSystem(){};
	PhysicalSystem( char const* filename, int numOfMoleculeInstances, MoleculeType** molTypeList, bool raw, abstractTip* tip_, abstractSurf* surf_ );
	PhysicalSystem( fileWrapper* geometryFile, int numOfMoleculeInstances, MoleculeType** molTypeList, abstractTip* tip_, abstractSurf* surf_ );
	virtual ~PhysicalSystem();

};

PhysicalSystem::~PhysicalSystem(){ // destructor
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
}

// ============== INITIALIZATION ==============

void PhysicalSystem::initParams(){  // optimization parameters
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

void PhysicalSystem::initTPoints(){  // point temporary
	nptmp = 0;
	for( int i = 0; i < nmols; i++ ){
		if( molecules[i]->natoms > nptmp ){	nptmp = molecules[i]->natoms; }
	}
	Tps_i = new Vec3d[nptmp];
	Tps_j = new Vec3d[nptmp];
	fs_i  = new Vec3d[nptmp];
	fs_j  = new Vec3d[nptmp];
	atypeList = molecules[0]->typeList;
}

PhysicalSystem::PhysicalSystem( char const* filename, int numOfMoleculeInstances, MoleculeType** molTypeList, bool raw, abstractTip* tip_, abstractSurf* surf_ ){
	FILE* pFile = fopen( filename, "r" );
	if( pFile == NULL ){
		printf( "PhysicalSystem: File %s cannot be opened. Default physical system created.\n", filename );
		nmols		= 0;
		molecules	= NULL;
		tip		    = NULL;
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
		//CHECK(3)
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
		//CHECK(5)
		if( tip != NULL ) tip->printTip();
//		surf->printSurf();
		//CHECK(4)
		initTPoints();
		makeFF();
	}
}

// ============== DESTRUCTION AND RESET ==============

void PhysicalSystem::resetGeometry( fileWrapper* file, Vec3d posProbe, Quat4d rotProbe ){
// resets positions and orientation of molecules in the system, input file is the same as is used in the constructor
// assumes there is a probe molecule in the system
	if( !file->openRead() ){
		printf( "resetGeometry: File %s cannot be opened. Default geometry used instead.\n", file->getFileName() );
		nmols		= 0;
		molecules	= NULL;
		tip		    = NULL;
		surf		= NULL;
	} else {
		printf( "resetGeometry: Reseting geometry based on values from %s.\n", file->getFileName() );
		int aux, aux2, count;
		Mat3d M;
		char str[400];
		file->readLineToString( str );
		count = sscanf( str, " %i", &aux );
		initParams();
		for( int i = 0; i < nmols - 1; i++ ){
			file->readLineToString( str );
			if( file->isRaw() ){
				count = sscanf( str, "%i	%lf %lf %lf	%lf %lf %lf %lf	%i", &aux,	&pos[i].x, &pos[i].y, &pos[i].z,	&rot[i].x, &rot[i].y, &rot[i].z, &rot[i].w,	&aux2 );
			} else {
				sscanf( str, "%i	%lf %lf %lf     %lf %lf %lf     %lf %lf %lf	%i", &aux, &pos[i].x, &pos[i].y, &pos[i].z,    &M.ax, &M.ay, &M.az,    &M.bx, &M.by, &M.bz,	&aux2 );
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
				count = sscanf( str, "%i	%lf %lf %lf	%lf %lf %lf %lf	%i", &aux,	&pos[i].x, &pos[i].y, &pos[i].z,	&rot[i].x, &rot[i].y, &rot[i].z, &rot[i].w,	&aux2 );
			} else {
				fscanf( pFile, "%i	%lf %lf %lf     %lf %lf %lf     %lf %lf %lf	%i", &aux, &pos[i].x, &pos[i].y, &pos[i].z,    &M.ax, &M.ay, &M.az,    &M.bx, &M.by, &M.bz,	&aux2 );
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

//void PhysicalSystem::update( bool& optimizingFlag, double& pixelDataListItem ){
void PhysicalSystem::update( bool& optimizingFlag ){
// update the relaxation
	if( optimizingFlag ){
		//int perFrame = 1;
		for( int i = 0; i < perFrame; i++ ){
			optSteps ++;
			//rigidOptStep( pixelDataListItem );
			rigidOptStep( );
			fmax = optimizer->getFmaxAbs();
//			printf( "fmax = %f\n", fmax );
			if( fmax < fmaxConv ){
				optimizingFlag = false;
//				printf( "Converged in %i !!!, Fmax = %f \n", optimizer->stepsDone, fmax );
				break;
			}
		}
//		printf( " %i fmax %f\n",  optimizer->stepsDone, fmax );
	}
}

//void PhysicalSystem::rigidOptStep( double& pixelDataListItem ){
void PhysicalSystem::rigidOptStep( ){
// one step of the relaxation
	// initialize everything associated with forces to zero
	for( int i = 0; i < nmols; i++ ){
		fpos[i].set( 0.0 );			// set all "forces positions" to zero
		frot[i].set( 0.0, 0.0, 0.0, 0.0 );	// set all "forces rotations" to zero
		rot[i].normalize();			// keep quaternion normalized, otherwise unstable !!!
	}
	//Vec3d vec;
	// calculate forces
	forcesMolecules();
	if( tip != NULL ){
		forcesTip( fTip );
	}
	//pixelDataListItem = fTip.z;
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
	forceFromPoints( vecaux, vecaux2, tip->probeMol->mol );
}

// calculate forces between the tip and a single atom atomToDraw_ in molecule molToDraw_, tip_sys are system coordinates of the tip
void PhysicalSystem::forcesTip( Vec3d& vec ){
	double mult = 1;
	Vec3d sysCoordAtom;
	Vec3d centre, force;
	//molToDrawNormal = tip->probeMol->mol;   // FIXME : Don't know what it does here ?
	sysCoordAtom = systemCoordsOfAtom    ( tip->probeMol->mol, tip->probeMol->atom      );
	centre       = moleculeToSystemCoords( tip->probeMol->mol, tip->probeMol->centreMol );
	force        = tip->getForceRad      ( sysCoordAtom, vec ); // forces to auxiliary point
	forceFromPoints( tip->probeMol->mol, tip->probeMol->atom, force );
	// draw axes
	//Vec3d vec_zero;
	//vec_zero.set( 0, 0, 0 );
	// drawCoordAxesRot( centre, force, vec_zero, vec_zero, normID[1], 5 );   // FIXME : Don't know what it does here ?
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

void PhysicalSystem::forceFromPoints( int npoints, Vec3d * points, Vec3d * forces, const Quat4d& q, Vec3d& fp, Quat4d& fq ){
	for( int i = 0; i < npoints; i++ ){
		q .addForceFromPoint( points[i], forces[i], fq );
		fp.add( forces[i] );
	}
}

void PhysicalSystem::forceFromPoints( const Vec3d& point, const Vec3d& force, const Quat4d& q, Vec3d& fp, Quat4d& fq ){
	q .addForceFromPoint( point, force, fq );
	fp.add( force );
}

void PhysicalSystem::forceFromPoints( const Vec3d& point, const Vec3d& force, int mol ){
	rot [mol].addForceFromPoint( point, force, frot[mol] );
	fpos[mol].add( force );
}

void PhysicalSystem::forceFromPoints( int mol, int atom, const Vec3d& force ){
	//rot [mol].addForceFromPoint( molecules[mol]->xyzs[atom], force, frot[mol] );
	//fpos[mol].add( force );
	forceFromPoints( molecules[mol]->xyzs[atom], force, mol );
}

void PhysicalSystem::cleanPointForce( int npoints, Vec3d * forces ){ // set forces to zero
	for( int i = 0; i < npoints; i++ ){	forces[i].set( 0.0 ); }
}

//void PhysicalSystem::setSysEvol( bool sysEvol_ ){	sysEvol = sysEvol_; } // fucking useless method
//bool PhysicalSystem::getSysEvol(               ){ return sysEvol;     } // other fucking useless method

void PhysicalSystem::chectTipMolAtom(){   // set a permanent auxiliary point
	if( tip == NULL || tip->probeMol == NULL ){
		printf( "chectTipMolAtom: Invalid tip. Halt.\n" );
		return;
	}
	if( tip->probeMol->mol >= nmols ){	printf( "chectTipMolAtom: Probe molecule index too big.\n" );
		tip->probeMol->mol = nmols - 1;
	}
	if( tip->probeMol->atom >= molecules[tip->probeMol->mol]->natoms ){
		printf( "chectTipMolAtom: Probe atom index too big.\n" );
		tip->probeMol->atom = molecules[tip->probeMol->mol]->natoms - 1;
	}
}

void PhysicalSystem::adjustMolToTip(){   // move aux_molToDraw_orig so as its aux_atomToDraw_orig-th atom lies in auxiliary point aux_force_point
	if( tip == NULL || tip->probeMol == NULL ){ printf( "adjustMolToTip: No tip. Halt.\n" ); return; }
	if( tip->probeMol->mol  == -1            ){ printf( "adjustMolToTip: Invalid mol.\n"  ); return; }
	if( tip->probeMol->atom == -1            ){	printf( "adjustMolToTip: Invalid atom.\n" ); return; }
	Vec3d atomPos, vec_diff;
	atomPos = moleculeToSystemCoords( tip->probeMol->mol, molecules[tip->probeMol->mol]->xyzs[tip->probeMol->atom] );
	vec_diff.set_sub( tip->pos, atomPos );
	pos[tip->probeMol->mol].add( vec_diff );
}

// return a distance between inda-th atom and indb-th atom of the indmol-th molecule
double PhysicalSystem::distBetweenTwoAtoms( int indmol, int inda, int indb ){
	Vec3d diff;
	diff.set_sub( molecules[indmol]->xyzs[inda], molecules[indmol]->xyzs[indb] );
	diff = moleculeToSystemCoords( indmol, diff );
	return diff.norm();
}

// ======== Molecule<->System coordinate transofrmation

// takes vector of system coordinates and express the same point in a molecule coordinates corresponding to molecule mol
Vec3d PhysicalSystem::systemToMoleculeCoords( int mol, Vec3d systemCoords ){
	Mat3d mat;
	Vec3d molCoordsAux, molCoords;
	rot[mol].toMatrix( mat );
	molCoordsAux.set_sub( systemCoords, pos[mol] );
	mat.dot_to( molCoordsAux, molCoords );
	return molCoords;
}

// express molecule coordinates stored in molCoords into system coordinates which are returned
// molCoords are coordinates in mol-th molecule coordinate system
Vec3d PhysicalSystem::systemCoordsOfAtom( int mol, int atom ){
	return moleculeToSystemCoords( mol, molecules[mol]->xyzs[atom] );
}

// express molecule coordinates stored in points into system coordinates which are stored into Tpoints
// pos and rot are a position and an orientation of a given molecule w.r.t. the system, respectively
// npoints is a number of points
void PhysicalSystem::moleculeToSystemCoords( const Vec3d& pos, const Quat4d& rot, int npoints, Vec3d* points, Vec3d* Tpoints ){
	Mat3d T;
	rot.toMatrix( T );
	for( int i = 0; i < npoints; i++ ){
		Vec3d Tp;
		T.dot_to_T( points[i], Tp );
		Tpoints[i].set_add( pos, Tp );
	}
}

// express molecule coordinates stored in molCoords into system coordinates which are returned
// molCoords are coordinates in mol-th molecule coordinate system
Vec3d PhysicalSystem::moleculeToSystemCoords( int mol, Vec3d molCoords ){
	Mat3d mat;
	Vec3d systemCoordsAux, systemCoords;
	rot[mol].toMatrix( mat );
	mat.dot_to_T( molCoords, systemCoordsAux );
	systemCoords.set_add( pos[mol], systemCoordsAux );
	return systemCoords;
}

// ============== I/O Rutines ==============

// export data about molecules to the file
void PhysicalSystem::exportData(){
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

#endif
