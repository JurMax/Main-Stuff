/*
 * redoxmain.cpp
 *
 *  Created on: 13 okt. 2016
 *      Author: jurriaanvandenberg
 */

#include "RedoxMain.hpp"

#include <iomanip>
#include <SDL_ttf.h>
#include <SDL_syswm.h>
#include <JAudio.hpp>
#include <JUI.hpp>
#include <JProfiler.hpp>
#include <JDebug.hpp>
#include <JPlatform.hpp>

#include "RedoxUI.hpp"


JTexture portrait;
JTexture landscape;
JTexture arrow;
JTexture maskTest;

JFont roboto;


void redoxmain::init() {
	portrait.setAndLoad("hello.bmp");
	landscape.setAndLoad("cap2.png");
	arrow.setAndLoad("doublearrow.png");
	maskTest.setAndLoad("masktest.png");

	roboto.set("Roboto-Regular.ttf");

	UI_Initialize();
}



void redoxmain::update() {
	if (Input::isKeyPressed(KEY_B)) {
		Platform_LoadDragAndDrop();
	}
	if (Input::isKeyPressed(KEY_A)) {
		Platform_Drag("/Users/jurriaanvandenberg/Desktop/C1akw25UUAARHH0.jpg");
	}

	UI_Update();
}


SDL_Joystick* gGameController = NULL;

