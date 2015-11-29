#include "buttons.h"

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

////////////////////////////////////////////////////////////////////
/////////////////////// Context Menu ///////////////////////////////
////////////////////////////////////////////////////////////////////


int contextMenu::maxNumOfButtons = 0;
GLuint* contextMenu::textures = NULL;
TTF_Font *contextMenu::buttonFont = NULL;
bool contextMenu::defaultFont = false;

contextMenu::contextMenu(){
// constructor
	
}

contextMenu::~contextMenu(){
// desctructor

	destroyMenu();
	
}

void contextMenu::setMenuWidth( float menuWidth_ ){
// set a context menu width

	menuWidth = menuWidth_;
	
}

float contextMenu::getMenuWidth(){
// get a context menu width

	return menuWidth;

}

float contextMenu::getMenuWidthDefault(){
// get a default context menu width

	return menuWidthDefault;

}

void contextMenu::setNumOfButtons( int numOfButtons_ ){
// set a number of buttons present in a context menu	

	(numOfButtons_ < 1) ? numOfButtons = 1 : numOfButtons = numOfButtons_;

}

void contextMenu::setButtonHeight( float buttonHeight_ ){
// set the height of all buttons in a context menu

	buttonHeight = buttonHeight_;
	
}

float contextMenu::getButtonHeight(){
// get a height of individual buttons		
		
	return buttonHeight;
		
}

float contextMenu::getButtonHeightDefault(){
// get a default height of individual buttons		
		
	return buttonHeightDefault;
		
}

void contextMenu::setMenuPosition( float x, float y, float z ){
// set positions of the context menu

	menuPosX = x;
	menuPosY = y;
	menuPosZ = z;
		
}

void contextMenu::createMenu( int numOfButtons_, float mouseX, float mouseY, float mouseZ, bool* activity_, buttonType* attr_ ){
// create a context menu

	setNumOfButtons( numOfButtons_ );
	setMenuPosition( mouseX, mouseY, mouseZ );
	
	listOfButtons	= new button[numOfButtons];
	activity	= new bool[numOfButtons];
	attr 		= new buttonType[numOfButtons];
	
	float tlcornerX = mouseX;
	float tlcornerY = mouseY;
	float tlcornerZ = mouseZ;
	
	for( int i = 0; i < numOfButtons; i++ ){
	
		activity[i]	= activity_[i];
		attr[i]		= attr_[i];
		listOfButtons[i].createButton( tlcornerX, tlcornerY, tlcornerZ, menuWidth, buttonHeight, activity[i], attr[i], textures );
		tlcornerY	+= buttonHeight;
		
	}
		
}

void contextMenu::createMenu( contextMenuSetting* setting, float menuCornerX, float menuCornerY, float menuCornerZ ){
// create a context menu from contextMenuSetting object

	createMenu( setting->numOfButtons, menuCornerX, menuCornerY, menuCornerZ, setting->activity, setting->attr );

}

void contextMenu::destroyMenu(){
// destroy a context menu

	if( listOfButtons != NULL ){
		
		delete [] listOfButtons;
		listOfButtons = NULL; 
		
		numOfButtons = 0;
	
	}
	
	if( attr != NULL ){
	
		delete [] attr;
		attr = NULL;
	}

	if( activity != NULL ){
	
		delete [] activity;
		activity = NULL;
	}

}
		
void contextMenu::drawMenu(){
// draw a context menu
		
	for( int i = 0; i < numOfButtons; i++ ){
		listOfButtons[i].drawButton();
	}
	
}

void contextMenu::moveOverMenu( float mousePosX, float mousePosY ){
// treat the situation when a mouse moves over the context menu

	for( int i = 0; i < numOfButtons; i++ ){
		listOfButtons[i].moveOverButton( mousePosX, mousePosY );
	}

}

void contextMenu::clickOnMenu( float mousePosX, float mousePosY ){
// treat the situation when a mouse clicks on the context menu

	setAction( BUTTON_NONE );

	for( int i = 0; i < numOfButtons; i++ ){
		if( listOfButtons[i].clickOnButton( mousePosX, mousePosY ) ){
			setAction( attr[i] );
		}
	}

}
		
