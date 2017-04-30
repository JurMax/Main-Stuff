/*
 * JObject.cpp
 *
 *  Created on: 25 apr. 2017
 *      Author: jurriaanvandenberg
 */

#include <JObject.hpp>

#include <JInput.hpp>


void updateObjectList( bool update );


std::vector<JObjectPointer> objects;

JObject debugObject1;


namespace JObjects {

	void Update() {

		if (Input::isKeyDown(KEY_O)) {
			debugObject1 = JObject(100, 100, 200, 200, "Debug object 1");
		}


		for (unsigned int i = 0; i < objects.size(); i++) {
			int i2 = objects.size() - 1 - i;
			if (objects.at(i2).allocated) {
				JObject* object = objects.at(i2).object;

				if (!object->independentUpdate) {
					object->update();
				}
			}
		}

	}


	void Render() {
		for (unsigned int i = 0; i < objects.size(); i++) {
			if (objects.at(i).allocated) {
				if (!objects.at(i).object->independentRender) {
					objects.at(i).object->render();
				}
			}
		}
	}
}


/**
 * This function sorts the list
 */
void updateObjectList( bool update ) {


}







/////////////////////// JObject Class ///////////////////////

JObject::JObject() {
	init();
}

JObject::JObject( float x, float y, float w, float h, std::string n ) {
	init();

	name = n;

	posX = x;
	posY = y;
	width = w;
	height = h;
}

JObject::~JObject() {
	name = "[removed]";

	removeColor();
	removeTexture();
	removeText();

	objects.at(vectorIndex).allocated = false;
}



void JObject::init() {
	name = "[no name]";
	width = 0;
	height = 0;
	posX = 0;
	posY = 0;
	zIndex = 0;

	independentRender = false;
	independentUpdate = false;

	color.a = 0xFF;
	color.r = 0xFF;
	color.b = 0xFF;
	color.a = 0xFF;
	renderColor = false;
	isMask = false;

	cTexture = NULL;
	cText = NULL;

	// Add this JObject to the objects vector
	bool added = false;
	for (unsigned int i = 0; i < objects.size(); i++) {
		if (objects.at(i).allocated) {
			JObject* other = objects.at(i).object;

			if (!added) {
				// The objects in vector have zIndex ascending
				if (zIndex < other->zIndex) {
					addToVector(i);
					added = true;
				}
			}
			else {
				// All objects after JObject have been pushed forward, so add 1 to their vector indices
				other->vectorIndex++;
			}
		}
	}

	// JObject not added yet, so add it to the back of the vector
	if (!added) {
		addToVector(objects.size());
	}

}


void JObject::addToVector( int index ) {
	vectorIndex = index;
	JObjectPointer pointer;
	pointer.object = this;
	pointer.allocated = true;
	objects.insert(objects.begin() + vectorIndex, pointer);
}


/**
 * Update zIndex of JObject and objects vector (so it remains sorted)
 */
void JObject::setZIndex( float zindex ) {

	if (zindex > zIndex) {
		for (unsigned int i = vectorIndex + 1; i < objects.size(); i++) {
			if (objects.at(i).allocated) {
				JObject* other = objects.at(i).object;

				bool moved = false;
				if (!moved) {
					if (zindex < other->zIndex) {
						// Move JObject to new index
						JObjectPointer pointer = objects.at(vectorIndex);
						objects.erase(objects.begin() + vectorIndex);
						objects.insert(objects.begin() + i - 1, pointer);
						moved = true;

						vectorIndex = i - 1;
						zIndex = zindex;
						break;
					}
					else {
						// All objects between old and new index of JObject will be pushed backward after JObject is moved.
						other->vectorIndex--;
					}
				}

				if (!moved) {
					JObjectPointer pointer = objects.at(vectorIndex);
					objects.erase(objects.begin() + vectorIndex);
					objects.push_back(pointer);

					vectorIndex = i - 1;
					zIndex = zindex;
				}
			}
		}
	}
	else if (zindex < zIndex) {
		//TODO: Dit
	}
}


void JObject::setColor( SDL_Color clr ) {
	color = clr;
}


void JObject::setColor( int r, int g, int b, int a ) {
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
}



void JObject::addColor() {
	renderColor = true;
}


void JObject::addTexture( JTexture* tex, bool fill, bool repeat ) {
	JCompTexture* ct = new JCompTexture;
	ct->texture = tex;
	ct->fill = fill;
	ct->repeat = repeat;
	cTexture = ct;
}


void JObject::addText( JFont* font, std::string text, int size ) {
	JCompText* ct = new JCompText;
	ct->font = font;
	ct->text = text;
	ct->size = size;
	//TODO complete this;
	cText = ct;
}



void JObject::removeColor() {
	renderColor = false;
}


void JObject::removeTexture() {
	delete cTexture;
	cTexture = NULL;
}

void JObject::removeText() {
	delete cText;
	cText = NULL;
}



void JObject::update() {
	for (unsigned int i = 0; i < children.size(); i++) {
		JObject* child = children.at(children.size() - i - 1);
		if (!child->independentUpdate) {
			child->update();
		}
	}

}


void JObject::render() {

}



float JObject::getZIndex() {
	return zIndex;
}


bool JObject::isPressed() {
	return inputPressed;
}

bool JObject::isDown() {
	return inputDown;
}

bool JObject::isHovered() {
	return inputHovered;
}





