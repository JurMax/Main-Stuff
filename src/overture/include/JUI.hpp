/*
 * JUI.hpp
 *
 *  Created on: 25 dec. 2016
 *      Author: jurriaanvandenberg
 */

#ifndef OVERTURE_INCLUDE_JUI_HPP_
#define OVERTURE_INCLUDE_JUI_HPP_

#include <string>
#include <vector>
#include <algorithm>

typedef enum {
	UI_ACTIVE = 0,
	UI_INACTIVE = 1,
	UI_HOVER = 2,
    UI_PRESSED = 3
} UIState;


typedef enum {
	UI_ALIGN_LEFT = 0,
	UI_ALIGN_CENTER = 1,
	UI_ALIGN_RIGHT = 2,
	UI_ALIGN_TOP = 0,
	UI_ALIGN_BOTTOM = 2
} UIAlign;

typedef void (*UIRenderFunc)(float, float, float, float);

/**
 * TODO:
 *  Container system X
 *   Alignment system X
 *  Interaction
 *   Pointers and keyboard/controller
 *   Consume clicks
 *  Render X
 *   Rounded and resizable rects
 *   Shadows
 *   text content
 *  Hover label
 */

class JUIObject {
public:
	JUIObject();
	~JUIObject();

	void update();
	void render();

	// Update the absolute position of the object. This functions gets called in render().
	void updatePosition();
	// IMPORTANT: make sure that you keep the JUIObject allocated. Othererwise this WILL result in a crash.
	void add( JUIObject *object );
	void sortObjects();

	void setMargins( float margin );
	void setPosition( float posx, float posy );
	void setBounds( float posx, float posy, float wdth, float hght );
	void setRenderFuncs( UIRenderFunc active, UIRenderFunc inactive, UIRenderFunc hover, UIRenderFunc pressed );

	bool isWindow();

public:
	UIState state;
	bool draggable;
	bool resizable;

	float posX;
	float posY;
	float width;
	float height;

	float aPosX;
	float aPosY;
	float aWidth;
	float aHeight;

	UIAlign alignX;
	UIAlign alignY;
	int zIndex;

	JUIObject *parent;
	std::vector<JUIObject *> childs;
	std::string textContent;  // TODO

	float marginRight;
	float marginLeft;
	float marginTop;
	float marginBottom;

	UIRenderFunc renderActiveFunc;
	UIRenderFunc renderInactiveFunc;
	UIRenderFunc renderHoverFunc;
	UIRenderFunc renderPressedFunc;

	void (*onClickFunc)();
	void (*onDragFunc)( int dragPointer );
	void (*animationInFunc)();  // TODO
	void (*animationOutFunc)();

	int dragPointerStates[16];

    bool operator< (const JUIObject &other) const {
        return zIndex < other.zIndex;
    }

private:
	void handleResizing();
	void updateDragValue( int pointer, int dragstate );
};

extern JUIObject Overture_UI;

namespace Overture {
	void updateUI();
}


#endif /* OVERTURE_INCLUDE_JUI_HPP_ */
