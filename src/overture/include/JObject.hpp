/*
 * JObject.hpp
 *
 *  Created on: 25 apr. 2017
 *      Author: jurriaanvandenberg
 */

#ifndef OVERTURE_INCLUDE_JOBJECT_HPP_
#define OVERTURE_INCLUDE_JOBJECT_HPP_

#include <string>
#include <vector>
#include <SDL.h>
#include <JResources.hpp>

/**
 * TODO
 * 	Sort by Zindex
 * 	More stuff
 * 	Position in relation to children (localposition)
 */


struct JCompTexture {
	JTexture* texture;

	bool fill;
	bool repeat;
};

struct JCompText {
	JFont* font;
	std::string text;
	int size;
	float lineHeight;

	bool wrapHorizontal;
	//TODO: etc.
};


class JObject {
public:
	JObject();
	// x = position x, y = position y, w = width, h = height, n = name
	JObject( float x, float y, float w, float h, std::string n = "[no name]" );
	~JObject();

	void init();

	void setZIndex( float zindex );
	void setColor( SDL_Color clr );
	void setColor( int r, int g, int b, int a = 255 );

	void addColor();
	void addTexture( JTexture* tex, bool fill = true, bool repeat = true );
	void addText( JFont* font, std::string text, int size );

	void removeColor();
	void removeTexture();
	void removeText();

	void update();
	void render();

	float getZIndex();

	bool isPressed();
	bool isDown();
	bool isHovered();

private:
	void addToVector( int index );

public:
	std::string name;

	float width;
	float height;
	float posX;
	float posY;

	bool independentRender;
	bool independentUpdate;

	SDL_Color color;
	bool renderColor;
	bool isMask;

	std::vector<JObject*> children;

	JCompTexture* cTexture;
	JCompText* cText;

private:
	unsigned int vectorIndex;
	float zIndex;

	bool inputPressed;
	bool inputDown;
	bool inputHovered;
};



struct JObjectPointer {
	bool allocated;
	JObject* object;
};




namespace JObjects {
	void Update();
	void Render();
}



#endif /* OVERTURE_INCLUDE_JOBJECT_HPP_ */
