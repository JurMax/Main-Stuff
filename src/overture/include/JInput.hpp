//
//  inputhandler.hpp
//  2d-game
//
//  Created by Jurriaan van den Berg on 10-04-16.
//  Copyright © 2016 JurMax. All rights reserved.
//

#ifndef inputhandler_hpp
#define inputhandler_hpp

#include <string>
#include <SDL.h>
#include <JKeys.hpp>
#include <JTypes.hpp>


namespace Overture {
    void Input_Update();
}


struct InputPointer;


namespace Input {

	// Amount of InputPointers that are accounted for (for multitouch).
	const int pointerAmount = 1;//8;
	extern InputPointer pointers[pointerAmount];


	bool isKeyDown( JKey code );
	bool isKeyPressed( JKey code );
	bool isModKeyDown();


	InputPointer* getMouse();

	// Transform screen position to render position (in relation with camera movement and screen scale).
	int screenToRenderX( float posX );
	int screenToRenderY( float posY );
	Vector2 screenToRender( Vector2 pos );

	Vector2 getPointerPosition( int index, bool renderposition = true );
	Vector2 getPointerPressedPosition( int index, bool renderposition = true );
	bool isScreenPosInRenderRect( float screenPosX, float screenPosY, float rectPosX, float rectPosY, float rectWidth, float rectHeight );

	bool isAreaHovered( float posX, float posY, float width, float height );
	int isAreaPressed( float posX, float posY, float width, float height, bool consumePress = true );
	bool isAreaDown( float posX, float posY, float width, float height );
	bool isAreaClicked( float posX, float posY, float width, float height );
	bool isAreaBeingClicked( float posX, float pos, float width, float height );

	bool isScreenPressed( bool consumePress = false );


	float getMouseScrollDiffX( float posX = 0.0f, float posY = 0.0f, float width = 99999.9f, float height = 99999.9f );
	float getMouseScrollDiffY( float posX = 0.0f, float posY = 0.0f, float width = 99999.9f, float height = 99999.9f );


	void startTextInput();
	void stopTextInput();
	bool isTextInputActive();
	std::string getInputLineChange();
	void setSelectedText( std::string str );

}



struct InputPointer {
	InputPointer() {
		active = false;
		fingerID = 0;
		down = false;
		pressed = false;
		released = false;
		pressConsumed = false;
		position = Vector2(0, 0);
		prevPosition = Vector2(0, 0);
		pressedPosition = Vector2(0, 0);
		releasedPosition = Vector2(0, 0);
	}

	bool active;
	SDL_FingerID fingerID;

	// This inputpointer will become inactive if down equals false
	bool down;
	bool pressed;
	bool released;
	bool pressConsumed;

	// Positions are normalised (0.0 to 1.0)
	Vector2 position;
	Vector2 prevPosition;
	Vector2 pressedPosition;
	Vector2 releasedPosition;
};


#endif /* inputhandler_hpp */


