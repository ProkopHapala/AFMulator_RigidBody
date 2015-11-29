
#ifndef _DRAW_UTILS_
#define _DRAW_UTILS_

///////////////////////////////////////
//    Geometry Primitives and Shapes
///////////////////////////////////////

void drawBox( float x0, float x1, float y0, float y1, float z0, float z1, float r, float g, float b ){
	glBegin(GL_QUADS);
		glColor3f( r, g, b );		          	     
		glNormal3f(0,0,-1); glVertex3f( x0, y0, z0 ); glVertex3f( x1, y0, z0 ); glVertex3f( x1, y1, z0 ); glVertex3f( x0, y1, z0 ); 
		glNormal3f(0,-1,0); glVertex3f( x0, y0, z0 ); glVertex3f( x1, y0, z0 ); glVertex3f( x1, y0, z1 ); glVertex3f( x0, y0, z1 ); 
		glNormal3f(-1,0,0); glVertex3f( x0, y0, z0 ); glVertex3f( x0, y1, z0 ); glVertex3f( x0, y1, z1 ); glVertex3f( x0, y0, z1 );	
		glNormal3f(0,0,+1); glVertex3f( x1, y1, z1 ); glVertex3f( x0, y1, z1 ); glVertex3f( x0, y0, z1 ); glVertex3f( x1, y0, z1 ); 
		glNormal3f(0,+1,1); glVertex3f( x1, y1, z1 ); glVertex3f( x0, y1, z1 ); glVertex3f( x0, y1, z0 ); glVertex3f( x1, y1, z0 ); 
		glNormal3f(+1,0,0); glVertex3f( x1, y1, z1 ); glVertex3f( x1, y0, z1 ); glVertex3f( x1, y0, z0 ); glVertex3f( x1, y1, z0 );		
	glEnd();
};

int makeBoxList( float x0, float x1, float y0, float y1, float z0, float z1, float r, float g, float b  ){
	int ilist=glGenLists(1);
	glNewList( ilist, GL_COMPILE );
		drawBox( x0, x1, y0, y1, z0, z1, r, g, b );
	glEndList();
	return( ilist );
	// don't forget use glDeleteLists( ilist ,1); later 
}

void drawAxis( float sc ){
	glDisable (GL_LIGHTING);
	glBegin   (GL_LINES);	          	     
		glColor3f( 1, 0, 0 ); glVertex3f( 0, 0, 0 ); glVertex3f( 1*sc, 0, 0 );
		glColor3f( 0, 1, 0 ); glVertex3f( 0, 0, 0 ); glVertex3f( 0, 1*sc, 0 );
		glColor3f( 0, 0, 1 ); glVertex3f( 0, 0, 0 ); glVertex3f( 0, 0, 1*sc );
	glEnd();
}

void drawCoordAxesRot( Vec3d pos, Vec3d vec_a, Vec3d vec_b, Vec3d vec_c, Vec3d vec_d, Vec3d vec_e, GLuint& normIDi ){
// draw coordinate system

	float sc = 1, scm = 10;
	glDeleteLists( normIDi, 1 );
	normIDi = glGenLists( 1 );
	glNewList( normIDi, GL_COMPILE );
	glBegin( GL_LINES );
		// drawing is done in system coordinates!!!       
		glColor3f( 1, 1, 1 ); // white
		glVertex3f( pos.x, pos.y, pos.z ); glVertex3f( scm*vec_a.x + pos.x, scm*vec_a.y + pos.y, scm*vec_a.z + pos.z );
		glColor3f( 1, 1, 0 ); // yellow
		glVertex3f( pos.x, pos.y, pos.z ); glVertex3f( scm*vec_b.x + pos.x, scm*vec_b.y + pos.y, scm*vec_b.z + pos.z );
		glColor3f( 1, 0, 1 ); // magenta
		glVertex3f( pos.x, pos.y, pos.z ); glVertex3f( scm*vec_c.x + pos.x, scm*vec_c.y + pos.y, scm*vec_c.z + pos.z );
		glColor3f( 0, 1, 1 ); // cyan
		glVertex3f( pos.x, pos.y, pos.z ); glVertex3f( sc *vec_d.x + pos.x, sc *vec_d.y + pos.y, sc *vec_d.z + pos.z );
		glColor3f( 0, 0, 0 ); // black
		glVertex3f( pos.x, pos.y, pos.z ); glVertex3f( sc *vec_e.x + pos.x, sc *vec_e.y + pos.y, sc *vec_e.z + pos.z );

	glEnd();
	glEndList();
}

void drawCoordAxesRot( Vec3d pos, Vec3d vec_a, Vec3d vec_b, Vec3d vec_c, GLuint& normIDi, int scm = 10 ){
// draw coordinate system

	glDeleteLists( normIDi, 1 );
	normIDi = glGenLists( 1 );
	glNewList( normIDi, GL_COMPILE );
	glBegin( GL_LINES );
		// drawing is done in system coordinates!!!       
		glColor3f( 1, 1, 1 ); // white
		glVertex3f( pos.x, pos.y, pos.z ); glVertex3f( scm*vec_a.x + pos.x, scm*vec_a.y + pos.y, scm*vec_a.z + pos.z );
		glColor3f( 1, 1, 0 ); // yellow
		glVertex3f( pos.x, pos.y, pos.z ); glVertex3f( scm*vec_b.x + pos.x, scm*vec_b.y + pos.y, scm*vec_b.z + pos.z );
		glColor3f( 1, 0, 1 ); // magenta
		glVertex3f( pos.x, pos.y, pos.z ); glVertex3f( scm*vec_c.x + pos.x, scm*vec_c.y + pos.y, scm*vec_c.z + pos.z );
	glEnd();
	glEndList();
}

void drawCoordAxesCross( Vec3d pos, Vec3d vec_a, Vec3d vec_b, Vec3d vec_c, GLuint& normIDi, int scm = 5 ){
// draw coordinate system

	glDeleteLists( normIDi, 1 );
	normIDi = glGenLists( 1 );
	glNewList( normIDi, GL_COMPILE );
	glBegin( GL_LINES );
		// drawing is done in system coordinates!!!       
		glColor3f( 1, 1, 1 ); // white
		glVertex3f( pos.x, pos.y, pos.z ); glVertex3f(  scm*vec_a.x + pos.x,  scm*vec_a.y + pos.y,  scm*vec_a.z + pos.z );
		glVertex3f( pos.x, pos.y, pos.z ); glVertex3f( -scm*vec_a.x + pos.x, -scm*vec_a.y + pos.y, -scm*vec_a.z + pos.z );
		glColor3f( 1, 1, 0 ); // yellow
		glVertex3f( pos.x, pos.y, pos.z ); glVertex3f(  scm*vec_b.x + pos.x,  scm*vec_b.y + pos.y,  scm*vec_b.z + pos.z );
		glVertex3f( pos.x, pos.y, pos.z ); glVertex3f( -scm*vec_b.x + pos.x, -scm*vec_b.y + pos.y, -scm*vec_b.z + pos.z );
		glColor3f( 1, 0, 1 ); // magenta
		glVertex3f( pos.x, pos.y, pos.z ); glVertex3f(  scm*vec_c.x + pos.x,  scm*vec_c.y + pos.y,  scm*vec_c.z + pos.z );
		glVertex3f( pos.x, pos.y, pos.z ); glVertex3f( -scm*vec_c.x + pos.x, -scm*vec_c.y + pos.y, -scm*vec_c.z + pos.z );
	glEnd();
	glEndList();
}

#endif
