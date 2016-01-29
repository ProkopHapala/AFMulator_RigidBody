
#ifndef _SCREEN_
#define _SCREEN_

class Screen{ // one screen of the program
	public:

	// ============= CONSTANTS AND VARIABLES
	static constexpr float VIEW_ZOOM_STEP     = 1.2f;
	static constexpr float VIEW_ZOOM_DEFAULT  = 10.0f;
	static constexpr float VIEW_DEPTH_DEFAULT = 1000.0;

	// ============= CONSTANTS AND VARIABLES

	// geometry constants and variables
	int   WIDTH,HEIGHT;
	float VIEW_DEPTH,ASPECT_RATIO;

	float zoom;

	// rotations

	Quat4f QCamera;				// quaternion defining the total rotation of the camera
	Quat4f QCameraLast; 			// quaternion defining the last rotation of the camera

	bool  mouse_spinning;			// if true the screen is supposed to move according the mouse movements
	float mouse_begin_x ;			// mouse x-position where the last rotation began
	float mouse_begin_y ;			// mouse y-position where the last rotation began

	// camera position
	Vec3d camDir, camUp, camRight;		// axes of the camera coordinates
	enum AXES_VIEW {			// predefined viewpoints on the screen
		AXIS_NONSPEC,
		AXIS_X, AXIS_Y, AXIS_Z,
		AXIS_W
	};
	AXES_VIEW axisView = AXIS_NONSPEC;	// current viewpoint

	// window
	int screenID;
	bool hasFocus;
	SDL_Window*      window;
	SDL_Renderer*    renderer;

	// keys and mouse
	int mouseX, mouseY;				         // mouse position on the screen
	bool shiftPressed		     = false;    // left SHIFT key is pressed
	bool ctrlPressed		     = false;    // left CTRL key is pressed
	bool leftMouseButtonPressed	 = false;    // left mouse button is pressed
	bool rightMouseButtonPressed = false;    // right mouse button is pressed

	// context menu
	enum contextMenuType {			// context menu type
		CONT_MENU_NONE,				// no buttons enabled
		CONT_MENU_SELECTION,		// when a context menu is invoked by clicking on a selection of atoms
		CONT_MENU_MOLECULE,			// when a context menu is invoked by clicking on a single atom in a molecule
		CONT_MENU_DIAL
	};
	float menuCornerX, menuCornerY, menuCornerZ;   // context menu position
	contextMenu* contMenu = new contextMenu;       // context menu
	bool contMenuExists   = false;                 // if true the context menu is created

	contextMenuSetting contMenuSettingSelection;   // context menu while selection is chosen
	contextMenuSetting contMenuSettingMolecule;    // context menu while single atom is chosen
	contextMenuSetting contMenuSettingTest;
	contextMenuSetting contMenuSettingDial;

	// openGLObjects and world
	//PhysicalSystemEditor* world;
	GLuint scanningBox = 0, measuringScalePerm = 0, measuringScaleTemp = 0;

	// ============= FUNCTIONS

	//setup
	//void initRenderer();
	void setupRenderer();
	void setDefaults();

	// screen
	void updateScreen( PhysicalSystemEditor* world );
	void drawScreen( PhysicalSystemEditor* world );

	// camera
	void camera();
	void mouseCamera( float x, float y );
	void setAxisView( AXES_VIEW axis );
	void getCameraDirections();

	// miscellaneous
	void rescaleMousePositions( float & mouseA, float & mouseB );
	void oneActionBack        ( PhysicalSystemEditor* world );
	void renderMeasuringScale ( Vec3d origin, Vec3d end, GLuint& scaleID );
	void renderScanningBox    ( float xoffset, float yoffset, float zoffset, float xstep, float ystep, float zstep, int xdim, int ydim, int zdim );
	void renderScanningBox    ( scanSpecification* scan );

	// rotations
	void startSpinning( float x, float y );
	void endSpinning();

	// atom picking and moving
	bool mousePickAtom    ( PhysicalSystemEditor* world );
	bool mousePickAtom    ( PhysicalSystemEditor* world, bool emphasized );
	bool mouseMoveAtom    ( PhysicalSystemEditor* world );
	bool mouseMoveMolecule( PhysicalSystemEditor* world );
	void mouseSetAuxPoint ( PhysicalSystemEditor* world );

	// context menu
	void initializeContextMenu();
	void setContextMenuTypes  ();
	void createContextMenuFromSetting( contextMenuType type );
	void drawContextMenu      ();
	void destroyContextMenu   ();
	void rescaleContextMenu( float rescaleFactor );

