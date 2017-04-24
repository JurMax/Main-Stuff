/*
 * renderer.hpp
 *
 *  Created on: 13 okt. 2016
 *      Author: jurriaanvandenberg
 */

#ifndef SRC_REDOX_SUPPORT_RENDERER_HPP_
#define SRC_REDOX_SUPPORT_RENDERER_HPP_

#include <JOverture.hpp>
#include <JResources.hpp>

extern SDL_Renderer* rRenderer;

bool Renderer_Init();

void setRenderTarget( JWindow* window );

void renderTexture( JTexture* texture, float posX, float posY, float width = NAF, float height = NAF, SDL_RendererFlip flip = SDL_FLIP_NONE, float rAngle = 0.0, float rPosX = NAF, float rPosY = NAF );
void renderRect( float posX, float posY, float width, float height );

// TODO: change this to create texture from font, which returns a texture which can be drawn everywhere.
void renderFont( JFont *font, std::string str, unsigned int size, float posx, float posy );

void setClearColor( int r, int g, int b );
void setClearColor( SDL_Color color );
SDL_Color getClearColor();

void setRenderColor( int r, int g, int b, int a = 0xFF );
void setRenderColor( SDL_Color color );
void resetRenderColor();


struct ClippingMask {

	SDL_Texture* maskData;

	float posX;
	float posY;
	float width;
	float height;

	float textured;
	JTexture* texture;
	SDL_RendererFlip flip;
};

void addClippingMask( JTexture* texture, float posX, float posY, float width = NAF, float height = NAF, SDL_RendererFlip flip = SDL_FLIP_NONE);
void addClippingMask( float posX, float posY, float width, float height);
void drawClippingMask();


#endif /* SRC_REDOX_SUPPORT_RENDERER_HPP_ */
