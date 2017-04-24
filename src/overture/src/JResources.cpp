/*
 * resources.cpp
 *
 *  Created on: 13 okt. 2016
 *      Author: jurriaanvandenberg
 */

#include <JResources.hpp>

#include <vector>
#include <cstring>
#include <SDL_image.h>
#include <SDL_thread.h>
#include <SDL_log.h>

#include <JOverture.hpp>
#include <JRenderer.hpp>


int loadResources( void* data );

SDL_Thread *loadingThread;
SDL_mutex *rThreadMutex;
bool threadActive = false;

std::vector<JResourcePointer> resourceBufferA;
std::vector<JResourcePointer> resourceBufferB;
bool writingBufferA = true;

JTexture textureBlank;
JTexture textureMissing;


bool Resources_Init() {
	bool succes = true;
	rThreadMutex = SDL_CreateMutex();

	textureBlank.set("blank.png");
	textureMissing.set("hello.bmp");
	textureBlank.loadFromFile();
	textureMissing.loadFromFile();

	return succes;
}


void Resources_Free() {
    SDL_WaitThread(loadingThread, NULL);
    loadingThread = NULL;
    SDL_DestroyMutex(rThreadMutex);
    rThreadMutex = NULL;

    //TODO Actually free resources (using resourcepointers)
}


bool Overture_IsLoadingTextures() {
	return threadActive;
}

SDL_mutex* Overture_GetThreadMutex() {
	return rThreadMutex;
}


int loadResources( void* data ) {
	std::vector<JResourcePointer> *buffer;

	if (writingBufferA) {
		buffer = &resourceBufferB;
	}
	else {
		buffer = &resourceBufferA;
	}
	std::cout << std::endl << "START" << std::endl;
	for (int i = 0; i < buffer->size(); ++i) {
		std::cout << " " << i << std::endl;
		JResourcePointer *rp = &buffer->at(i);
		if (rp->defined) {
			switch (rp->resourceType) {
				case RESOURCE_TEXTURE : {
					if (!rp->texture->loaded) {
						if (!rp->texture->loadFromFile()) {
							SDL_Log("ERROR: Texture [%s] failed to load!\n", rp->texture->path.c_str());
						}
					}
				}
				break;

				case RESOURCE_FONT : {
					if (!rp->font->getFontData(rp->fontsize)->loaded) {
						rp->font->loadFromFile(rp->fontsize);
					}
				}
				break;

				default : {
					SDL_Log("ERROR: unspecified resource pointer in buffer [%d]", writingBufferA);
				}
				break;
			}
		}
	}


	buffer->clear();
	bool isDone = true;

	SDL_LockMutex(rThreadMutex);

	writingBufferA = !writingBufferA;
	if (resourceBufferA.size() != 0 || resourceBufferB.size() != 0) {
		isDone = false;
	}
	else {
		threadActive = false;
	}

	SDL_UnlockMutex(rThreadMutex);

	if (!isDone) {
		loadResources(NULL);
	}

    return 0;
}


void startLoadThread() {
	if (!threadActive) {
		if (loadingThread != NULL) {
			SDL_WaitThread(loadingThread, NULL);
			loadingThread = NULL;
		}
		loadingThread = SDL_CreateThread(loadResources, "loadthr", (void*) NULL);
		threadActive = true;
	}
}




//////////////////// JTexture ////////////////////////

JTexture::JTexture() {
	defined = false;
	path = "";

	loaded = false;
	textureData = NULL;
	pitch = 0;

	width = 0;
	height = 0;
}

JTexture::~JTexture() {
	free();
}

void JTexture::free() {
	if (textureData != NULL) {
		SDL_DestroyTexture(textureData);
		textureData = NULL;
		loaded = false;
		pitch = 0;
		width = 0;
		height = 0;
	}
}

void JTexture::set( std::string pth ) {
	path = pth;
	defined = true;
}

