/*
 * JPlatform.cpp
 *
 *  Created on: 25 Apr 2017
 *      Author: Jurriaan en Maxim
 */

#include <JPlatform.hpp>

#include <string>
#include <JOverture.hpp>

#ifdef _WIN32
	#include <windows.h>

	#ifdef _WIN64
		#define OS "Windows (64-bit)"
	#else
		#define OS "Windows (32-bit)"
	#endif
#elif __APPLE__
	#include "TargetConditionals.h"

	#include <CoreFoundation.h>
	#include <CFString.h>
	#include <CFBase.h>
	#include <CFAttributedString.h>
	#include <objc/objc.h>
	#include <objc/objc-runtime.h>

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


std::string currentos = OS;
#undef OS


#ifdef __APPLE__
	extern "C" int NSRunAlertPanel(CFStringRef strTitle, CFStringRef strMsg, CFStringRef strButton1, CFStringRef strButton2, CFStringRef strButton3, ...);

	bool loadedWindow = false;
	id window;
#endif





void Platform_Init() {

	#ifdef _WIN32
		SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
	#elif __APPLE__
		// Do Apple stuff
	#elif __ANDROID__
		// Do Android stuff
	#else
		//Error: unsupported OS
	#endif

}


std::string Platform_GetOS() {
	return currentos;
}



void Platform_LoadDragAndDrop() {

	#ifdef __APPLE__
		if (!loadedWindow) {
			id app = NULL;

			id pool = (id) objc_getClass("NSAutoreleasePool");
			if (!pool) {
				SDL_Log("Unable to get NSAutoreleasePool!!");
			}
			else {
				pool = objc_msgSend(pool, sel_registerName("alloc"));
				if (!pool) {
					SDL_Log("Unable to create NSAutoreleasePool!!");
				}
				else {
					pool = objc_msgSend(pool, sel_registerName("init"));

					app = objc_msgSend((id) objc_getClass("NSApplication"), sel_registerName("sharedApplication"));
					if (!app) {
						SDL_Log("Unable to load NSApplication!!");
					}
					else {

						id windows = objc_msgSend(app, sel_registerName("windows"));
						if (!windows) {
							SDL_Log("Unable to load NSWindows!!");
						}
						else {
							window = objc_msgSend(windows, sel_registerName("objectAtIndex:"), 0);
							if (!window) {
								SDL_Log("Unable to load NSWindow!!");
							} else {
								loadedWindow = true;
							}
						}
					}
				}
			}
			objc_msgSend(pool, sel_registerName("release"));
		}
	#endif

}


void Platform_Drag(std::string file) {

	#ifdef __APPLE__
		id nsview = objc_msgSend(window, sel_registerName("contentView"));
		if (!nsview) {
			SDL_Log("Unable to load NSView!!");
		} else {
			objc_msgSend(nsview, sel_registerName("dragFile:fromRect:slideBack:event:"), CFSTR("/Users/jurriaanvandenberg/Desktop/C1akw25UUAARHH0.jpg"), 0, FALSE, 0);
			loadedWindow = true;
		}
	#endif

}


