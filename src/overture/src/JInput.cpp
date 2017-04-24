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


void handleKeyInput( SDL_Event e );
void addKey( JKey key );
void removeKey( JKey key );
void handlePointerInput( SDL_Event e );

JKey keysDown[16];
bool keysPressed[16]; /* only active for one tick */
int itemsInKeysDown = 0;

std::string inputText = "";
bool typing = false;

float x,y,z;
#define SIZE_DING 7
int lx[SIZE_DING],ly[SIZE_DING],lz[SIZE_DING];
int ding = 0;


////////////////////// PUBLIC //////////////////////

namespace Input {

	InputPointer pointers[16];

	SDL_Cursor *CURSOR_ARROW = NULL;
	SDL_Cursor *CURSOR_IBEAM = NULL;
	SDL_Cursor *CURSOR_WAIT = NULL;
	SDL_Cursor *CURSOR_CROSSHAIR = NULL;
	SDL_Cursor *CURSOR_WAITARROW = NULL;
	SDL_Cursor *CURSOR_SIZENWSE = NULL;
	SDL_Cursor *CURSOR_SIZENESW = NULL;
	SDL_Cursor *CURSOR_SIZEWE = NULL;
	SDL_Cursor *CURSOR_SIZENS = NULL;
	SDL_Cursor *CURSOR_SIZEALL = NULL;
	SDL_Cursor *CURSOR_NO = NULL;
	SDL_Cursor *CURSOR_HAND = NULL;


	bool isKeyPressed( JKey code ) {
		for (int i = 0; i < itemsInKeysDown; i++) {
			if (keysDown[i] == code && keysPressed[i]) {
				return true;
			}
		}
		return false;
	}


	bool isKeyDown( JKey code ) {
		for (int i = 0; i < itemsInKeysDown; i++) {
			if (keysDown[i] == code) {
				return true;
			}
		}
		return false;
	}



	bool isScreenPressed() {
		for (int i = 0; i < 16; i++) {
			if (pointers[i].active && pointers[i].pressed) {
				return true;
			}
		}
		return false;
	}


	bool isScreenDown() {
		for (int i = 0; i < 16; i++) {
			if (pointers[i].active && pointers[i].down) {
				return true;
			}
		}
		return false;
	}


	bool isAreaHovered( float posX, float posY, float width, float height ) {
		for (int i = 0; i < 16; i++) {
			if (pointers[i].active) {
				float pposx = pointers[i].position.x * Overture_GetWindow()->width;
				float pposy = pointers[i].position.y * Overture_GetWindow()->height;

				if (pposx >= posX && pposx <= posX + width && pposy >= posY && pposy <= posY + height) {
					return true;
				}
			}
		}
		return false;
	}


	bool isAreaPressed( float posX, float posY, float width, float height ) {
		for (int i = 0; i < 16; i++) {
			if (pointers[i].active && pointers[i].pressed) {
				float pposx = pointers[i].pressedPosition.x * Overture_GetWindow()->width;
				float pposy = pointers[i].pressedPosition.y * Overture_GetWindow()->height;

				if (pposx >= posX && pposx <= posX + width && pposy >= posY && pposy <= posY + height) {
					return true;
				}
			}
		}
		return false;
	}


	bool isAreaDown( float posX, float posY, float width, float height ) {
		for (int i = 0; i < 16; i++) {
			if (pointers[i].active && pointers[i].down) {
				float pposx = pointers[i].position.x * Overture_GetWindow()->width;
				float pposy = pointers[i].position.y * Overture_GetWindow()->height;
				if (pposx >= posX && pposx <= posX + width && pposy >= posY && pposy <= posY + height) {
					return true;
				}
			}
		}
		return false;
	}


