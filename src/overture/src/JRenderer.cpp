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
#include <JWorld.hpp>

//#define GL3_PROTOTYPES 1
/*#ifdef __ANDROID__
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#include <OpenGL/gl.h>
#endif*/

using namespace Overture;
using namespace Renderer;


RenderTarget mainTarget;

SDL_Color clearColor = { 0xFF, 0xFF, 0xFF, 0xFF };
SDL_Color renderColor = { 0xFF, 0xFF, 0xFF, 0xFF };

std::vector<ClippingMask> clippingMasks;
std::vector<ClippingMask*> activeClippingMasks;
bool clippingMasksPaused = false;


// Extern variables.
namespace Renderer {
	SDL_Renderer* rRenderer = NULL;

	// RenderTarget only used if no clipping mask are active.
	RenderTarget* renderTarget = NULL;
	bool resolutionChanged = true;

	// Render the same, but at a different resolution (so same pixels are used when upscaled).
	float resolutionScaleX = 1.0f;
	float resolutionScaleY = 1.0f;

	// Scale render, but keep the same resolution (so more pixels are used when upscaled).
	float renderScaleX = 1.0f;
	float renderScaleY = 1.0f;
}


namespace Overture {

	Vector2 cameraPos;

	bool Renderer_Init() {
		bool success = true;
		rRenderer = Overture_GetWindow()->renderer;
		mainTarget.resize(100, 100);
		renderTarget = &mainTarget;
		return success;
	}


	void Renderer_Close() {
		for (int i = 0; i < clippingMasks.size(); i++) {
			SDL_DestroyTexture(clippingMasks[i].texture);
		}
		clippingMasks.clear();

		renderTarget->destroy();
		if (renderTarget != &mainTarget) {
			mainTarget.destroy();
		}
	}


