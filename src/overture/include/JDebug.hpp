/*
 * JDebug.hpp
 *
 *  Created on: 23 feb. 2017
 *      Author: jurriaanvandenberg
 */

#ifndef OVERTURE_JDEBUG_HPP_
#define OVERTURE_JDEBUG_HPP_

#include <string>
#include <cstdarg>
#include <sstream>
#include <iostream>
#include <SDL.h>
#include <time.h>


int Debug_Init();
void Debug_Update();
void Debug_Render();

void Debug_OpenWindow();
void Debug_CloseWindow();

void Debug_Add(...);

typedef enum {
	LOG_INFO = 0,
	LOG_ERROR = 0,
	LOG_WARNING = 0,
} JLog_Type;


struct JLog_Message {
	std::string message;
	JLog_Type type;
	tm time;
	bool submessage;

	SDL_Texture* texture;
};

void JLog( std::string message, bool submessage = false, JLog_Type type = LOG_INFO );

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
void JLog( T1 a1 = "", T2 a2 = "", T3 a3 = "", T4 a4 = "", T5 a5 = "", T6 a6 = ""  ) {
	std::ostringstream os;
	os << a1 << a2 << a3 << a4 << a5 << a6;
	JLog(os.str());
}


void JError( std::string message, bool submessage = false );

//https://sourceware.org/git/?p=glibc.git;a=blob;f=stdio-common/printf.c;h=4c8f3a2a0c38ab27a2eed4d2ff3b804980aa8f9f;hb=HEAD



#endif /* OVERTURE_JDEBUG_HPP_ */
