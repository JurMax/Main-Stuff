/*
 * RhythmChess.hpp
 *
 *  Created on: 29 jan. 2017
 *      Author: jurriaanvandenberg
 */

#ifndef SRC_RHYTHMCHESS_RHYTHMCHESS_HPP_
#define SRC_RHYTHMCHESS_RHYTHMCHESS_HPP_

#include <JOverture.hpp>
#include <JRenderer.hpp>

extern JTexture toren;
extern JTexture shadow;

extern int fieldWidth;
extern int fieldHeight;
extern int tileWidth;
extern int tileHeight;

void RCInit();
void RCUpdate();
void RCRender();

#endif /* SRC_RHYTHMCHESS_RHYTHMCHESS_HPP_ */
