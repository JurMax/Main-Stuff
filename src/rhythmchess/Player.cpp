/*
 * Player.cpp
 *
 *  Created on: 1 feb. 2017
 *      Author: jurriaanvandenberg
 */

#include "Player.hpp"

#include <math.h>
#include <JInput.hpp>
#include <JAudio.hpp>
#include <JRenderer.hpp>
#include "RhythmChess.hpp"

using namespace Renderer;

Player::Player() {
	posX = 0;
	posY = 0;
	offsetY = 0;
	tileX = 0;
	tileY = 0;

	destinationTileX = 0;
	destinationTileY = 0;

	character = CHARACTER_UNKOWN;
	canMove = true;
	isMoving = false;
	moveTime = 0;
	moveStartTime = 0;
	halfway = false;

	keyUp = KEY_UNKOWN;
	keyDown = KEY_UNKOWN;
	keyLeft = KEY_UNKOWN;
	keyRight = KEY_UNKOWN;
}


Player::Player( int startPosX, int startPosY, JKey up, JKey down, JKey left, JKey right ) {
	posX = (float) startPosX;
	posY = (float) startPosY;
	offsetY = 0;
	tileX = startPosX;
	tileY = startPosY;

	destinationTileX = startPosX;
	destinationTileY = startPosY;

	character = CHARACTER_UNKOWN;
	canMove = true;
	isMoving = false;
	moveTime = 0;
	moveStartTime = 0;
	halfway = false;

	setKeys(up, down, left, right);
}


void Player::setKeys( JKey up, JKey down, JKey left, JKey right ) {
	keyUp = up;
	keyDown = down;
	keyLeft = left;
	keyRight = right;
}


void Player::update() {


	if (audioOnBeat()) {
		if (canMove && !isMoving) {
			bool keyPressed = false;
			if (Input::isKeyPressed(keyUp)) {
				keyPressed = true;
				destinationTileY = tileY - 1;
			}
			else if (Input::isKeyPressed(keyDown)) {
				keyPressed = true;
				destinationTileY = tileY + 1;
			}
			else if (Input::isKeyPressed(keyLeft)) {
				keyPressed = true;
				destinationTileX = tileX - 1;
			}
			else if (Input::isKeyPressed(keyRight)) {
				keyPressed = true;
				destinationTileX = tileX + 1;
			}

			if (keyPressed) {
				isMoving = true;
				int beatTicks, beatRangeFor, beatRangeAfter;
				beatProgress(NULL, &beatTicks);
				getBeatRange(&beatRangeFor, &beatRangeAfter);
				//TODO fix als het voor de beat komt!!

				if (beatTicks <= beatRangeFor)
					beatTicks += getPlayingMusic()->getBeatLength();

				moveTime = beatTicks - beatRangeFor;

				moveStartTime = SDL_GetTicks();
				halfway = false;
			}
		}
	}
	else if (audioOnOffbeat()) {

	}
	else {
		// TODO: failed animation
	}



	if (isMoving) {
		int halfWayTime = moveTime/2;

		int ticks = SDL_GetTicks() - moveStartTime;

		if (!halfway && ticks > halfWayTime) {
			halfway = true;

			bool cantMove = false;
			// check al de players voor destinationTileX if moving
			if (cantMove) {
				destinationTileX = tileX;
				destinationTileX = tileY;
			}
		}

		//float testspeed = 0.05f;


		float offset = (float) (ticks*2) / (float) (moveTime);
		if (offset > 1.0f)
			offset = 1.0f;

		offsetY = -sin(offset*M_PI)/2;

		if (tileX > destinationTileX) {
			posX = (float) (tileX) - offset;
		}
		else if (tileX < destinationTileX) {
			posX = (float) (tileX) + offset;
		}
		if (tileY > destinationTileY) {
			posY = (float) (tileY) - offset;
		}
		else if (tileY < destinationTileY) {
			posY = (float) (tileY) + offset;
		}

		SDL_Log("%d: movetime", moveTime);
		if (ticks >= moveTime) {
			isMoving = false;
			posX = (float) destinationTileX;
			posY = (float) destinationTileY;
			tileX = destinationTileX;
			tileY = destinationTileY;
		}
	}
}


void Player::render( float offstX, float offstY ) {
	setRenderColor(22, 22, 22, 100);
	renderTexture(shadow, offstX + posX * tileWidth + 1, offstY + posY * tileHeight + 22);
	resetRenderColor();
	renderTexture(toren, offstX + posX * tileWidth, offstY + (posY + offsetY) * tileHeight);
}
