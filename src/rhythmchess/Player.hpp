/*
 * Player.hpp
 *
 *  Created on: 1 feb. 2017
 *      Author: jurriaanvandenberg
 */

#ifndef SRC_RHYTHMCHESS_PLAYER_HPP_
#define SRC_RHYTHMCHESS_PLAYER_HPP_

#include <JKeys.hpp>

/**
 *  Hoe gaat bewegen werken?
 * 		Een functie returned true zodra er een beat is, en een bepaald periode daaromheen.
 *		In die periode er op een van de directies wordt gedrukt start de beweging
 *		Deze beweging duurt zo lang zodat hij precies op de volgende beat op het volgende vakje komt.
 *		De karakter maakt een sprong, hij is op het hoogste punt in het midden van de animationTime.
 *		Zet de destinationTileX,Y. Als dit hetzelfde is als een andere player dan vallen beide players
 *			terug nadat het hoogste punt is bereikt. Dit gebeurt ook tegen de zijkant van het veld aan.
 *
 *		Offbeat voor specials. Werkt hetzelfde als de onbeats, maar elke character doet iets speciaals.
 *		Specials gaan door elkaar heen, als twee players eindigen op dezelfde plek zijn ze beide dood.
 *
 *		if (abs(timeFromBeat) < beatPeriode) {
 *			animationTime = TimeBetweenBeats - timeFromBeat.
 *			destinationTileX = iets;
 *			destinationTileY = iets;
 *		}
 *
 */

typedef enum  {
	CHARACTER_UNKOWN = -1,
	CHARACTER_PAWN = 0,
	CHARACTER_ROOK = 1,
	CHARACTER_KNIGHT = 2,
	CHARACTER_BISHOP = 3,
	CHARACTER_QUEEN = 4,
	CHARACTER_KING = 5
} Character;

class Player {
public:
	Player();
	Player( int startPosX, int startPosY, JKey up, JKey down, JKey left, JKey right );

	void setKeys( JKey up, JKey down, JKey left, JKey right );

	void update();
	void render( float offstX, float offstY );

	float posX;
	float posY;
	float offsetY;
	int tileX;
	int tileY;

	int destinationTileX;
	int destinationTileY;

	// 0 = pion, 1 = tower, 2 = horse, 3 = loper, 4 queen, 5 = king
	Character character;
	bool canMove;
	bool isMoving;
	int moveTime;
	Uint32 moveStartTime;
	bool halfway;

	JKey keyUp;
	JKey keyDown;
	JKey keyLeft;
	JKey keyRight;
};

#endif /* SRC_RHYTHMCHESS_PLAYER_HPP_ */
