//
//  inputhandler.cpp
//  2d - game
//
//  Created by Jurriaan van den Berg on 11-02-16.
//  Copyright © 2016 Spasticom. All rights reserved.
//

#include <JInput.hpp>

#include <JOverture.hpp>
#include <JRenderer.hpp>
#include <string>


void handleKeyInput( SDL_Event e );
void addKey( JKey key );
void removeKey( JKey key );
void handlePointerInput( SDL_Event e );



JKey keysDown[16];
bool keysPressed[16]; /* only active for one tick */
int itemsInKeysDown = 0;


bool mouseScrollConsumedX = false;
bool mouseScrollConsumedY = false;
float scrollDiffX = 0.0f;
float scrollDiffY = 0.0f;


std::string inputTextChange = "";
std::string selectedText = "";
bool textInputActive = false;


/*float x,y,z;
#define SIZE_DING 7
int lx[SIZE_DING],ly[SIZE_DING],lz[SIZE_DING];
int ding = 0;*/




////////////////////// PUBLIC //////////////////////

namespace Input {

	InputPointer pointers[pointerAmount];


	bool isKeyDown( JKey code ) {
		for (int i = 0; i < itemsInKeysDown; i++) {
			if (keysDown[i] == code) {
				return true;
			}
		}
		return false;
	}


	bool isKeyPressed( JKey code ) {
		for (int i = 0; i < itemsInKeysDown; i++) {
			if (keysDown[i] == code && keysPressed[i]) {
				return true;
			}
		}
		return false;
	}


	bool isModKeyDown() {
	    if (Overture_OSIsMac()) {
	    	return SDL_GetModState() & (KMOD_LGUI | KMOD_RGUI);
	    }
	    else {
	    	return SDL_GetModState() & KMOD_CTRL;
	    }
	}


	InputPointer* getMouse() {
		return &pointers[0];
	}



	int screenToRenderX( float posX ) {
		float screenwidth = (float) Renderer::renderTarget->width;
		return (int) roundf((posX * screenwidth - screenwidth/2.0f) / Renderer::renderScaleX + Overture::cameraPos.x);
	}


	int screenToRenderY( float posY ) {
		float screenheight = (float) Renderer::renderTarget->height;
		return (int) roundf((posY * screenheight - screenheight/2.0f) / Renderer::renderScaleY + Overture::cameraPos.y);
	}


	Vector2 screenToRender( Vector2 pos ) {
		return Vector2(screenToRenderX(pos.x), screenToRenderY(pos.y));
	}




	Vector2 getPointerPosition( int index, bool renderposition ) {
		Vector2 vector;
		if (renderposition) {
			vector.x = screenToRenderX(pointers[index].position.x);
			vector.y = screenToRenderY(pointers[index].position.y);
		}
		else {
			vector.x = pointers[index].position.x;
			vector.y = pointers[index].position.y;
		}
		return vector;
	}


	Vector2 getPointerPressedPosition( int index, bool renderposition ) {
		Vector2 vector;
		if (renderposition) {
			vector.x = screenToRenderX(pointers[index].pressedPosition.x);
			vector.y = screenToRenderY(pointers[index].pressedPosition.y);
		}
		else {
			vector.x = pointers[index].pressedPosition.x;
			vector.y = pointers[index].pressedPosition.y;
		}
		return vector;
	}


	bool isScreenPosInRenderRect( float screenPosX, float screenPosY, float rectPosX, float rectPosY, float rectWidth, float rectHeight ) {
		float posX = screenToRenderX(screenPosX);
		float posY = screenToRenderY(screenPosY);

		if (posX >= rectPosX && posX < rectPosX + rectWidth && posY >= rectPosY && posY < rectPosY + rectHeight) {
			return true;
		}
		else {
			return false;
		}
	}



	bool isAreaHovered( float posX, float posY, float width, float height ) {
		for (int i = 0; i < pointerAmount; i++) {
			if (pointers[i].active) {
				float pposx = pointers[i].position.x;
				float pposy = pointers[i].position.y;

				if (isScreenPosInRenderRect(pposx, pposy, posX, posY, width, height)) {
					return true;
				}
			}
		}
		return false;
	}