bool contextMenu::isCreated(){
// is a context menu initialized?

	if( listOfButtons != NULL ){
		return true;
	}

	return false;
}

bool contextMenu::isActivated( float mousePosX, float mousePosY ){
// has a mouse moved over the context menu?

	if( mousePosX < menuPosX || mousePosX > menuPosX + menuWidth ||
	    mousePosY < menuPosY || mousePosY > menuPosY + buttonHeight*numOfButtons ){
	    
	    	return false;
	    
	} else {
	
		return true;
	}

}

contextMenu::actionType contextMenu::getAction(){
// which action should be accomplished?

	return action;

}

void contextMenu::setAction( contextMenu::buttonType type ){
// set action

	switch( type ){
	
		case BUTTON_NONE:
			action = ACTION_NONE;
			break;
		case BUTTON_EXPORT:
			action = ACTION_EXPORT;
			break;
		case BUTTON_ADJUST:
			action = ACTION_ADJUST;
			break;
		case BUTTON_ADJUST3:
			action = ACTION_ADJUST3;
			break;
		case BUTTON_ADJUST_X:
			action = ACTION_ADJUST_X;
			break;
		case BUTTON_ADJUST_Y:
			action = ACTION_ADJUST_Y;
			break;
		case BUTTON_ADJUST_Z:
			action = ACTION_ADJUST_Z;
			break;
		case BUTTON_DIAL:
			action = ACTION_DIAL;
			break;
	
	}

}

void contextMenu::createButtonText( int type, std::string& text ){
// create buttonText string based on input attribute type

	switch( type ){
			
		case BUTTON_NONE:
			text = "BUTTON_NONE";
			break;
		case BUTTON_EXPORT:
			text = "EXPORT";
			break;
		case BUTTON_ADJUST:
			text = "ADJUST";
			break;
		case BUTTON_ADJUST3:
			text = "ADJUST_3";
			break;			
		case BUTTON_ADJUST_X:
			text = "ADJUST_X";
			break;			
		case BUTTON_ADJUST_Y:
			text = "ADJUST_Y";
			break;			
		case BUTTON_ADJUST_Z:
			text = "ADJUST_Z";
			break;			
		case BUTTON_DIAL:
			text = "BUTTON_DIAL";
			break;

	}

}

void contextMenu::initializeMenu( int maxNumOfButtons_, bool defaultFont_, std::string fontPath ){
// initializes basic characteristics of a context menu common for all instances of a menu in a program
// specifically, textures for buttons are generated
// assumes there is at least maxNumOfButtons_ textures available

	defaultFont = defaultFont_;
	maxNumOfButtons = maxNumOfButtons_;
	textures = new GLuint[maxNumOfButtons];
	
	int textureWidth, textureHeight, num;
	float* pixels;
	std::string buttonText;
	alphabet capitalLettersAlph;
	
	const int verticalGaps	  = 5;
	const int horizontalGaps  = 10;
	const int fontSize	  = 28;		

	if( !defaultFont ){
		if( TTF_Init() == -1 ){
			printf( "contextMenu::initializeMenu: TTF cannot be initialized (%s). The default font is used instead.\n", TTF_GetError() );
			defaultFont = true;
		} else {	
			buttonFont = TTF_OpenFont( fontPath.c_str(), fontSize );
			if( buttonFont == NULL ){
				printf( "contextMenu::initializeMenu: Font cannot be opened (%s). The default font is used instead.\n", TTF_GetError() );
				defaultFont = true;
			}
		}	
	}		
					
	glEnable( GL_TEXTURE_2D );
	glGenTextures( maxNumOfButtons, textures );

	// each button is assigned a text from which the texture is created
	for( int i = 0; i < maxNumOfButtons; i++ ){
	
		glBindTexture( GL_TEXTURE_2D, textures[i] );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	
		createButtonText( i, buttonText );

		if( defaultFont || !setTextToButtonFromFiles( horizontalGaps, verticalGaps, buttonText, textureWidth, textureHeight, pixels ) ){
			printf( "contextMenu::initializeMenu: The font from files is not uploaded. The default font is used instead.\n" );
			setTextToButton( horizontalGaps, verticalGaps, buttonText, textureWidth, textureHeight, pixels, capitalLettersAlph );
		}
		
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_FLOAT, pixels );
		
		delete [] pixels;

	}
	
	glDisable( GL_TEXTURE_2D );	
}