void redoxmain::render() {
	Overture_GetWindow()->setTitle("SDL Window | FPS: " + toString(Overture_GetFramerate()));

	static bool bl = true;
	if (bl) {
		bl = false;
		//Overture_LoadResources();
	}
	if (Input::isKeyPressed(KEY_L) || (Overture_IsMobile() && Input::pointers[3].pressed)) {
		//Overture_LoadResources();
		portrait.load(true);
		//landscape.free();
		landscape.load(true);
		arrow.load(true);
		roboto.free();
		roboto.load(100);
	    gGameController = SDL_JoystickOpen(0);
	}


	static bool isMoving = false;
	static bool fromLeft = true;
	static float tmovement = 0;

	if (Input::isKeyDown(KEY_SPACE)) {
		SDL_SetRenderDrawColor(rRenderer, 0, 0xFF, 0xFF, 0xFF);
	}
	else {
		SDL_SetRenderDrawColor(rRenderer, 0xFF, 0, 0xFF, 0xFF);
		//SDL_Rect rectvalue = { 0, 0, 100, 500 };
		//rectding = &rectvalue;
	}

	float speed = 0.02;
	if (isMoving) {
		if (fromLeft) {
			tmovement += speed;
			if (tmovement >= 1.0) {
				tmovement = 1.0;
				isMoving = false;
				fromLeft = false;
			}
		}
		else {
			tmovement -= speed;
			if (tmovement <= 0.0) {
				tmovement = 0.0;
				isMoving = false;
				fromLeft = true;
			}
		}
	}


	int posX = 200;
	int posY = 50;
	float distance = 300;

	/*
	float nx = (float) (x) / 32767.0f;
	float ny = (float) (y) / 32767.0f;
	float nz = (float) (x) / 32767.0f;

	float angle = -atan(nx/ny)*180.0f/3.14159265f;

	if (ny < 0)
		angle += 180.0f;
	*/

	setClearColor(0x30, 0xCF, 0x63);


	Bezier bezier1 = Bezier(Vector2(1, 0), Vector2(0, 1));
	bezier1.render(10, posY, 190, 190);

	resetRenderColor();
	renderTexture(&portrait, posX + tmovement*distance, posY);


	if (Input::isAreaClicked(posX + tmovement*distance, posY, 128, 128)) {
		isMoving = !isMoving;
	}
	if (Input::isAreaBeingClicked(posX + tmovement*distance, posY, 128, 128)) {
		setClearColor(0xCC, 0x33, 0x00);
	}

	posY += 150;

	setRenderColor( 255, 0, 0 );
	renderTexture(&portrait, posX + tmovement*distance + 300, posY);
	if (Input::pointers[0].active) {
		float maskSizeX = 300;
		float maskSizeY = 300;
		float maskPosX = Input::pointers[0].position.x * Overture_GetWindow()->width - maskSizeX/2;
		float maskPosY = Input::pointers[0].position.y * Overture_GetWindow()->height - maskSizeY/2;

		addClippingMask(maskPosX, maskPosY, maskSizeX, maskSizeY);
	}

	resetRenderColor();
	renderTexture(&landscape, posX + bezier1.getVectorValue(tmovement).y*distance, posY);

	addClippingMask(posX + tmovement*distance+300, posY-150, 50, 50);
	renderTexture(&portrait, posX + tmovement*distance+300, posY-150);
	drawClippingMask();

	drawClippingMask();

	addClippingMask(posX + tmovement*distance+500, posY-150, 50, 50);
	renderTexture(&portrait, posX + tmovement*distance+500, posY-150);
	drawClippingMask();


	if (Input::isKeyPressed(KEY_T)) {
		time_t t = time(0);
	    tm now = *localtime(&t);
	    std::string str = toString(now.tm_year + 1900) + "-" + toString(now.tm_mon + 1) + "-" + toString(now.tm_mday) +
	    		" " + toString(now.tm_hour) + ":" + toString(now.tm_min) + ":" + toString(now.tm_sec);
	    //JLog(str);
	    //func(6, "dcff", 3, 'a', 1.999, " ", 42.5);
	    //JLog(3, 5, "asdf", 0.2, " ");
	    toString(3);
	}

	setRenderColor( 255, 0, 0 );
	renderTexture(&portrait, posX + tmovement*distance, posY);


	if (Input::isAreaDown(posX + bezier1.getVectorValue(tmovement).y*distance, posY, 128, 128)) {
		setClearColor(0x40, 0xA2, 0xBF);
	}

	posY += 250;
	Bezier bezier2 = Bezier(Vector2(0, 0), Vector2(0, 1));
	bezier2.render(10, posY, 190, 190);

	//renderTexture(&arrow, posX + bezier2.getFloatValue(tmovement)*distance, posY, NAF, NAF, SDL_FLIP_NONE, angle);

	Overture_UI.render();


	setRenderColor(255, 255, 255);
	static int size = 100;
	int sizespeed = 5;
	if (Input::isKeyDown(KEY_LEFT))
		size -= sizespeed;
	if (Input::isKeyDown(KEY_RIGHT))
		size += sizespeed;

	static bool dash = false;
	static int dtime = 0;
	std::string dashstr = "";
	dtime -= 1;
	if (dtime < 0) {
		dtime = 60;
		dash = !dash;
	}
	if (dash) dashstr.append("_");

	setRenderColor(100, 244, 255, 200);
	renderFont(&roboto, (Input::getInputLine() + dashstr).c_str(), size, 20, 20);

	if (Overture_IsMobile())
    for (int i = 0; i < 16; i++) {
    	if (Input::pointers[i].active) {
    		float posX = Input::pointers[i].position.x * Overture_GetWindow()->width - 10;
    		float posY = Input::pointers[i].position.y * Overture_GetWindow()->height - 10;

    		setRenderColor(0, 0xFF, 0, 0x88);
    		renderTexture(&arrow, posX, posY, 20, 20);


    		if (i == 0) {
        		renderTexture(&arrow, posX, posY-5, 20, 20);
    		}
    	}
    }

	if (Input::isKeyPressed(KEY_SPACE) || (Overture_IsMobile() && Input::isScreenPressed())) {

	    /* Init Simple-SDL2-Audio */
	    static bool adsf = true;
	    if (adsf) {
	    	Audio_Init();
	    	adsf = false;
	    	Profiler::clearAllData();
	    }
	    Profiler::printData();

	    /* Play music and a sound */
	    //playAudio();
	}

	if (Input::isKeyDown(KEY_1)) {
		Debug_OpenWindow();
	}
	if (Input::isKeyDown(KEY_2)) {
		Debug_CloseWindow();
	}
}



