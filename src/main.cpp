/*
 * main.cpp
 *
 *  Created on: 13 okt. 2016
 *      Author: jurriaanvandenberg
 */

#include <SDL.h>
#include <JOverture.hpp>
#include "redox/RedoxMain.hpp"
#include "rhythmchess/RhythmChess.hpp"

#undef main
#include <iostream>
#include <stdio.h>
#include <windows.h>

using namespace std;

int main() {
	std::cout << "test" << std::endl;

	//TODO: platform specific stuff

	if (Overture_Init()) {
		bool redox = true;
		if (redox) {
			redoxmain::init();
			Overture_SetUpdateFunc(redoxmain::update);
			Overture_SetRenderFunc(redoxmain::render);
		}
		else {
			RCInit();
			Overture_SetUpdateFunc(RCUpdate);
			Overture_SetRenderFunc(RCRender);
		}

		Overture_Start();
		Overture_Close();
	}

	return 0;
}

// .\Users\"Jurriaan en Maxim"\Documents\Documenten\Programming\WeeklyGames\Main-Stuff\Debug\Main-Stuff.exe