	int isAreaPressed( float posX, float posY, float width, float height, bool consumePress ) {
		for (int i = 0; i < pointerAmount; i++) {
			if (pointers[i].active && pointers[i].pressed && !(pointers[i].pressConsumed && consumePress)) {
				float ppposx = pointers[i].pressedPosition.x;
				float ppposy = pointers[i].pressedPosition.y;

				if (isScreenPosInRenderRect(ppposx, ppposy, posX, posY, width, height)) {
					if (consumePress) {
						pointers[i].pressConsumed = true;
					}
					return i + 1;
				}
			}
		}
		return 0;
	}


	bool isAreaDown( float posX, float posY, float width, float height ) {
		for (int i = 0; i < pointerAmount; i++) {
			if (pointers[i].active && pointers[i].down) {
				float pposx = pointers[i].position.x;
				float pposy = pointers[i].position.y;

				if (isScreenPosInRenderRect(pposx, pposy, posX, posY, width, height)) {
					return true;
				}
			}
		}
		return false;
	}


	bool isAreaClicked( float posX, float posY, float width, float height ) {
		for (int i = 0; i < pointerAmount; i++) {
			if (pointers[i].active) {
				float pposx = pointers[i].pressedPosition.x;
				float pposy = pointers[i].pressedPosition.y;

				if (isScreenPosInRenderRect(pposx, pposy, posX, posY, width, height)) {
					float rpposx = pointers[i].releasedPosition.x;
					float rpposy = pointers[i].releasedPosition.y;

					if (pointers[i].released && isScreenPosInRenderRect(rpposx, rpposy, posX, posY, width, height)) {
						return true;
					}
				}
			}
		}
		return false;
	}


	bool isAreaBeingClicked( float posX, float posY, float width, float height ) {
		for (int i = 0; i < pointerAmount; i++) {
			if (pointers[i].active) {
				float pposx = pointers[i].pressedPosition.x;
				float pposy = pointers[i].pressedPosition.y;

				if (isScreenPosInRenderRect(pposx, pposy, posX, posY, width, height)) {
					float ppposx = pointers[i].position.x;
					float ppposy = pointers[i].position.y;

					if (pointers[i].down && isScreenPosInRenderRect(ppposx, ppposy, posX, posY, width, height)) {
						return true;
					}
				}
			}
		}
		return false;
	}



	bool isScreenPressed( bool consumePress ) {
		for (int i = 0; i < pointerAmount; i++) {
			if (pointers[i].active && pointers[i].pressed && !(pointers[i].pressConsumed && consumePress)) {
				if (consumePress) {
					pointers[i].pressConsumed = true;
				}
				return true;
			}
		}
		return false;
	}



	float getMouseScrollDiffX( float posX, float posY, float width, float height ) {
		if (!mouseScrollConsumedX) {
			Vector2 ppos = getMouse()->position;

			if (isScreenPosInRenderRect(ppos.x, ppos.y, posX, posY, width, height)) {
				mouseScrollConsumedX = true;
			}
		}
		return 0.0f;
	}


	float getMouseScrollDiffY( float posX, float posY, float width, float height ) {
		if (!mouseScrollConsumedY) {
			Vector2 ppos = getMouse()->position;

			if (isScreenPosInRenderRect(ppos.x, ppos.y, posX, posY, width, height)) {
				mouseScrollConsumedY = true;
				if (!isKeyDown(KEY_LSHIFT)) {
					return scrollDiffY;
				}
				else {
					return scrollDiffX;
				}
			}
		}
		return 0.0f;
	}




	void startTextInput() {
		textInputActive = true;
		SDL_StartTextInput();
	}


	void stopTextInput() {
		textInputActive = false;
		SDL_StopTextInput();
	}


	bool isTextInputActive() {
		return textInputActive;
	}


	std::string getInputLineChange() {
		return inputTextChange;
	}


	void setSelectedText( std::string str ) {
		selectedText = str;
	}

}




////////////////////// PRIVATE //////////////////////

using namespace Input;


