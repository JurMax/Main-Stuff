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



/** Used to identify null non-pointer function parameters */
#define NAF 3.402823E+38f
//TODO V make this a vector V
#define MAX_FONT_SIZE 128
#define FONT_AMOUNT 8
#define THREADED_LOADING false



typedef enum {
	RESOURCE_UNDEFINED = 0,
	RESOURCE_TEXTURE = 1,
	RESOURCE_FONT = 2,
	RESOURCE_AUDIO = 3
} ResourceType;



class JTexture {
public:
	JTexture();
	~JTexture();

	void set( std::string pth );
	void load( bool reload = false );
	void setAndLoad( std::string pth );
	void free();
	bool loadFromFile();

	void setBlendMode( SDL_BlendMode blending );

	bool defined;
	std::string path;

	bool loaded;
	SDL_Texture *textureData;
	int pitch;

	int width;
	int height;
};


struct JFontData {
	bool loading;
	bool loaded;
	int size;
	TTF_Font *font;
};

class JFont {
public:
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

/**
 *	JTexture fontToTexture( int width = -1, int height = -1, int fontsize = -1, bool wrap, bool   );
 *		fontsize = -1 means use fontsize that fits in the width/height
 */


class JAudio {
public:
	JAudio();
	~JAudio();

	void set( std::string pth );
	void load();
	void free();
	void setAndLoad( std::string pth, int bmp = -1, int offset = 0 );
	bool loadFromFile();

	bool defined;
	std::string path;

	bool loaded;
	Mix_Chunk *chunk;

	// Equals -1 if the audio isn't music
	int bmp;
	// The time between beats in miliseconds
	float beatLength;
	// Offset in miliseconds, used to match the beats with the song
	int offset;
};



class JResourcePointer {
public:
	JResourcePointer() {
		defined = false;
		resourceType = RESOURCE_UNDEFINED;
		texture = NULL;
		font = NULL;
		fontsize = 0;
	}
	JResourcePointer( JTexture *txtr ) {
		defined = true;
		resourceType = RESOURCE_TEXTURE;
		texture = txtr;
		font = NULL;
		fontsize = 0;
	}
	JResourcePointer( JFont *fnt, int size ) {
		defined = true;
		resourceType = RESOURCE_FONT;
		texture = NULL;
		font = fnt;
		fontsize = size;
	}

	bool defined;
	ResourceType resourceType;

	JTexture *texture;
	JFont *font;
	int fontsize;
	//JAudio *audio;
};


extern JTexture textureBlank;
extern JTexture textureMissing;  // unused

bool Resources_Init();
void Resources_Free();

bool Overture_IsLoadingTextures();
SDL_mutex* Overture_GetThreadMutex();


#endif /* SRC_REDOX_SUPPORT_RESOURCES_HPP_ */