	void Renderer_Begin() {

		// Change target resolution after system window resolution or resolution scale change.
		static bool resize = false;
		mainTarget.resized = false;

		if (resolutionChanged) {
			resize = true;
			resolutionChanged = false;
		}
		else if (resize) {
			mainTarget.resize((int) ceilf(Overture_GetWindow()->width / resolutionScaleX), (int) ceilf(Overture_GetWindow()->height / resolutionScaleY));
			resize = false;
			resolutionChanged = false;
		}


		// Clear the window.
		SDL_SetRenderTarget(rRenderer, NULL);
		setRenderColor(0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(rRenderer);
		setRenderColor(0xFF, 0xFF, 0xFF, 0xFF);
		renderRect(-10000, -10000, 20000, 20000);

		// Clear the main render target.
		SDL_SetRenderTarget(rRenderer, mainTarget.texture);
		setRenderColor(0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(rRenderer);
		setRenderColor(getClearColor());
		renderRect(-100000, -100000, 200000, 200000);
	}


	void Renderer_End() {

		// Render mainTarget to window.
		SDL_SetRenderTarget(rRenderer, NULL);
		SDL_Rect renderRect;
		renderRect.w = mainTarget.width * resolutionScaleX;
		renderRect.h = mainTarget.height * resolutionScaleY;
		renderRect.x = Overture_GetWindow()->width/2 - renderRect.w/2;
		renderRect.y = Overture_GetWindow()->height/2 - renderRect.h/2;
		resetRenderColor();
		SDL_RenderCopyEx(rRenderer, mainTarget.texture, NULL, &renderRect, 0.0f, NULL, SDL_FLIP_NONE);


		// Destroy unused clippingmasks.
		for (int i = 0; i < clippingMasks.size(); i++) {
			ClippingMask* clippingmask = &clippingMasks[i];
			if (!clippingmask->createdManually && !clippingmask->used) {
				destroyClippingMask(clippingmask->width, clippingmask->height);
				i--;
			}
		}

		// Render to the screen.
		SDL_RenderPresent(rRenderer);
	}
}




SDL_Color SDLColor( int r, int g, int b, int a ) {
	SDL_Color color;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	return color;
}


SDL_Color SDLRandomColor( int a ) {
	SDL_Color color;
	color.r = rand() % 255;
	color.g = rand() % 255;
	color.b = rand() % 255;
	color.a = a;
	return color;
}




namespace Renderer {

	void setRenderTarget( RenderTarget& target ) {
		renderTarget = &target;
		SDL_SetRenderTarget(rRenderer, renderTarget->texture);
	}


	void resetRenderTarget() {
		setRenderTarget(mainTarget);
		if (activeClippingMasks.size() > 0 && !clippingMasksPaused) {
			ClippingMask* cm = activeClippingMasks[activeClippingMasks.size() - 1];
			SDL_SetRenderTarget(rRenderer, cm->texture);
		}
	}



	void setResolutionScale( float scale ) {
		resolutionScaleX = scale;
		resolutionScaleY = scale;
		resolutionChanged = true;
	}


	float getResolutionScale() {
		return resolutionScaleY;
	}


	void setRenderScale( float scale ) {
		renderScaleX = scale;
		renderScaleY = scale;
	}


	float getRenderScale() {
		return renderScaleY;
	}


	SDL_Rect getRenderRect( float posX, float posY, float width, float height, JTexture* texture ) {
		SDL_Rect renderRect;

		renderRect.x = (int) roundf((posX - cameraPos.x) * renderScaleX) + renderTarget->width/2;
		renderRect.y = (int) roundf((posY - cameraPos.y) * renderScaleY) + renderTarget->height/2;

		if (width == NAF && texture != NULL) {
			renderRect.w = (int) roundf(texture->width * renderScaleX);
		}
		else {
			renderRect.w = (int) roundf(width * renderScaleX);
			//renderRect.w = (int) (roundf((width + posX) * renderScaleX) - roundf(posX * renderScaleX));
		}
		if (height == NAF && texture != NULL) {
			renderRect.h = (int) roundf(texture->height * renderScaleY);
		}
		else {
			renderRect.h = (int) roundf(height * renderScaleY);
			//renderRect.h = (int) (roundf((height + posY) * renderScaleY) - roundf(posY * renderScaleY));
		}

		return renderRect;
	}



	void setClearColor( int r, int g, int b, int a ) {
		clearColor.r = r;
		clearColor.g = g;
		clearColor.b = b;
		clearColor.a = a;
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



	void renderSDLTexture( SDL_Texture* texture, SDL_Rect* renderRect, SDL_Rect* textureRect, SDL_RendererFlip flip, float rAngle, SDL_Point* rotationPoint) {
		SDL_SetTextureColorMod(texture, renderColor.r, renderColor.g, renderColor.b);
		SDL_SetTextureAlphaMod(texture, renderColor.a);

		if (activeClippingMasks.size() > 0 && renderRect != NULL && !clippingMasksPaused) {
			// Renderering to a clipping mask.
			ClippingMask* cm = activeClippingMasks[activeClippingMasks.size() - 1];
			SDL_Rect cmRect = getRenderRect(cm->posX, cm->posY, cm->width, cm->height);
			renderRect->x -= cmRect.x;
			renderRect->y -= cmRect.y;
		}

		SDL_RenderCopyEx(rRenderer, texture, textureRect, renderRect, rAngle, rotationPoint, flip);
	}


	void renderSDLLine( float posX1, float posY1, float posX2, float posY2 ) {
		SDL_SetRenderDrawColor(rRenderer, renderColor.r, renderColor.g, renderColor.b, renderColor.a);
		SDL_Rect renderRect = getRenderRect(posX1, posY1, 0.0f, 0.0f, NULL);
		SDL_Rect renderRect2 = getRenderRect(posX2, posY2, 0.0f, 0.0f, NULL);

		if (activeClippingMasks.size() > 0 && !clippingMasksPaused) {
			// Renderering to a clipping mask
			ClippingMask* cm = activeClippingMasks[activeClippingMasks.size() - 1];
			SDL_Rect cmRect = getRenderRect(cm->posX, cm->posY, cm->width, cm->height);
			renderRect.x -= cmRect.x;
			renderRect.y -= cmRect.y;
			renderRect2.x -= cmRect.x;
			renderRect2.y -= cmRect.y;
		}
		SDL_RenderDrawLine(rRenderer, renderRect.x, renderRect.y, renderRect2.x, renderRect2.y);
	}


	void renderTexture( JTexture& texture, float posX, float posY, float width, float height, SDL_Rect* textureRect, SDL_RendererFlip flip, float rAngle, float rPosX, float rPosY ) {
		if (texture.loaded) {
			SDL_Rect renderRect = getRenderRect(posX, posY, width, height, &texture);
			SDL_Point rotationPoint = { 0, 0 };

			if (rPosX == NAF)  rotationPoint.x = renderRect.w / 2;
			else  rotationPoint.x = rPosX;
			if (rPosY == NAF)  rotationPoint.y = renderRect.h / 2;
			else  rotationPoint.y = rPosY;

			renderSDLTexture(texture.textureData, &renderRect, textureRect, flip, rAngle, &rotationPoint);
		}
	}


	void renderRect( float posX, float posY, float width, float height ) {
		renderTexture(textureBlank, posX, posY, width, height);
	}


	void renderCharacter( JBitmapFont& bitmap, unsigned int character, float posX, float posY, float width, float height ) {
		if (bitmap.loaded) {
			int charRow = bitmap.charBounds[character*3 + 0];
			int charLeft = bitmap.charBounds[character*3 + 1];
			int charRight = bitmap.charBounds[character*3 + 2];
			int charWidth = charRight - charLeft;

			if (width == NAF) {
				width = charWidth;
			}
			if (height == NAF) {
				height = bitmap.cellHeight;
			}

			SDL_Rect renderRect = getRenderRect(posX, posY, width, height);
			SDL_Rect textureRect = { charLeft, charRow * bitmap.cellHeight, charWidth, bitmap.cellHeight };

			renderSDLTexture(bitmap.bitmapTexture, &renderRect, &textureRect);
		}
	}


	void renderString( JBitmapFont& bitmap, std::string str, float posX, float posY, float maxWidth, float height ) {
		float posx = 0;

		float widthModifier = 1.0f;
		if (height != NAF) {
			widthModifier = height / (float) (bitmap.cellHeight);
		}

		for (int i = 0; i < str.length(); i++) {

			if (str[i] == ' ') {
				posx += (float) (bitmap.cellWidth / 2) * widthModifier;
			}
			else {
				unsigned int c = (unsigned int) str[i];
				float charWidth = bitmap.charBounds[c*3 + 2] - bitmap.charBounds[c*3 + 1];

				if (posx + charWidth >= maxWidth) {
					break;
				}

				renderCharacter(bitmap, c, posX + posx, posY, charWidth * widthModifier, height);
				posx += charWidth * widthModifier;
			}

			posx += (float) (bitmap.charDistance) * widthModifier;
		}
	}


	void renderTile( JTexture& tilemapTexture, int tileType, float posX, float posY ) {
		if (tilemapTexture.loaded) {
			int tileSize = TILE_SIZE;

			int textureX = abs(tileType) * tileSize;
			int textureY = textureX / tilemapTexture.width;
			textureX -= textureY * tilemapTexture.width;

			SDL_RendererFlip flip = SDL_FLIP_NONE;
			if (tileType < 0) {
				flip = SDL_FLIP_HORIZONTAL;
			}

			SDL_Rect renderRect = getRenderRect(posX, posY, tileSize, tileSize);
			SDL_Rect textureRect = { textureX, textureY * tileSize, tileSize, tileSize };

			renderSDLTexture(tilemapTexture.textureData, &renderRect, &textureRect, flip);
		}
	}



	void createClippingMask( float width, float height, bool manual ) {
		if (getClippingMask(width, height) == NULL) {
			ClippingMask cm;
			cm.inUse = false;
			cm.createdManually = manual;
			cm.used = false;

			cm.posX = 0.0f;
			cm.posY = 0.0f;
			cm.width = (int) roundf(width * renderScaleX);
			cm.height = (int) roundf(height * renderScaleY);
			cm.texture = SDL_CreateTexture(rRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, cm.width, cm.height);
			SDL_SetTextureBlendMode(cm.texture, SDL_BLENDMODE_BLEND);

			clippingMasks.push_back(cm);
		}
	}


	void destroyClippingMask( float width, float height, bool destroyAll ) {
		int index;
		ClippingMask* cmask = getClippingMask(width, height, &index, true);
		if (cmask != NULL) {
			if (cmask->texture != NULL) {
				unloadSDLTexture(cmask->texture);
				cmask->texture = NULL;
			}
			clippingMasks.erase(clippingMasks.begin() + index);
			if (destroyAll) {
				destroyClippingMask(width, height);
			}
		}
	}


	ClippingMask* getClippingMask( float width, float height, int* index, bool manual ) {
		ClippingMask* cmask = NULL;
		for (int i = 0; i < clippingMasks.size(); i++) {
			ClippingMask* clippingmask = &clippingMasks[i];
			if (manual) {
				if (!clippingmask->createdManually) {
					break;
				}
			}

			if (clippingmask->width == (int) roundf(width * renderScaleX) && clippingmask->height == (int) roundf(height * renderScaleY) && !clippingmask->inUse) {
				cmask = &clippingMasks[i];
				if (index != NULL) {
					*index = i;
				}
				break;
			}
		}
		return cmask;
	}


	bool activateClippingMask( float posX, float posY, float width, float height, bool createmask ) {
		if (getClippingMask(width, height, NULL, false) == NULL) {
			if (createmask) {
				createClippingMask(width, height, false);
			}
			else {
				return false;
			}
		}

		ClippingMask* clippingmask = getClippingMask(width, height, NULL, false);
		if (clippingmask != NULL) {
			clippingmask->inUse = true;
			clippingmask->used = true;
			clippingmask->posX = posX;
			clippingmask->posY = posY;

			if (clippingmask->width > 0 && clippingmask->height > 0 && !clippingMasksPaused) {
				SDL_SetRenderTarget(rRenderer, clippingmask->texture);

				SDL_SetRenderDrawColor(rRenderer, 0x00, 0x00, 0x00, 0x00);
				SDL_RenderClear(rRenderer);
				SDL_SetRenderDrawColor(rRenderer, renderColor.r, renderColor.g, renderColor.b, renderColor.a);
			}
			activeClippingMasks.push_back(clippingmask);
			return true;
		}
		else {
			return false;
		}
	}


	void drawClippingMask() {
		if (activeClippingMasks.size() != 0) {
			ClippingMask* clippingmask = activeClippingMasks[activeClippingMasks.size() - 1];

			if (clippingmask->width > 0 && clippingmask->height > 0) {
				SDL_Rect renderRect = getRenderRect(clippingmask->posX, clippingmask->posY, clippingmask->width, clippingmask->height);
				renderRect.w = clippingmask->width;
				renderRect.h = clippingmask->height;

				if (activeClippingMasks.size() > 1 && !clippingMasksPaused) {
					// Renderering to a clipping mask
					ClippingMask* ocm = activeClippingMasks[activeClippingMasks.size() - 2];
					SDL_Rect ocmRect = getRenderRect(ocm->posX, ocm->posY, ocm->width, ocm->height);
					renderRect.x -= ocmRect.x;
					renderRect.y -= ocmRect.y;
					SDL_SetRenderTarget(rRenderer, ocm->texture);
				}
				else {
					SDL_SetRenderTarget(rRenderer, renderTarget->texture);
				}

				SDL_RenderCopyEx(rRenderer, clippingmask->texture, NULL, &renderRect, 0.0, NULL, SDL_FLIP_NONE);
			}
			clippingmask->inUse = false;
			activeClippingMasks.pop_back();
		}
	}


	void pauseClippingMasks( bool pause ) {
		// Disable clippingmasks untill unpaused.
		if (pause) {
			clippingMasksPaused = true;
			SDL_SetRenderTarget(rRenderer, renderTarget->texture);
		}
		else {
			clippingMasksPaused = false;
			if (activeClippingMasks.size() > 0) {
				ClippingMask* cmask = activeClippingMasks[activeClippingMasks.size() - 1];
				SDL_SetRenderTarget(rRenderer, cmask->texture);
			}
		}
	}
}




////////////////////// Render Target //////////////////////

RenderTarget::RenderTarget() {
	texture = NULL;
	width = 0;
	height = 0;
	previousWidth = 0;
	previousHeight = 0;
	resized = false;
}


RenderTarget::RenderTarget( int width, int height, SDL_Texture* texture ) {
	if (texture != NULL) {
		this->width = width;
		this->height = height;
		this->texture = texture;
	}
	else {
		resize(width, height);
	}
}


RenderTarget::~RenderTarget() {
	destroy();
}


void RenderTarget::create() {
	destroy();
	texture = SDL_CreateTexture(rRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
}


void RenderTarget::destroy() {
	if (texture != NULL) {
		unloadSDLTexture(texture);
		texture = NULL;
	}
}


void RenderTarget::resize( int width, int height ) {
	this->previousWidth = this->width;
	this->previousHeight = this->height;
	this->width = width;
	this->height = height;
	this->resized = true;
	create();
}


JTexture RenderTarget::toJTexture() {
	JTexture jtexture;
	jtexture.textureData = texture;
	jtexture.width = width;
	jtexture.height = height;
	return jtexture;
}