void Overture::Input_Update() {

    // Set all keysPressed to false, so that isKeyPressed(key) only remains true for one cycle.
    if (itemsInKeysDown > 0) {
        for (int i = 0; i < itemsInKeysDown; i++) {
            keysPressed[i] = false;
        }
    }

    // Update pointers.
    for (int i = 0; i < pointerAmount; i++) {
    	pointers[i].pressConsumed = false;
    	pointers[i].pressed = false;

    	if (pointers[i].released) {
    		pointers[i].released = false;

    		if (Overture_IsMobile()) {
    			// Pointer becomes useless after finger has been lifted.
    			pointers[i].active = false;
    		}
    	}

    	pointers[i].prevPosition.x = pointers[i].position.x;
    	pointers[i].prevPosition.y = pointers[i].position.y;
    }

    mouseScrollConsumedX = false;
    mouseScrollConsumedY = false;
    scrollDiffX = reduceFloat(scrollDiffX, 0.3f);
    scrollDiffY = reduceFloat(scrollDiffY, 0.3f);

    inputTextChange = "";


    SDL_Event e;
    while (SDL_PollEvent(&e)){
    	switch (e.type) {

		case SDL_QUIT :
			Overture_Stop();
			break;

		case SDL_WINDOWEVENT :
			Overture_GetWindow()->handleEvents(e);
			//debugwindow.handleEvents(e);
			break;

		case SDL_KEYDOWN :
		case SDL_KEYUP :
		case SDL_TEXTINPUT :
			handleKeyInput(e);
			break;

		case SDL_MOUSEBUTTONUP :
		case SDL_MOUSEBUTTONDOWN :
		case SDL_MOUSEMOTION :
		case SDL_FINGERDOWN :
		case SDL_FINGERUP :
		case SDL_FINGERMOTION :
			handlePointerInput(e);
			break;

		case SDL_MOUSEWHEEL :
			scrollDiffX = e.wheel.x;
			scrollDiffY = e.wheel.y;
			break;


		case SDL_DROPFILE :
			char* dropped_filedir;
			dropped_filedir = e.drop.file;
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "You dropped a file!", dropped_filedir, Overture_GetWindow()->data);
			// Hiermee is het mogelijk om super-secrete messages te geven als je bepaalde bestanden drop op bepaalde momenten in een spel.
			SDL_free(dropped_filedir);
			break;
    	}
    }
}



void handleKeyInput( SDL_Event e ) {

    bool keyModDown = isModKeyDown();

    if (e.type == SDL_KEYDOWN) {
		JKey key = (JKey) e.key.keysym.scancode;

    	if (!textInputActive) {
    		if (!keyModDown) {
    			switch (key) {
				case KEY_F11 :
					Overture_GetWindow()->toggleFullscreen();
					break;

				default :
					addKey(key);
					break;
    			}
    		}
    		else {
    			switch (key) {
    			case KEY_Z :
    				//Do stuff
    				break;

    			default :
    				break;
    			}
    		}
    	}
    	else {
    		// Keyboard actions and shortcuts while textInputActive text.

    		switch (key) {
			case KEY_BACKSPACE :
				inputTextChange += '\1';
				break;
			case KEY_DELETE :
				inputTextChange += '\2';
				break;
			case KEY_RETURN :
				inputTextChange += '\n';
				addKey(key);
				break;
			case KEY_TAB :
				inputTextChange += "   ";
				break;

			case KEY_C :
				if (keyModDown) {
					SDL_SetClipboardText(selectedText.c_str());
				}
				break;
			case KEY_X :
				if (keyModDown) {
					SDL_SetClipboardText(selectedText.c_str());
					inputTextChange += "\1";
				}
				break;
			case KEY_V :
				if (keyModDown) {
					inputTextChange += SDL_GetClipboardText();
				}
				break;

			case KEY_LEFT : case KEY_RIGHT : case KEY_UP : case KEY_DOWN :
			case KEY_LSHIFT : case KEY_RSHIFT :
			case KEY_ESCAPE :
				addKey(key);
				break;

			default :
				break;
    		}
    	}
    }
    else if (e.type == SDL_KEYUP) {
    	JKey key = (JKey) e.key.keysym.scancode;
    	removeKey(key);
    }
	else if (e.type == SDL_TEXTINPUT) {
		if (!keyModDown) {
			inputTextChange += e.text.text;
		}
	}
}


/**
 *  Add a key to the keybuffers. These are used to determine if a key is (still) pressed.
 */
