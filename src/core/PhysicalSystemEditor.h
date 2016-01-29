
#ifndef PhysicalSystemEditor_h
#define PhysicalSystemEditor_h

#include <string.h>
#include <stdio.h>

class PhysicalSystemEditor: public PhysicalSystem {
	public:

	// picked atom and molecule indices
	Vec3d*	atomToDrawOrigPos = NULL;		// position of a chosen atom at the moment of a mouse click, used to determine the plane in which the atom is moved by a mouse
	double  distance;
	bool    mouseTip = false;

	// picked atom and molecule indices and flags
	int	  numPickedAtoms = 3;               // how many atoms form a complete selection
	int*  pickedAtoms    = NULL;            // indices of atoms in a selection
	int   pickedAtomCounter = 0;            // how many atoms are already picked to form a selection
	int	  molToDrawPrev  = -1;              // which molecule was chosen previously
	bool  atomHold   = false;               // atomHold == true iff the mouse is holding an atom
	int   atomToDraw = -1, molToDraw = -1;  // indices of atom and corresponding molecule which are to be picked by a mouse click
	bool  molHold       = false;            // molHold == true iff the mouse is holding a molecule
	float atomscale	    = 0.3;              // scale with which the atoms are rendered on the screen
	float atomscaleEmph	= 0.7;              // scale with which the emphasized atoms are rendered on the screen

	// miscellaneous graphics
	GLuint normalID = 0;
	GLuint normID[3] = { 0, 0, 0 };
	int molToDrawNormal = -1;
	int* lines = NULL;

	// ============= FUNCTIONS

	void draw();
	void drawAuxPoint();
	void mouseSetAuxPoint( float mouseA, float mouseB, Vec3d scCamRight, Vec3d scCamUp, Vec3d scCamDir );

	// selection of atoms
	bool addPickedAtom        ();
	bool addPickedAtomDrawLink();
	bool destroySelection     ();
	bool selectionChosen      ();
	void resetOneAtomMove     ();
	bool moveAtom      ( const Vec3d& systemCoords );
	bool moveMolecule  ( const Vec3d& systemCoords );
	bool rayPickAtomSet( const Vec3d& ray0, const Vec3d& hRay, bool emphasized );

	void setTemporaryAuxPoint( bool temporaryAuxPoint_ );
	bool getTemporaryAuxPoint();

	// molecule adjusting
	void adjustMolecule             ( char* end, double phi, Vec3d vec_pos, Vec3d vec_a, Vec3d vec_b );
	void adjustMoleculeSelection    ( char* end,             Vec3d vec_pos, Vec3d vec_a, Vec3d vec_b );
	void setFromStdinAdjustMolecule         ();
	void setFromStdinAdjustMoleculeSelection();
	void adjustMoleculeSelectionXaxis       ();
	void adjustMoleculeSelectionYaxis       ();
	void adjustMoleculeSelectionZaxis       ();

	void adjustNormalAxis      ( Vec3d axis_one, Vec3d axis_two );
	void adjustNormalAxisMatrix( Vec3d axis_one, Vec3d axis_two );
	void rotateMolAxis         ( Vec3d a,        Vec3d b );
	void rotateMolAxisNoGonio  ( Vec3d a,        Vec3d b );
	void adjustPosition        ( Vec3d vec_pos    );

	// ============= inline functions

	int  getMolToDraw (                 ){  return molToDraw;      }
	int  getAtomToDraw(                 ){  return atomToDraw;     }
	void setAtomHold  ( bool atomHold_  ){  atomHold = atomHold_;  }
	bool getAtomHold  (                 ){  return atomHold;       }
	void setMolHold   ( bool molHold_   ){  molHold = molHold_;    }
	bool getMolHold   (                 ){  return molHold;        }

	// ==== construnctor & destructor
	PhysicalSystemEditor( fileWrapper* geometryFile, int numOfMoleculeInstances, MoleculeType** molTypeList, abstractTip* tip_, abstractSurf* surf_ );
	virtual ~PhysicalSystemEditor();
};

// ============== Constructor & Destructor ==============

