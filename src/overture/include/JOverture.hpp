/*
 * JOverture.hpp
 *
 *  Created on: 12 nov. 2016
 *      Author: jurriaanvandenberg
 */

#ifndef OVERTURE_JOVERTURE_HPP_
#define OVERTURE_JOVERTURE_HPP_

#include <string>
#include <sstream>
#include <SDL.h>
#include <SDL_log.h>

#include <JInput.hpp>
#include <JProfiler.hpp>
#include <JSettings.hpp>

// "Not A Float", used to identify null non-pointer float function parameters.
#define NAF 3.402823E+38f
#undef NULL
#define NULL nullptr


class JWindow {
public:
	JWindow();
	~JWindow();

	void free();
	int init( int wdth = -1, int hght = -1, Uint32 flags = 0 );
	void handleEvents( SDL_Event e );

	void setTitle( std::string title );
	void setFullscreen( bool fullscreen );
	void toggleFullscreen();
	void setFocus( bool focus );
	void setLogicalSize( int width, int height );

	SDL_Window* data;
	SDL_Renderer* renderer;

	bool initialized;
	std::string title;
	unsigned int windowID;
	float width;
	float height;

	bool fullscreen;
	bool logicalSize;
	bool focused;
};


int Overture_Init();
void Overture_Close();

void Overture_Start();
void Overture_Stop();

void Overture_SetUpdateFunc( void (*f)() );
void Overture_SetRenderFunc( void (*f)() );

bool Overture_SetOption( std::string option, int value );

std::string Overture_GetOS();
bool Overture_OSIsMac();
JWindow* Overture_GetWindow();

bool Overture_IsRunning();
bool Overture_IsExiting();
bool Overture_IsMobile();
int Overture_GetFramerate();
bool Overture_HasWindowLogicalSize();
bool Overture_IsWindowFullscreen();

// dt() and Overture_getDeltaTime() are identical.
float dt();
float Overture_getDeltaTime();
// Get the ticks left for this frame. This is negative if there is lag.
float Overture_getTicksLeft();


// Android doesn't natively support std::to_string
template <typename T>
std::string toString(T value) {
	std::ostringstream os;
	os << value;
	return os.str();
}

bool containsString( std::string str1, std::string str2 );
bool stringIsInt( std::string str );
bool stringIsFloat( std::string str );
float reduceFloat( float& f, float by );

/**
 * TODO-lijst
 *
 *  Framerate/updates (no vsync)
 *  Font rendering
 *
 *  Menus
 *  	Text
 *  		Load to texture
 *  		Opmaak, animation
 *  Options
 *  	Thread on/off (off with mobile)
 *  	Keybindings
 *  	Save => http://lazyfoo.net/tutorials/SDL/33_file_reading_and_writing/index.php
 *  Audio
 *  	Time offset (play earlier or wait for some time)
 *
 *  https://www.youtube.com/watch?v=H0mI4owz994
 *  http://gafferongames.com/game-physics/fix-your-timestep/
 */

#endif /* OVERTURE_JOVERTURE_HPP_ */


