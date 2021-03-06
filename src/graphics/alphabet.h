
#define ZERO 0., 0., 0.
#define ONE  1., 1., 1.
#define HALF 0.5, 0.5, 0.5

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