	bool isAreaClicked( float posX, float posY, float width, float height ) {
		for (int i = 0; i < 16; i++) {
			if (pointers[i].active && pointers[i].released) {
				float pposx = pointers[i].pressedPosition.x * Overture_GetWindow()->width;
				float pposy = pointers[i].pressedPosition.y * Overture_GetWindow()->height;
				float rposx = pointers[i].releasedPosition.x * Overture_GetWindow()->width;
				float rposy = pointers[i].releasedPosition.y * Overture_GetWindow()->height;

				if (pposx >= posX && pposx <= posX + width && pposy >= posY && pposy <= posY + height &&
					rposx >= posX && rposx <= posX + width && rposy >= posY && rposy <= posY + height) {
					return true;
				}
			}
		}
		return false;
	}


	bool isAreaBeingClicked( float posX, float posY, float width, float height ) {
		for (int i = 0; i < 16; i++) {
			if (pointers[i].active && pointers[i].down) {
				float posx = pointers[i].position.x * Overture_GetWindow()->width;
				float posy = pointers[i].position.y * Overture_GetWindow()->height;
				float pposx = pointers[i].pressedPosition.x * Overture_GetWindow()->width;
				float pposy = pointers[i].pressedPosition.y * Overture_GetWindow()->height;

				if (posx >= posX && posx <= posX + width && posy >= posY && posy <= posY + height &&
					pposx >= posX && pposx <= posX + width && pposy >= posY && pposy <= posY + height) {
					return true;
				}
			}
		}
		return false;
	}



	std::string getInputLine() {
		// to number: ::atof(inputText.c_str())
		return inputText;
	}


	bool isTyping() {
		return typing;
	}



	void Input_LoadCursors() {
		CURSOR_ARROW = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
		CURSOR_IBEAM = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
		CURSOR_WAIT = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
		CURSOR_CROSSHAIR = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
		CURSOR_WAITARROW = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAITARROW);
		CURSOR_SIZENWSE = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
		CURSOR_SIZENESW = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
		CURSOR_SIZEWE = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
		CURSOR_SIZENS = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
		CURSOR_SIZEALL = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
		CURSOR_NO = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);
		CURSOR_HAND = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
	}

}



////////////////////// PRIVATE //////////////////////
using namespace Input;


void Overture::Input_Update() {

    /* set all keysPressed to false */
    if (itemsInKeysDown > 0) {
        for (int i = 0; i < itemsInKeysDown; i++) {
            keysPressed[i] = false;
        }
    }
    for (int i = 0; i < 16; i++) {
    	if (pointers[i].pressed) {
    		pointers[i].pressed = false;
    	}
    	if (pointers[i].released) {
    		pointers[i].released = false;

    		if (Overture_IsMobile()) {
    			/** Pointer becomes useless after finger has been lifted */
    			pointers[i].active = false;
    		}
    	}
    	pointers[i].prevPosition.x = pointers[i].position.x;
    	pointers[i].prevPosition.y = pointers[i].position.y;
    }


    SDL_Event e;
    while (SDL_PollEvent(&e)) {
    	switch (e.type) {
    		case SDL_QUIT : {
    			Overture_Stop();
    		}
    		break;

    		case SDL_WINDOWEVENT : {
    			Overture_GetWindow()->handleEvents(e);
    			//debugwindow.handleEvents(e);
    		}
    		break;

    		case SDL_KEYDOWN :
    		case SDL_KEYUP :
    		case SDL_TEXTINPUT : {
    			handleKeyInput(e);
    		}
    		break;

    		case SDL_MOUSEBUTTONUP :
    		case SDL_MOUSEBUTTONDOWN :
    		case SDL_MOUSEMOTION :
    		case SDL_FINGERDOWN :
    		case SDL_FINGERUP :
    		case SDL_FINGERMOTION : {
    			handlePointerInput(e);
    		}
    		break;

    		case SDL_JOYAXISMOTION : {
    			if (e.jaxis.axis == 0) {
    				lx[ding] = e.jaxis.value;
    			}
    			if (e.jaxis.axis == 1) {
    				ly[ding] = e.jaxis.value;
    			}
    			if (e.jaxis.axis == 2) {
    				lz[ding] = e.jaxis.value;

    				int totalx = 0;
    				int totaly = 0;
    				int totalz = 0;
    				for (int i = 0; i < SIZE_DING; i++) {
    					totalx += lx[i];
    					totaly += ly[i];
    					totalz += lz[i];
    				}
    				x = (int) ((float) (totalx)/(float) (SIZE_DING));
    				y = (int) ((float) (totaly)/(float) (SIZE_DING));
    				z = (int) ((float) (totalz)/(float) (SIZE_DING));

    				//SDL_Log("x: %f, y: %f, z: %f", x, y, z);

    				ding += 1;
    				if (ding >= SIZE_DING) {
    					ding = 0;
    				}
    			}
    		}
    		break;

    		case SDL_DROPFILE : {
    	    	char* dropped_filedir;
    	        dropped_filedir = e.drop.file;
    	        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "You dropped a file!", dropped_filedir, Overture_GetWindow()->data);
    	        // Hiermee is het mogelijk om super-secrete messages te geven als je bepaalde bestanden drop op bepaalde momenten in een spel.
    	        SDL_free(dropped_filedir);
    		}
    		break;


    	}
    }

}


