
class button {

	public:
		
		button();
		~button();
		
		enum buttonType{
		
			BUTTON_NONE,
			BUTTON_EXPORT,
			BUTTON_ADJUST,
			BUTTON_ADJUST3,
			BUTTON_ADJUST_X,
			BUTTON_ADJUST_Y,
			BUTTON_ADJUST_Z,
			BUTTON_DIAL
			
		};

		void createButton( float tlcornerX, float tlcornerY, float tlcornerZ, float w, float h, bool enabled_, buttonType attr, GLuint* textures );
		void destroyButton();
		void setButtonPosition( float x, float y, float z );
		void setButtonWidth( float buttonWidth_ );
		void setButtonHeight( float buttonHeight_ );
		
		void moveOverButton( float mousePosX, float mousePosY );
		bool clickOnButton( float mousePosX, float mousePosY );
		void drawButton();
		
		bool isActivated();
		bool isClicked();
		
		void setDefaultColor();
		void setActiveColor();
		void setClickColor();
		void setDisabledColor();
		void setDefaultColorActivity();
		
		void setActivity( bool enabled_ );
		void setAttributeTex( button::buttonType attribute, GLuint* textures );
		void renderButton();
	
	private:
	
		float buttonX, buttonY, buttonZ;
		float buttonWidth	= 5.;
		float buttonHeight	= 3.;
		GLuint buttonID		= 0;
		
		bool buttonIsClicked	= false;
		bool buttonIsActivated	= false;
		bool buttonIsEnabled	= true;
	
		GLfloat* buttonColor	= NULL;
		
		static GLfloat buttonColorDefault [4];
		static GLfloat buttonColorActive  [4];
		static GLfloat buttonColorClick   [4];
		static GLfloat buttonColorDisabled[4];
		
		GLuint tex;

};



////////////////////////////////////////////////////////////////////
////////////////////////// Button //////////////////////////////////
////////////////////////////////////////////////////////////////////

GLfloat button::buttonColorDefault [4] = {1., 1., 1., 1.};
GLfloat button::buttonColorActive  [4] = {1., 1., 0., 1.};
GLfloat button::buttonColorClick   [4] = {0., 1., 0., 1.};
GLfloat button::buttonColorDisabled[4] = {0.7, 0.7, 0.7, 1.};

button::button(){
// constructor
	
}

button::~button(){
// destructor

	destroyButton();
	
}

void button::createButton( float tlcornerX, float tlcornerY, float tlcornerZ, float w, float h, bool enabled_, buttonType attr, GLuint* textures ){
// create a button

	setButtonPosition( tlcornerX, tlcornerY, tlcornerZ );
	setButtonWidth( w );
	setButtonHeight( h );

	setActivity( enabled_ );
	setAttributeTex( attr, textures );
	setDefaultColorActivity();
	
	renderButton();

}
	
void button::setActivity( bool enabled_ ){
// set whether the button is enabled or disabled

	buttonIsEnabled = enabled_;

}

void button::setDefaultColor(){
// set the button color to the default one

	buttonColor = buttonColorDefault;
	
}

void button::setDefaultColorActivity(){
// set the button color to the default one when the button is enabled, otherwise set the button color to the disabled one

	buttonIsEnabled ? setDefaultColor() : setDisabledColor();
	
}

void button::setActiveColor(){
// set the button color to the active one

	buttonColor = buttonColorActive;
	
}

void button::setClickColor(){
// set the button color to the clicked one

	buttonColor = buttonColorClick;
	
}

void button::setDisabledColor(){
// set the button color to the disabled one

	buttonColor = buttonColorDisabled;
	
}

void button::destroyButton(){
// destroy a button

	if( buttonID != 0 ){
		glDeleteLists( buttonID, 1 );
	}
	buttonID = 0;
	
	buttonColor = NULL;

}

void button::setButtonPosition( float x, float y, float z ){
// set x and y positions of a button

	 buttonX = x;
	 buttonY = y;
	 buttonZ = z;
	
}