PhysicalSystemEditor :: ~PhysicalSystemEditor(){
	delete    atomToDrawOrigPos;
	delete [] pickedAtoms;
	delete [] lines;
};

PhysicalSystemEditor::PhysicalSystemEditor( fileWrapper* geometryFile, int numOfMoleculeInstances, MoleculeType** molTypeList, abstractTip* tip_, abstractSurf* surf_ ){

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
		//CHECK(8)
		geometryFile->close();

		tip = tip_;
		surf = surf_;
		//if( tip != NULL ) tip->printTip();
		//surf->printSurf();
		//CHECK(7)
		initTPoints();
		makeFF();
	}
}

// ============== Graphics ==============

void PhysicalSystemEditor::draw(){
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
			// draw the normal, if it exists
			glDisable( GL_LIGHTING );
			//printf( "molToDrawNormal = %i\n", molToDrawNormal );
			if( molToDrawNormal == i && normalID != 0 ){  glCallList( normalID ); }
			if( molToDrawNormal == i && tip != NULL ){
				for( int i = 0; i < 3; i++ ){
					if( normID[i] ){ glCallList( normID[i] ); }
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

// ============== Molecule Manipulations ==============

void PhysicalSystemEditor::resetOneAtomMove(){   // go one step backward, i.e. reset the last moving of an atom
	if( atomToDrawOrigPos != NULL && molToDraw != -1 && atomToDraw != -1 ){
		(molecules[molToDraw]->xyzs[atomToDraw]).set( *atomToDrawOrigPos );
		molecules[molToDraw]->redrawMol();
	}
}

bool PhysicalSystemEditor::moveAtom( const Vec3d& systemCoords ){    // if a picked atom is moved, set the new position of the atom
	if( !atomHold ){return false;}
	Vec3d atomCoords;
	atomCoords = systemToMoleculeCoords( molToDraw, systemCoords );
	molecules[molToDraw]->xyzs[atomToDraw].set( atomCoords );
	molecules[molToDraw]->redrawMol();
	return true;
}

bool PhysicalSystemEditor::moveMolecule( const Vec3d& systemCoords ){  // if a picked molecule is moved, set the new position of the molecule
	if( !molHold ){ return false; }
	Vec3d molCoords;
	pos[molToDraw].set( molCoords );
	molecules[molToDraw]->redrawMol();
	return true;
}

// suppose vector a is fixed in a molecule coord. system and vector b is fixed in the global coord. system
// rotateMolAxis rotates molecule molToDraw so that vector a is rotated into vector b
void PhysicalSystemEditor::rotateMolAxis( Vec3d a, Vec3d b ){
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

// suppose vector a is fixed in a molecule coord. system and vector b is fixed in the global coord. system
// rotateMolAxisNoGonio rotates molecule molToDraw so that vector a is rotated into vector b
// unlike rotateMolAxis this procedure avoids using goniometric functions as much as possible
void PhysicalSystemEditor::rotateMolAxisNoGonio( Vec3d a, Vec3d b ){
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


void PhysicalSystemEditor::mouseSetAuxPoint( float mouseA, float mouseB, Vec3d scCamRight, Vec3d scCamUp, Vec3d scCamDir ){
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

// ============== SELECTION OF ATOMS ==============

bool PhysicalSystemEditor::rayPickAtomSet( const Vec3d& ray0, const Vec3d& hRay, bool emphasized ){  // if a mouse is clicked over the atom, choose this atom
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
		//atomToDrawAux = molecules[i]->rayPickAtom_sphereAlt( ray0, hRay, xyzs_aux, atomscaleLoc, dist );
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
	//if no atom has been chosen by mouse...
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

bool PhysicalSystemEditor::addPickedAtom(){  // if an atom is chosen, add it to the selection
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


// if an atom is chosen, add it to the selection
// returns true iff there are at least two atoms in the selection, simultaneously calculates the distance between the first atom and the current atom in the selection
bool PhysicalSystemEditor::addPickedAtomDrawLink(){
	if( pickedAtomCounter == 0 ){
		pickedAtoms = new int[numPickedAtoms];
		lines 	    = new int[numPickedAtoms - 1];
		molToDrawPrev   = molToDraw;
		molToDrawNormal = molToDraw;
		pickedAtoms[pickedAtomCounter] = atomToDraw;
		pickedAtomCounter++;
		//return true;
		return false;
	}
	if( (molToDrawPrev != molToDraw) || (pickedAtomCounter >= numPickedAtoms) ){
		for( int i = 0; i < pickedAtomCounter - 1; i++){   glDeleteLists( lines[i], 1 );  }
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
	//printf( "pickedAtomCounter = %i\n", pickedAtomCounter );
	//glPushAttrib( GL_LINE_BIT );
	//glLineWidth( 3 );
	float color[] = { 0, 0, 0 }; // black
	switch( pickedAtomCounter ){
		case 1: color[0] = 1;	color[1] = 1;	break;  // yellow
		case 2: color[0] = 1;	color[2] = 1; 	break;  // magenta
		case 3: color[1] = 1;	color[2] = 1;	break;  // cyan
		default:	break;
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
	//glPopAttrib();
	pickedAtomCounter++;
	return true;
}

bool PhysicalSystemEditor::destroySelection(){  // destroy selection of atoms
	if( pickedAtoms == NULL ){ return false; }
	delete [] pickedAtoms;
	pickedAtoms = NULL;
	if( lines == NULL       ){ return false; }
	delete [] lines;
	lines = NULL;
	pickedAtomCounter = 0;
	molToDrawPrev = -1;
	return true;
}

bool PhysicalSystemEditor::selectionChosen(){   // is selection complete and chosen by mouse?
	int i;
	if ( pickedAtomCounter < numPickedAtoms ){ return false;	}
	if ( molToDrawPrev != molToDraw         ){ return false;	}
	for( i = 0; i < numPickedAtoms; i++     ){ if( pickedAtoms[i] == atomToDraw ){	break;	}	}
	if ( i == numPickedAtoms                ){ return false; 	}
	return true;
}

// ============== MOLECULE ADJUSTING ==============

// adjustMolecule procedure with user interface
void PhysicalSystemEditor::setFromStdinAdjustMolecule(){
	double x, y, z, phi;
	char end[40];
	Vec3d vec_pos, vec_a, vec_b;
	printf( "Molecule position specification by coordinate system: (y/i/n)? (i = yes, but use incrementation) " );
	scanf( "%s", end );
	if( !strcmp( end, "n" ) || !strcmp( end, "N" ) ) return;
	printf( "Specify the position of the molecule: " );	scanf( "%lf %lf %lf", &x, &y, &z );	vec_pos.set( x, y, z );
	printf( "Specify the first axis: " );	            scanf( "%lf %lf %lf", &x, &y, &z );	vec_a.set( x, y, z );
	printf( "Specify the second axis: " );	            scanf( "%lf %lf %lf", &x, &y, &z );	vec_b.set( x, y, z );
	printf( "Specify the angle in degrees: " );	        scanf( "%lf", &phi );
	phi = phi * (2*PI) / 360.0 ; // from degrees to radians
	//printf( "phi = %f\n", phi );
	adjustMolecule( end, phi, vec_pos, vec_a, vec_b );
}

// based on user input adjust position and rotation of the chosen molecule
// molecule coordinate system is used to adjust the molecule appropriately
void PhysicalSystemEditor::adjustMolecule( char* end, double phi, Vec3d vec_pos, Vec3d vec_a, Vec3d vec_b ){
	printf( "adjustMolecule()\n" );
	if( molToDraw == -1 ){
		printf( "adjustMolecule: No molecule chosen.\n" );
		return;
	}
	Vec3d normal;
	Quat4d auxq;
	// set position of the molecule
	if( !strcmp( end, "i" ) ){  pos[molToDraw].add( vec_pos ); }  // relative setting -- increment
	else                     { pos[molToDraw].set( vec_pos );  }  // absolute setting
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
		if( !strcmp( end, "i" ) ){ rot[molToDraw].qmul( auxq ); }  // relative setting -- increment
		else                     { rot[molToDraw].set( auxq );  }  // absolute setting
		printf( "adjustMolecule: New orientation set.\n" );
//		printf( "%f %f %f %f\n", rot[molToDraw].x, rot[molToDraw].y, rot[molToDraw].z, rot[molToDraw].w );
	}
}

// adjustMoleculeSelection procedure with user interface
void PhysicalSystemEditor::setFromStdinAdjustMoleculeSelection(){
	double x, y, z;
	char end[10];
	Vec3d vec_pos, vec_a, vec_b;
	printf( "Molecule position specification by coordinate system: (y/i/n)?\n (i = yes, but use incrementation for position) " );
	scanf( "%s", end );
	if( !strcmp( end, "n" ) || !strcmp( end, "N" ) ) return;
	printf( "Specify the first axis:  " );	scanf( "%lf %lf %lf", &x, &y, &z );	vec_a.set( x, y, z );
	printf( "Specify the second axis: " );	scanf( "%lf %lf %lf", &x, &y, &z );	vec_b.set( x, y, z );
	printf( "Specify the position of the molecule: " );	scanf( "%lf %lf %lf", &x, &y, &z );	vec_pos.set( x, y, z );
	adjustMoleculeSelection( end, vec_pos, vec_a, vec_b );
}

// orientate molecule molToDraw such that it is perpendicular to vec_a and aligned with vec_b
// for details see docstrings for adjustNormalAxis and adjustPosition
void PhysicalSystemEditor::adjustMoleculeSelection( char* end, Vec3d vec_pos, Vec3d vec_a, Vec3d vec_b ){
	Vec3d vec_pos_aux;
	printf( "adjustMoleculeSelection()\n" );
	if( molToDraw         == -1   ){ printf( "adjustMoleculeSelection: No molecule chosen.\n"               ); return; }
	if( pickedAtoms       == NULL ){ printf( "adjustMoleculeSelection: No selection of atoms.\n"            ); return; }
	if( pickedAtomCounter < 3     ){ printf( "adjustMoleculeSelection: Selection of atoms is incomplete.\n" ); return; }
	//moleculeToSystemCoords( pos[molToDraw], rot[molToDraw], 1, &molecules[molToDraw]->xyzs[pickedAtoms[0]], &vec_pos_aux );
	vec_pos_aux = moleculeToSystemCoords( molToDraw, molecules[molToDraw]->xyzs[pickedAtoms[0]] );
	adjustNormalAxis( vec_a , vec_b );
	if( !strcmp( end, "i" ) ){ vec_pos.add( vec_pos_aux ); }  // if increment mode...
	adjustPosition( vec_pos );
}

// assumes that at least one atom is selected
// moves molecule molToDraw so that position of the selected atom coincides with vec_pos
void PhysicalSystemEditor::adjustPosition( Vec3d vec_pos ){
	Vec3d vec_diff, vec_pos_aux;
	// express the first picked atom position in a system coordinates
	//moleculeToSystemCoords( pos[molToDraw], rot[molToDraw], 1, &molecules[molToDraw]->xyzs[pickedAtoms[0]], &vec_pos_aux );
	vec_pos_aux = moleculeToSystemCoords( molToDraw, molecules[molToDraw]->xyzs[pickedAtoms[0]] );
	vec_diff.set_sub( vec_pos, vec_pos_aux );   // find translation vector between the position of the atom and the specified position
	pos[molToDraw].add( vec_diff );             // set new position of the molecule
}

// orientate molecule molToDraw such that it is perpendicular to x-axis
// for details see docstring for adjustNormalAxis
void PhysicalSystemEditor::adjustMoleculeSelectionXaxis(){
	printf( "adjustMoleculeSelectionXaxis()\n" );
	if( molToDraw         == -1   ){  printf( "adjustMoleculeSelectionXaxis: No molecule chosen.\n" );               return; }
	if( pickedAtoms       == NULL ){  printf( "adjustMoleculeSelectionXaxis: No selection of atoms.\n" );            return; }
	if( pickedAtomCounter < 3     ){  printf( "adjustMoleculeSelectionXaxis: Selection of atoms is incomplete.\n" ); return; }
	Vec3d xAxis, yAxis;
	xAxis.set( 1, 0, 0 ); // x-axis
	yAxis.set( 0, 1, 0 ); // y-axis
	//adjustNormalAxis( xAxis, yAxis );
	adjustNormalAxisMatrix( xAxis, yAxis );
}

// orientate molecule molToDraw such that it is perpendicular to y-axis
// for details see docstring for adjustNormalAxis
void PhysicalSystemEditor::adjustMoleculeSelectionYaxis(){
	printf( "adjustMoleculeSelectionYaxis()\n" );
	if( molToDraw         == -1   ){ printf( "adjustMoleculeSelectionYaxis: No molecule chosen.\n" );               return; }
	if( pickedAtoms       == NULL ){ printf( "adjustMoleculeSelectionYaxis: No selection of atoms.\n" );            return; }
	if( pickedAtomCounter < 3     ){ printf( "adjustMoleculeSelectionYaxis: Selection of atoms is incomplete.\n" ); return; }
	Vec3d yAxis, zAxis;
	yAxis.set( 0, 1, 0 ); // y-axis
	zAxis.set( 0, 0, 1 ); // z-axis
	//adjustNormalAxis( yAxis, zAxis );
	adjustNormalAxisMatrix( yAxis, zAxis );
}

// orientate molecule molToDraw such that it is perpendicular to y-axis
// for details see docstring for adjustNormalAxis
void PhysicalSystemEditor::adjustMoleculeSelectionZaxis(){
	printf( "adjustMoleculeSelectionZaxis()\n" );
	if( molToDraw         == -1   ){ printf( "adjustMoleculeSelectionZaxis: No molecule chosen.\n" );                 return; }
	if( pickedAtoms       == NULL ){ printf( "adjustMoleculeSelectionZaxis: No selection of atoms.\n" );              return; }
	if( pickedAtomCounter <  3    ){ printf( "adjustMoleculeSelectionZaxis: Selection of atoms is incomplete.\n" );   return; }
	Vec3d xAxis, zAxis;
	xAxis.set( 1, 0, 0 ); // x-axis
	zAxis.set( 0, 0, 1 ); // z-axis
	//adjustNormalAxis( zAxis, xAxis );
	adjustNormalAxisMatrix( zAxis, xAxis );
}

// assumes there is a selection of (at least) three atoms pickedAtoms; their positions define a plane
// adjustNormalAxis orientates molecule molToDraw such that the plane is perpendicular to axis_one and first two atoms lie in axis_two
void PhysicalSystemEditor::adjustNormalAxis( Vec3d axis_one, Vec3d axis_two ){
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
	//rotateMolAxis( normal_sys, axis_one );
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
	//rotateMolAxis( vec_10_sys, axis_two_proj );
	rotateMolAxisNoGonio( vec_10_sys, axis_two_proj );
}

void PhysicalSystemEditor::adjustNormalAxisMatrix( Vec3d axis_one, Vec3d axis_two ){
// assumes there is a selection of (at least) three atoms pickedAtoms; their positions define a plane
// adjustNormalAxis orientates molecule molToDraw such that the plane is perpendicular to axis_one and first two atoms lie in axis_two
// unlike adjustNormalAxis this procedure avoids using angle approach at all and uses matrices instead
	Vec3d normal_sys, vec_10_sys, vec_20_sys, axis_two_proj, binormal_sys, binormal_axes;
	Vec3d pos_0_mol, pos_1_mol, pos_2_mol;
	Vec3d pos_0_sys, pos_1_sys, pos_2_sys;
	Mat3d mat_sys, mat_axes, mat;
	Quat4d qrot;

	// ---- SELECTION
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

	// ---- AXES
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

	// ---- ROTATION
	mat.set_mmul_TN( mat_sys, mat_axes );
	qrot.fromMatrix( mat );
	qrot.normalize();
//	qrot.mul( -1 );
	rot[molToDraw].qmul( qrot );
	rot[molToDraw].normalize();

}

#endif
