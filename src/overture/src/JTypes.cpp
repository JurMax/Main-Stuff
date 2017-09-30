/*
 * JTypes.cpp
 *
 *  Created on: 11 sep. 2017
 *      Author: jurriaanvandenberg
 */

#include <JTypes.hpp>

#include <algorithm>
#include <limits>
#include <SDL_Log.h>



Vector2::Vector2() {
	x = 0.0f;
	y = 0.0f;
}


Vector2::Vector2( float x, float y ) {
	this->x = x;
	this->y = y;
}



void Vector2::print() {
	SDL_Log("Vector2(%f, %f)", x, y);
}



float Vector2::extrapolateX( Vector2 other, float atY ) {
	float diffX = other.x - x;
	float diffY = other.y - y;

	return (diffX / diffY) * (atY - y) + x;
}


float Vector2::extrapolateY( Vector2 other, float atX ) {
	float diffX = other.x - x;
	float diffY = other.y - y;

	return (diffY / diffX) * (atX - x) + y;
}


float Vector2::angle() {
	float rad = atan2f(-y, x);
	if (rad < 0.0f) {
		rad += M_PI * 2;
	}
	return rad / M_PI * 180.0f;
}



bool Vector2::isInside( float x, float y, float w, float h ) {
	if (this->x >= x && this->x < x + w && this->y >= y && this->y < y + h) {
		return true;
	}
	return false;
}


bool Vector2::isInside( Vector2 pos, Vector2 size ) {
	return isInside(pos.x, pos.y, size.x, size.y);
}




Quadrilateral::Quadrilateral() {
	p0.x = 0.0f,	p0.y = 0.0f;
	p1.x = 0.0f,	p1.y = 0.0f;
	p2.x = 0.0f,	p2.y = 0.0f;
	p3.x = 0.0f,	p3.y = 0.0f;
}

Quadrilateral::Quadrilateral( float p0x, float p0y, float p1x, float p1y, float p2x, float p2y, float p3x, float p3y ) {
	this->p0.x = p0x,	this->p0.y = p0y;
	this->p1.x = p1x,	this->p1.y = p1y;
	this->p2.x = p2x,	this->p2.y = p2y;
	this->p3.x = p3x,	this->p3.y = p3y;
}

Quadrilateral::Quadrilateral( float posx, float posy, float width, float height ) {
	this->p0.x = posx, 			this->p0.y = posy;
	this->p1.x = posx + width, 	this->p1.y = posy;
	this->p2.x = posx, 			this->p2.y = posy + height;
	this->p3.x = posx + width, 	this->p3.y = posy + height;
}

Quadrilateral::Quadrilateral( Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3 ) {
	this->p0 = p0;
	this->p1 = p1;
	this->p2 = p2;
	this->p3 = p3;
}


void Quadrilateral::flipX( float offset ) {
	Quadrilateral mirroredQuad = Quadrilateral(p0, p1, p2, p3);
	mirroredQuad *= Vector2(-1.0f, 1.0f);
	mirroredQuad += Vector2(offset, 0);

	p0 = mirroredQuad.p1;
	p1 = mirroredQuad.p0;
	p2 = mirroredQuad.p3;
	p3 = mirroredQuad.p2;
}


bool Quadrilateral::hasValues() {
	return  p0.x != 0.0f || p0.y != 0.0f ||
			p1.x != 0.0f || p1.y != 0.0f ||
			p2.x != 0.0f || p2.y != 0.0f ||
			p3.x != 0.0f || p3.y != 0.0f;
}