	// selection of atoms
	void addPickedAtom   ( PhysicalSystemEditor* world );
	bool selectionChosen ( PhysicalSystemEditor* world );
	bool destroySelection( PhysicalSystemEditor* world );

	// event handling
	void inputHandlingBare    ( SDL_Event event, PhysicalSystemEditor* world, bool & loopEnd );
	void handleKeyDown        ( SDL_Event event, PhysicalSystemEditor* world );
	void handleMouseMotion    ( SDL_Event event, PhysicalSystemEditor* world );
	void handleMouseButtonDown( SDL_Event event, PhysicalSystemEditor* world );
	void handleMouseButtonUp  ( SDL_Event event, PhysicalSystemEditor* world );
	void handleMouseAndKeyStates();
	void handleMouseWheel     ( SDL_Event event, PhysicalSystemEditor* world );

	// ==== construnctor & destructor
	Screen ( int& id, int WIDTH_, int HEIGHT_ );
	Screen (){};
	~Screen();
};

// ============== ROTATIONS ==============

void Screen::startSpinning( float x, float y ){
	mouse_spinning = true;
	mouse_begin_x  = x;
	mouse_begin_y  = y;
}

void Screen::endSpinning(){	mouse_spinning = false; }

// ============== SETUP FUNCTIONS AND DESTRUCTOR ==============

void Screen::setDefaults(){
	VIEW_DEPTH   = VIEW_DEPTH_DEFAULT;
	ASPECT_RATIO = WIDTH / float( HEIGHT );
	zoom 	     = VIEW_ZOOM_DEFAULT;
	mouse_spinning = false;
	mouse_begin_x  = 0;
	mouse_begin_y  = 0;
	//printf( "zoom, ASPECT_RATIO, VIEW_DEPTH = %f, %f, %f \n", zoom, ASPECT_RATIO, VIEW_DEPTH );
}



Screen::Screen( int& id, int WIDTH_, int HEIGHT_ ){
	char str[40];
	WIDTH  = WIDTH_;
	HEIGHT = HEIGHT_;
	setDefaults();
	SDL_CreateWindowAndRenderer( WIDTH, HEIGHT, SDL_WINDOW_OPENGL, &window, &renderer );
	id = SDL_GetWindowID(window);
	screenID = id;
	sprintf( str, "Window id = %d\n", id );
	SDL_SetWindowTitle( window, str );
	setupRenderer();
	QCamera.trackballQ( 0, 0, 0, 0 );
	initializeContextMenu();
}

void Screen::setupRenderer(){ // setup renderer
	float ambient  [] = { 0.1f, 0.15f, 0.25f, 1.0f };
	float diffuse  [] = { 0.9f, 0.8f,  0.7f,  1.0f };
	float specular [] = { 1.0f, 1.0f,  1.0f,  1.0f };
	float shininess[] = { 80.0f                    };
	float lightPos [] = { 1.0f, 1.0f, +1.0f, 0.0f  };
	glMaterialfv ( GL_FRONT_AND_BACK, GL_AMBIENT,   ambient );
	glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE,   diffuse );
	glMaterialfv ( GL_FRONT_AND_BACK, GL_SPECULAR,  specular );
	glMaterialfv ( GL_FRONT_AND_BACK, GL_SHININESS, shininess );
	glEnable     ( GL_COLOR_MATERIAL );
	glLightfv    ( GL_LIGHT0, GL_POSITION, lightPos );
	glLightfv    ( GL_LIGHT0, GL_AMBIENT, ambient );
	glEnable     ( GL_LIGHTING	);
	glEnable     ( GL_LIGHT0	);
	glEnable     ( GL_NORMALIZE	);
	glEnable     ( GL_DEPTH_TEST	);
	glHint       ( GL_LINE_SMOOTH_HINT, GL_NICEST );
	glShadeModel ( GL_SMOOTH	);
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

Screen::~Screen(){
	//delete window;
	//delete renderer;
	contMenu->terminateMenu();
	contMenuSettingSelection.destroy();
	contMenuSettingMolecule.destroy();
	contMenuSettingTest.destroy();
	contMenuSettingDial.destroy();
}

// ============== SCREEN ==============

