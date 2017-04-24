/*
 * Saltbrigde.hpp
 *
 *  Created on: 13 okt. 2016
 *      Author: jurriaanvandenberg
 */

#ifndef SRC_REDOX_OBJECTS_SALTBRIDGE_HPP_
#define SRC_REDOX_OBJECTS_SALTBRIDGE_HPP_

#include "../includes.hpp"
//#include "container.hpp"

class Saltbridge;

class Saltbridge {
public:
	Saltbridge();

	void render();
	void update();

	//vector<*Container> connectedContainers;
};

#endif /* SRC_REDOX_OBJECTS_SALTBRIDGE_HPP_ */
