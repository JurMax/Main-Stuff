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


JTexture textureBlank;
JTexture textureMissing;

std::vector<JResource*> loadVector;
std::vector<JResource*> unloadVector;
std::vector<SDL_Texture*> unloadTextureVector; // For unloading SDL_Textures created in runtime.


namespace Overture {

	bool Resources_Init() {
		bool succes = true;

		textureBlank.setPath("blank.png");
		textureMissing.setPath("hello.bmp");
		textureBlank.loadNow();
		textureMissing.loadNow();

		return succes;
	}


	void Resources_Close() {
		// All already loaded resources get freed on destruction.
		for (int i = 0; i < unloadVector.size(); i++) {
			unloadVector[i]->unloadNow();
		}
		for (int i = 0; i < unloadTextureVector.size(); i++) {
			SDL_DestroyTexture(unloadTextureVector[i]);
		}
	}


	void Resources_Update() {
		if (loadVector.size() != 0) {
			loadVector[0]->loadNow();
			loadVector.erase(loadVector.begin());
		}

		if (unloadVector.size() != 0) {
			unloadVector[0]->unloadNow();
			unloadVector.erase(unloadVector.begin());
		}

		for (int i = 0; i < unloadTextureVector.size(); i++) {
			SDL_DestroyTexture(unloadTextureVector.at(i));
		}
		unloadTextureVector.clear();
	}
}


std::string getFilePath( std::string localpath ) {
	std::string filepath;
	if (!Overture_IsMobile()) {
		filepath = SDL_GetBasePath();
		filepath += "../assets/" + localpath;
	}
	else {
		filepath = localpath;
	}
	return filepath;
}


bool Resources_IsLoading() {
	return loadVector.size() != 0;
}


void unloadSDLTexture(SDL_Texture* texture) {
	if (!Overture_IsExiting()) {
		unloadTextureVector.push_back(texture);
	}
	else {
		SDL_DestroyTexture(texture);
	}
}




///////////////////////////////////// JResource /////////////////////////////////////

JResource::JResource() {
	path = "[not set]";
	loaded = false;
}


void JResource::loadLater() {
	loadVector.push_back(this);
}


void JResource::unloadLater() {
	unloadVector.push_back(this);
}



///////////////////////////////////// JTexture /////////////////////////////////////


JTexture::JTexture() : JResource() {
	textureData = NULL;

	pitch = 0;
	width = 0;
	height = 0;
}


JTexture::~JTexture() {
	unloadNow();
}


void JTexture::setPath( std::string path ) {
	this->path = path;
}


bool JTexture::setAndLoad( std::string path, bool loadnow ) {
	setPath(path);

	if (loadnow) {
		return loadNow();
	} else {
		loadLater();
		return true;
	}
}


bool JTexture::loadNow() {
	if (!loaded && path.compare("[not set]")) {
		textureData = loadImageFromFile(path, width, height, pitch, NULL);
		if (textureData != NULL) {
			loaded = true;
		}
	}

	return loaded;
}


void JTexture::unloadNow() {
	if (loaded) {
		SDL_DestroyTexture(textureData);
		textureData = NULL;
		loaded = false;
		pitch = 0;
		width = 0;
		height = 0;
	}
}



///////////////////////////////////// JAudio /////////////////////////////////////

JAudio::JAudio() : JResource() {
	chunk = NULL;
	bmp = -1;
	offset = 0;
}


JAudio::~JAudio() {
	unloadNow();
}


void JAudio::set( std::string pth, int bmp, int offset ) {
	path = pth;
	this->bmp = bmp;
	this->offset = offset;
}


void JAudio::setAndLoad( std::string pth, int bmp, int offset, bool loadnow ) {
	set(pth, bmp, offset);

	if (loadnow) {
		loadNow();
	} else {
		loadLater();
	}
}


bool JAudio::loadNow() {
	if (!loaded && path.compare("[not set]")) {
		if (!containsString(path, ".wav") && !containsString(path, ".ogg")) {
			SDL_Log("ERROR: Audio file structure of [%s] not supported!", path.c_str());
			return false;
		}

		std::string absolutePath = getFilePath(path);

		chunk = Mix_LoadWAV(absolutePath.c_str());

		if (chunk == NULL) {
			SDL_Log("ERROR: Failed to load audio [%s]! SDL_mixer Error: %s\n", path.c_str(), Mix_GetError());
		}
		else {
			loaded = true;
		}
	}
	return loaded;
}


void JAudio::unloadNow() {
	loaded = false;
	if (chunk != NULL) {
		Mix_FreeChunk(chunk);
		chunk = NULL;
	}
}


float JAudio::getBeatLength() {
	return (1.0f / ((float) (bmp) / 60.0f)) * 1000.0f;
}




