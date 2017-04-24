/*
 * container.h
 *
 *  Created on: 13 okt. 2016
 *      Author: jurriaanvandenberg
 */

#ifndef SRC_REDOX_OBJECTS_CONTAINER_HPP_
#define SRC_REDOX_OBJECTS_CONTAINER_HPP_

#include <math.h>
#include "../includes.hpp"
#include "particles/elektron.hpp"
#include "particles/molecule.hpp"
#include "saltbridge.hpp"


class Container {
public:
	Container();

	void update();
	void render();
	void remove();

	void print();

	float posX;
	float posY;
	float width;
	float height;

	//vector<*Molecule> molecules;
	//vector<*Elektron> elektrons;
	Saltbridge connectedSaltbrigde;
};



class JPoint {
public:
	JPoint() {
		x = 0; y = 0;
	}
	JPoint(double x, double y) : x(x), y(y) { }

	double distance(const JPoint& other) const;

	double x;
	double y;
};



#endif /* SRC_REDOX_OBJECTS_CONTAINER_HPP_ */
