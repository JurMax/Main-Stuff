/*
 * JCursor.cpp
 *
 *  Created on: 13 jun. 2017
 *      Author: jurriaanvandenberg
 */

#include <JCursor.hpp>

#include <vector>
#include <SDL.h>

const bool CAN_CURSOR_BE_SET = false;

std::vector<SDL_Cursor*> cursors;
int cursorAmount = 13;


namespace Overture {
	void Cursor_Init() {
		// Initialize cursor list (allocate space)
		for (int i = 0; i < cursorAmount; i++) {
			cursors.push_back(NULL);
		}
	}

	void Cursor_Close() {
		// Free all loaded cursors
		for (int i = 0; i < cursors.size(); i++) {
			if (cursors.at(i) != NULL) {
				SDL_FreeCursor(cursors.at(i));
				cursors.at(i) = NULL;
			}
		}
	}
}


namespace Cursor {
	bool isSet = false;

	void set( int cursor, bool reSet ) {
		if (reSet || !isSet) {
			if (cursor >= cursorAmount) {
				SDL_Log("ERROR: Non-existing cursor is trying to be set");
				return;
			}

			if (cursors.at(cursor) == NULL) {
				load(cursor, NULL);
			}

			if (CAN_CURSOR_BE_SET) {
				SDL_SetCursor(cursors.at(cursor));
			}
			isSet = true;
		}
	}


	int load( int cursor, const char *image[] ) {
		if (cursor >= cursorAmount) {
			SDL_Log("ERROR: Non-existing cursor is trying to be loaded");
			return 0;
		}
		else {
			SDL_Cursor* c;

			if (image != NULL) {
				// Source: https://wiki.libsdl.org/SDL_CreateCursor
				int i, row, col;
				Uint8 data[4 * 32];
				Uint8 mask[4 * 32];
				int hot_x, hot_y;

				i = -1;
				for (row = 0; row < 32; row++) {
					for (col = 0; col < 32; col++) {
						if (col % 8) {
							data[i] <<= 1;
							mask[i] <<= 1;
						} else {
							i++;
							data[i] = mask[i] = 0;
						}
						switch (image[4+row][col]) {
						case 'X':
							data[i] |= 0x01;
							mask[i] |= 0x01;
							break;
						case '.':
							mask[i] |= 0x01;
							break;
						case ' ':
							break;
						}
					}
				}
				sscanf(image[4 + row], "%d,%d", &hot_x, &hot_y);
				c = SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
			}
			else {
				switch (cursor) {
				case CURSOR_ARROW : c = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW); break;
				case CURSOR_IBEAM : c = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM); break;
				case CURSOR_WAIT : c = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT); break;
				case CURSOR_CROSSHAIR : c = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR); break;
				case CURSOR_WAITARROW : c = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAITARROW); break;
				case CURSOR_SIZENWSE : c = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE); break;
				case CURSOR_SIZENESW : c = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW); break;
				case CURSOR_SIZEWE : c = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE); break;
				case CURSOR_SIZENS : c = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS); break;
				case CURSOR_SIZEALL : c = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL); break;
				case CURSOR_NO : c = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO); break;
				case CURSOR_HAND : c = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND); break;
				default : c = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW); break;
				}
			}

			if (cursors.at(cursor) != NULL) {
				SDL_FreeCursor(cursors.at(cursor));
			}
			cursors.at(cursor) = c;
		}
		return cursor;
	}


	int createCustom() {
		int i = cursorAmount;
		cursorAmount += 1;
		cursors.push_back(NULL);
		return i;
	}


	void hide() {
		SDL_ShowCursor(SDL_DISABLE);
	}


	void show() {
		SDL_ShowCursor(SDL_ENABLE);
	}


	void toggleVisibility() {
		if (SDL_ShowCursor(SDL_QUERY) == 0) {
			SDL_ShowCursor(SDL_ENABLE);
		}
		else {
			SDL_ShowCursor(SDL_DISABLE);
		}
	}

}


