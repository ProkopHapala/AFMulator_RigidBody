
#ifndef _BUTTONS_H_
#define _BUTTONS_H_

#include <string>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_ttf.h>

#define ZERO 0., 0., 0.
#define ONE  1., 1., 1.
#define HALF 0.5, 0.5, 0.5

struct alphabet;		// forward declaration
struct contextMenuSetting;	// forward declaration

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

class contextMenu : button {

	public:
	
		contextMenu();
		~contextMenu();
		
		enum actionType {
		
			ACTION_NONE,
			ACTION_EXPORT,
			ACTION_ADJUST,
			ACTION_ADJUST3,
			ACTION_ADJUST_X,
			ACTION_ADJUST_Y,
			ACTION_ADJUST_Z,
			ACTION_DIAL
	
		};
		
		void setMenuPosition( float x, float y, float z );
		void setMenuWidth( float menuWidth_ );
		float getMenuWidth();
		float getMenuWidthDefault();
		void setNumOfButtons( int numOfButtons_ );
		void setButtonHeight( float buttonHeight_ );
		float getButtonHeight();
		float getButtonHeightDefault();
	
		void createMenu( int numOfButtons_, float mouseX, float mouseY, float mouseZ, bool* activity_, buttonType* attr_ );
		void createMenu( contextMenuSetting* setting, float menuCornerX, float menuCornerY, float menuCornerZ );	
		void destroyMenu();
		void drawMenu();
		
		bool isCreated();
		bool isActivated( float mousePosX, float mousePosY );
		
		actionType getAction();
		void setAction( buttonType type );
		
		void moveOverMenu( float mousePosX, float mousePosY );
		void clickOnMenu( float mousePosX, float mousePosY );
		
		static void initializeMenu( int maxNumOfButtons_, bool defaultFontInput, std::string fontPath = \
//			"/home/jaroslav/Plocha/FZU/molOptRigidElecSurf_test14/include/Textures/LobsterTwo-Bold.ttf" );
//			"/home/jaroslav/Plocha/FZU/molOptRigidElecSurf_test14/include/Textures/Merriweather-Light.ttf");
//			"/home/jaroslav/Plocha/FZU/molOptRigidElecSurf_test14/include/Textures/Asana-Math.ttf");
			"/home/jaroslav/Plocha/FZU/skenovaniAFM/molOptRigidElecSurf_test22/include/Textures/Merriweather-Heavy.ttf");
		static void terminateMenu();
		
		void redrawTexture( buttonType textureNumber, char* newButtonText );
		
		
	private:
	
		static void addLetter( float* pixels, int width, int height, int totalLetterWidth, int currentLetter, float* capitalLetter, int horGap, int verGap );
		static void setTextToButton( int horizontalGaps, int interletterGaps, std::string buttonText, int& textureWidth, int& textureHeight, float*& pixels, alphabet letters );
//		static bool setTextToButtonFromFiles( std::string buttonText, int& textureWidth, int& textureHeight, float*& pixels, int horGap, int verGap );
		static bool setTextToButtonFromFiles( int horizontalGaps, int verticalGaps, std::string buttonText, int& textureWidth, int& textureHeight, float*& pixels );
//		static bool setTextToButtonFromFiles( std::string buttonText, int& textureWidth, int& textureHeight, float*& pixels );
		static void createButtonText( int type, std::string& text );
		
		float	menuPosX, menuPosY, menuPosZ;
		float	menuWidthDefault	= 5.;
		float	buttonHeightDefault	= 1.;
		float	menuWidth	= menuWidthDefault;		// menuWidth can be changed during the application e.g. when zooming
		float	buttonHeight	= buttonHeightDefault;		// buttonHeight can be changed during the application e.g. when zooming
		int	numOfButtons	= 0;
		button* listOfButtons	= NULL;
		static TTF_Font *buttonFont;
		bool*	activity	= NULL;
		
		buttonType*  attr	= NULL;
		
		actionType action = ACTION_NONE;
		static GLuint* textures;
		static int maxNumOfButtons;
		static bool defaultFont;

};

struct contextMenuSetting {
// structure storing the basic characteristics of a given context menu

	int numOfButtons;
	bool* activity = NULL;
	button::buttonType* attr = NULL;
	
	void set( contextMenuSetting contMenuSetting_ );
	void set( int numOfButtons_, bool* activity_, button::buttonType* attr_ );
	bool set( char* filename );
	void destroy();

};


struct alphabet {
// alphabet
// the last character in letters is reserved to substitute all characters not included in letters array

