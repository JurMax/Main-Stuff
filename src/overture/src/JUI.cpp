/*
 * JUI.cpp
 *
 *  Created on: 25 dec. 2016
 *      Author: jurriaanvandenberg
 */

#include <JUI.hpp>

#include <SDL_Log.h>
#include <JOverture.hpp>
#include <JInput.hpp>
#include <JRenderer.hpp>


JUIObject Overture_UI;

bool clickConsumed = false;
bool cursorSet = false;


void Overture::updateUI() {
	clickConsumed = false;
	cursorSet = false;
	SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));

	Overture_UI.width = Overture_GetWindow()->width;
	Overture_UI.height = Overture_GetWindow()->height;

	Overture_UI.update();
}



////////////////////JUIObject////////////////////////

JUIObject::JUIObject() {
	state = UI_ACTIVE;
	draggable = true;
	resizable = true;

	/** relative position/size */
	posX = 0;
	posY = 0;
	width = 50;
	height = 50;

	/** absolute position/size */
	aPosX = 0;
	aPosY = 0;
	aWidth = 0;
	aHeight = 0;

	alignX = UI_ALIGN_LEFT;
	alignY = UI_ALIGN_TOP;
	zIndex = 0;

	parent = NULL;

	marginRight = 0;
	marginLeft = 0;
	marginTop = 0;
	marginBottom = 0;

	renderActiveFunc = NULL;
	renderInactiveFunc = NULL;
	renderHoverFunc = NULL;
	renderPressedFunc = NULL;

	onClickFunc = NULL;
	onDragFunc = NULL;
}


JUIObject::~JUIObject() {
	// TODO Auto-generated constructor stub
}


void JUIObject::update() {
	for (int i = 0; i < childs.size(); i++) {
		childs.at(childs.size() - i - 1)->update();
	}

	if (state != UI_INACTIVE) {
		if (Input::isAreaClicked(aPosX, aPosY, aWidth, aHeight)) {
			if (onClickFunc != NULL) {
				onClickFunc();
			}
		}

		if (Input::isAreaBeingClicked(aPosX, aPosY, aWidth, aHeight)) {
			state = UI_PRESSED;
			//SDL_SetCursor(CURSOR_SIZEALL);
		}
		else if (Input::isAreaHovered(aPosX, aPosY, aWidth, aHeight)) {
			state = UI_HOVER;
			//if (childs.size() == 0)
			//SDL_SetCursor(CURSOR_WAIT);
		}
		else {
			state = UI_ACTIVE;
		}
	}

	handleResizing();
}


void JUIObject::handleResizing() {
	int resizeWidth = 8;

	if ((draggable || resizable) && !isWindow()) {
		for (int i = 0; i < 16; i++) {
			if (Input::pointers[i].active) {
				float pposx = Input::pointers[i].position.x * Overture_GetWindow()->width;
				float pposy = Input::pointers[i].position.y * Overture_GetWindow()->height;

				bool inside = pposx > aPosX + resizeWidth && pposx < aPosX + aWidth - resizeWidth &&
						      pposy > aPosY + resizeWidth && pposy < aPosY + aHeight - resizeWidth;

				bool left = pposx >= aPosX && pposx <= aPosX + resizeWidth &&
							pposy >= aPosY && pposy < aPosY + aHeight;
				bool right = pposx >= aPosX + aWidth - resizeWidth && pposx < aPosX + aWidth &&
							 pposy >= aPosY && pposy < aPosY + aHeight;
				bool top = pposx >= aPosX && pposx < aPosX + aWidth &&
						   pposy >= aPosY && pposy <= aPosY + resizeWidth;
				bool bottom = pposx >= aPosX && pposx < aPosX + aWidth &&
							  pposy >= aPosY + aHeight - resizeWidth && pposy < aPosY + aHeight;

				if (inside) {
					if (!cursorSet) SDL_SetCursor(Input::CURSOR_HAND);
					updateDragValue(i, 1);
				}
				else if (left && top) {
					if (!cursorSet) SDL_SetCursor(Input::CURSOR_CROSSHAIR);
					updateDragValue(i, 2);
				}
				else if (left && bottom) {
					if (!cursorSet) SDL_SetCursor(Input::CURSOR_CROSSHAIR);
					updateDragValue(i, 3);
				}
				else if (right && top) {
					if (!cursorSet) SDL_SetCursor(Input::CURSOR_CROSSHAIR);
					updateDragValue(i, 4);
				}
				else if (right && bottom) {
					if (!cursorSet) SDL_SetCursor(Input::CURSOR_CROSSHAIR);
					updateDragValue(i, 5);
				}
				else if (left) {
					if (!cursorSet) SDL_SetCursor(Input::CURSOR_SIZEWE);
					updateDragValue(i, 6);
				}
				else if (right) {
					if (!cursorSet) SDL_SetCursor(Input::CURSOR_SIZEWE);
					updateDragValue(i, 7);
				}
				else if (top) {
					if (!cursorSet) SDL_SetCursor(Input::CURSOR_SIZENS);
					updateDragValue(i, 8);
				}
				else if (bottom) {
					if (!cursorSet) SDL_SetCursor(Input::CURSOR_SIZENS);
					updateDragValue(i, 9);
				}


				if (dragPointerStates[i] != 0) {
					if (!cursorSet) {
						SDL_SetCursor(Input::CURSOR_SIZEALL);
						cursorSet = true;
					}
					float diffX = (Input::pointers[i].position.x - Input::pointers[i].prevPosition.x) * Overture_GetWindow()->width;
					float diffY = (Input::pointers[i].position.y - Input::pointers[i].prevPosition.y) * Overture_GetWindow()->height;

					switch (dragPointerStates[i]) {
						case 1 : {  // middle
							posX += diffX;
							posY += diffY;
						} break;
						case 2 : {  // left top
							posX += diffX;
							posY += diffY;
							width -= diffX;
							height -= diffY;
						} break;
						case 3 : {  // left bottom
							posX += diffX;
							width -= diffX;
							height += diffY;
						} break;
						case 4 : {  // right top
							posY += diffY;
							width += diffX;
							height -= diffY;
						} break;
						case 5 : {  // right bottom
							width += diffX;
							height += diffY;
						} break;
						case 6 : {  // left
							posX += diffX;
							width -= diffX;
						} break;
						case 7 : {  // right
							width += diffX;
						} break;
						case 8 : {  // top
							posY += diffY;
							height -= diffY;
						} break;
						case 9 : {  // bottom
							height += diffY;
						} break;
					}

					if (Input::pointers[i].released) {
						dragPointerStates[i] = 0;
					}
				}
			}
		}
	}
}