void contextMenu::redrawTexture( buttonType textureNumber, char* newButtonText ){
// rerenders a texture for a single button

	int textureNumberLoc;
	( textureNumber >= maxNumOfButtons ) ? textureNumberLoc = maxNumOfButtons - 1 : textureNumberLoc = textureNumber;
	
	int textureWidth, textureHeight;
	float* pixels;
	std::string buttonText;
	alphabet capitalLettersAlph;
	
	const int verticalGaps	  = 5;
	const int horizontalGaps  = 10;
					
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, textures[textureNumber] );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	buttonText.assign( newButtonText );
	if( defaultFont || !setTextToButtonFromFiles( horizontalGaps, verticalGaps, buttonText, textureWidth, textureHeight, pixels ) ){
		printf( "contextMenu::redrawTexture: The font from files is not uploaded. The default font is used instead.\n" );
		setTextToButton( horizontalGaps, verticalGaps, buttonText, textureWidth, textureHeight, pixels, capitalLettersAlph );
	}

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_FLOAT, pixels );	
	delete [] pixels;
	
	glDisable( GL_TEXTURE_2D );	
}

bool contextMenu::setTextToButtonFromFiles( int horizontalGaps, int verticalGaps, std::string buttonText, int& textureWidth, int& textureHeight, float*& pixels ){
// create a texture based on bottonText from external font, the texture is stored in pixels array
// verGap is a left and right border width, horGap is a top and bottom border height

	SDL_Color buttonColorSDL = { 0, 0, 0, 0 };
	const int numOfColors = 3;
	SDL_Color *color;
	int i, j, incr_i;
	Uint8 index;	
	int horGap, verGap;
	
	( horizontalGaps  < 0 ) ? horGap = 0 : horGap = horizontalGaps;
	( verticalGaps    < 0 ) ? verGap = 0 : verGap = verticalGaps;
		
//	printf( "buttonText.c_str() = %s\n", buttonText.c_str() );
		
	SDL_Surface* textSurface = TTF_RenderText_Solid( buttonFont, buttonText.c_str(), buttonColorSDL );
	if( textSurface == NULL ){
		printf( "contextMenu::setTextToButtonFromFiles: Cannot create textSurface.\n" );
		return false;
	}
	
	textureWidth  = textSurface->w + 2*verGap;
	textureHeight = textSurface->h + 2*horGap;
	SDL_PixelFormat *fmt = textSurface->format;
	
	if( fmt->BitsPerPixel != 8 ){
	
		printf( "Font stored in non 8-bit form.\n" );
		SDL_FreeSurface( textSurface );
		return false;
		
	}

	if( SDL_MUSTLOCK( textSurface ) ) SDL_LockSurface( textSurface );
		
	int width	= textureWidth*numOfColors;
	int num		= width*textureHeight;
	int pitch	= textSurface->pitch;				
	int incr	= (verGap + textSurface->w)*numOfColors;
	
	pixels		= new float[num];

	// horizontal gaps
	for( i = 0; i < horGap; i++ ){
	
		incr_i = i*width;
	
		for( j = 0; j < width; j++ ){
			pixels[incr_i + j] = 1;
			pixels[num - incr_i - 1 - j] = 1;
		}
	}

	// letter copying
	for( i = 0; i < textSurface->h; i++ ){
	
		incr_i = (i + horGap)*width;
	
		// vertical gaps
		for( j = 0; j < verGap; j++ ){
			for( int k = 0; k < numOfColors; k++ ){
				pixels[incr_i + numOfColors*j + k] = 1.;
				pixels[incr_i + numOfColors*j + k + incr] = 1.;
			}
		}
	
		incr_i = (i + horGap)*width + verGap*numOfColors;
			
		// i-th row of a text
		for( j = 0; j < textSurface->w; j++ ){
		
			index = ( ( Uint8* ) textSurface->pixels )[i*pitch + j];
			color = &fmt->palette->colors[index];
			
			pixels[incr_i + numOfColors*j + 0] = (float) color->r / 256.0;
			pixels[incr_i + numOfColors*j + 1] = (float) color->g / 256.0;
			pixels[incr_i + numOfColors*j + 2] = (float) color->b / 256.0;
			
		}
	}
	
	if( SDL_MUSTLOCK( textSurface ) ) SDL_UnlockSurface( textSurface );
 	SDL_FreeSurface( textSurface );

	return true;	
	
}

