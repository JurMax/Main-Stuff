/*

 * functions.hpp
 *
 *  Created on: 13 okt. 2016
 *      Author: jurriaanvandenberg
 */

#ifndef SRC_REDOX_SUPPORT_FUNCTIONS_HPP_
#define SRC_REDOX_SUPPORT_FUNCTIONS_HPP_

#include <vector>
#include <JVector.hpp>

class Bezier {
public:
	Bezier(Vector2 p1, Vector2 p2) : p1(p1), p2(p2) {
		aDuration = 0;
		aSize = 0;
		aHasFinished = true;
		aTime = 0;
	}
	//Bezier(Bezier copy) : p1(copy.p1), p2(copy.p2) { }

	float getFloatValue(float t);
	Vector2 getVectorValue(float x);
	void calculateTXValues();

	void setValues(float duration, float size);
	void startAnimation();
	void startAnimation(float duration, float size);

	void render(float posx, float posy, float width, float height);

	Vector2 p1;
	Vector2 p2;

	// Animation variables
	float aDuration;
	float aSize;
	bool aHasFinished;
	float aTime;
};


namespace Overture {
	void updateBezierAnimations();
}

extern Bezier ease_in;
extern Bezier ease_in_ease_out;


#endif /* SRC_REDOX_SUPPORT_FUNCTIONS_HPP_ */
