/*
 * JDebug.cpp
 *
 *  Created on: 23 feb. 2017
 *      Author: jurriaanvandenberg
 */

#include <JDebug.hpp>

#include <vector>
#include <iostream>
#include <cstdarg>
#include <JOverture.hpp>



JWindow debugWindow;
std::vector<JLog_Message> consoleLog;
std::string currentString;

int SCREEN_WIDTH = 400;
int SCREEN_HEIGHT = 300;



int Debug_Init() {
	int success = 0;
    //Create window

    return success;
}

void Debug_Update() {

}

void Debug_Render() {

}


void Debug_OpenWindow() {
	if (!debugWindow.initialized) {
		debugWindow.init(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_ALWAYS_ON_TOP);
	}
}

void Debug_CloseWindow() {
	if (debugWindow.initialized) {
		debugWindow.free();
	}
}


void JLog( std::string message, bool submessage, JLog_Type type ) {
	JLog_Message lm;
	lm.message = message;
	lm.type = type;
	time_t t = time(0);
	lm.time = *localtime(&t);
	lm.submessage = submessage;

	SDL_Log("%s", message.c_str());
	consoleLog.push_back(lm);
}


void JError( std::string message, bool submessage ) {
	JLog(message, submessage, LOG_ERROR);
}


