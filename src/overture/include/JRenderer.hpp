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


struct RenderTarget;
struct ClippingMask;


namespace Renderer {
	extern SDL_Renderer* rRenderer;

	extern RenderTarget* renderTarget;
	extern bool resolutionChanged;

	extern float resolutionScaleX;
	extern float resolutionScaleY;
	extern float renderScaleX;
	extern float renderScaleY;
}


namespace Overture {
	extern Vector2 cameraPos;

	bool Renderer_Init();
	void Renderer_Close();
	void Renderer_Begin();
	void Renderer_End();
}


SDL_Color SDLColor( int r = 0xFF, int g = 0xFF, int b = 0xFF, int a = 0xFF );
SDL_Color SDLRandomColor( int a = 0xFF );


namespace Renderer {

	void setRenderTarget( RenderTarget& target );
	void resetRenderTarget();

	void setResolutionScale( float scale = 1.0f );
	float getResolutionScale();
	void setRenderScale( float scale = 1.0f );
	float getRenderScale();
	SDL_Rect getRenderRect( float posX, float posY, float width, float height, JTexture* texture = NULL);

	void setClearColor( int r, int g, int b, int a = 0xFF );
	void setClearColor( SDL_Color color );
	SDL_Color getClearColor();

	void setRenderColor( int r, int g, int b, int a = 0xFF );
	void setRenderColor( SDL_Color color );
	void resetRenderColor();

	void renderSDLTexture( SDL_Texture* texture, SDL_Rect* renderRect, SDL_Rect* textureRect = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, float rAngle = 0.0f, SDL_Point* rotationPoint = NULL);
	void renderSDLLine( float posX1, float posY1, float posX2, float posY2 );

	void renderTexture( JTexture& texture, float posX, float posY, float width = NAF, float height = NAF, SDL_Rect* textureRect = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, float rAngle = 0.0, float rPosX = NAF, float rPosY = NAF );
	void renderRect( float posX, float posY, float width, float height );
	void renderCharacter( JBitmapFont& bitmap, unsigned int character, float posX, float posY, float width = NAF, float height = NAF );
	void renderString( JBitmapFont& bitmap, std::string str, float posX, float posY, float maxWidth = NAF, float height = NAF );
	void renderTile( JTexture& tilemapTexture, int tileType, float posX, float posY );

	void createClippingMask( float width, float height, bool manual = true );
	void destroyClippingMask( float width, float height, bool destroyAll = false );
	ClippingMask* getClippingMask( float width, float height, int* index = NULL, bool manual = true );
	bool activateClippingMask( float posX, float posY, float width, float height, bool createmask = true );
	void drawClippingMask();
	void pauseClippingMasks( bool disable );
}


struct RenderTarget {
	RenderTarget();
	RenderTarget( int width, int height, SDL_Texture* texture = NULL );
	~RenderTarget();

	void create();
	void destroy();
	void resize( int width, int height );

	JTexture toJTexture();

	SDL_Texture* texture;

	int width;
	int height;

	bool resized;
	int previousWidth;
	int previousHeight;

};


struct ClippingMask {

	SDL_Texture* texture;
	bool inUse;
	bool createdManually;
	bool used; // Is this clippingmask used last frame? (delete if not).

	float posX;
	float posY;
	int width;
	int height;
};

#endif /* SRC_REDOX_SUPPORT_RENDERER_HPP_ */
