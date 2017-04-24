/*
 * renderer.cpp
 *
 *  Created on: 13 okt. 2016
 *      Author: jurriaanvandenberg
 */

#include <JRenderer.hpp>

#include <vector>
#include <math.h>
#include <SDL_ttf.h>
#include <JOverture.hpp>

//#define GL3_PROTOTYPES 1
/*#ifdef __ANDROID__
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#include <OpenGL/gl.h>
#endif*/

SDL_Renderer* rRenderer = NULL;

SDL_Color clearColor = { 0xFF, 0xFF, 0xFF, 0xFF };
SDL_Color renderColor = { 0xFF, 0xFF, 0xFF, 0xFF };

std::vector<ClippingMask> clippingMasks;


bool Renderer_Init() {
	bool success = true;
	setRenderTarget(Overture_GetWindow());
	return success;
}

void setRenderTarget( JWindow* window ) {
	rRenderer = window->renderer;
}


void renderTexture( JTexture *texture, float posX, float posY, float width, float height, SDL_RendererFlip flip, float rAngle, float rPosX, float rPosY ) {
	if (texture->loaded) {
		SDL_Rect renderRect = { (int) roundf(posX), (int) roundf(posY), 0, 0 };
		SDL_Point rotationPoint = { 0, 0 };

		if (width == NAF)  renderRect.w = texture->width;
		else  renderRect.w = (int) roundf(width);
		if (height == NAF)  renderRect.h = texture->height;
		else  renderRect.h = (int) roundf(height);

		if (rPosX == NAF)  rotationPoint.x = renderRect.w / 2;
		else  rotationPoint.x = rPosX;
		if (rPosY == NAF)  rotationPoint.y = renderRect.h / 2;
		else  rotationPoint.y = rPosY;

		SDL_SetTextureColorMod(texture->textureData, renderColor.r, renderColor.g, renderColor.b);
		SDL_SetTextureAlphaMod(texture->textureData, renderColor.a);

		if (clippingMasks.size() > 0) {

			renderRect.x -= (int) roundf(clippingMasks.at(clippingMasks.size() - 1).posX);
			renderRect.y -= (int) roundf(clippingMasks.at(clippingMasks.size() - 1).posY);

			SDL_Texture* renderTarget = SDL_GetRenderTarget(rRenderer);
			SDL_SetRenderTarget(rRenderer, clippingMasks.at(clippingMasks.size() - 1).maskData);

			SDL_RenderCopyEx(rRenderer, texture->textureData, NULL, &renderRect, rAngle, &rotationPoint, flip);

			SDL_SetRenderTarget(rRenderer, renderTarget);
		}
		else {
			SDL_SetRenderTarget(rRenderer, NULL);
			SDL_RenderCopyEx(rRenderer, texture->textureData, NULL, &renderRect, rAngle, &rotationPoint, flip);
		}
	}
}

void renderRect( float posX, float posY, float width, float height ) {
	renderTexture(&textureBlank, posX, posY, width, height);
}


void renderFont( JFont *font, std::string str, unsigned int size, float posx, float posy ) {
	if (font->getFontData(size) == NULL) {
		font->load(size);
	}
	if (font->getFontData(size) != NULL) {
		if (font->getFontData(size)->loaded && str != "") {
			if (Input::isKeyPressed(KEY_1)) TTF_SetFontStyle(font->getFontData(size)->font, TTF_STYLE_NORMAL);
			if (Input::isKeyPressed(KEY_2)) TTF_SetFontStyle(font->getFontData(size)->font, TTF_STYLE_ITALIC);
			if (Input::isKeyPressed(KEY_3)) TTF_SetFontStyle(font->getFontData(size)->font, TTF_STYLE_BOLD);
			if (Input::isKeyPressed(KEY_4)) TTF_SetFontStyle(font->getFontData(size)->font, TTF_STYLE_STRIKETHROUGH);
			if (Input::isKeyPressed(KEY_5)) TTF_SetFontStyle(font->getFontData(size)->font, TTF_STYLE_UNDERLINE);

			SDL_Surface *surface = TTF_RenderText_Blended(font->getFontData(size)->font, str.c_str(), renderColor);
			SDL_Texture *texture = SDL_CreateTextureFromSurface(rRenderer, surface);

			SDL_Rect rect;
			rect.x = (int) roundf(posx);
			rect.y = (int) roundf(posy);
			rect.w = surface->w;
			rect.h = surface->h;

			SDL_SetTextureColorMod(texture, renderColor.r, renderColor.g, renderColor.b);
			SDL_SetTextureAlphaMod(texture, renderColor.a);

			SDL_RenderCopy(rRenderer, texture, NULL, &rect);

			SDL_FreeSurface(surface);
			SDL_DestroyTexture(texture);
		}
	}
}


void setClearColor( int r, int g, int b ) {
	clearColor.r = r;
	clearColor.g = g;
	clearColor.b = b;
}