///////////////////////////////////// JFont /////////////////////////////////////


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

		if (Settings::IsMultithreaded) {
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




///////////////////////////////////// JBitmapFont /////////////////////////////////////


JBitmapFont::JBitmapFont() : JResource() {
	loadFromFont = false;

	bitmapTexture = NULL;
	width = 0;
	height = 0;
	pitch = 0;

	fontTTF = NULL;
	fontPath = "[not set]";
	fontSize = 0;

	cellWidth = -1;
	cellHeight = -1;
	charDistance = 0;

	// Mark every bound as uninitialised.
	for (int i = 0; i < 256 * 3; i++) {
		charBounds[i] = -1;
	}
}


JBitmapFont::~JBitmapFont() {
	unloadNow();
}


void JBitmapFont::setTexture( std::string texturePth, int chardistance, int cellwidth, int cellheight ) {
	path = texturePth;
	cellWidth = cellwidth;
	cellHeight = cellheight;
	charDistance = chardistance;
	loadFromFont = false;
}


void JBitmapFont::setTextureAndLoad( std::string texturePth, int chardistance, int cellwidth, int cellheight, bool loadnow ) {
	setTexture(texturePth, chardistance, cellwidth, cellheight);

	if (loadnow) {
		loadNow();
	} else {
		loadLater();
	}
}


void JBitmapFont::setFont( std::string fontPth, unsigned int size ) {
	fontPath = fontPth;
	fontSize = size;
	loadFromFont = true;
}

void JBitmapFont::setFontAndLoad( std::string fontPth, unsigned int size, bool loadnow ) {
	setFont(fontPth, size);

	if (loadnow) {
		loadNow();
	} else {
		loadLater();
	}
}


bool JBitmapFont::loadNow() {
	if (!loaded) {
		if (!loadFromFont) {
			bitmapTexture = loadImageFromFile(path, width, height, pitch, this);
			if (bitmapTexture != NULL) {
				loaded = true;
			}
		}
		else {

		}
	}

	return loaded;
}


void JBitmapFont::unloadNow() {
	if (loaded) {
		if (bitmapTexture != NULL) {
			SDL_DestroyTexture(bitmapTexture);
		}
		//TODO
	}
}




SDL_Texture* loadImageFromFile( std::string path, int& width, int& height, int& pitch, JBitmapFont* font ) {
	// This code is based on a couple lazyfoo.net tutorial (soure: http://lazyfoo.net/tutorials/SDL/41_bitmap_fonts/index.php).

	SDL_Surface* loadedSurface = NULL;
	SDL_Texture* newTexture = NULL;

	// Get the file location
	std::string filepath = getFilePath(path);

	if (containsString(path, ".bmp")) {
		loadedSurface = SDL_LoadBMP(filepath.c_str());
	}
	else if (containsString(path, ".png")) {
		loadedSurface = IMG_Load(filepath.c_str());
	}
	else {
		SDL_Log("ERROR: Image file structure of [%s] not supported!", path.c_str());
		return NULL;
	}

	if (loadedSurface == NULL) {
		SDL_Log("ERROR: Failed to load texture file [%s], it probably doesn't exist! SDL Error: %s\n", filepath.c_str(), SDL_GetError());
	}
	else {
		// Optimize the loaded surface for the current display format and free the original surface.
		SDL_Surface* optimizedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0);
		SDL_FreeSurface(loadedSurface);

		if (optimizedSurface == NULL) {
			SDL_Log("ERROR: Unable to convert loaded surface to display format! %s\n", SDL_GetError());
		}
		else {
			newTexture = SDL_CreateTexture(Renderer::rRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, optimizedSurface->w, optimizedSurface->h);

			if(newTexture == NULL) {
				SDL_Log("ERROR: Unable to create blank texture! SDL Error: %s\n", SDL_GetError());
			}
			else {
				SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND);

				// Copy the pixel data from the optimized surface to the SDL_Texture
				void* tPixels;
				SDL_LockTexture(newTexture, &optimizedSurface->clip_rect, &tPixels, &pitch);
				memcpy(tPixels, optimizedSurface->pixels, optimizedSurface->pitch * optimizedSurface->h);

				width = optimizedSurface->w;
				height = optimizedSurface->h;

				Uint32* pixels = (Uint32*) tPixels;
				int pixelCount = (pitch / 4) * height;

				if (font != NULL) {
					Uint32 colorKey = SDL_MapRGB(optimizedSurface->format, 0, 0xFF, 0xFF);
					Uint32 transparent = SDL_MapRGBA(optimizedSurface->format, 0x00, 0x00, 0x00, 0x00);

					if (font->cellWidth == -1) {
						font->cellWidth = width / 16;
					}
					if (font->cellHeight == -1) {
						font->cellHeight = height / 16;
					}

					int charsPerRow = width / font->cellWidth;

					for (int i = 0; i < pixelCount; ++i) {
						if (pixels[i] == colorKey) {
							pixels[i] = transparent;
						}

						if (pixels[i] != transparent) {
							int row = i / width;
							int pixelx = i - row * width;
							int charx = pixelx / font->cellWidth;
							int chary = row / font->cellHeight;
							int c = chary * charsPerRow + charx;

							if (font->charBounds[c*3 + 0] == -1) {
								font->charBounds[c*3 + 0] = chary;
							}
							if (pixelx < font->charBounds[c*3 + 1] || font->charBounds[c*3 + 1] == -1) {
								font->charBounds[c*3 + 1] = pixelx;
							}
							if (pixelx >= font->charBounds[c*3 + 2] || font->charBounds[c*3 + 2] == -1) {
								font->charBounds[c*3 + 2] = pixelx + 1;
							}

						}
					}
				}

				SDL_UnlockTexture(newTexture);
			}
			SDL_FreeSurface(optimizedSurface);
		}
	}

	return newTexture;
}