void contextMenu::setTextToButton( int horizontalGaps, int interletterGaps, std::string buttonText, int& textureWidth, int& textureHeight, float*& pixels, alphabet letters ){
// create a texture based on bottonText

	int numOfLetter, ind;
	
	// alphabet specification
	int maxChar = letters.maxChar;		// number of letters in the alphabet
	int heightOfLetter = letters.letterHeight;
	int widthOfLetter = letters.letterWidth;
	int numOfColors = letters.numberOfColors;
	
	int wordLength = buttonText.length();	// length of buttonText
	
	if( horizontalGaps  < 0 ) horizontalGaps  = 0;
	if( interletterGaps < 0 ) interletterGaps = 0;
		
	textureWidth = wordLength*widthOfLetter;
	textureHeight = heightOfLetter;
	
	int num = textureHeight*textureWidth*numOfColors; // number of items for output texture
	
	// top and bottom borders
	// 2 sides (top and bottom)
	num += 2*textureWidth*numOfColors*horizontalGaps;
	textureHeight += 2*horizontalGaps;

	// vertical borders between letters
	// number of vertical lines between letters is equal to number of letters plus one
	num += (wordLength + 1)*interletterGaps*textureHeight*numOfColors;	
	textureWidth += (wordLength + 1)*interletterGaps;
	
	pixels = new float[num];

	// convert text to texture
	for( int i = 0; i < wordLength; i++ ){
		
		// find index for given letter
		numOfLetter = ( int ) buttonText[i];
		// 65 ... integer equivalent of 'A' in ASCII table
		if( numOfLetter - 65 < 0 || numOfLetter - 65 > maxChar ) numOfLetter = 65 + maxChar - 1;
		ind = numOfLetter - 65;

		// add the letter bitmap to texture
		addLetter( pixels, textureWidth*numOfColors, textureHeight, widthOfLetter*numOfColors, i, letters.letters[ind], horizontalGaps, interletterGaps );

	}
	
}

void contextMenu::addLetter( float* pixels, int width, int height, int totalLetterWidth, int currentLetter, float* capitalLetter, int horGap, int verGap ){
// add one letter to texture pixels
// padding by white color between distinct letters is given by verGap
// padding by white color from top and bottom is given by horGap

	int numOfRows = height - 2*horGap;
	const int numOfColors = 3;
	
	int i, j, k;
	int num  = height*width;
	int incr = currentLetter*totalLetterWidth + (currentLetter + 1)*verGap*numOfColors;
	int incr_i;
	
	// horizontal gaps
	for( i = 0; i < horGap; i++ ){
	
		incr_i = i*width;
		
		for( j = 0; j < width; j++ ){
			pixels[incr_i + j] = 1.;
			pixels[num - incr_i - 1 - j] = 1.;
		}
	}
	
	// letter copying
	for( i = 0; i < numOfRows; i++ ){
		
		incr_i = (i + horGap)*width;
		
		// if vertical gaps enabled draw them
		for( j = 0; j < verGap; j++ ){
			for( k = 0; k < numOfColors; k++ ){
				pixels[incr_i + numOfColors*j + k] = 1.;
				pixels[incr_i + numOfColors*j + k + incr + totalLetterWidth] = 1.;
			}
		}
	
		// i-th row of a letter
		for( j = 0; j < totalLetterWidth; j++ ){
			pixels[incr_i + incr + j] = capitalLetter[i*totalLetterWidth + j];
		}
		
	}

}

void contextMenu::terminateMenu(){
// deallocates textures

	TTF_CloseFont( buttonFont );
	buttonFont = NULL;
	TTF_Quit();

	if( textures != NULL ){
		glDeleteTextures( maxNumOfButtons, textures );
		delete [] textures;
		textures = NULL;
	}
	
}

