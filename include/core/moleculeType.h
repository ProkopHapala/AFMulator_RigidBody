
#ifndef _MOLECULE_TYPE_
#define _MOLECULE_TYPE_

class MoleculeType {
	public:
	AtomTypes* typeList;
	int natoms, nbonds = 0;
	int    * atypes = NULL;
	Vec3d  * xyzs   = NULL;
	double * Qs     = NULL;
	int    * bonds  = NULL;
	int viewlist = 0;
	
//	float* graphicsList = NULL; // pak vyhodit, neb je to grafika
	static float graphicsList[4];			
	
	~MoleculeType(){
	
//		delete [] typeList;
		delete [] atypes;
		delete [] xyzs;
		delete [] Qs;
		delete [] bonds;
		
//		typeList	= NULL;
		atypes		= NULL;
		xyzs		= NULL;
		Qs		= NULL;
		bonds		= NULL;
	}

	void init         (       ){ bonds = NULL; viewlist = 0; nbonds = 0; }
	void allocateAtoms( int n ){   
		atypes = new    int[ natoms ];
		xyzs   = new  Vec3d[ natoms ];       
		Qs     = new double[ natoms ];  
	}

	MoleculeType( ){ init(); };

	MoleculeType( int natoms_ ){
		natoms  = natoms_;
		allocateAtoms( natoms );
		init();
	}

	bool loadFromFile_bas( char const* filename ){
		printf(" filename: %s \n", filename );
		FILE * pFile;
		pFile = fopen (filename,"r");
  		fscanf (pFile, " %i", &natoms);
		//printf("natoms %i \n", natoms );
		allocateAtoms( natoms );
		for (int i=0; i<natoms; i++){
			double q=0;
  			int nw = fscanf (pFile, " %i %lf %lf %lf %lf", &atypes[i], &xyzs[i].x, &xyzs[i].y, &xyzs[i].z, &q );
			if( nw > 4 ){  Qs[i] = q; }else{ Qs[i] = 0; }
			atypes[i]--;
			//printf( " %i %f %f %f %f %f %i \n", atypes[i], xyzs[i].x, xyzs[i].y, xyzs[i].z, Qs[i], q, nw );
		}
  		fclose (pFile);
  		return 0;
	}

	bool loadFromFile( char const* filename ){
		FILE* pFile = fopen( filename, "r" );
		char str[400];
		int count;
		if( pFile ){
			printf( "MoleculeType::loadFromFile: \t %s.\n", filename );
			readLineComment( pFile, str );
	  		sscanf( str, " %i", &natoms );
			allocateAtoms( natoms );
			for( int i = 0; i < natoms; i++ ){					
				double q = 0;		
				readLineComment( pFile, str );
				count = sscanf( str, " %i %lf %lf %lf %lf", &atypes[i], &xyzs[i].x, &xyzs[i].y, &xyzs[i].z, &q );
				if( count > 4 ){
					Qs[i] = q;
				} else {
					printf( "MoleculeType::loadFromFile: Atom no. %i has no charge specified. Default value used instead.\n", i );
					Qs[i] = 0;
				}
				atypes[i]--;
				//printf( " %i %f %f %f %f %f %i \n", atypes[i], xyzs[i].x, xyzs[i].y, xyzs[i].z, Qs[i], q, nw );
			}
	  		fclose( pFile );
	  		return true;
  		} else {
			printf( "MoleculeType::loadFromFile: File %s cannot be loaded. Default value used instead.\n", filename );
			// DODELAT IMPLICITNI HODNOTY KDYZTAK
  			return false;
  		}
	}
	