void JTexture::load( bool reload ) {
	if (reload) {
		free();
	}
	else {
		if (loaded) {
			return;
		}
	}

	if (THREADED_LOADING) {
		JResourcePointer resourcepointer = JResourcePointer(this);
		if (writingBufferA) {
			resourceBufferA.push_back(resourcepointer);
		}
		else {
			resourceBufferB.push_back(resourcepointer);
		}
		startLoadThread();
	}
	else {
		if (!loadFromFile()) {
			SDL_Log("ERROR: texture [%s] failed to load!", path.c_str());
		}
	}
}

void JTexture::setAndLoad( std::string pth ) {
	set(pth);
	load();
}

/** Currently only works for Desktop and Android */
bool JTexture::loadFromFile() {

	std::string absolutePath;
	SDL_Surface* loadedSurface = NULL;
	SDL_Texture* newTexture = NULL;

	if (path.length() < 4) {
		SDL_Log("ERROR: Texture path [%s] doesn't have enough characters", path.c_str());
		return false;
	}

	if (!Overture_IsMobile()) {
		absolutePath = SDL_GetBasePath();
		absolutePath += "../assets/" + path;
	}
	else {
		absolutePath = path;
	}

    if (containsString(path, ".bmp")) {
    	loadedSurface = SDL_LoadBMP(absolutePath.c_str());
    }
    else if (containsString(path, ".png")) {
    	loadedSurface = IMG_Load(absolutePath.c_str());
    }
    else {
    	SDL_Log("ERROR: Image file structure of [%s] not supported!", path.c_str());
    	return false;
    }


	if (loadedSurface == NULL) {
		SDL_Log("ERROR: Unable to load image [%s]! SDL Error: %s\n", path.c_str(), SDL_GetError());
	}
	else {
		SDL_Surface* optimizedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0);

		if (optimizedSurface == NULL) {
			SDL_Log("ERROR: Unable to convert loaded surface to display format! %s\n", SDL_GetError());
		}
		else {

			if (THREADED_LOADING)
				SDL_LockMutex(rThreadMutex);

			newTexture = SDL_CreateTexture(rRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, optimizedSurface->w, optimizedSurface->h);

			if (THREADED_LOADING)
				SDL_UnlockMutex(rThreadMutex);


			if(newTexture == NULL) {
				SDL_Log("ERROR: Unable to create blank texture! SDL Error: %s\n", SDL_GetError());
			}
			else {
				SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND);
				void* tPixels;
				SDL_LockTexture(newTexture, &optimizedSurface->clip_rect, &tPixels, &pitch);
				memcpy(tPixels, optimizedSurface->pixels, optimizedSurface->pitch * optimizedSurface->h);

				width = optimizedSurface->w;
				height = optimizedSurface->h;

				Uint32* pixels = (Uint32*) tPixels;
				int pixelCount = (pitch / 4) * height;

				Uint32 colorKey = SDL_MapRGB(optimizedSurface->format, 0, 0xFF, 0xFF);
				Uint32 transparent = SDL_MapRGBA(optimizedSurface->format, 0x00, 0xFF, 0xFF, 0x00);

				for (int i = 0; i < pixelCount; ++i) {
					if (pixels[i] == colorKey) {
						pixels[i] = transparent;
					}
				}

				SDL_UnlockTexture(newTexture);
			}
			SDL_FreeSurface(optimizedSurface);
		}
		SDL_FreeSurface(loadedSurface);
	}

	textureData = newTexture;

	if (textureData != NULL) {
		loaded = true;
	}

	return loaded;
}

void JTexture::setBlendMode( SDL_BlendMode blending ) {
	SDL_SetTextureBlendMode(textureData, blending);
}



//////////////////// JFont ////////////////////////

JFont::JFont() {
	defined = false;
	path = "";
}

JFont::~JFont() {
	free();
}