void JUIObject::updateDragValue( int pointer, int dragstate ) {
	if (Input::pointers[pointer].pressed && !clickConsumed) {
		dragPointerStates[pointer] = dragstate;
		clickConsumed = true;
	}
}


void JUIObject::render() {

	updatePosition();

	if (state == UI_INACTIVE && renderInactiveFunc != NULL) {
		renderInactiveFunc(aPosX, aPosY, aWidth, aHeight);
	}
	else if (state == UI_HOVER && renderHoverFunc != NULL) {
		renderHoverFunc(aPosX, aPosY, aWidth, aHeight);
	}
	else if (state == UI_PRESSED && renderPressedFunc != NULL) {
		renderPressedFunc(aPosX, aPosY, aWidth, aHeight);
	}
	else {
		if (renderActiveFunc != NULL) {
			renderActiveFunc(aPosX, aPosY, aWidth, aHeight);
		}
	}

	addClippingMask(aPosX, aPosY, aWidth, aHeight);
	//SDL_Log("%f, %f", aWidth, aHeight);

	for (int i = 0; i < childs.size(); i++) {
		childs.at(i)->render();
	}

	drawClippingMask();
}



void JUIObject::updatePosition() {
	if (isWindow()) {
		aPosX = posX;
		aPosY = posY;
		aWidth = width;
		aHeight = height;
	}
	else {
		if (alignX == UI_ALIGN_LEFT)
			aPosX = parent->aPosX + parent->marginLeft + posX;
		else if (alignX == UI_ALIGN_CENTER)
			aPosX = parent->aPosX + parent->aWidth/2 + posX - aWidth/2;
		else if (alignX == UI_ALIGN_RIGHT)
			aPosX = parent->aPosX + parent->aWidth - parent->marginRight + posX - width;

		if (alignY == UI_ALIGN_TOP)
			aPosY = parent->aPosY + parent->marginTop + posY;
		else if (alignY == UI_ALIGN_CENTER)
			aPosY = parent->aPosY + parent->aHeight/2 + posY - height/2;
		else if (alignY == UI_ALIGN_BOTTOM)
			aPosY = parent->aPosY + parent->aHeight - parent->marginBottom + posY - height;

		aWidth = width;
		aHeight = height;
	}
}


void JUIObject::add( JUIObject *object ) {
	if (object->parent == NULL) {
		object->parent = this;
		childs.push_back(object);
		//sortObjects();
	}
	else {
		SDL_Log("ERROR: UI element already has a parent");
	}
}


void JUIObject::sortObjects() {
	std::sort(childs.begin(), childs.end());
}


void JUIObject::setMargins( float margin ) {
	marginRight = margin;
	marginLeft = margin;
	marginTop = margin;
	marginBottom = margin;
}

void JUIObject::setPosition( float posx, float posy ) {
	posX = posy;
	posY = posy;
}

void JUIObject::setBounds( float posx, float posy, float wdth, float hght ) {
	posX = posy;
	posY = posy;
	width = wdth;
	height = hght;
}

void JUIObject::setRenderFuncs( UIRenderFunc active, UIRenderFunc inactive, UIRenderFunc hover, UIRenderFunc pressed ) {
	renderActiveFunc = active;
	renderInactiveFunc = inactive;
	renderHoverFunc = hover;
	renderPressedFunc = pressed;
}

bool JUIObject::isWindow() {
	return parent == NULL;
}



