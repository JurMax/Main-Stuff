/*
 * Molecule.h
 *
 *  Created on: 13 okt. 2016
 *      Author: jurriaanvandenberg
 */

#ifndef SRC_REDOX_PARTICLES_MOLECULE_HPP_
#define SRC_REDOX_PARTICLES_MOLECULE_HPP_

#include "../../includes.hpp"


class Molecule {
public:
	Molecule();

	void update();
	void render();

	float posX;
	float posY;
	//float size = 80;

	std::string currentLabel;
	std::string name;
};

#endif /* SRC_REDOX_PARTICLES_MOLECULE_HPP_ */
