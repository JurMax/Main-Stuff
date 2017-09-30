/*
 * JOFont.hpp
 *
 *  Created on: 17 jun. 2017
 *      Author: jurriaanvandenberg
 */

#ifndef OVERTURE_SRC_OBJECT_JOFONT_HPP_
#define OVERTURE_SRC_OBJECT_JOFONT_HPP_

#include <string>
#include <vector>
#include <JObject.hpp>
#include <JResources.hpp>
#include <JRenderer.hpp>


typedef enum {
	FONT_NORMAL = 0,
	FONT_BOLD = 1,
	FONT_ITALIC = 2,
	FONT_UNDERLINE = 3, // TODO
	FONT_STRIKETHROUGH = 4
} FontType;


typedef enum {
	ANIMATION_EMPTY = 0,
	ANIMATION_SHAKE_1 = 1,   // Shake gently
	ANIMATION_SHAKE_2 = 2,   // Shake intensly
	ANIMATION_WAVE_1 = 3,    // Wave as a sinus
	ANIMATION_WAVE_2 = 4,    // Wave as a cosinus
	ANIMATION_WOBBLE_1 = 5,  // Words move up and down
	ANIMATION_WOBBLE_2 = 6,  // Words move left and right
	ANIMATION_SCALE_1 = 7,   // Words scale up and down
	ANIMATION_SCALE_2 = 8,   // Words only scale up
	ANIMATION_TILT_1 = 9,    // Words tilt (sinus)
	ANIMATION_TILT_2 = 10,   // Words tilt (cosinus)
	ANIMATION_COLOR_1 = 11,  // Words change color (flash)
	ANIMATION_COLOR_2 = 12,  // Words change color (fade)
} CharAnimation;



struct CharData {
	char character;
	int width;

	int offsetX;
	int offsetY;

	FontType font;
	SDL_Color color;

	int appearTime;  // In miliseconds (ticks).

	std::vector<CharAnimation> animations;
};



struct JOText : JOComponent {
	JOText( JBitmapFont* bitmapfont, SDL_Color color, std::string str = "", bool selectable = true );
	~JOText();

	virtual void update();
	virtual void render();

	std::string getText();

	void startEditing();
	void stopEditing();

	void setString( std::string str );
	void insertString( std::string str, int at );
	int charIndexToStringIndex( int charIndex );

	// Render the text to a SDL_Texture, which means it can't be modified or animated. (TODO)
	void lockText();
	void unlockText( bool destroyTexture = false );


private:
	void rebuildCharData();
	void addCharData( char character );
	void applyCharCommand( CharData* currentChar, unsigned int command, std::string arguments );
	CharData* getCurrentChar();

	// Used in render().
	JBitmapFont* getActiveFont( FontType fonttype );
	void handleAnimation( CharAnimation ca, int index, float& posX, float& posY );


public:
	JBitmapFont* bitmapFontNormal;
	JBitmapFont* bitmapFontBold;
	JBitmapFont* bitmapFontItalics;

	std::string buildString;
	std::vector<CharData> charData;

	bool allowOverflowX;
	bool animationsEnabled;
	bool selectable;
	bool editable;

	SDL_Color baseColor;
	float charWidthOffset;
	float charHeightOffset;
	float spaceWidth;


	Uint32 animationTicksStart;
	int timeBetweenChars;

	bool selected;
	int selectionStart;
	int selectionEnd;

	bool textLocked;
	bool textureCreated;
	RenderTarget lockedTexture;
};

#endif /* OVERTURE_SRC_OBJECT_JOFONT_HPP_ */


/**
 * 	Font style modifiers:
 *    #N: reset font style
 *    #B: bold
 *    #L: italic
 *    #U: underline
 *    #K: strikethrough
 *
 *  Other:
 *    \n: new line
 *    ##: add # char
 *
 *	Animation modifiers (uppercase is different version of the animation):
 *    #s or #S: letters shake randomly
 *    #w or #W: letters wave
 *    #o or #O: words wobble
 *    #a or #A: words scale up and down
 *    #i or #I: words tilt left and right
 *    #c or #C: text color flashes/fades over
 *    #t100 or #T100: Set letter/word appear time
 *    #t or #T: resets letter appear time to default
 *    #e or #E: remove last added animation
 */


