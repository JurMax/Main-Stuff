/*
 * JORender.hpp
 *
 *  Created on: 8 jun. 2017
 *      Author: jurriaanvandenberg
 */

#ifndef OVERTURE_SRC_OBJECT_JORENDER_HPP_
#define OVERTURE_SRC_OBJECT_JORENDER_HPP_

#include <JObject.hpp>
#include <JRenderer.hpp>


struct JOColor : JOComponent {
	JOColor( SDL_Color clr );
	JOColor( int r, int g, int b, int a = 0xFF );

	virtual void update();
	virtual void render();

	SDL_Color color;
};


struct JOTexture : JOComponent {
	JOTexture( JTexture* texture, bool fill = true, bool repeat = true );
	JOTexture( JTexture* texture, float width, float height, bool repeat = true, float offsetx = 0.0f, float offsety = 0.0f );

	virtual void update();
	virtual void render();

	JTexture* texture;
	SDL_Color color;

	bool fill;
	bool repeat;

	float offsetX;
	float offsetY;
	float width;
	float height;

	float motionX;
	float motionY;
};



#endif /* OVERTURE_SRC_OBJECT_JORENDER_HPP_ */
