/*
 * main.cpp
 *
 *  Created on: 13 okt. 2016
 *      Author: jurriaanvandenberg
 */

#include <JOverture.hpp>

#include "redox/RedoxMain.hpp"
#include "rhythmchess/RhythmChess.hpp"


int main( int argc, char* argv[] ) {
	bool redox = true;
	if (redox) {
		if (Overture_Init()) {
			redoxmain::init();
			Overture_SetUpdateFunc(redoxmain::update);
			Overture_SetRenderFunc(redoxmain::render);

			Overture_Start();
			Overture_Close();
		}
	}
	else {
		if (Overture_Init()) {
			RCInit();
			Overture_SetUpdateFunc(RCUpdate);
			Overture_SetRenderFunc(RCRender);

			Overture_Start();
			Overture_Close();
		}
	}

	return 0;
}