void setClearColor( SDL_Color color ) {
	clearColor = color;
}

SDL_Color getClearColor() {
	return clearColor;
}


void setRenderColor( int r, int g, int b, int a ) {
	renderColor.r = r;
	renderColor.g = g;
	renderColor.b = b;
	renderColor.a = a;
	SDL_SetRenderDrawColor(rRenderer, renderColor.r, renderColor.g, renderColor.b, renderColor.a);
}

void setRenderColor( SDL_Color color ) {
	renderColor = color;
	SDL_SetRenderDrawColor(rRenderer, renderColor.r, renderColor.g, renderColor.b, renderColor.a);
}

void resetRenderColor() {
	renderColor.r = 0xFF;
	renderColor.g = 0xFF;
	renderColor.b = 0xFF;
	renderColor.a = 0xFF;
	SDL_SetRenderDrawColor(rRenderer, renderColor.r, renderColor.g, renderColor.b, renderColor.a);
}



void addClippingMask( JTexture* texture, float posX, float posY, float width, float height, SDL_RendererFlip flip ) {
	ClippingMask clippingmask;
	clippingmask.posX = posX;
	clippingmask.posY = posY;

	if (width == NAF)  clippingmask.width = texture->width;
	else  clippingmask.width = width;
	if (height == NAF)  clippingmask.height = texture->height;
	else  clippingmask.height = height;

	if (texture->textureData == textureBlank.textureData) {
		clippingmask.textured = false;
		clippingmask.texture = NULL;
		clippingmask.flip = SDL_FLIP_NONE;
	}
	else {
		clippingmask.textured = true;
		clippingmask.texture = texture;
		clippingmask.flip = flip;
	}

	clippingmask.maskData = SDL_CreateTexture(rRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, clippingmask.width, clippingmask.height);
	SDL_SetTextureBlendMode(clippingmask.maskData, SDL_BLENDMODE_BLEND);

	SDL_Texture* renderTarget = SDL_GetRenderTarget(rRenderer);
	SDL_SetRenderTarget(rRenderer, clippingmask.maskData);

	SDL_SetRenderDrawColor(rRenderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(rRenderer);
	SDL_SetRenderDrawColor(rRenderer, renderColor.r, renderColor.g, renderColor.b, renderColor.a);

	SDL_SetRenderTarget(rRenderer, renderTarget);

	clippingMasks.push_back(clippingmask);
}

void addClippingMask( float posX, float posY, float width, float height ) {
	addClippingMask(&textureBlank, posX, posY, width, height, SDL_FLIP_NONE);
}

void drawClippingMask() {
	if (clippingMasks.size() > 0) {
		ClippingMask* cm = &clippingMasks.at(clippingMasks.size() - 1);

		if (cm->textured) {
			// TODO: DIT WERKT NIET, WANT IK HEB GEEN GOEDE BLENDMODE
			// Delete parts that aren't part of the mask
			SDL_Texture* renderTarget = SDL_GetRenderTarget(rRenderer);
			SDL_SetRenderTarget(rRenderer, cm->maskData);


			SDL_SetTextureColorMod(cm->texture->textureData, 0xFF, 0xFF, 0xFF);
			SDL_SetTextureAlphaMod(cm->texture->textureData, 0xFF);
			//SDL_SetRenderDrawBlendMode(rRenderer, SDL_BLENDMODE_NONE);
			SDL_SetTextureBlendMode(cm->texture->textureData, SDL_BLENDMODE_ADD);

			SDL_Rect maskTextureRect = { 0, 0, (int) roundf(cm->width), (int) roundf(cm->height) };
			SDL_RenderCopyEx(rRenderer, cm->texture->textureData, NULL, &maskTextureRect, 0.0, NULL, cm->flip);

			SDL_SetRenderTarget(rRenderer, renderTarget);
		}

		SDL_Rect maskRect = { (int) roundf(cm->posX), (int) roundf(cm->posY), (int) roundf(cm->width), (int) roundf(cm->height) };

		if (clippingMasks.size() > 1) {
			maskRect.x -= (int) roundf(clippingMasks.at(clippingMasks.size() - 2).posX);
			maskRect.y -= (int) roundf(clippingMasks.at(clippingMasks.size() - 2).posY);

			SDL_SetRenderTarget(rRenderer, clippingMasks.at(clippingMasks.size()-2).maskData);
		}
		else {
			SDL_SetRenderTarget(rRenderer, NULL);
		}

		SDL_SetRenderDrawBlendMode(rRenderer, SDL_BLENDMODE_BLEND);


		SDL_RenderCopyEx(rRenderer, cm->maskData, NULL, &maskRect, 0.0f, NULL, SDL_FLIP_NONE);

		SDL_DestroyTexture(cm->maskData);


		clippingMasks.pop_back();
	}
}