void Screen::updateScreen( PhysicalSystemEditor* world ){ // update the screen scene
	// if shift is hold and an atom is hold, move an atom
	if( shiftPressed && world->getAtomHold() && !world->getMolHold() ){
		mouseMoveAtom( world );
	}
	if( shiftPressed && world->getMolHold() ){
		mouseMoveMolecule( world );
	}
	camera();                 // set a camera position
	getCameraDirections();   // store the camera position in appropriate variables
	glClearColor( 0.5f, 0.5f, 0.5f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

//void Screen::drawScreen( PhysicalSystemEditor* world ){
//// draw the screen
//	glEnable( GL_LIGHTING );
//	glShadeModel( GL_FLAT );
//	glColor3f( 0.2f, 0.8f, 0.2f );
//	for( int i = 0; i < openGLObjects->n; i++ ){
//		int obj = openGLObjects->data[i];
//		if( obj > 0 ){
//			glCallList( obj );
//		}
//	}
//	// draw molecules
//	world->draw();
//	// we do not want a context menu to reflect light, so disable lighting
//	glDisable( GL_LIGHTING );
//	// if a context menu exists, draw it
//	if( contMenuExists ){
//		drawContextMenu();
//	}
//	// draw axes
//	drawAxis( 10 );
//	SDL_RenderPresent( renderer );
//}

void Screen::drawScreen( PhysicalSystemEditor* world ){  // draw the screen
	glEnable( GL_LIGHTING );
	glShadeModel( GL_FLAT );
	glColor3f( 0.2f, 0.8f, 0.2f );
	world->draw();   // draw molecules
	glDisable( GL_LIGHTING );  	// we do not want a context menu to reflect light, so disable lighting
	if( contMenuExists ){   // if a context menu exists, draw it
		drawContextMenu();
	}
	glCallList( scanningBox );
	glCallList( measuringScalePerm );
	glCallList( measuringScaleTemp );
	drawAxis( 10 );  // draw axes
	SDL_RenderPresent( renderer );
}

// ============== CAMERA ==============

// store in QCameraLast the last rotation of the system done by mouse
// store in QCamera a total rotation of the system done by mouse since the start of the application
void Screen::mouseCamera( float x, float y ){
	if( mouse_spinning ){
		// calculate the last rotation done by mouse
		QCameraLast.trackballQ(
			(2.0f*mouse_begin_x - WIDTH) / WIDTH, (HEIGHT - 2.0f*mouse_begin_y) / HEIGHT,
			(2.0f*x - WIDTH            ) / WIDTH, (HEIGHT - 2.0f*y            ) / HEIGHT
		);
		// to the total rotation of the system add the last rotation
		QCamera.qmul( QCameraLast );
		QCamera.normalize();
		// reset the origin w.r.t. which new rotation is calculated
		mouse_begin_x = x; mouse_begin_y = y;
	}
}

// choose a predefined viewpoint
void Screen::setAxisView( AXES_VIEW axis ){	axisView = axis; }

void Screen::getCameraDirections(){  // store position of a camera w.r.t. the screen in camRight, camUp and camDir
	float mat[4][4];
	glGetFloatv( GL_MODELVIEW_MATRIX, &mat[0][0] );
	camRight.set( mat[0][0], mat[1][0], mat[2][0] );
	camUp   .set( mat[0][1], mat[1][1], mat[2][1] );
	camDir  .set( mat[0][2], mat[1][2], mat[2][2] );
	camDir.mul( -1 ); // for some reason it is inverted

}

void Screen::camera(){  // set a viewpoint of a camera
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( -zoom*ASPECT_RATIO, zoom*ASPECT_RATIO, -zoom, zoom, -VIEW_DEPTH, +VIEW_DEPTH );
	// if a predefined viewpoint is chosen by the user, set QCamera accordingly
	if( axisView != AXIS_NONSPEC ){
		switch( axisView ){
			case AXIS_X: // viewpoint parallel to the x-axis
				QCamera.set( 1.d/2, 1.d/2, 1.d/2, 1.d/2 );
				break;
			case AXIS_Y: // viewpoint parallel to the y-axis
				QCamera.set( 0, 1/sqrt(2), 1/sqrt(2), 0 );
				break;
			case AXIS_Z: // viewpoint parallel to the z-axis
				QCamera.set( 0, 0, 0, 1.d );
				break;
			case AXIS_W: // viewpoint not parallel to any axis
				QCamera.set( 0.146447, 0.353553, 0.853553, 0.353553 );
				break;
		}
	}
	setAxisView( AXIS_NONSPEC );     // reset a viewpoint
	glMatrixMode( GL_MODELVIEW );    // rotate the camera according to QCamera
	float camMatrix[4][4];
	QCamera.build_rotmatrix( camMatrix );
	glLoadMatrixf( &camMatrix[0][0] );
}

// ============== MISCELLANEOUS ==============

// if a user wants to go one action (i.e. movement of an atom) backward, do it
void Screen::oneActionBack( PhysicalSystemEditor* world ){  world->resetOneAtomMove();  }

// rescale a mouse position on the screen
void Screen::rescaleMousePositions( float& mouseA, float& mouseB ){
	mouseA = 2 * zoom * ( mouseX / float( HEIGHT ) - 0.5 * ASPECT_RATIO );
	mouseB = 2 * zoom * ( 0.5 - mouseY / float( HEIGHT ) );
}

// render measuring scale
void Screen::renderMeasuringScale( Vec3d origin, Vec3d end, GLuint& scaleID ){
	const double unit = 1.;
	const double tickHalfLenght = .2;
	int num;
	Vec3d dirVec, dirAux, dirPerp, dirPerp2;
	dirAux.set( 1, 0, 0 );
	dirVec.set_sub( end, origin );
	num = (int) floor( dirVec.norm() / (double) unit  );
	dirVec.normalize();
	//printf( "dirVec.norm() = %f\n", dirVec.norm() );
	dirPerp.set_cross( dirVec, dirAux );
	if( dirPerp.norm() < 1e-5 ){
		dirAux.set( 0, 1, 0 );
		dirPerp.set_cross( dirVec, dirAux );
	}
	dirPerp.normalize();
	dirPerp2.set_cross( dirVec, dirPerp );
	dirPerp2.normalize();
	dirPerp.mul( tickHalfLenght );
	dirPerp2.mul( tickHalfLenght );
	scaleID = glGenLists( 1 );
	//printf( "num = %i,\tdirPerp.norm() = %f\n", num, dirPerp.norm() );
	glNewList( scaleID, GL_COMPILE );
		glPushAttrib( GL_LINE_BIT );
		glColor3f( 1, 1, 1 );
		glLineWidth( 3. );
		glBegin( GL_LINES ); // axis
			glVertex3f( origin.x, origin.y, origin.z ); glVertex3f( end.x, end.y, end.z );
		glEnd();
		glLineWidth( 1. );
		glBegin( GL_LINES ); // ticks
			for( int i = 0; i < num; i++ ){
				glVertex3f( origin.x + i*dirVec.x + dirPerp.x, origin.y + i*dirVec.y + dirPerp.y, origin.z + i*dirVec.z + dirPerp.z );
				glVertex3f( origin.x + i*dirVec.x - dirPerp.x, origin.y + i*dirVec.y - dirPerp.y, origin.z + i*dirVec.z - dirPerp.z );
				glVertex3f( origin.x + i*dirVec.x + dirPerp2.x, origin.y + i*dirVec.y + dirPerp2.y, origin.z + i*dirVec.z + dirPerp2.z );
				glVertex3f( origin.x + i*dirVec.x - dirPerp2.x, origin.y + i*dirVec.y - dirPerp2.y, origin.z + i*dirVec.z - dirPerp2.z );
			}
		glEnd();
		glPopAttrib();
	glEndList();
}

void Screen::renderScanningBox( scanSpecification* scan ){
	renderScanningBox( scan->xoffset, scan->yoffset, scan->zoffset, scan->xstep, scan->ystep, scan->zstep, scan->xdim, scan->ydim, scan->zdim );
}

// render scanning box demonstrating the region of the system to be scanned
void Screen::renderScanningBox( float xoffset, float yoffset, float zoffset, float xstep, float ystep, float zstep, int xdim, int ydim, int zdim ){
	float z_low = 0;
	scanningBox = glGenLists( 1 );
	glNewList( scanningBox, GL_COMPILE );
		glPushAttrib( GL_LINE_BIT );
		glColor3f( 0, 0, 0 );
		glLineWidth( 3. );
		glBegin( GL_LINE_LOOP );
			glVertex3f( xoffset, yoffset, zoffset );
			glVertex3f( xoffset + xstep*xdim, yoffset, zoffset );
			glVertex3f( xoffset + xstep*xdim, yoffset + ystep*ydim, zoffset );
			glVertex3f( xoffset, yoffset + ystep*ydim, zoffset );
		glEnd();
//		glBegin( GL_LINE_LOOP );
//			glVertex3f( xoffset, yoffset, zoffset + zstep*zdim );
//			glVertex3f( xoffset + xstep*xdim, yoffset, zoffset + zstep*zdim );
//			glVertex3f( xoffset + xstep*xdim, yoffset + ystep*ydim, zoffset + zstep*zdim );
//			glVertex3f( xoffset, yoffset + ystep*ydim, zoffset + zstep*zdim );
//		glEnd();
//		glBegin( GL_LINES );
//			glVertex3f( xoffset, yoffset, zoffset );
//			glVertex3f( xoffset, yoffset, zoffset + zstep*zdim );
//
//			glVertex3f( xoffset + xstep*xdim, yoffset, zoffset );
//			glVertex3f( xoffset + xstep*xdim, yoffset, zoffset + zstep*zdim );

//			glVertex3f( xoffset + xstep*xdim, yoffset + ystep*ydim, zoffset );
//			glVertex3f( xoffset + xstep*xdim, yoffset + ystep*ydim, zoffset + zstep*zdim );

//			glVertex3f( xoffset, yoffset + ystep*ydim, zoffset );
//			glVertex3f( xoffset, yoffset + ystep*ydim, zoffset + zstep*zdim );
//		glEnd();
		glBegin( GL_LINE_LOOP );
			glVertex3f( xoffset, yoffset, zoffset - zstep*zdim );
			glVertex3f( xoffset + xstep*xdim, yoffset, zoffset - zstep*zdim );
			glVertex3f( xoffset + xstep*xdim, yoffset + ystep*ydim, zoffset - zstep*zdim );
			glVertex3f( xoffset, yoffset + ystep*ydim, zoffset - zstep*zdim );
		glEnd();
		glBegin( GL_LINES );
			glVertex3f( xoffset, yoffset, zoffset );
			glVertex3f( xoffset, yoffset, zoffset - zstep*zdim );
			glVertex3f( xoffset + xstep*xdim, yoffset, zoffset );
			glVertex3f( xoffset + xstep*xdim, yoffset, zoffset - zstep*zdim );
			glVertex3f( xoffset + xstep*xdim, yoffset + ystep*ydim, zoffset );
			glVertex3f( xoffset + xstep*xdim, yoffset + ystep*ydim, zoffset - zstep*zdim );
			glVertex3f( xoffset, yoffset + ystep*ydim, zoffset );
			glVertex3f( xoffset, yoffset + ystep*ydim, zoffset - zstep*zdim );
		glEnd();
		glBegin( GL_LINE_LOOP );
			glVertex3f( xoffset, yoffset, z_low );
			glVertex3f( xoffset + xstep*xdim, yoffset, z_low );
			glVertex3f( xoffset + xstep*xdim, yoffset + ystep*ydim, z_low );
			glVertex3f( xoffset, yoffset + ystep*ydim, z_low );
		glEnd();
		glBegin( GL_LINES );
			glVertex3f( xoffset, yoffset, zoffset );
			glVertex3f( xoffset, yoffset, z_low );
			glVertex3f( xoffset + xstep*xdim, yoffset, zoffset );
			glVertex3f( xoffset + xstep*xdim, yoffset, z_low );
			glVertex3f( xoffset + xstep*xdim, yoffset + ystep*ydim, zoffset );
			glVertex3f( xoffset + xstep*xdim, yoffset + ystep*ydim, z_low );
			glVertex3f( xoffset, yoffset + ystep*ydim, zoffset );
			glVertex3f( xoffset, yoffset + ystep*ydim, z_low );
		glEnd();
		glPopAttrib();
	glEndList();
}


// ============== ATOM PICKING AND MOVING ==============

// pick an atom providing the mouse clicks on it
bool Screen::mousePickAtom( PhysicalSystemEditor* world ){
	Vec3d mouseCoords, a, b, c;
	float mouseA, mouseB;
	rescaleMousePositions  ( mouseA, mouseB );                        // rescale mouse positions on a screen
	mouseCoords.set_lincomb( mouseA, camRight, mouseB, camUp );       // set to mouseCoords the position of a mouse in the system coordinates (without the z-component)
	bool found = world->rayPickAtomSet( mouseCoords, camDir, true );  // mouse picking itself
	return found;
}

bool Screen::mousePickAtom( PhysicalSystemEditor* world, bool emphasized ){  // pick an atom providing the mouse clicks on it
	Vec3d mouseCoords, a, b, c;
	float mouseA, mouseB;
	rescaleMousePositions( mouseA, mouseB );                                // rescale mouse positions on a screen
	mouseCoords.set_lincomb( mouseA, camRight, mouseB, camUp );             // set to mouseCoords the position of a mouse in the system coordinates (without the z-component)
	bool found = world->rayPickAtomSet( mouseCoords, camDir, emphasized );  // mouse picking itself
	return found;
}

bool Screen::mouseMoveAtom( PhysicalSystemEditor* world ){  // move a picked atom where the mouse is
	Vec3d atomSystemCoords, atomInSystemPos;
	float mouseA, mouseB;
	rescaleMousePositions( mouseA, mouseB );  // rescale mouse positions on a screen
	// in atomInSystemPos store the original position of the picked atom expressed in system coordinates
	world->moleculeToSystemCoords( world->pos[world->getMolToDraw()], world->rot[world->getMolToDraw()], 1, world->atomToDrawOrigPos, &atomInSystemPos );
	// get the z-coordinate of atomInSystemPos in camera coordinates
	float z_coord = camDir.x*atomInSystemPos.x + camDir.y*atomInSystemPos.y + camDir.z*atomInSystemPos.z;
	// in atomSystemCoords store a new position of the moved atom expressed in system coordinates
	// z-coordinate is chosen in such a way that moving of the atom by mouse is done in a plane parallel to the screen
	atomSystemCoords.set_lincomb( mouseA, mouseB, z_coord, camRight, camUp, camDir );
	bool found = world->moveAtom( atomSystemCoords );  // moving itself
	//bool found = false;
	return found;
}

bool Screen::mouseMoveMolecule( PhysicalSystemEditor* world ){  // move a picked molecule where the mouse is
	Vec3d molSystemCoords, molInSystemPos, atomSystemCoords, atomInSystemPos, vec_diff;
	float mouseA, mouseB, z_coord;
	bool found;
	rescaleMousePositions( mouseA, mouseB );  // rescale mouse positions on a screen
	// in atomInSystemPos store the original position of the picked atom expressed in system coordinates
	world->moleculeToSystemCoords( world->pos[world->getMolToDraw()], world->rot[world->getMolToDraw()], 1, world->atomToDrawOrigPos, &atomInSystemPos );
	// get the z-coordinate of atomInSystemPos in camera coordinates
	z_coord = camDir.x*atomInSystemPos.x + camDir.y*atomInSystemPos.y + camDir.z*atomInSystemPos.z;
	// in atomSystemCoords store a new position of the moved atom expressed in system coordinates
	// z-coordinate is chosen in such a way that moving of the molecule by mouse is done in a plane parallel to the screen
	atomSystemCoords.set_lincomb( mouseA, mouseB, z_coord, camRight, camUp, camDir );
	// calculate translational vector and add this vector to the molecule position
	vec_diff.set_sub( atomSystemCoords, atomInSystemPos );
	world->pos[world->getMolToDraw()].add( vec_diff );
	return true;
}

void Screen::mouseSetAuxPoint( PhysicalSystemEditor* world ){   // move a picked molecule where the mouse is
	float mouseA, mouseB, z_coord;
	rescaleMousePositions( mouseA, mouseB );   // rescale mouse positions on a screen
	world->mouseSetAuxPoint( mouseA, mouseB, camRight, camUp, camDir );   // set auxiliary point
}

// ============== SELECTION OF ATOMS ==============

bool Screen::selectionChosen( PhysicalSystemEditor* world ){   // is the selection of atoms picked by a mouse?
	return world->selectionChosen();
}

bool Screen::destroySelection( PhysicalSystemEditor* world ){  // destroy the selection of atoms
	return world->destroySelection();
}

void Screen::addPickedAtom( PhysicalSystemEditor* world ){  // add a picked atom
	char str[40];
	if( world->addPickedAtomDrawLink() ){
		createContextMenuFromSetting( CONT_MENU_DIAL );
		sprintf( str, "%5.3lf", world->distance );
		contMenu->redrawTexture( button::BUTTON_DIAL, str );
	}
}

// ============== CONTEXT MENU ==============

// set types of context menus
// this procedure sets all the necessary properties of context menus used in the application
// this procedure should be used as the only access point to the context menu settings relevant for current screen
// contextMenuSetting structure is the carrier of the setting
void Screen::setContextMenuTypes(){
	int numOfButtons_selection = 6;
	int numOfButtons_molecule  = 1;
	bool activity_selection[] = { true, true, true, true, true, true };
	bool activity_molecule[]  = { true };
	button::buttonType attr_selection[] = { button::BUTTON_EXPORT, button::BUTTON_ADJUST3, button::BUTTON_ADJUST, button::BUTTON_ADJUST_X, button::BUTTON_ADJUST_Y, button::BUTTON_ADJUST_Z };
	button::buttonType attr_molecule[]  = { button::BUTTON_ADJUST };
	contMenuSettingSelection.set( numOfButtons_selection, activity_selection, attr_selection );
	contMenuSettingMolecule.set( numOfButtons_molecule, activity_molecule, attr_molecule );
	if( !contMenuSettingTest.set( "InputData/test.contmenu" ) ){
		printf( "setContextMenuTypes: Unsuccesssful loading of contMenuSettingTest.\n" );
	}
	if( !contMenuSettingDial.set( "InputData/dial.contmenu" ) ){
		printf( "setContextMenuTypes: Unsuccesssful loading of contMenuSettingDial.\n" );
	}
}

void Screen::initializeContextMenu(){   // initilize a context menu at the beginning of the application
	int numOfTextures = 8;
	setContextMenuTypes();
	contMenu->initializeMenu( numOfTextures, false );
}

void Screen::createContextMenuFromSetting( contextMenuType type ){   // show a context menu on the screen
	// set position of the context menu
	rescaleMousePositions( menuCornerX, menuCornerY );
	menuCornerZ = 300.; // some large number in order to be always on top of everything else
	contextMenuSetting contMenuSetting;
	// set context menu type
	switch( type ){
		case CONT_MENU_SELECTION:
//			contMenuSetting.set( contMenuSettingSelection );
			contMenuSetting.set( contMenuSettingTest );
			break;
		case CONT_MENU_MOLECULE:
			contMenuSetting.set( contMenuSettingMolecule );
			break;
		case CONT_MENU_DIAL:
			contMenuSetting.set( contMenuSettingDial );
			break;
		default:
			break;
	}
	// create a context menu
	rescaleContextMenu( zoom );
	contMenu->createMenu( &contMenuSetting, menuCornerX, menuCornerY, menuCornerZ );
	contMenuExists = true;
}

void Screen::drawContextMenu   (){  contMenu->drawMenu();  }

void Screen::destroyContextMenu(){   contMenu->destroyMenu();   }

void Screen::rescaleContextMenu( float rescaleFactor ){
	float constResc = 0.1;
	contMenu->setMenuWidth   ( contMenu->getMenuWidthDefault()*rescaleFactor*constResc );
	contMenu->setButtonHeight( contMenu->getButtonHeightDefault()*rescaleFactor*constResc );
}

// ============== EVENT HANDLING ==============

void Screen::inputHandlingBare( SDL_Event event, PhysicalSystemEditor* world, bool & loopEnd ){  // handle the input events
	switch( event.type ){
		case SDL_KEYDOWN:			handleKeyDown( event, world );			break;
		case SDL_MOUSEMOTION:		handleMouseMotion( event, world );		break;
		case SDL_MOUSEBUTTONDOWN:	handleMouseButtonDown( event, world );	break;
		case SDL_MOUSEBUTTONUP:		handleMouseButtonUp( event, world );	break;
		case SDL_MOUSEWHEEL:		handleMouseWheel( event, world );		break;
	}
	handleMouseAndKeyStates();
}

void Screen::handleKeyDown( SDL_Event event, PhysicalSystemEditor* world ){  // handle an event when some key is pressed down
	destroyContextMenu();
	switch( event.key.keysym.sym ){
		case SDLK_KP_MINUS:		zoom *= VIEW_ZOOM_STEP;	break;
		case SDLK_KP_PLUS:		zoom /= VIEW_ZOOM_STEP;	break;
		case SDLK_BACKSPACE:	oneActionBack( world );	break;
		case SDLK_UP:			setAxisView( AXIS_Z );	break;
		case SDLK_LEFT:			setAxisView( AXIS_Y );	break;
		case SDLK_RIGHT:		setAxisView( AXIS_X );	break;
		case SDLK_DOWN:			setAxisView( AXIS_W );	break;

//		// following used in replay mode
//		case SDLK_KP_4://		replayMoveLeft();//			break;
//		case SDLK_KP_6://		replayMoveRight();//		break;
//		case SDLK_KP_2://		replayMoveDown();//			break;
//		case SDLK_KP_8://		replayMoveUp();//			break;
//		case SDLK_e://			world->exportData();//		break;

	}
}

void Screen::handleMouseMotion( SDL_Event event, PhysicalSystemEditor* world ){   // handle an event when the mouse moves
	if( !shiftPressed && !rightMouseButtonPressed && !contMenuExists ){
		mouseCamera( event.motion.x, event.motion.y );
	}
	if( contMenuExists ){
		float mouseA, mouseB;
		rescaleMousePositions( mouseA, mouseB );
		contMenu->moveOverMenu( mouseA, mouseB );
	}
}

void Screen::handleMouseButtonUp( SDL_Event event, PhysicalSystemEditor* world ){  // handle an event when the mouse button is released
	if( event.button.button == SDL_BUTTON_LEFT ){
		leftMouseButtonPressed = false;
		endSpinning();
		world->setAtomHold( false );
		world->setMolHold( false );
	}
	if( event.button.button == SDL_BUTTON_RIGHT ){
		rightMouseButtonPressed = false;
		world->setMolHold( false );
		world->setAtomHold( false );
		world->mouseTip = false;
	}
}

void Screen::handleMouseButtonDown( SDL_Event event, PhysicalSystemEditor* world ){  // handle an event when the mouse button is pressed down
	setAxisView( AXIS_NONSPEC );
	float mouseA, mouseB;
	rescaleMousePositions( mouseA, mouseB );
	// left mouse button
//	if( event.button.button == SDL_BUTTON_LEFT ){
	if( event.button.button == SDL_BUTTON_LEFT && !rightMouseButtonPressed ){
		leftMouseButtonPressed = true;
		if( contMenuExists ){
			if( contMenu->isActivated( mouseA, mouseB ) ){
				contMenu->clickOnMenu( mouseA, mouseB );
				switch( contMenu->getAction() ){
					case contextMenu::ACTION_EXPORT:		world->exportData();							break;
					case contextMenu::ACTION_ADJUST:		world->setFromStdinAdjustMolecule();			break;
					case contextMenu::ACTION_ADJUST3:		world->setFromStdinAdjustMoleculeSelection();	break;
					case contextMenu::ACTION_ADJUST_X:		world->adjustMoleculeSelectionXaxis();			break;
					case contextMenu::ACTION_ADJUST_Y:		world->adjustMoleculeSelectionYaxis();			break;
					case contextMenu::ACTION_ADJUST_Z:		world->adjustMoleculeSelectionZaxis();			break;
					//case contextMenu::ACTION_DIAL:						contMenu->redrawTexture( button::BUTTON_DIAL, floatToString( distance ) );	break;
				}
			} else {
				destroyContextMenu();
				contMenuExists = false;
				startSpinning( event.button.x, event.button.y );
			}
		} else {
			if( !ctrlPressed ){
				destroySelection( world );
				startSpinning( event.button.x, event.button.y );
			}
			world->setAtomHold( mousePickAtom( world, true ) );
			if( world->getAtomHold() ){
				if( ctrlPressed && !shiftPressed ){
					addPickedAtom( world );
				}
			}
		}
	}

	// right mouse button
	//if( event.button.button == SDL_BUTTON_RIGHT ){
	if( event.button.button == SDL_BUTTON_RIGHT && !leftMouseButtonPressed  ){
		rightMouseButtonPressed = true;
		if( contMenuExists && !contMenu->isActivated( mouseA, mouseB ) ){
			contMenu->isActivated( mouseA, mouseB );
		}
		world->setAtomHold( mousePickAtom( world, true ) );
		if( world->getAtomHold() ){
			if( shiftPressed ){
				if( ctrlPressed ){
					mouseSetAuxPoint( world );
					world->mouseTip = true;
					world->setAtomHold( false );
				} else {
					world->setMolHold( true );
				}
			} else {
				if( selectionChosen( world ) ){
					createContextMenuFromSetting( CONT_MENU_SELECTION );
				} else {
					createContextMenuFromSetting( CONT_MENU_MOLECULE );
					world->setAtomHold( false );
				}
			}
		}
	}
}

void Screen::handleMouseAndKeyStates(){   // handle an event when some keys are hold pressed
	SDL_GetMouseState( &mouseX, &mouseY );
	const Uint8* keyboardState = SDL_GetKeyboardState( NULL );
	keyboardState[SDL_SCANCODE_LSHIFT] ? shiftPressed = true : shiftPressed = false;
	keyboardState[SDL_SCANCODE_LCTRL]  ? ctrlPressed  = true : ctrlPressed  = false;
}

void Screen::handleMouseWheel( SDL_Event event, PhysicalSystemEditor* world ){ // handle an event when a mouse wheel is rotated
	destroyContextMenu();
	if( event.wheel.y > 0 ){
		zoom /= VIEW_ZOOM_STEP;
	} else if( event.wheel.y < 0 ){
		zoom *= VIEW_ZOOM_STEP;
	}
	rescaleContextMenu( zoom );
}

#endif