void handleKeyInput( SDL_Event e ) {

    bool keyModDown = false;
    if (containsString(Overture_GetOS(), "MacOS")) {
    	keyModDown = SDL_GetModState() & (KMOD_LGUI | KMOD_RGUI);
    }
    else {
    	keyModDown = SDL_GetModState() & KMOD_CTRL;
    }

    if (e.type == SDL_KEYDOWN) {
		JKey key = (JKey) e.key.keysym.scancode;

    	if (!typing) {
    		if (!keyModDown) {
    			switch (key) {
					case KEY_F11 : {
						Overture_GetWindow()->toggleFullscreen();
					} break;

	    			default : {
	        			addKey(key);
	    			}
	    			break;
    			}
    		}
    		else {
    			switch (key) {
					case KEY_Z : {
						//Do stuff
					} break;

	    			default :
	    			break;
    			}
    		}


    		/** TODO-debug: this is for testing purposes */
    		if (key == KEY_RETURN) {
    			typing = true;
        		SDL_StartTextInput();
        		SDL_Log("-Started typing-");
    		}
    	}
    	else {
    		//// Text keyboard commands ////
    		switch (key) {
				case KEY_BACKSPACE : {
					if (inputText.length() > 0) {
						inputText.pop_back();
						if (keyModDown) {
							inputText.clear();
						}
					}
				} break;

				case KEY_C : {
					if (keyModDown) {
						SDL_SetClipboardText(inputText.c_str());
					}
				} break;
				case KEY_X : {
					if (keyModDown) {
						SDL_SetClipboardText(inputText.c_str());
						inputText = "";
					}
				} break;
				case KEY_V : {
					if (keyModDown) {
						inputText += SDL_GetClipboardText();
					}
				} break;

				default :
				break;
    		}


			if (key == KEY_RETURN) {
	    		typing = false;
	    		SDL_StopTextInput();
	    		SDL_Log("-Stoped typing-");
			}
    	}

    }
    else if (e.type == SDL_KEYUP) {
    	JKey key = (JKey) e.key.keysym.scancode;
    	removeKey(key);
    }
	else if (e.type == SDL_TEXTINPUT) {
		if (!keyModDown) {
			inputText += e.text.text;
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
    	//* Removes the inactive pointers so the beginning of so the remaining pointers are all adjoined at the start of the buffer */
		for (int i = 0; i < 16; i++) {
			if (!pointers[i].active) {
				for (int i2 = i+1; i2 < 16; i2++) {
					pointers[i2-1] = pointers[i2];
				}
			}
		}

		if (e.type == SDL_FINGERMOTION) {
			for (int i = 0; i < 16; i++) {
				if (pointers[i].fingerID == e.tfinger.fingerId && pointers[i].active) {
					pointers[i].position.x = e.tfinger.x;
					pointers[i].position.y = e.tfinger.y;
				}
			}
		}
		else if (e.type == SDL_FINGERDOWN) {
			int pointerSize = 16;
			for (int i = 0; i < 16; i++) {
				if (!pointers[i].active) {
					pointerSize = i;
					break;
				}
			}

			if (pointerSize < 15) {
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
			for (int i = 0; i < 16; i++) {
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


