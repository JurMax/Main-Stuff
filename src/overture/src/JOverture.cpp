/*
 * JOverture.cpp
 *
 *  Created on: 12 nov. 2016
 *      Author: jurriaanvandenberg
 */

#ifdef _WIN32
	#ifdef _WIN64
		#define OS "Windows (64-bit)"
	#else
		#define OS "Windows (32-bit)"
	#endif
#elif __APPLE__
	#include "TargetConditionals.h"
	#if TARGET_IPHONE_SIMULATOR
		#define OS "IOS (Simulated)"
	#elif TARGET_OS_IPHONE
		#define OS "IOS"
	#elif TARGET_OS_MAC
		#define OS "MacOS"
	#else
    	#define OS "Unknown Apple platform"
	#endif
#elif __ANDROID__
	#define OS "Android"
#elif __linux__
	#define OS "Linux"
#elif __unix__ // unix system not caught above
	#define OS "Unkown Unix OS"
#else
	#define OS "Unkown OS"
#endif

#include <JOverture.hpp>

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <JRenderer.hpp>
#include <JAudio.hpp>
#include <JResources.hpp>
#include <JProfiler.hpp>
#include <JBezier.hpp>
#include <JUI.hpp>


int initSDL();
void updateFPS();

std::string Overture_CurrentOS = OS;

JWindow mainWindow;
bool renderFocusloss = false;

bool isRunning = true;
int frametimelast = 0;
int framecount = 0;
int framerate = 0;

void (*renderFunc)();
void (*updateFunc)();


int Overture_Init() {
	int success = 0;

	SDL_Log("== Starting Overture Engine ==");
	SDL_Log("Running on [%s]", Overture_CurrentOS.c_str());

	if (!initSDL()) {
		SDL_Log("ERROR: Failed to initialise SDL!\n");
	}
	else {
		if (!Renderer_Init()) {
			SDL_Log("ERROR: Failed to initialise renderer!\n");
		}
		else {
			if (!Audio_Init()) {
				SDL_Log("ERROR: Failed to initialise audio!\n");
			}
			else {
				if (!Resources_Init()) {
					SDL_Log("ERROR: Failed to initialise resources!\n");
				}
				else {
					SDL_Log("Initialising renderer, audio and resources successful");
					Input::Input_LoadCursors();
					success = 1;
				}
			}
		}
	}

	return success;
}


int initSDL() {
	int success = 0;

	//Set texture filtering to linear
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	SDL_SetHint(SDL_HINT_ACCELEROMETER_AS_JOYSTICK, "1");

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL failed to initialise! SDL Error: %s\n", SDL_GetError());
	}
	else {
		SDL_StopTextInput();
		SDL_GL_SetSwapInterval(1);

		mainWindow.init(640, 480);

		if (!mainWindow.initialized) {
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Window could not be initialised!");
		}
		else {
            int imgFlags = IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) & imgFlags)) {
            	SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_image could not initialise! SDL_image Error: %s\n", IMG_GetError());
            }
            else {
                if (TTF_Init() == -1) {
                	SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_ttf could not initialise! SDL_ttf Error: %s\n", TTF_GetError());
                }
                else {
            		SDL_Log("SDL Initialisation successful");
            		success = 1;
                }
            }
		}
	}

	return success;
}


void Overture_Close() {
	Resources_Free();

	mainWindow.free();

	IMG_Quit();
	TTF_Quit();
    Mix_CloseAudio();
	Mix_Quit();

	SDL_Log("Application closed after %i milliseconds", SDL_GetTicks());
	SDL_Quit();
}