////////////////////////////////////////////////////////////////////
//////////////////// contextMenuSetting ////////////////////////////
////////////////////////////////////////////////////////////////////

void contextMenuSetting::destroy(){
// deallocate memory

	delete [] activity;
	delete [] attr;

}

void contextMenuSetting::set( contextMenuSetting contMenuSetting_ ){
// copy contents of contMenuSetting_

	numOfButtons = contMenuSetting_.numOfButtons;
	activity = new bool[numOfButtons];
	attr = new button::buttonType[numOfButtons];
	
	for( int i = 0; i < numOfButtons; i++ ){
		activity[i] = contMenuSetting_.activity[i];
		attr[i] = contMenuSetting_.attr[i];
	}
}

void contextMenuSetting::set( int numOfButtons_, bool* activity_, button::buttonType* attr_ ){
// set contents
	
	numOfButtons = numOfButtons_;
	activity = new bool[numOfButtons];
	attr = new button::buttonType[numOfButtons];
	
	for( int i = 0; i < numOfButtons; i++ ){
		activity[i] = activity_[i];
		attr[i] = attr_[i];
	}

}

bool contextMenuSetting::set( char* filename ){
// set contents from file
// the format of the file should be as follows:
//
// numberOfButtons
//
// activity_1
// activity_2
// ...
// activity_numberOfButtons
//
// attr_1
// attr_2
// ...
// attr_numberOfButtons

	int success = 0;
	char item[10];

	FILE* loadfile = fopen( filename, "r" );
	if( loadfile == NULL ){
		printf( "contextMenuSetting::set: File %s cannot be accessed.\n", filename );
		return false;
	}
	
//	printf( "contextMenuSetting::set: Data loading...\n" );
	success = fscanf( loadfile, "%i", &numOfButtons );
	
	if( success != 1 ){
		printf( "contextMenuSetting::set: Invalid number of buttons.\n" );
		return false;
	}
	
	activity = new bool[numOfButtons];
	attr	 = new button::buttonType[numOfButtons];
	
	for( int i = 0; i < numOfButtons; i++ ){
		success = fscanf( loadfile, "%s", item );
		if( success != 1 ){
			printf( "contextMenuSetting::set: Invalid context menu activities specification.\n" );
			return false;
		}
		if( !strcmp( item, "true" ) ){
			activity[i] = true;
		} else if( !strcmp( item, "false" ) ){
			activity[i] = false;
		} else {
			printf( "contextMenuSetting::set: Invalid context menu activities specification.\n" );
			return false;
		}
	}
	
	for( int i = 0; i < numOfButtons; i++ ){
		success = fscanf( loadfile, "%s", item );
		if( success != 1 ){
			printf( "contextMenuSetting::set: Invalid context menu attributes specification.\n" );
			return false;
		}
				
		if( !strcmp( item, "BUTTON_NONE" ) ){
			attr[i] = button::BUTTON_NONE;
		} else if( !strcmp( item, "BUTTON_EXPORT" ) ){
			attr[i] = button::BUTTON_EXPORT;
		} else if( !strcmp( item, "BUTTON_ADJUST" ) ){
			attr[i] = button::BUTTON_ADJUST;
		} else if( !strcmp( item, "BUTTON_ADJUST3" ) ){
			attr[i] = button::BUTTON_ADJUST3;
		} else if( !strcmp( item, "BUTTON_ADJUST_X" ) ){
			attr[i] = button::BUTTON_ADJUST_X;
		} else if( !strcmp( item, "BUTTON_ADJUST_Y" ) ){
			attr[i] = button::BUTTON_ADJUST_Y;
		} else if( !strcmp( item, "BUTTON_ADJUST_Z" ) ){
			attr[i] = button::BUTTON_ADJUST_Z;
		} else if( !strcmp( item, "BUTTON_DIAL" ) ){
			attr[i] = button::BUTTON_DIAL;
		} else {
			printf( "contextMenuSetting::set: Invalid context menu attributes specification.\n" );
			return false;
		}
	}
//	printf( "contextMenuSetting::set: ...is successful.\n" );

	return true;
}
