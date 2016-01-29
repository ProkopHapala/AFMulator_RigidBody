
#ifndef _TIP_PROCEDURES_
#define _TIP_PROCEDURES_

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>

// ======== PROBE PARTICLE

class probeMolecule {  // probe molecule
	public:
	Quat4d rot;
	Vec3d  pos;
	Vec3d  centreMol;
	int atom = -1;
	int mol  = -1;
	std::string fileName;
	// ==== construnctor & destructor
	probeMolecule(){};
	probeMolecule( int mol_, int atom_, Vec3d pos_, Quat4d rot_, Vec3d centreMol_, std::string fileName_ );
};

probeMolecule::probeMolecule( int mol_, int atom_, Vec3d pos_, Quat4d rot_, Vec3d centreMol_, std::string fileName_ )
: mol( mol_ ), atom( atom_ ), pos( pos_ ), rot( rot_ ), centreMol( centreMol_ ), fileName( fileName_ ){};

// ============ TIPS

class abstractTip {  // parent class for all tips
	public:
	GLuint tipID;
	Vec3d pos;
	probeMolecule* probeMol = NULL;
	// ==== functions
	virtual void  renderTip( Vec3d pos1, double radius ){};
	virtual void  drawTip(){};
	virtual Vec3d getForceRad( Vec3d sysCoordAtom, Vec3d& vec ){};
	virtual void  getForceOrient( Quat4d rot_, Vec3d& force2, Vec3d& force3, Vec3d& vec2, Vec3d& vec3 ){};
	virtual void  setTipParameters( char* filename ){};
	virtual void  printTip(){};
	// ==== inline functions
	void setPosition( Vec3d pos_                            ){	pos.set( pos_ );	         };
	void setPosition( double xpos, double ypos, double zpos ){	pos.set( xpos, ypos, zpos ); };
};

class tipOnePoint : public abstractTip {   // tip represented by a fixed point
	public:
	Vec3d xaxis, yaxis, zaxis;
	// ==== functions
	void printTip();
	// ==== construnctor & destructor
	tipOnePoint();
	tipOnePoint( Vec3d zaxis );
	tipOnePoint( Vec3d xaxis, Vec3d yaxis, Vec3d zaxis );
};

class tipOnePointMol : public tipOnePoint { // tip with a probe molecule
	public:
	double kcoeff;
	double kcoeffrot;
	double kcoeffrot2;
	double kcoeffrot3;
	Quat4d rotInitAux;
	char tipSettingFile[400];
	// ==== functions
	void getForceOrient( Quat4d rot_, Vec3d& force2, Vec3d& force3, Vec3d& vec2, Vec3d& vec3 );
	Vec3d getForceRad( Vec3d sysCoordAtom, Vec3d& vec );
	void setTipParams( const char* filename );
	void setTipParameters( char* filename );
	void printTip();
	void renderTip( Vec3d pos1, double radius );
	void drawTip();
	// ==== construnctor & destructor
	tipOnePointMol( char* filename );
	tipOnePointMol( char* filename, Vec3d zaxis );
	tipOnePointMol( char* filename, Vec3d xaxis, Vec3d yaxis, Vec3d zaxis );
};

// ===================== tipOnePoint procedures =====================

tipOnePoint::tipOnePoint() // constructor
: abstractTip(){}

tipOnePoint::tipOnePoint( Vec3d xaxis, Vec3d yaxis, Vec3d zaxis ) // constructor
: xaxis( xaxis ), yaxis( yaxis ), zaxis( zaxis ){}

tipOnePoint::tipOnePoint( Vec3d zaxis ) // constructor
: zaxis( zaxis ){}

void tipOnePoint::printTip(){ // print parameters
	printf( "xaxis = %lf %lf %lf\n", xaxis.x, xaxis.y, xaxis.z );
	printf( "yaxis = %lf %lf %lf\n", yaxis.x, yaxis.y, yaxis.z );
	printf( "zaxis = %lf %lf %lf\n", zaxis.x, zaxis.y, zaxis.z );
}

// ===================== tipOnePointMol procedures =====================

tipOnePointMol::tipOnePointMol( char* filename ) // constructor
: tipOnePoint(){
	probeMol = new probeMolecule();
	setTipParams( filename );
	setTipParameters( tipSettingFile );
}


tipOnePointMol::tipOnePointMol( char* filename, Vec3d xaxis, Vec3d yaxis, Vec3d zaxis )
// constructor
: tipOnePoint( xaxis, yaxis, zaxis ){
	probeMol = new probeMolecule();
	setTipParams( filename );
	setTipParameters( tipSettingFile );
}

tipOnePointMol::tipOnePointMol( char* filename, Vec3d zaxis )
// constructor
: tipOnePoint( zaxis ){
	probeMol = new probeMolecule();
	setTipParams( filename );
	setTipParameters( tipSettingFile );
}