void JFont::free() {
	for (int i = 0; i < fontData.size(); i++) {
		if (fontData.at(i).font != NULL) {
			TTF_CloseFont(fontData.at(i).font);
			fontData.at(i).font = NULL;
		}
		fontData.at(i).loaded = false;
		fontData.at(i).loading = false;
		fontData.at(i).size = 0;
	}
	fontData.clear();
}

void JFont::set( std::string pth ) {
	defined = true;
	path = pth;
}

void JFont::load( unsigned int size ) {
	if (getFontData(size) == NULL) {
		JFontData fontdata;
		fontdata.loaded = false;
		fontdata.loading = true;
		fontdata.size = size;
		fontdata.font = NULL;
		fontData.push_back(fontdata);

		if (THREADED_LOADING) {
			JResourcePointer resourcepointer = JResourcePointer(this, size);
			if (writingBufferA) {
				resourceBufferA.push_back(resourcepointer);
			}
			else {
				resourceBufferB.push_back(resourcepointer);
			}
			startLoadThread();
		}
		else {
			loadFromFile(size);
		}
	}
}

void JFont::setAndLoad( std::string pth, unsigned int size ) {
	set(pth);
	load(size);
	defined = false;
	path = pth;
}

void JFont::loadFromFile( unsigned int size ) {
	if (getFontData(size) != NULL) {

		std::string texturepath;
		if (!Overture_IsMobile()) {
			texturepath = SDL_GetBasePath();
			texturepath += "../assets/" + path;
		}
		else {
			texturepath = path;
		}

		getFontData(size)->font = TTF_OpenFont(texturepath.c_str(), size);
		if (getFontData(size)->font == NULL) {
			SDL_Log("ERROR: Font [%s] with size [%d] failed to load!", path.c_str(), size);
		}
		else {
			getFontData(size)->loaded = true;
			getFontData(size)->loading = false;
		}
	}
}

JFontData *JFont::getFontData( unsigned int size ) {
	for (int i = 0; i < fontData.size(); i++) {
		if (fontData.at(i).size == size) {
			return &fontData.at(i);
		}
	}
	return NULL;
}



//////////////////// JAudio ////////////////////////

JAudio::JAudio() {
	defined = false;
	path = "";
	loaded = false;
	chunk = NULL;
	bmp = -1;
	beatLength = 0.0f;
	offset = 0;
}

JAudio::~JAudio() {
	free();
}

void JAudio::free() {
	defined = false;
	path = "";
	loaded = false;
	Mix_FreeChunk(chunk);
	chunk = NULL;
	bmp = -1;
	beatLength = 0.0f;
	offset = 0;
}

void JAudio::set( std::string pth ) {
	path = pth;
	defined = true;
}

void JAudio::load() {
	// TODO: threaded loading (maybe)
	loadFromFile();
}

void JAudio::setAndLoad( std::string pth, int bmp, int offset ) {
	set(pth);
	this->bmp = bmp;
	this->offset = offset;
	this->beatLength = (1.0f / ((float) (this->bmp) / 60.0f)) * 1000.0f;
	load();
}

bool JAudio::loadFromFile() {
	std::string absolutePath = "";

	if (path.length() < 4) {
		SDL_Log("ERROR: Audio path [%s] doesn't have enough characters", path.c_str());
		return false;
	}
	if (!containsString(path, ".wav") && !containsString(path, ".ogg")) {
		SDL_Log("ERROR: Audio file structure of [%s] not supported!", path.c_str());
		return false;
	}

	if (!Overture_IsMobile()) {
		absolutePath = SDL_GetBasePath();
		absolutePath += "../assets/" + path;
	}
	else {
		absolutePath = path;
	}

	chunk = Mix_LoadWAV(absolutePath.c_str());

    if (chunk == NULL) {
        SDL_Log("ERROR: Failed to load audio [%s]! SDL_mixer Error: %s\n", path.c_str(), Mix_GetError());
    }
    else {
    	loaded = true;
    }

	return loaded;
}


