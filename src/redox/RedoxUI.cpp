/*
 * RedoxUI.cpp
 *
 *  Created on: 23 feb. 2017
 *      Author: jurriaanvandenberg
 */

#include "RedoxUI.hpp"

#include <JRenderer.hpp>
#include <JUI.hpp>


JTexture windowCorner;
JTexture windowCornerTop;
JTexture windowCornerBottom;
JTexture windowSide;
JTexture shadowCorner;
JTexture shadowSide;

JUIObject container;
JUIObject uiobject;
JUIObject subuiobject;
JUIObject uiobject2;
JUIObject uiobject3;
JUIObject uiobject4;
JUIObject uiobject5;


void renderWindow( float posx, float posy, float width, float height ) {
	windowCorner.setAndLoad("ui/corner.png");
	windowSide.setAndLoad("ui/side.png");

	float cwidth = (float) windowCorner.width;
	float cheight = (float) windowCorner.height;
	float sheight = (float) windowSide.height;

	setRenderColor(0xFF, 0xFF, 0xFF);
    renderRect(posx + cwidth, posy + sheight, width - cwidth*2, height - sheight*2);
    renderRect(posx + sheight, posy + cheight, width - sheight*2, height - cheight*2);

	renderTexture(&windowCorner, posx, posy + height - cheight);
	renderTexture(&windowCorner, posx + width - cwidth, posy + height - cheight, NAF, NAF, SDL_FLIP_HORIZONTAL);
	renderTexture(&windowCorner, posx, posy, NAF, NAF, SDL_FLIP_VERTICAL);
	renderTexture(&windowCorner, posx + width, posy, - cwidth, NAF, SDL_FLIP_NONE);

	renderTexture(&windowSide, posx + cwidth, posy + height - sheight, width - cwidth*2, sheight);
	renderTexture(&windowSide, posx + cwidth, posy, width - cwidth*2, sheight, SDL_FLIP_VERTICAL);
	renderTexture(&windowSide, posx + sheight, posy + cheight, height - cheight*2, sheight, SDL_FLIP_NONE, 90.0f, 0, 0);
	renderTexture(&windowSide, posx + width, posy + cheight, height - cheight*2, sheight, SDL_FLIP_VERTICAL, 90.0f, 0, 0);

}
void renderactive( float posx, float posy, float width, float height ) {
	setRenderColor(0, 0xFF, 0);
	renderRect(posx, posy, width, height );
}
void renderinactive( float posx, float posy, float width, float height ) {
	setRenderColor(0xFF, 0xFF, 0);
	renderRect(posx, posy, width, height );
}
void renderhover( float posx, float posy, float width, float height ) {
	setRenderColor(0xFF, 0, 0xFF);
	renderRect(posx, posy, width, height );
}
void renderpressed( float posx, float posy, float width, float height ) {
	setRenderColor(0, 0xFF, 0xFF);
	renderRect(posx, posy, width, height );
}

void rendercontainer( float posx, float posy, float width, float height ) {
	setRenderColor(0xFF, 0xFF, 0xFF);
	renderRect(posx, posy, width, height);
}

void renderOtherThing( float posx, float posy, float width, float height ) {
	setRenderColor(0xFF, 0x00, 0x00);
	renderRect(posx, posy, width, height);
}


void UI_Initialize() {

	Overture_UI.setMargins(20.0f);
	container.width = 200;
	container.height = 400;
	container.setMargins(5.0f);
	container.renderActiveFunc = renderWindow;
	Overture_UI.add(&container);

	uiobject.zIndex = 5;
	uiobject.setRenderFuncs(renderactive, renderinactive, renderhover, renderpressed);
	container.add(&uiobject);

	subuiobject.zIndex = 5;
	subuiobject.width = 20;
	subuiobject.height = 20;
	subuiobject.setRenderFuncs(renderOtherThing, renderOtherThing, renderOtherThing, renderOtherThing);
	uiobject.add(&subuiobject);


	uiobject2.zIndex = 3;
	uiobject2.alignX = UI_ALIGN_RIGHT;
	uiobject2.setRenderFuncs(renderactive, renderinactive, renderhover, renderpressed);
	container.add(&uiobject2);

	uiobject3.zIndex = -4;
	uiobject3.alignY = UI_ALIGN_CENTER;
	uiobject3.setRenderFuncs(renderactive, renderinactive, renderhover, renderpressed);
	container.add(&uiobject3);

	uiobject4.zIndex = -4;
	uiobject4.alignY = UI_ALIGN_BOTTOM;
	uiobject4.setRenderFuncs(renderactive, renderinactive, renderhover, renderpressed);
	container.add(&uiobject4);

	uiobject5.zIndex = 40;
	uiobject5.setRenderFuncs(renderactive, renderinactive, renderhover, renderpressed);
	uiobject5.alignX = UI_ALIGN_RIGHT;
	uiobject5.alignY = UI_ALIGN_BOTTOM;
	container.add(&uiobject5);

}

void UI_Update() {

}

void UI_Render() {

}