void addKey( JKey key ) {
    if (itemsInKeysDown < 16) {
        bool isAlreadyAdded = false;
        for (int i = 0; i < itemsInKeysDown; i++) {
            if (keysDown[i] == key) {
                isAlreadyAdded = true;
            }
        }
        if (isAlreadyAdded == false) {
            keysDown[itemsInKeysDown] = key;
            keysPressed[itemsInKeysDown] = true;
            itemsInKeysDown++;
        }
    }
}


void removeKey( JKey key ) {
    if (itemsInKeysDown > 0) {
        for (int i = 0; i < itemsInKeysDown; i++) {
            if (keysDown[i] == key) {
            	// Move al keys after this one back.
                for (int i2 = i; i2 < itemsInKeysDown - 1; i2++) {
                    keysDown[i2] = keysDown[i2+1];
                }
                itemsInKeysDown--;
            }
        }
    }
}



void handlePointerInput( SDL_Event e ) {
    if (!Overture_IsMobile()) {
    	if (!pointers[0].active) {
			pointers[0].active = true;
    	}

    	if (e.type == SDL_MOUSEMOTION) {
    		pointers[0].position.x = ((float) e.button.x) / Overture_GetWindow()->width;
    		pointers[0].position.y = ((float) e.button.y) / Overture_GetWindow()->height;
    	}
    	else if (e.type == SDL_MOUSEBUTTONDOWN) {
        	if (e.button.button == SDL_BUTTON_LEFT) {
        		pointers[0].down = true;
				pointers[0].pressed = true;

				pointers[0].pressedPosition.x = ((float) e.button.x) / Overture_GetWindow()->width;
				pointers[0].pressedPosition.y = ((float) e.button.y) / Overture_GetWindow()->height;
        	}
        }
        else if (e.type == SDL_MOUSEBUTTONUP) {
        	if (e.button.button == SDL_BUTTON_LEFT) {
        		pointers[0].down = false;
        		pointers[0].released = true;

				pointers[0].releasedPosition.x = ((float) e.button.x) / Overture_GetWindow()->width;
				pointers[0].releasedPosition.y = ((float) e.button.y) / Overture_GetWindow()->height;
        	}
        }
    }

    else {
    	// Removes the inactive pointers so the beginning of so the remaining pointers are all adjoined at the start of the buffer
		for (int i = 0; i < pointerAmount; i++) {
			if (!pointers[i].active) {
				for (int i2 = i + 1; i2 < pointerAmount; i2++) {
					pointers[i2-1] = pointers[i2];
				}
			}
		}

		if (e.type == SDL_FINGERMOTION) {
			for (int i = 0; i < pointerAmount; i++) {
				if (pointers[i].fingerID == e.tfinger.fingerId && pointers[i].active) {
					pointers[i].position.x = e.tfinger.x;
					pointers[i].position.y = e.tfinger.y;
				}
			}
		}
		else if (e.type == SDL_FINGERDOWN) {
			int pointerSize = 16;
			for (int i = 0; i < pointerAmount; i++) {
				if (!pointers[i].active) {
					pointerSize = i;
					break;
				}
			}

			if (pointerSize < pointerAmount - 1) {
				pointers[pointerSize].fingerID = e.tfinger.fingerId;
				pointers[pointerSize].active = true;
				pointers[pointerSize].down = true;
				pointers[pointerSize].pressed = true;
				pointers[pointerSize].released = false;

				pointers[pointerSize].position.x =
						pointers[pointerSize].prevPosition.x =
								pointers[pointerSize].pressedPosition.x = e.tfinger.x;
				pointers[pointerSize].position.y =
						pointers[pointerSize].prevPosition.y =
								pointers[pointerSize].pressedPosition.y = e.tfinger.y;
			}
		}
		else if (e.type == SDL_FINGERUP) {
			for (int i = 0; i < pointerAmount; i++) {
				if (pointers[i].fingerID == e.tfinger.fingerId && pointers[i].active) {
					pointers[i].down = false;
					pointers[i].released = true;

					pointers[i].releasedPosition.x = e.tfinger.x;
					pointers[i].releasedPosition.y = e.tfinger.y;
				}
			}
		}
    }
}


