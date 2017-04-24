/*
 * JVector.hpp
 *
 *  Created on: 13 nov. 2016
 *      Author: jurriaanvandenberg
 */

#ifndef OVERTURE_JVECTOR_HPP_
#define OVERTURE_JVECTOR_HPP_

#include <SDL_Log.h>

class Vector2 {
public:
	Vector2() {
		x = 0, y = 0;
	}
	Vector2(float x, float y) : x(x), y(y) {  }

	void print() {
		SDL_Log("Vector2: (%f, %f)", x, y);
	}

	float x;
	float y;


	Vector2 operator++(int) {
		x++; y++;
		return *this;
	}
	Vector2& operator++() {
		x++; y++;
		return *this;
	}

    Vector2& operator=(const float& rhs) {
    	x = rhs;
    	y = rhs;
    	return *this; // return the result by reference
    }

    Vector2& operator*=(const Vector2& rhs) {
        x *= rhs.x; y *= rhs.y;
        return *this;
    }
    Vector2& operator*=(const float& rhs) {
        x *= rhs; y *= rhs;
        return *this;
    }
    friend Vector2 operator*(Vector2 lhs, const Vector2& rhs) {
    	lhs *= rhs;
    	return lhs;
    }
    friend Vector2 operator*(Vector2 lhs, const float& rhs) {
    	lhs *= rhs;
    	return lhs;
    }

    Vector2& operator+=(const Vector2& rhs) {
    	x += rhs.x;
    	y += rhs.y;
    	return *this; // return the result by reference
    }
    Vector2& operator+=(const float& rhs) {
    	x += rhs;
    	y += rhs;
    	return *this; // return the result by reference
    }

    friend Vector2 operator+(Vector2 lhs, const Vector2& rhs) {
    	lhs += rhs; // reuse compound assignment
    	return lhs; // return the result by value (uses move constructor)
    }

    friend Vector2 operator+(Vector2 lhs, const float& rhs) {
    	lhs += rhs; // reuse compound assignment
    	return lhs; // return the result by value (uses move constructor)
    }
};


#endif /* OVERTURE_JVECTOR_HPP_ */
