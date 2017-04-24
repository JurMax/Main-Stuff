/*
 * redoxmain.hpp
 *
 *  Created on: 13 okt. 2016
 *      Author: jurriaanvandenberg
 */

#ifndef SRC_REDOX_REDOXMAIN_HPP_
#define SRC_REDOX_REDOXMAIN_HPP_

#include <JOverture.hpp>
#include <JRenderer.hpp>
#include "includes.hpp"
#include "objects/container.hpp"

using namespace std;

class Redox {
public:

	vector<Container> containers;
	vector<Saltbridge> saltbridges;
	vector<Molecule> molecules;
	vector<Elektron> elektrons;
};


namespace redoxmain {
	void init();
	void update();
	void render();
}



#endif /* SRC_REDOX_REDOXMAIN_HPP_ */
