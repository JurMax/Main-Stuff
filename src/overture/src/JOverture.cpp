/*
 * JOverture.cpp
 *
 *  Created on: 12 nov. 2016
 *      Author: jurriaanvandenberg
 */

#include <JOverture.hpp>

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <JPlatform.hpp>
#include <JRenderer.hpp>
#include <JAudio.hpp>
#include <JResources.hpp>
#include <JBezier.hpp>
#include <JSettings.hpp>
#include <JCursor.hpp>
#include <JWorld.hpp>


using namespace Overture;

int initSDL();
void updateFPS();

JWindow mainWindow;
bool OSisMacOS;

bool isRunning = true;
bool isExiting = false;
int frametimelast = 0;
int framecount = 0;
int framerate = 0;

// The ticks at the start of the frame.
int FRAMERATE = 60;
long ticksFrameStart;

void (*renderFunc)();
void (*updateFunc)();


int Overture_Init() {
	int success = 0;

	SDL_Log("== Starting Overture Engine ==");

	Platform_Init();
	SDL_Log("Running on [%s]", Platform_GetOS().c_str());
	OSisMacOS = containsString(Overture_GetOS(), "MacOS");

	if (!initSDL()) {
		SDL_Log("ERROR: Failed to initialise SDL!\n");
	}
	else {
		Settings_Init();

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
					Cursor_Init();
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
	//SDL_SetHint(SDL_HINT_ACCELEROMETER_AS_JOYSTICK, "1");

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL failed to initialise! SDL Error: %s\n", SDL_GetError());
	}
	else {
		SDL_StopTextInput();
		SDL_GL_SetSwapInterval(1);

		mainWindow.init(800, 600);

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
	isExiting = true;
	World_Close();
	Object_Close();
	Renderer_Close();
	Cursor_Close();
	Resources_Close();

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
		ticksFrameStart = SDL_GetTicks();

		Profiler::start("update");
		updateFPS();
		Cursor::isSet = false;
		Input_Update();
		Audio_Update();

		Overture::updateBezierAnimations();
		updateFunc();
		Profiler::stop("update");

		Profiler::start("render");
		Renderer_Begin();
		renderFunc();
		Profiler::stop("render");

		Resources_Update();
		if (!Settings::VSync || !mainWindow.focused) {
			//TODO: Framerate stuff
			int delay = floorf(Overture_getTicksLeft());
			if (delay > 0)
				SDL_Delay(delay);
		}
		Renderer_End();

	}
}


void updateFPS() {
	framecount += 1;

	if (SDL_GetTicks() - frametimelast > 1000) {
		frametimelast = SDL_GetTicks();
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
	return Platform_GetOS();
}

bool Overture_OSIsMac() {
	return OSisMacOS;
}


JWindow* Overture_GetWindow() {
	return &mainWindow;
}


bool Overture_IsRunning() {
	return isRunning;
}


bool Overture_IsExiting() {
	return isExiting;
}


bool Overture_IsMobile() {
	std::string ios = "IOS";

	if (Platform_GetOS() == "Android" || Platform_GetOS().compare(0, ios.length(), ios) == 0) {
		return true;
	}
	else {
		return false;
	}
}


int Overture_GetFramerate() {
	return framerate;
}



float dt() {
	return Overture_getDeltaTime();
}


float Overture_getDeltaTime() {
	return 1.0f / (float) (FRAMERATE);
}


float Overture_getTicksLeft() {
	float ticksPerFrame = 1000.0f / (float) FRAMERATE;
	return ticksPerFrame - (SDL_GetTicks() - ticksFrameStart);
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
    	Uint32 renderflags = SDL_RENDERER_ACCELERATED;
    	if (Settings::VSync) {
    		renderflags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    	}

		renderer = SDL_CreateRenderer(data, -1, renderflags);

		if (renderer == NULL) {
			SDL_Log("ERROR: SDL Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		}
		else {
			SDL_SetRenderDrawColor(Renderer::rRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
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
				Renderer::resolutionChanged = true;
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
		SDL_SetWindowFullscreen(data, SDL_WINDOW_FULLSCREEN);
		//SDL_SetWindowFullscreen(data, SDL_WINDOW_FULLSCREEN_DESKTOP);
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
	focused = focus;
}


void JWindow::setLogicalSize( int width, int height ) {
	this->logicalSize = true;
	this->width = (float) width;
	this->height = (float) height;
	SDL_RenderSetLogicalSize(renderer, width, height);
}




bool containsString( std::string str1, std::string str2 ) {
	if (str1.length() >= str2.length() && str1.compare(str1.length() - str2.length(), str2.length(), str2) == 0) {
		return true;
	}
	else {
		return false;
	}
}


bool stringIsInt( std::string str ) {
	bool isInt = false;
	if (str.length() > 0) {
		isInt = true;
		for (int i = 0; i < str.length(); i++) {
			char c = str[i];
			bool isNumber = c == '0' ||  c == '1' || c == '2' || c == '3' || c == '4';
			isNumber = isNumber || c == '5' || c == '6' ||  c == '7' || c == '8' || c == '9';
			isNumber = isNumber || (i == 0 && str.length() > 1 && c == '-');

			if (!isNumber) {
				isInt = false;
			}
		}
	}
	return isInt;
}


bool stringIsFloat( std::string str ) {
	bool isFloat = false;
	if (str.length() > 0) {
		isFloat = true;
		for (int i = 0; i < str.length(); i++) {
			char c = str[i];
			bool isNumber = c == '0' ||  c == '1' || c == '2' || c == '3' || c == '4';
			isNumber = isNumber || c == '5' || c == '6' ||  c == '7' || c == '8' || c == '9';
			isNumber = isNumber || (i == 0 && str.length() > 1 && c == '-');
			isNumber = isNumber || (i != 0 && c == '.');

			if (!isNumber) {
				isFloat = false;
			}
		}
	}
	return isFloat;
}


float reduceFloat( float& f, float by ) {
    if (f > 0.0f) {
    	f -= by;
    	if (f < 0.0f) {
    		f = 0.0f;
    	}
    }
    else if (f < 0.0f) {
    	f += by;
    	if (f > 0.0f) {
    		f = 0.0f;
    	}
    }
    return f;
}