	bool loadFromFile( fileWrapper* file ){
		file->openRead();
		char str[400];
		int count;
		if( file->isOpened() ){
			printf( "MoleculeType::loadFromFile: \t %s.\n", file->getFileName() );
			file->readLineToString( str );
	  		sscanf( str, " %i", &natoms );
			allocateAtoms( natoms );
			for( int i = 0; i < natoms; i++ ){					
				double q = 0;		
				file->readLineToString( str );
				count = sscanf( str, " %i %lf %lf %lf %lf", &atypes[i], &xyzs[i].x, &xyzs[i].y, &xyzs[i].z, &q );
				if( count > 4 ){
					Qs[i] = q;
				} else {
					printf( "MoleculeType::loadFromFile: Atom no. %i has no charge specified. Default value used instead.\n", i );
					Qs[i] = 0;
				}
				atypes[i]--;
				//printf( " %i %f %f %f %f %f %i \n", atypes[i], xyzs[i].x, xyzs[i].y, xyzs[i].z, Qs[i], q, nw );
			}
			file->close();
	  		return true;
  		} else {
			printf( "MoleculeType::loadFromFile: File %s cannot be loaded. Default value used instead.\n", file->getFileName() );
			// DODELAT IMPLICITNI HODNOTY KDYZTAK
  			return false;
  		}
	}
	
	MoleculeType( char const* filename ){ loadFromFile_bas( filename ); init(); };
	
	MoleculeType( char const* filename, AtomTypes* typeList_, float bonds, float graphicsList_[4] /*float* graphicsList_*/ ){
		//printf( "filename = %s\n", filename );
		if( loadFromFile( filename ) ){
			init(); 	
			typeList = typeList_;
			toCOG_average();
			findBonds( bonds );
			if( graphicsList_ != NULL ){
				for( int i = 0; i < 4; i++) graphicsList[i] = graphicsList_[i];
			}
			redrawMol();			
		} else {
			printf( "MoleculeType: Molecule cannot be loaded.\n" );
		}
	}

	MoleculeType( fileWrapper* file, AtomTypes* typeList_, float bonds, float graphicsList_[4]/*float* graphicsList_*/ ){

		if( loadFromFile( file ) ){
			init(); 	
			typeList = typeList_;
			toCOG_average();
			findBonds( bonds );
			
			if( graphicsList_ != NULL ){
				for( int i = 0; i < 4; i++) graphicsList[i] = graphicsList_[i];
			}
			redrawMol();			
		} else {
			printf( "MoleculeType: Molecule cannot be loaded.\n" );
		}
	}
	
	void redrawMol(){
		if( graphicsList != NULL ){
			makeViewCPK( (int) graphicsList[0], (int) graphicsList[1], graphicsList[2], graphicsList[3] );
		} else {
			printf( "redrawMol: Invalid graphics specification.\n" );
		}
	}

	int findBonds( double sc ){
		nbonds = 0;
		int * bonds_ = new int[ 2*natoms*natoms ];
		for (int i=0; i<natoms; i++ ){
			for (int j=i+1; j<natoms; j++ ){
				double rijmax = sc * ( typeList->vdwRs[atypes[i]] + typeList->vdwRs[atypes[j]] );
				Vec3d d; d.set_sub( xyzs[i], xyzs[j] );
				double rij2 = d.norm2();
				//printf( " %i %i %i %i %f %f    %f %f   %f %f %f \n", i, j, atypes[i], atypes[j], rij2, rijmax, typeList->vdwRs[atypes[i]],typeList->vdwRs[atypes[j]], d.x, d.y, d.z  );
				if( rij2 < ( rijmax*rijmax ) ){ 
					bonds_[nbonds]=i; bonds_[nbonds+1]=j; 
					nbonds+=2; 
				}
			}
		}
		if ( bonds != NULL ){ delete bonds; printf( " delete bonds \n"); }
		bonds = new  int[ nbonds ];
		for (int i=0; i<nbonds; i++ ){  bonds[i] = bonds_[i]; };
		delete [] bonds_;
		return nbonds;
	}


	int drawAtom( int i, int nsphere, float atomscale, Uint32 color ){
		setColorInt32( color );
		int nvert = drawSphere_oct( nsphere, atomscale*(typeList->vdwRs[atypes[i]]), xyzs[i] );
		return nvert;
	}

	int drawBond( int i, int j, int nstick, float bondwidth  ){
		Vec3f ai,aj;
		convert( xyzs[i], ai ); 
		convert( xyzs[j], aj );
		int nvert = drawCylinderStrip( nstick, bondwidth, bondwidth, ai, aj );
		return nvert;
	}

