/*
 * container.cpp
 *
 *  Created on: 13 okt. 2016
 *      Author: jurriaanvandenberg
 */

#include "container.hpp"


Container::Container() {
	printf("ding");
	width = 200;
	height = 200;
	posX = 200;
	posY = 200;
}


void Container::print() {
	std::cout << TEST << std::endl;

	JPoint point = JPoint(2.0, 3.0);
	std::cout << point.distance(JPoint(3.0,3.0)) << std::endl;

	JPoint point2;
	std::cout << point2.distance(JPoint(3.0,3.0)) << std::endl;
}


double JPoint::distance(const JPoint& other) const {
	double dx = x - other.x;
	double dy = y - other.y;
	return sqrt(dx * dx + dy * dy);
}

