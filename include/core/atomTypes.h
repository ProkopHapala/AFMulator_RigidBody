
#ifndef _ATOM_TYPES_
#define _ATOM_TYPES_

class AtomTypes {
	public:
	int ntypes;
	int    * Zs;
	char  ** names; 
	double * vdwRs;
	double * vdwEs;
	Uint32 * colors;

	bool loadAtomsFromFile( char const* filename ){
		printf( "loadAtomsFromFile:\tAtom types loaded from\t\t%s.\n", filename );
		FILE* pFile = fopen( filename, "r" );
		if( pFile == NULL ){
			printf( "loadAtomsFromFile: Atom types cannot be uploaded.\n" );
			return false;
		} else {
			fscanf( pFile, "%i", &ntypes );
			//printf("ntypes %i \n", ntypes );
			Zs       = new    int[ ntypes ];
			vdwRs    = new double[ ntypes ];
			vdwEs    = new double[ ntypes ];
			names    = new char* [ ntypes ];
			colors   = new Uint32[ ntypes ];

			char hexstring[8];
			for( int i = 0; i < ntypes; i++ ){
				names[i] = new char[6];
				fscanf( pFile, " %lf %lf %i %s %s", &vdwRs[i], &vdwEs[i], &Zs[i], names[i], hexstring );
				colors[i] = (Uint32)strtol( hexstring, NULL, 16 );
				//printf( "%i %f %f %i %s  %s %i\n", i, vdwRs[i], vdwEs[i], Zs[i], names[i], hexstring, colors[i] );
			}
			fclose( pFile );
			return true;
		}
	}

	AtomTypes( char const* filename ){
		loadAtomsFromFile( filename );
	}
	
	~AtomTypes(){
		delete [] Zs;
		delete [] vdwRs;
		delete [] vdwEs;
		delete [] colors;
		
		Zs	= NULL;
		vdwEs	= NULL;
		vdwRs	= NULL;
		colors	= NULL;
		
		for( int i = 0; i < ntypes; i++){
			delete [] names[i];			
		}
		delete [] names;
		names	= NULL;
	}
};

#endif

