/*
 * functions.cpp
 *
 *  Created on: 13 okt. 2016
 *      Author: jurriaanvandenberg
 */

#include <JBezier.hpp>

#include <JRenderer.hpp>


/**
 * A array of al the beziers that are currently activaly animated. Updates are being done by updateBezierAnimations() (see below)
 */
std::vector<Bezier*> animatingBeziers;


void Overture::updateBezierAnimations() {
	int remove[animatingBeziers.size()];
	int removeSize = 0;
	for (int i = 0; i < animatingBeziers.size(); i++) {
	    if (!(*animatingBeziers[i]).aHasFinished) {
	    	//aTime += 1;
	    	//TODO
	    }
	    else {
	    	remove[removeSize] = i;
	    	removeSize++;
	    }
	}

	for (int i = 0; i < removeSize; i++) {
		animatingBeziers.erase(animatingBeziers.begin() + remove[i]);
	}
}



////////////////////JBezier////////////////////////

Vector2 Bezier::getVectorValue(float t) {
	float u = 1.0f - t;
	float tt = t*t;
	float uu = u*u;
	float ttt = tt * t;

	Vector2 vec = p1 * 3 * uu * t  +  p2 * 3 * u * tt  +  ttt;

	return vec;
}


float Bezier::getFloatValue(float x) {
	float y = 1.0f;
	int precision = 2000;
	for(int i = 0; i < precision; i++) {
		float t = (float) i / (float) precision;

		Vector2 vec = getVectorValue(t);
		if (vec.x >= x) {
			y = vec.y;
			break;
		}
	}
	return y;
}


void Bezier::render(float posx, float posy, float width, float height) {
	Vector2 pixel = Vector2(0, 1);
	Vector2 prefPixel;

	int SEGMENT_COUNT = 20;
	for(int i = 0; i <= SEGMENT_COUNT; i++) {
		float t = (float) (i) / (float) SEGMENT_COUNT;
		prefPixel = pixel;
		pixel = getVectorValue(t);
		pixel.y = 1.0 - pixel.y;

		//Renderer::setRenderColor(0, 0, 255.0f*t, 0xFF);
		//Renderer::renderSDLLine(prefPixel.x*width + posx, prefPixel.y*height + posy, pixel.x*width + posx, pixel.y*height + posy);
	}
}


/**
 * Set the values of the bezier animation. Duration is in seconds
 */
void Bezier::setValues(float duration, float size) {
	aDuration = duration*1000;
	aSize = size;
}


void Bezier::startAnimation() {
	if (aSize != 0) {
		aHasFinished = false;
		aTime = 0;
		animatingBeziers.push_back(this);
	}
}

void Bezier::startAnimation(float duration, float size) {
	setValues(duration, size);
	startAnimation();
}