void Overture_Start() {
	frametimelast = SDL_GetTicks();

	isRunning = true;
	while(isRunning) {

		if (THREADED_LOADING && Overture_IsLoadingTextures()) {
			SDL_LockMutex(Overture_GetThreadMutex());
		}

		updateFPS();

		Overture::Input_Update();
		Audio_Update();

		if (mainWindow.focused) {
			profiler::start("update");

			Overture::updateBezierAnimations();
			Overture::updateUI();

			updateFunc();

			profiler::stop("update");


			profiler::start("render");

			setRenderColor(0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(rRenderer);
			setRenderColor(getClearColor());
			renderRect(0, 0, mainWindow.width, mainWindow.height);

			renderFunc();

			SDL_RenderPresent(rRenderer);

			profiler::stop("render");

		}
		else {
			if (renderFocusloss) {
				renderFocusloss = false;
				setRenderColor(0x00, 0x00, 0x00, 0x99);
				renderRect(0, 0, mainWindow.width, mainWindow.height);
				SDL_RenderPresent(rRenderer);
			}

			// Don't update as frequently when the window lost focus //
			SDL_Delay(500);
		}

		if (THREADED_LOADING && Overture_IsLoadingTextures()) {
			SDL_UnlockMutex(Overture_GetThreadMutex());
		}
	}
}


void updateFPS() {
	framecount += 1;

	if (SDL_GetTicks() - frametimelast > 1000) {
		frametimelast= SDL_GetTicks();
		framerate = framecount;
		framecount = 0;
	}
}


void Overture_Stop() {
	SDL_Log("Shutting down...");
	isRunning = false;
}


void Overture_SetUpdateFunc( void (*f)() ) {
	updateFunc = (*f);
}


void Overture_SetRenderFunc( void (*f)() ) {
	renderFunc = (*f);
}


bool Overture_SetOption( std::string option, int value ) {
	bool isValidOption = true;
	//TODO: options
	return isValidOption;
}


std::string Overture_GetOS() {
	return Overture_CurrentOS;
}


JWindow* Overture_GetWindow() {
	return &mainWindow;
}


bool Overture_IsRunning() {
	return isRunning;
}


bool Overture_IsMobile() {
	std::string ios = "IOS";

	if (Overture_CurrentOS == "Android" || Overture_CurrentOS.compare(0, ios.length(), ios) == 0) {
		return true;
	}
	else {
		return false;
	}
}


int Overture_GetFramerate() {
	return framerate;
}


bool containsString( std::string str1, std::string str2 ) {
	if (str1.compare(str1.length() - str2.length(), str2.length(), str2) == 0) {
		return true;
	}
	else {
		return false;
	}
}




////////////////////// JWindow //////////////////////

JWindow::JWindow() {
	data = NULL;
	renderer = NULL;

	initialized = false;
	title = "Overture Window";
	windowID = -1;
	width = 200;
	height = 170;
	//TODO: save screen resolution

	fullscreen = false;
	logicalSize = false;
	focused = false;
}

JWindow::~JWindow() {
	free();
}

void JWindow::free() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(data);

	data = NULL;
	renderer = NULL;
	initialized = false;
	title = "";
	windowID = -1;
	width = 0;
	height = 0;
	fullscreen = false;
	logicalSize = false;
	focused = false;
}

int JWindow::init( int wdth, int hght, Uint32 flags ) {
	int succes = 0;

	width = wdth;
	height = hght;

	// Set max width and height for display
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) == 0) {
    	if (wdth == -1) {
    		width = displayMode.w;
    	}
    	if (hght == -1) {
    		height = displayMode.h;
    	}
    }

    Uint32 windowflags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

    data = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, windowflags);
    windowID = SDL_GetWindowID(data);

    if (data == NULL) {
		SDL_Log("ERROR: SDL Window could not be created! SDL Error: %s\n", SDL_GetError());
    }
    else {
		renderer = SDL_CreateRenderer(data, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

		if (renderer == NULL) {
			SDL_Log("ERROR: SDL Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		}
		else {
			SDL_SetRenderDrawColor(rRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			initialized = true;
			focused = true;
			succes = 1;
		}
    }

    return succes;
}

void JWindow::handleEvents( SDL_Event e ) {
	if (e.window.windowID == windowID) {
		if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
			if (!logicalSize) {
				int w, h;
				SDL_GetWindowSize(data, &w, &h);

				width = (float) w;
				height = (float) h;
			}
		}
		else if(e.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
			//setFocus(false);
			//TODO: Fix focus for debugwindow
		}
		else if(e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
			//setFocus(true);
		}
	}
}

void JWindow::setTitle( std::string str ) {
	title = str;
	SDL_SetWindowTitle(data, str.c_str());
}

void JWindow::setFullscreen( bool fullscreen ) {
	if (fullscreen) {
		SDL_SetWindowFullscreen(data, SDL_WINDOW_FULLSCREEN_DESKTOP);
		this->fullscreen = true;
	}
	else {
		SDL_SetWindowFullscreen(data, 0);
		this->fullscreen = false;
	}
}

void JWindow::toggleFullscreen() {
	setFullscreen(!fullscreen);
}

void JWindow::setFocus( bool focus ) {
	if (focus == false) {
		renderFocusloss = true;
	}
	focused = focus;
}

void JWindow::setLogicalSize( int width, int height ) {
	this->logicalSize = true;
	this->width = (float) width;
	this->height = (float) height;
	SDL_RenderSetLogicalSize(renderer, width, height);
}




