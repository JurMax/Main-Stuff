//
//  inputhandler.hpp
//  2d-game
//
//  Created by Jurriaan van den Berg on 10-04-16.
//  Copyright © 2016 JurMax. All rights reserved.
//

#ifndef inputhandler_hpp
#define inputhandler_hpp

#include <stdio.h>
#include <iostream>
#include <JKeys.hpp>
#include <JVector.hpp>
#include <SDL.h>

namespace Input {
	bool isKeyDown( JKey code );
	bool isKeyPressed( JKey code );

	/** Directly after mouse/touch down */
	bool isScreenPressed();
	bool isAreaHovered( float posX, float posY, float width, float height );
	bool isAreaPressed( float posX, float posY, float width, float height );
	bool isAreaDown( float posX, float posY, float width, float height );
	/** When mouse/touch is pressed (isAreaBeingClicked) and released (isAreaClicked) from same area */
	bool isAreaClicked( float posX, float posY, float width, float height );
	bool isAreaBeingClicked( float posX, float posY, float width, float height );

	std::string getInputLine();
	bool isTyping();

	void Input_LoadCursors();
}


namespace Overture {
    void Input_Update();
}


class InputPointer {
public:
	InputPointer() {
		active = false;
		fingerID = 0;
		down = false;
		pressed = false;
		released = false;
		clickConsumed = false;
		position = Vector2(0, 0);
		prevPosition = Vector2(0, 0);
		pressedPosition = Vector2(0, 0);
		releasedPosition = Vector2(0, 0);
	}

	bool active;
	SDL_FingerID fingerID;

	/** This inputpointer will become inactive if down equals false */
	bool down;
	bool pressed;
	bool released;
	bool clickConsumed;

	/** positions are normalised (0 to 1) */
	Vector2 position;
	Vector2 prevPosition;
	Vector2 pressedPosition;
	Vector2 releasedPosition;

};


namespace Input {
	extern InputPointer pointers[16];

	extern SDL_Cursor *CURSOR_ARROW;
	extern SDL_Cursor *CURSOR_IBEAM;
	extern SDL_Cursor *CURSOR_WAIT;
	extern SDL_Cursor *CURSOR_CROSSHAIR;
	extern SDL_Cursor *CURSOR_WAITARROW;
	extern SDL_Cursor *CURSOR_SIZENWSE;
	extern SDL_Cursor *CURSOR_SIZENESW;
	extern SDL_Cursor *CURSOR_SIZEWE;
	extern SDL_Cursor *CURSOR_SIZENS;
	extern SDL_Cursor *CURSOR_SIZEALL;
	extern SDL_Cursor *CURSOR_NO;
	extern SDL_Cursor *CURSOR_HAND;
}


#endif /* inputhandler_hpp */


