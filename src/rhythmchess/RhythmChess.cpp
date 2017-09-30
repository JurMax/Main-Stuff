/*
 * RhythmChess.cpp
 *
 *  Created on: 29 jan. 2017
 *      Author: jurriaanvandenberg
 */

#include "RhythmChess.hpp"

#include <vector>
#include <JAudio.hpp>
#include <JInput.hpp>
#include "Player.hpp"

using namespace Renderer;


int fieldWidth = 8;
int fieldHeight = 8;
int tileWidth = 16;
int tileHeight = 16;
int tileTexture[8*8];
SDL_Color backgroundColor = { 0x78, 0xA3, 0x79, 0xFF };

JTexture tileLight0;
JTexture tileLight1;
JTexture tileLight2;
JTexture tileLight3;
JTexture tileDark0;
JTexture tileDark1;
JTexture tileDark2;
JTexture tileDark3;

JTexture toren;
JTexture shadow;

JAudio music1;
JAudio music2;
JAudio music3;
JAudio music4;
JAudio music5;

JAudio thingy;

std::vector<Player> players;


void RCInit() {
	Overture_GetWindow()->setLogicalSize(240, 180);

	//TODO: set threaded loading false

	tileLight0.setAndLoad("rhythmchess/tilelight0.png");
	tileLight1.setAndLoad("rhythmchess/tilelight1.png");
	tileLight2.setAndLoad("rhythmchess/tilelight2.png");
	tileLight3.setAndLoad("rhythmchess/tilelight3.png");
	tileDark0.setAndLoad("rhythmchess/tiledark0.png");
	tileDark1.setAndLoad("rhythmchess/tiledark0.png");
	tileDark2.setAndLoad("rhythmchess/tiledark0.png");
	tileDark3.setAndLoad("rhythmchess/tiledark0.png");

	toren.setAndLoad("rhythmchess/toren.png");
	shadow.setAndLoad("rhythmchess/shadow.png");

	music1.setAndLoad("audio/discodecent.ogg", 115, 0);
	music2.setAndLoad("audio/song.wav", 125, 0);
	music3.setAndLoad("audio/song028.wav", 94, 100);
	music4.setAndLoad("audio/Crypteque.ogg", 130, 0);
	music5.setAndLoad("audio/maximsding.ogg", 120, 0);

	setBeatRange(150, 150);

	thingy.setAndLoad("audio/Bass-Drum-1.wav");

	bool dark = false;
	for (int i = 0; i < fieldWidth * fieldHeight; i++) {
		int tex = rand() % 4;
		if (dark) {
			tex += 4;
		}
		if (i % fieldWidth != fieldWidth-1)
			dark = !dark;
		tileTexture[i] = tex;
	}


	players.push_back(Player(0, 0, KEY_W, KEY_S, KEY_A, KEY_D));
	players.push_back(Player(6, 6, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT));

	SDL_Log("init done");
}


void RCUpdate() {

	if (Input::isKeyPressed(KEY_1)) {
		playAudio(&music1);
	}
	if (Input::isKeyPressed(KEY_2)) {
		playAudio(&music2);
	}
	if (Input::isKeyPressed(KEY_3)) {
		playAudio(&music3);
	}
	if (Input::isKeyPressed(KEY_4)) {
		playAudio(&music4);
	}
	if (Input::isKeyPressed(KEY_5)) {
		playAudio(&music5);
	}
	if (Input::isKeyPressed(KEY_9)) {
		pauseAudio();
	}
	if (Input::isKeyPressed(KEY_0)) {
		stopAudio();
	}

	for (unsigned int i = 0; i < players.size(); i++) {
		players.at(i).update();
	}
}


void RCRender() {
	Overture_GetWindow()->setTitle("SDL Window | FPS: " + toString(Overture_GetFramerate()));


	///// Render Background /////
	static bool beat = true;
	static bool draw = true;
	if (audioOnBeat()) {
		if (beat) {
			beat = false;
			draw = !draw;
		}
	}
	else {
		beat = true;
	}

	if (draw) {
		setRenderColor(backgroundColor);
	}
	else {
		setRenderColor(191, 30, 30);

	}
	renderRect(0, 0, Overture_GetWindow()->width, Overture_GetWindow()->height);



	///// Render Chess board ////
	float offsetX = Overture_GetWindow()->width/2 - fieldWidth*tileWidth/2;
	float offsetY = Overture_GetWindow()->height/2 - fieldHeight*tileHeight/2;

	setRenderColor(115, 75, 62);
	renderRect(offsetX - 17, offsetY - 17, fieldWidth*tileWidth + 34, fieldHeight*tileHeight + 34);
	setRenderColor(228, 239, 131);
	renderRect(offsetX - 1, offsetY - 1, fieldWidth*tileWidth + 2, fieldHeight*tileHeight + 2);

	resetRenderColor();
	for (int i = 0; i < fieldHeight; i++) {
		for (int i2 = 0; i2 < fieldWidth; i2++) {
			int tex = tileTexture[i*fieldWidth + i2];
			JTexture* squaretexture;

			if (tex == 0) squaretexture = &tileLight0;
			else if (tex == 1) squaretexture = &tileLight1;
			else if (tex == 2) squaretexture = &tileLight2;
			else if (tex == 3) squaretexture = &tileLight3;
			else if (tex == 4) squaretexture = &tileDark0;
			else if (tex == 5) squaretexture = &tileDark1;
			else if (tex == 6) squaretexture = &tileDark2;
			else if (tex == 7) squaretexture = &tileDark3;
			else squaretexture = &textureBlank;

			renderTexture(*squaretexture, offsetX + i2*tileWidth, offsetY + i*tileHeight, tileHeight, tileWidth);
		}
	}


	for (unsigned int i = 0; i < players.size(); i++) {
		players.at(i).render(offsetX, offsetY);
	}



	/////// Render Beat thingy ///////

	float centerX = Overture_GetWindow()->width/2, centerY = 10;
	static float middleSizeX = 4, middleSizeY = 16;

	float distanceBetween = 20;
	float offset = beatProgress() * (float) distanceBetween;
	float offsetOffbeat = offbeatProgress() * (float) distanceBetween;
	float width = 2, height = 14;

	for (int i = 0; i < 6; i++) {

		setRenderColor(35, 234, 61);
		renderRect(centerX - width/2 + offset + distanceBetween * i, centerY - height/2, width, height);
		renderRect(centerX - width/2 - offset - distanceBetween * i, centerY - height/2, width, height);

		setRenderColor(255, 255, 255);
		renderRect(centerX - width/2 + offsetOffbeat + distanceBetween * i, centerY - width/2, width, width);
		renderRect(centerX - width/2 - offsetOffbeat - distanceBetween * i, centerY - width/2, width, width);
	}

	static bool beatIndicator = false;
	if (audioOnBeat()) {
		if (Input::isKeyPressed(KEY_SPACE)) {
			beatIndicator = true;
			SDL_Log("Beat!");
			playAudio(&thingy);
		}
		middleSizeX = 4;
		middleSizeY = 18;
	}
	else {
		beatIndicator = false;
		middleSizeX = 2;
		middleSizeY = 16;
	}

	if (beatIndicator) {
		setRenderColor(255, 0, 0);
	}
	else {
		setRenderColor(255, 255, 255);
	}

	if (audioOnOffbeat()) {
		if (Input::isKeyPressed(KEY_SPACE)) {
			SDL_Log("Offbeat!");
		}
	}

	renderRect(centerX - middleSizeX/2, centerY - middleSizeY/2, middleSizeX, middleSizeY);
}


void moveDown() {

}