void button::setButtonWidth( float buttonWidth_ ){
// set a button width

	buttonWidth = buttonWidth_;

}

void button::setButtonHeight( float buttonHeight_ ){
// set a button height

	buttonHeight = buttonHeight_;

}


void button::moveOverButton( float mousePosX, float mousePosY ){
// check whether the mouse moves over the button and if so treat such a situation

	if( !buttonIsEnabled ) return;

	if( mousePosX < buttonX || mousePosX > buttonX + buttonWidth ||
	    mousePosY < buttonY || mousePosY > buttonY + buttonHeight ){

		setDefaultColor();
		if( buttonIsActivated ) renderButton();
	    	buttonIsActivated = false;
		buttonIsClicked = false;

	} else {

		setActiveColor();
		if( !buttonIsActivated ) renderButton();
		buttonIsActivated = true;
		buttonIsClicked = false;
		
	}

}

bool button::clickOnButton( float mousePosX, float mousePosY ){
// check whether the mouse clicks on the button and if so treat such a situation

	if( !buttonIsEnabled ){
		buttonIsClicked = false;
		return buttonIsClicked;		
	}

	if( mousePosX < buttonX || mousePosX > buttonX + buttonWidth ||
	    mousePosY < buttonY || mousePosY > buttonY + buttonHeight ){
	    
		setDefaultColor();
    		if( buttonIsActivated ) renderButton();
	    	buttonIsActivated = false;
		buttonIsClicked = false;

	} else {

		setClickColor();
		if( !buttonIsClicked ) renderButton();
	    	buttonIsActivated = true;
		buttonIsClicked = true;

	}
	
	return buttonIsClicked;

}

void button::setAttributeTex( button::buttonType attribute, GLuint* textures ){
// set the text(ure) appearing on the button according to the attribute
// textures array is assumed to have at least num values, where num is the number of items in buttonType
// textures should be generated during context menu initialization

	tex = textures[attribute];

}

void button::renderButton(){
// create a button graphics
	
	float mat[4][4];
	float corner_tl[3], corner_tr[3], corner_bl[3], corner_br[3];
	glGetFloatv( GL_MODELVIEW_MATRIX, &mat[0][0] );

	for( int i = 0; i < 4; i++){
	
		corner_tl[i] = buttonX			* mat[i][0] + buttonY			* mat[i][1] + buttonZ * mat[i][2];
		corner_tr[i] = buttonX			* mat[i][0] + (buttonY + buttonHeight)	* mat[i][1] + buttonZ * mat[i][2];
		corner_bl[i] = (buttonX + buttonWidth)	* mat[i][0] + buttonY			* mat[i][1] + buttonZ * mat[i][2];
		corner_br[i] = (buttonX + buttonWidth)	* mat[i][0] + (buttonY + buttonHeight)	* mat[i][1] + buttonZ * mat[i][2];
		
	}

	glDeleteLists( buttonID, 1 );
	buttonID = glGenLists( 1 );
	
	glNewList( buttonID, GL_COMPILE );
	glBegin( GL_QUADS );
		glColor4fv( buttonColor );
		glTexCoord2f( 0., 1. ); glVertex3f( corner_tl[0], corner_tl[1], corner_tl[2] );
		glTexCoord2f( 0., 0. ); glVertex3f( corner_tr[0], corner_tr[1], corner_tr[2] );
		glTexCoord2f( 1., 0. ); glVertex3f( corner_br[0], corner_br[1], corner_br[2] );
		glTexCoord2f( 1., 1. ); glVertex3f( corner_bl[0], corner_bl[1], corner_bl[2] );
	glEnd();
	glEndList();

}

void button::drawButton(){
// draw a button
	
	glEnable( GL_TEXTURE_2D	);
	glBindTexture( GL_TEXTURE_2D, tex );
//	renderButton();
	glCallList( buttonID );	
	glDisable( GL_TEXTURE_2D );
}


bool button::isActivated(){
// has a mouse moved over the button?

	return buttonIsEnabled;

}

bool button::isClicked(){
// has a mouse clicked on the button?

	return buttonIsClicked;

}