void tipOnePointMol::setTipParameters( char* filename ){  // sets parameters of the tip
	char str[400];
	FILE* tipfile = fopen( filename, "r" );
	if( tipfile ){
		printf( "tipOnePoint::setTipParameters: Values loaded from %s.\n", filename );
		readLineComment( tipfile, str ); 		sscanf( str, "%lf", &kcoeff );
		readLineComment( tipfile, str );		sscanf( str, "%lf", &kcoeffrot );
		readLineComment( tipfile, str );		sscanf( str, "%lf", &kcoeffrot2 );
		readLineComment( tipfile, str );		sscanf( str, "%lf", &kcoeffrot3 );
		readLineComment( tipfile, str );		sscanf( str, "%lf %lf %lf", &tipOnePoint::yaxis.x, &tipOnePoint::yaxis.y, &tipOnePoint::yaxis.z );
		yaxis.normalize();
		readLineComment( tipfile, str );		sscanf( str, "%lf %lf %lf %lf", &rotInitAux.x, &rotInitAux.y, &rotInitAux.z, &rotInitAux.w );
		readLineComment( tipfile, str );
		sscanf( str, "%lf %lf %lf", &probeMol->centreMol.x, &probeMol->centreMol.y, &probeMol->centreMol.z );
		fclose( tipfile );
	} else {
		printf( "tipOnePoint::setTipParameters: File %s cannot be opened. Default values used instead.\n", filename );
		kcoeff		= 1e2;
		kcoeffrot	= 1e2;
		kcoeffrot2	= 1e2;
		kcoeffrot3	= 1e2;
		yaxis.set( 1, 0, 0 );
		rotInitAux.setOne();
	}
}

void tipOnePointMol::getForceOrient( Quat4d rot_, Vec3d& force2, Vec3d& force3, Vec3d& vec2, Vec3d& vec3 ){  // return rotating force acting on a multi-atom molecule in an external homogeneous field
	Quat4d rotInit;
	Mat3d M;
	rotInit.setQmul( rotInitAux, rot_ ); // rot_ = rot[tip->getMol()]
	rotInit.normalize();
	rotInit.toMatrix( M );
//	tip->setCoordAxis( rot[tip->getMol()] );
//	drawCoordAxesCross( centre, M.a, M.b, M.c, normID[0] );
	// vertical orientation
	forceSpringRotCoordSys( kcoeffrot2, tipOnePoint::zaxis, M.b, force2 );
	vec2.set( M.b );
	// horizontal orientation
	forceSpringRotCoordSys( kcoeffrot3, tipOnePoint::yaxis, M.c, force3 );
	vec3.set( M.c );
}

Vec3d tipOnePointMol::getForceRad( Vec3d sysCoordAtom, Vec3d& vec ){  // return force acting on a point-like molecule connected to the tip
	Vec3d dR, force;
	dR.set_sub( sysCoordAtom, pos );
	forceSpring( kcoeff, dR, force );
	vec.set( force );
	forceSpringRotCoordSysRadius( kcoeffrot, zaxis, dR, force );
	force.add( vec );
	return force;
}

void tipOnePointMol::renderTip( Vec3d posMol, double radius ){  // render tip graphics
	double bondwidth = 0.1;
	glDeleteLists( tipID, 1 );
	tipID = glGenLists( 1 );
	glNewList( tipID, GL_COMPILE );
		glPushAttrib( GL_CURRENT_BIT );
		glColor4f( 1, 0, 1, 0.5 );
		drawSphere_oct( 5, radius, pos );
		glColor4f( 1, 0.5, 0, 1 );
		glPushAttrib( GL_LINE_BIT );
		glDisable( GL_LIGHTING );
		glLineWidth( 20. );
		glBegin( GL_LINES );
			glVertex3f( posMol.x, posMol.y, posMol.z );
			glVertex3f( pos.x, pos.y, pos.z );
		glEnd();
		glEnable( GL_LIGHTING );
		glPopAttrib();
		glPopAttrib();
	glEndList();
}

void tipOnePointMol::drawTip(){   // draw the tip
	if( tipID > 0 ){  glCallList( tipID ); }
}

void tipOnePointMol::printTip(){  // print parameters
	tipOnePoint::printTip();
	printf( "kcoeff = %lf\n", kcoeff );
	printf( "kcoeffrot, kcoeffrot2, kcoeffrot3 = %lf %lf %lf\n", kcoeffrot, kcoeffrot2, kcoeffrot3 );
	printf( "zaxis = %lf %lf %lf\n", zaxis.x, zaxis.y, zaxis.z );
}

void tipOnePointMol::setTipParams( const char* filename	){  // set parameters pertaining to tip geometry
	char str[400], filenameLoc[400];
	int count;
	FILE* tipFile = fopen( filename, "r" );
	if( tipFile ){
		printf( "setTipParams:\t\tValues loaded from\t\t%s.\n", filename );
		readLineComment( tipFile, str );		count = sscanf( str, "%399s", &filenameLoc );
		probeMol->fileName.assign( filenameLoc );
		readLineComment( tipFile, str );		count = sscanf( str, "%i", &probeMol->atom );
		readLineComment( tipFile, str );
		count = sscanf( str, "%lf %lf %lf", &probeMol->pos.x, &probeMol->pos.y, &probeMol->pos.z );
		readLineComment( tipFile, str );		count = sscanf( str, "%lf %lf %lf %lf", &probeMol->rot.x, &probeMol->rot.y, &probeMol->rot.z, &probeMol->rot.w );
		readLineComment( tipFile, str );		count = sscanf( str, "%399s", tipSettingFile );
		fclose( tipFile );
	} else {
		printf( "setTipParams:\t\tFile %s cannot be opened. Default values used instead.\n", filename );
	}
}

#endif