	int makeViewCPK ( int nsphere, int nstick, float atomscale, float bondwidth ){
		if( viewlist > 0 ) glDeleteLists( viewlist, 1 );
		viewlist = glGenLists( 1 );
		int nvert = 0;

		glNewList( viewlist, GL_COMPILE );
			glShadeModel( GL_SMOOTH );
			for( int i=0; i<natoms; i++ ){	nvert+= drawAtom( i, nsphere, atomscale, typeList->colors[ atypes[i] ] );	}
			glColor3f( 0.2f, 0.2f, 0.2f );
			for( int ib=0; ib<nbonds; ib+=2 ){	nvert+= drawBond( bonds[ib], bonds[ib+1], nstick, bondwidth );	}
		glEndList();
//		printf( " nvert %i \n", nvert );
		return viewlist;
	}

	void toCOG_minmax(){
		Vec3d pmin,pmax; pmin.set( 100000,100000,100000 );  pmax.set( -100000,-100000,-100000 );
		for (int i=0; i<natoms; i++){
			pmin.x = fmin( pmin.x, xyzs[i].x  ); pmin.y = fmin( pmin.y, xyzs[i].y ); pmin.z = fmin( pmin.z, xyzs[i].z );
			pmax.x = fmax( pmax.x, xyzs[i].x  ); pmax.y = fmax( pmax.y, xyzs[i].y ); pmax.z = fmax( pmax.z, xyzs[i].z );
		}
		pmin.add(pmax); pmin.mul(0.5);
		for (int i=0; i<natoms; i++){
			xyzs[i].sub( pmin ); 
		}
	}

	void toCOG_average(){
		Vec3d cog; cog.set(0.0);
		for (int i=0; i<natoms; i++){  cog.add( xyzs[i] ); }
		cog.mul( 1.0/natoms );
		for (int i=0; i<natoms; i++){  xyzs[i].sub( cog ); }
	}
	
	int rayPickAtom_sphere( const Vec3d& ray0, const Vec3d& hRay, const Vec3d *xyzs_aux ){
		double tmin = INFINITY; int imin = -1;
		for (int i=0; i<natoms; i++){
			double t = raySphere( ray0, hRay, typeList->vdwRs[atypes[i]], xyzs_aux[i] );
			if( t<tmin ){ 
				//printf( " i %i imin %i t %f tmin %f \n", i, imin, t, tmin );
				tmin=t; imin=i; 
			}
		}
		return imin;
	}

	int rayPickAtom_sphereAlt( const Vec3d& ray0, const Vec3d& hRay, const Vec3d *xyzs_aux, float atomscale, float& dist ){
		double tmin = INFINITY; int imin = -1;
		for (int i=0; i<natoms; i++){
			double t = raySphere( ray0, hRay, atomscale*typeList->vdwRs[atypes[i]], xyzs_aux[i] );
			if( t<tmin ){ 
				//printf( " i %i imin %i t %f tmin %f \n", i, imin, t, tmin );
				tmin=t; imin=i; 
			}
		}
		dist = tmin;
		return imin;
	}

	int rayPickAtom_sphereAlt( const Vec3d& ray0, const Vec3d& hRay, const Vec3d *xyzs_aux, float atomscale, float atomscaleEmph, int pickedAtomCounter, int* pickedAtoms, float& dist ){
		double tmin = INFINITY;
		int imin = -1;
		float atomscaleLoc = atomscale;
		for( int i = 0; i < natoms; i++ ){
			if( pickedAtoms != NULL ){
				for( int j = 0; j < pickedAtomCounter; j++ ){
					if( i == pickedAtoms[j] ) atomscaleLoc = atomscaleEmph;
				}
			}
			double t = raySphere( ray0, hRay, atomscaleLoc*typeList->vdwRs[atypes[i]], xyzs_aux[i] );
			atomscaleLoc = atomscale;
			if( t < tmin ){ 
				//printf( " i %i imin %i t %f tmin %f \n", i, imin, t, tmin );
				tmin = t; imin = i; 
			}
		}
		dist = tmin;
		return imin;
	}
};

float MoleculeType::graphicsList[] = { 5, 4, 1, 0.1 };

#endif