	int maxChar = 26 + 1;
	int letterWidth = 5;
	int letterHeight = 7;
	int numberOfColors = 3;
	
	float letters[26 + 1][7*5*3] = {

	{ // A
	 ONE, ZERO, ZERO, ZERO,  ONE,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO, ZERO, ZERO, ZERO, ZERO,
	 ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO
	},
	{ // B
	ZERO, ZERO, ZERO, ZERO,  ONE,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO, ZERO, ZERO, ZERO,  ONE,
	ZERO,  ONE,  ONE,  ONE, ZERO, 	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO, ZERO, ZERO, ZERO,  ONE
	},
	{ // C
	 ONE, ZERO, ZERO, ZERO,  ONE,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE,  ONE,	ZERO,  ONE,  ONE,  ONE,  ONE,
	ZERO,  ONE,  ONE,  ONE,  ONE,	ZERO,  ONE,  ONE,  ONE, ZERO,	 ONE, ZERO, ZERO, ZERO,  ONE
	},
	{ // D
	ZERO, ZERO, ZERO, ZERO,  ONE,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,
	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO, ZERO, ZERO, ZERO,  ONE
	},
	{ // E
	ZERO, ZERO, ZERO, ZERO, ZERO,	ZERO,  ONE,  ONE,  ONE,  ONE,	ZERO,  ONE,  ONE,  ONE,  ONE,	ZERO, ZERO, ZERO, ZERO, ZERO,
	ZERO,  ONE,  ONE,  ONE,  ONE,	ZERO,  ONE,  ONE,  ONE,  ONE,	ZERO, ZERO, ZERO, ZERO, ZERO
	},
	{ // F
	ZERO, ZERO, ZERO, ZERO, ZERO,	ZERO,  ONE,  ONE,  ONE,  ONE,	ZERO,  ONE,  ONE,  ONE,  ONE,	ZERO, ZERO, ZERO, ZERO, ZERO,
	ZERO,  ONE,  ONE,  ONE,  ONE,	ZERO,  ONE,  ONE,  ONE,  ONE,	ZERO,  ONE,  ONE,  ONE,  ONE
	},
	{ // G
	 ONE, ZERO, ZERO, ZERO,  ONE,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE,  ONE,	ZERO,  ONE, ZERO, ZERO, ZERO,
	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	 ONE, ZERO, ZERO, ZERO,  ONE	
	},
	{ // H
	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO, ZERO, ZERO, ZERO, ZERO,
	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO
	},
	{ // I
	 ONE,  ONE, ZERO,  ONE,  ONE,	 ONE,  ONE, ZERO,  ONE,  ONE,	 ONE,  ONE, ZERO,  ONE,  ONE,	 ONE,  ONE, ZERO,  ONE,  ONE,
	 ONE,  ONE, ZERO,  ONE,  ONE,	 ONE,  ONE, ZERO,  ONE,  ONE,	ONE,  ONE, ZERO,  ONE,  ONE
	},
	{ // J
	 ONE,  ONE,  ONE,  ONE, ZERO,	 ONE,  ONE,  ONE,  ONE, ZERO,	 ONE,  ONE,  ONE,  ONE, ZERO,	 ONE,  ONE,  ONE,  ONE, ZERO,
	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	 ONE, ZERO, ZERO, ZERO,  ONE
	},
	{ // K
	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE, ZERO,  ONE,	ZERO,  ONE, ZERO,  ONE,  ONE,	ZERO, ZERO,  ONE,  ONE,  ONE,
	ZERO,  ONE, ZERO,  ONE,  ONE,	ZERO,  ONE,  ONE, ZERO,  ONE,	ZERO,  ONE,  ONE,  ONE, ZERO
	},
	{ // L
	ZERO,  ONE,  ONE,  ONE,  ONE,	ZERO,  ONE,  ONE,  ONE,  ONE,	ZERO,  ONE,  ONE,  ONE,  ONE,	ZERO,  ONE,  ONE,  ONE,  ONE,
	ZERO,  ONE,  ONE,  ONE,  ONE,	ZERO,  ONE,  ONE,  ONE,  ONE,	ZERO, ZERO, ZERO, ZERO, ZERO
	},
	{ // M
	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO, ZERO,  ONE, ZERO, ZERO,	ZERO,  ONE, ZERO,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,
	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO
	},
	{ // N
	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO, ZERO,  ONE,  ONE, ZERO,	ZERO,  ONE, ZERO,  ONE, ZERO,
	ZERO,  ONE,  ONE, ZERO, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO
	},
	{ // O
	 ONE, ZERO, ZERO, ZERO,  ONE,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,
	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	 ONE, ZERO, ZERO, ZERO,  ONE
	},
	{ // P
	ZERO, ZERO, ZERO, ZERO,  ONE,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO, ZERO, ZERO, ZERO,  ONE,
	ZERO,  ONE,  ONE,  ONE,  ONE,	ZERO,  ONE,  ONE,  ONE,  ONE,	ZERO,  ONE,  ONE,  ONE,  ONE
	},
	{ // Q
	 ONE, ZERO, ZERO, ZERO,  ONE,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,
	ZERO,  ONE, ZERO,  ONE, ZERO,	ZERO,  ONE,  ONE, ZERO, ZERO,	 ONE, ZERO, ZERO, ZERO, ZERO	
	},
	{ // R
	ZERO, ZERO, ZERO, ZERO,  ONE,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO, ZERO, ZERO, ZERO,  ONE,	
	ZERO,  ONE, ZERO,  ONE,  ONE,	ZERO,  ONE,  ONE, ZERO,  ONE,	ZERO,  ONE,  ONE,  ONE, ZERO	
	},
	{ // S
	 ONE, ZERO, ZERO, ZERO,  ONE,	ZERO,  ONE,  ONE,  ONE, ZERO,	 ONE, ZERO,  ONE,  ONE,  ONE,	 ONE,  ONE, ZERO,  ONE,  ONE,
	 ONE,  ONE,  ONE, ZERO,  ONE,	ZERO,  ONE,  ONE,  ONE, ZERO,	 ONE, ZERO, ZERO, ZERO,  ONE
	},
	{ // T
	ZERO, ZERO, ZERO, ZERO, ZERO,	 ONE,  ONE, ZERO,  ONE,  ONE,	 ONE,  ONE, ZERO,  ONE,  ONE,	 ONE,  ONE, ZERO,  ONE,  ONE,
	 ONE,  ONE, ZERO,  ONE,  ONE,	 ONE,  ONE, ZERO,  ONE,  ONE,	 ONE,  ONE, ZERO,  ONE,  ONE
	},
	{ // U
	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,
	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	 ONE, ZERO, ZERO, ZERO,  ONE
	},
	{ // V
	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	 ONE, ZERO,  ONE, ZERO,  ONE,
	 ONE, ZERO,  ONE, ZERO,  ONE,	 ONE,  ONE, ZERO,  ONE,  ONE,	 ONE,  ONE, ZERO,  ONE,  ONE	
	},
	{ // W
	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE, ZERO,  ONE, ZERO,	ZERO,  ONE, ZERO,  ONE, ZERO,
	ZERO,  ONE, ZERO,  ONE, ZERO,	 ONE, ZERO, ZERO, ZERO, ZERO,	 ONE, ZERO,  ONE, ZERO,  ONE
	},
	{ // X
	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	 ONE, ZERO,  ONE, ZERO,  ONE,	 ONE,  ONE, ZERO,  ONE,  ONE,
	 ONE, ZERO,  ONE, ZERO,  ONE,	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,
	},
	{ // Y
	ZERO,  ONE,  ONE,  ONE, ZERO,	ZERO,  ONE,  ONE,  ONE, ZERO,	 ONE, ZERO,  ONE, ZERO,  ONE,	 ONE,  ONE, ZERO,  ONE,  ONE,
	 ONE,  ONE, ZERO,  ONE,  ONE,	 ONE,  ONE, ZERO,  ONE,  ONE,	 ONE,  ONE, ZERO,  ONE,  ONE
	},
	{ // Z
	ZERO, ZERO, ZERO, ZERO, ZERO,	 ONE,  ONE,  ONE,  ONE, ZERO,	 ONE,  ONE,  ONE, ZERO,  ONE,	 ONE,  ONE, ZERO,  ONE,  ONE,
	 ONE, ZERO,  ONE,  ONE,  ONE,	ZERO,  ONE,  ONE,  ONE,  ONE,	ZERO, ZERO, ZERO, ZERO, ZERO
	},
	{ // GRAY FIELD
	HALF, HALF, HALF, HALF, HALF,	HALF, HALF, HALF, HALF, HALF,	HALF, HALF, HALF, HALF, HALF,	HALF, HALF, HALF, HALF, HALF,
	HALF, HALF, HALF, HALF, HALF,	HALF, HALF, HALF, HALF, HALF,	HALF, HALF, HALF, HALF, HALF
	}
	}; 

};

#endif
