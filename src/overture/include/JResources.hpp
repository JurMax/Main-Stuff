/*
 * resources.hpp
 *
 *  Created on: 13 okt. 2016
 *      Author: jurriaanvandenberg
 */

#ifndef SRC_REDOX_SUPPORT_RESOURCES_HPP_
#define SRC_REDOX_SUPPORT_RESOURCES_HPP_

#include <iostream>
#include <vector>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>



namespace Overture {
	bool Resources_Init();
	void Resources_Close();
	void Resources_Update();
}

struct JBitmapFont;


std::string getFilePath( std::string localpath );
bool Resources_IsLoading();
void unloadSDLTexture( SDL_Texture* texture );
SDL_Texture* loadImageFromFile( std::string path, int& width, int& height, int& pitch, JBitmapFont* font = NULL );

struct JTexture;
extern JTexture textureBlank;
extern JTexture textureMissing;


struct JResource {
	JResource();
	virtual ~JResource() {};

	virtual bool loadNow() = 0;
	void loadLater();

	virtual void unloadNow() = 0;
	void unloadLater();

	std::string path;
	bool loaded;
};


struct JTexture : JResource {
	JTexture();
	virtual ~JTexture();

	void setPath( std::string path );
	bool setAndLoad( std::string path, bool loadnow = true );

	virtual bool loadNow();
	virtual void unloadNow();

	SDL_Texture* textureData;

	int pitch;
	int width;
	int height;
};


struct JAudio : JResource {
	JAudio();
	virtual ~JAudio();

	void set( std::string pth, int bmp = -1, int offset = 0.0f );
	void setAndLoad( std::string pth, int bmp = -1, int offset = 0.0f, bool loadnow = true );

	virtual bool loadNow();
	virtual void unloadNow();

	float getBeatLength();

	Mix_Chunk *chunk;

	// Equals -1 if the audio isn't music
	int bmp;
	// Offset in miliseconds, used to match the beats with the song
	int offset;
};


struct JFontData {
	bool loading;
	bool loaded;
	int size;
	TTF_Font* font;
};

struct JFont {
	JFont();
	~JFont();

	void set( std::string pth );
	void load( unsigned int size );
	void free();
	void setAndLoad( std::string pth, unsigned int size );
	void loadFromFile( unsigned int size );

	JFontData *getFontData( unsigned int size );

	bool defined;
	std::string path;

	std::vector<JFontData> fontData;
};


struct JBitmapFont : JResource {
	JBitmapFont();
	virtual ~JBitmapFont();

	void setTexture( std::string texturePth, int chardistance, int cellwidth = -1, int cellheight = -1 );
	void setTextureAndLoad( std::string texturePth, int chardistance, int cellwidth = -1, int cellheight = -1, bool loadnow = true );
	void setFont( std::string fontPth, unsigned int size );
	void setFontAndLoad( std::string fontPth, unsigned int size, bool loadnow = true );

	virtual bool loadNow();
	virtual void unloadNow();

	// True = load a texture as bitmap, false = load a TTF font as bitmap.
	bool loadFromFont;

	SDL_Texture* bitmapTexture;
	int width;
	int height;
	int pitch;

	// Only used when loaded from font.
	TTF_Font* fontTTF;
	std::string fontPath;
	unsigned int fontSize;

	int cellWidth;
	int cellHeight;
	int charDistance; // Distance between chars when rendered in pixels.
	int charBounds[256 * 3];  // int[char + 0] = row, int[char + 1] = left bound, int[char + 2] = right bound.
};

/**
 *	JTexture fontToTexture( int width = -1, int height = -1, int fontsize = -1, bool wrap, bool   );
 *		fontsize = -1 means use fontsize that fits in the width/height
 */

//TODO V make this a vector V
#define MAX_FONT_SIZE 128
#define FONT_AMOUNT 8



#endif /* SRC_REDOX_SUPPORT_RESOURCES_HPP_ */


