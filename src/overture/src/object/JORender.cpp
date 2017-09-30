/*
 * JORender.cpp
 *
 *  Created on: 8 jun. 2017
 *      Author: jurriaanvandenberg
 */

#include <JORender.hpp>

using namespace Renderer;


////////// JObject Color Component //////////

JOColor::JOColor( SDL_Color clr ) {
	color = clr;
}


JOColor::JOColor( int r, int g, int b, int a ) {
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
}


void JOColor::update() {
}


void JOColor::render() {
	setRenderColor(color);
	renderRect(object->getPosX(), object->getPosY(), object->getWidth(), object->getHeight());
}



////////// JObject Texture Component //////////

JOTexture::JOTexture( JTexture* texture, bool fill, bool repeat ) {
	this->texture = texture;
	color = SDLColor();

	this->fill = fill;
	this->repeat = repeat;

	offsetX = 0.0f;
	offsetY = 0.0f;
	width = (float) (texture->width);
	height = (float) (texture->height);

	motionX = 0.0f;
	motionY = 0.0f;
}


JOTexture::JOTexture( JTexture* texture, float width, float height, bool repeat, float offsetx, float offsety ) {
	this->texture = texture;
	color = SDLColor();

	fill = false;
	this->repeat = repeat;

	offsetX = offsetx;
	offsetY = offsety;
	this->width = width;
	this->height = height;

	motionX = 0.0f;
	motionY = 0.0f;
}


void JOTexture::update() {
	offsetX += motionX * dt();
	offsetY += motionY * dt();
}


void JOTexture::render() {
	activateClippingMask(object->getPosX(), object->getPosY(), object->getWidth(), object->getHeight());
	setRenderColor(color);

	float basePosX = object->getPosX() + offsetX;
	float basePosY = object->getPosY() + offsetY;
	float baseWidth;
	float baseHeight;

	if (fill) {
		baseWidth = (width / (float) texture->width) * object->getWidth();
		baseHeight = (height / (float) texture->height) * object->getHeight();
	}
	else {
		baseWidth = width;
		baseHeight = height;
	}

	if (repeat) {
		int offsetx = ceilf((float) (offsetX / baseWidth));
		int offsety = ceilf((float) (offsetY / baseHeight));

		for (int i = 0; true; i++) {
			for (int i2 = 0; true; i2++) {

				renderTexture(*texture, basePosX + baseWidth * (i2 - offsetx), basePosY + baseHeight * (i - offsety), baseWidth, baseHeight);

				if (baseWidth * (i2 + 1 - offsetx) + offsetX >= object->getWidth()) {
					break;
				}
			}

			if (baseHeight * (float) (i + 1 - offsety) + offsetY >= object->getHeight()) {
				break;
			}
		}
	}
	else {
		renderTexture(*texture, basePosX, basePosY, baseWidth, baseHeight);
	}

	drawClippingMask();
}
