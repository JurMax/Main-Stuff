/*
 * JObject.cpp
 *
 *  Created on: 25 apr. 2017
 *      Author: jurriaanvandenberg
 */

#include <JObject.hpp>

#include <JInput.hpp>
#include <JCursor.hpp>
#include <JRenderer.hpp>
#include <JORender.hpp>
#include <JOCollision.hpp>

using namespace Renderer;
using namespace JObjects;


namespace Overture {

	void Object_Close() {
		for (unsigned int i = 0; i < objects.size(); i++) {
			if (objects[i] != NULL && objects[i]->destroyable) {
				delete objects[i];
				objects[i] = NULL;
			}
			// Other JObjects will be freed automatically on program exit.
		}
	}
}



namespace JObjects {

	std::vector<JObject*> objects;


	void Update() {

		for (unsigned int i = 0; i < objects.size(); i++) {
			JObject* object = objects[objects.size() - 1 - i];

			if (object != NULL && object->initialised) {
				if (!object->independentUpdate) {
					object->update();
				}
			}
		}

		Collision::resolveCollisions();
	}


	void Render() {
		for (unsigned int i = 0; i < objects.size(); i++) {
			if (objects[i] != NULL && objects[i]->initialised) {
				if (!objects[i]->independentRender) {
					objects[i]->render();
				}
			}
		}
	}

}




/////////////////////// JObject Class ///////////////////////


JObject::JObject( bool canBeDeleted ) {
	name = "[not initialised]";
	tag = 0;

	initialised = false;
	active = true;
	destroyable = canBeDeleted;

	localPosX = 0.0f;
	localPosY = 0.0f;
	localPosZ = 0.0f;
	localWidth = 0.0f;
	localHeight = 0.0f;

	prevPosX = 0.0f;
	prevPosY = 0.0f;
	prevPosZ = 0.0f;
	prevWidth = 0.0f;
	prevHeight = 0.0f;

	minWidth = 0.0f;
	minHeight = 0.0f;

	sizeChanged = false;
	posChanged = false;

	independentRender = false;
	independentUpdate = false;

	isHUD = false;
	isMask = false;

	pressedPointerIndex = -1;
	clickedPointerIndex = -1;

	parent = NULL;
	vectorIndex = -1;
}


JObject::~JObject() {
	if (initialised) {
		// Delete and remove all components.
		for (unsigned int i = 0 ; i < components.size() ; i++) {
			delete components[i];
		}
		components.clear();

		// Remove object from global objects vector.
		objects[vectorIndex] = NULL;
		for (unsigned int i = vectorIndex; i < objects.size(); i++) {
			if (objects[i] != NULL) {
				objects[i]->vectorIndex--;
			}
		}
		objects.erase(objects.begin() + vectorIndex);

		// If object has a parent, remove this object from their children vector.
		if (parent != NULL) {
			for (unsigned int i = 0; i < parent->children.size(); i++) {
				if (parent->children[i] == this) {
					parent->children.erase(parent->children.begin() + i);
					break;
				}
			}
		}

		// Remove parent from children objects and destroy destroyable children, as they are no longer needed.
		for (unsigned int i = 0; i < children.size(); i++) {
			if (children[i]->destroyable) {
				children[i]->destroy();
			}
			else {
				children[i]->setParent(NULL);
			}
		}
	}
}



void JObject::init( std::string n ) {
	name = n;
	initialised = true;

	// Add this JObject to the objects vector
	bool added = false;
	for (unsigned int i = 0; i < objects.size(); i++) {
		JObject* other = objects[i];

		if (other != NULL) {
			if (other == this) {
				// Object is already in vector
				added = true;
				break;
			}

			if (!added) {
				// The objects in the vector have their Z positions ascending
				if (other->getPosZ() > getPosZ()) {
					objects.insert(objects.begin() + i, this);
					vectorIndex = i;
					added = true;
				}
			}
			else {
				// All objects after this object will be pushed forward, so add 1 to their vector indices
				other->vectorIndex++;
			}
		}
	}

	// Not yet added, so add it to the back of the vector
	if (!added) {
		vectorIndex = objects.size();
		objects.push_back(this);
	}
}


void JObject::init( float x, float y, float w, float h, std::string n ) {
	localPosX = x;
	localPosY = y;
	localWidth = w;
	localHeight = h;

	init(n);
}


void JObject::destroy() {

	// Destroy al pointer-created children.
	for (int i = children.size() - 1; i >= 0; i--) {
		children[i]->destroy();
	}

	if (destroyable) {
		name = "[removed]";
		active = false;
		delete this;
	}

}


void JObject::update() {
	if (active) {
		if (getPosX() != prevPosX || getPosY() != prevPosY) {
			posChanged = true;
		}
		else {
			posChanged = false;
		}

		if (getWidth() != prevWidth || getHeight() != prevHeight) {
			sizeChanged = true;
		}
		else {
			sizeChanged = false;
		}


		if (pressedPointerIndex != -1 && !Input::pointers[pressedPointerIndex].pressed) {
			pressedPointerIndex = -1;
		}


		prevPosX = getPosX();
		prevPosY = getPosY();
		prevWidth = getWidth();
		prevHeight = getHeight();

		for (int i = 0; i < components.size(); i++) {
			int i2 = components.size() - 1 - i;
			// Make sure that the components object points to this object, even after a copy of the component.
			components[i2]->object = this;
			if (components[i2]->active) {
				components[i2]->update();
			}
		}
	}
}


void JObject::render() {
	if (active) {

		startParentMask();

		for (unsigned int i = 0; i < components.size(); i++) {
			// Make sure that the components object points to this object, even after a copy of the component.
			components[i]->object = this;
			if (components[i]->active) {
				components[i]->render();
			}
		}

		renderParentMask();
	}
}



void JObject::startParentMask() {
	if (parent != NULL && parent->isMask) {
		activateClippingMask(parent->getPosX(), parent->getPosY(), parent->getWidth(), parent->getHeight());
		parent->startParentMask();
	}
}


void JObject::renderParentMask() {
	if (parent != NULL && parent->isMask) {
		parent->renderParentMask();
		drawClippingMask();
	}
}


void JObject::getUnmaskedBounds( float* x, float* y, float* w, float* h ) {

	float posX = getPosX();
	float posY = getPosY();
	float width = getWidth();
	float height = getHeight();

	for (JObject* oParent = parent; oParent != NULL; oParent = oParent->parent) {
		if (oParent->isMask) {
			float parentX = oParent->getPosX();
			float parentY = oParent->getPosY();
			float parentWidth = oParent->getWidth();
			float parentHeight = oParent->getHeight();

			if (parentX > posX) {
				width -= parentX - posX;
				posX = parentX;
			}
			if (oParent->getPosY() > posY) {
				height -= parentY - posY;
				posY = parentY;
			}
			if (parentX + parentWidth < posX + width) {
				width = parentX + parentWidth - posX;
			}
			if (parentY + parentHeight < posY + height) {
				height = parentY + parentHeight - posY;
			}
		}
	}

	if (width < 0.0f)
		width = 0.0f;
	if (height < 0.0f)
		height = 0.0f;

	if (x != NULL)
		*x = posX;
	if (y != NULL)
		*y = posY;
	if (w != NULL)
		*w = width;
	if (h != NULL)
		*h = height;
}



void JObject::setPosX( float newPosX ) {
	localPosX = newPosX;

	if (parent != NULL) {
		localPosX += parent->getPosX();
	}
}


void JObject::setPosY( float newPosY ) {
	localPosY = newPosY;

	if (parent != NULL) {
		localPosY += parent->getPosY();
	}
}


/**
 * Update zIndex of JObject and objects vector (so it remains sorted).
 */
void JObject::setLocalPosZ( float newLocalPosZ ) {
	if (initialised) {
		float newPosZ = newLocalPosZ;
		if (parent != NULL) {
			newPosZ += parent->getPosZ();
		}

		if (newPosZ > prevPosZ) {
			bool moved = false;

			for (unsigned int i = vectorIndex + 1; i < objects.size(); i++) {
				JObject* other = objects[i];

				if (other != NULL) {
					if (other->getPosZ() > newPosZ) {
						// Add the object pointer to this new place and remove the old pointer.
						objects.insert(objects.begin() + i, this);
						objects.erase(objects.begin() + vectorIndex);
						vectorIndex = i - 1;
						moved = true;
						break;
					}
					else {
						// Move vectorIndices in between the old and new location of this JObject one back.
						other->vectorIndex--;
					}
				}
			}

			// Not moved, so move to back of vector (newPosZ is highest of al objects).
			if (!moved) {
				objects.push_back(this);
				objects.erase(objects.begin() + vectorIndex);
				vectorIndex = objects.size() - 1;
			}
		}
		else if (newPosZ < prevPosZ) {
			bool moved = false;

			for (int i = vectorIndex - 1; i >= 0; i--) {
				JObject* other = objects[i];

				if (other != NULL) {
					// Move vectorIndex in between the old and new location of the JObject one forward
					other->vectorIndex++;
					if (other->getPosZ() < newPosZ) {
						// Add the object pointer to this new place and remove the original pointer object from vector.
						objects.insert(objects.begin() + i + 1, this);
						objects.erase(objects.begin() + vectorIndex + 1);
						vectorIndex = i + 1;
						moved = true;
						break;
					}
				}
			}

			// Not moved, so move to begin of vector (posZ is lowest)
			if (!moved) {
				objects.insert(objects.begin(), this);
				objects.erase(objects.begin() + vectorIndex + 1);
				vectorIndex = 0;
			}
		}
	}

	localPosZ = newLocalPosZ;
	prevPosZ = localPosZ;

	for (unsigned int i = 0; i < children.size(); i++) {
		children[i]->setLocalPosZ(children[i]->localPosZ);
	}
}


void JObject::setSize( float width, float height ) {
	localWidth = width;
	localHeight = height;
}



void JObject::setActive( bool active ) {
	this->active = active;
	for (int i = 0; i < children.size(); i++) {
		children[i]->setActive(active);
	}
}



void JObject::setPressed( int index ) {
	if (index >= 0 && index < Input::pointerAmount) {
		ownsPointers[index] = true;
	}
	//isPressed = true;
}




float JObject::getPosX() {
	float posx = localPosX;
	if (parent != NULL) {
		posx += parent->getPosX();
	}
	else if (isHUD) {
		posx += Overture::cameraPos.x - Renderer::renderTarget->width / 2;
	}
	return posx;
}


float JObject::getPosY() {
	float posy = localPosY;
	if (parent != NULL) {
		posy += parent->getPosY();
	}
	else if (isHUD) {
		posy += Overture::cameraPos.y - Renderer::renderTarget->height / 2;
	}
	return posy;
}


float JObject::getPosZ() {
	float posz = localPosZ;
	if (parent != NULL) {
		posz += parent->getPosZ();
	}
	return posz;
}


Vector2 JObject::getPos() {
	return Vector2(getPosX(), getPosY());
}



float JObject::getWidth() {
	float width = localWidth;
	if (width < minWidth) {
		width = minWidth;
	}
	return width;
}


float JObject::getHeight() {
	float height = localHeight;
	if (height < minHeight) {
		height = minHeight;
	}
	return height;
}


Vector2 JObject::getSize() {
	return Vector2(getWidth(), getHeight());
}



bool JObject::consumePointers() {
	Vector2 unmaskedPos, unmaskedSize;
	getUnmaskedBounds(&unmaskedPos.x, &unmaskedPos.y, &unmaskedSize.x, &unmaskedSize.y);
	bool pointerConsumend = false;

	if (unmaskedSize.x > 0.0f && unmaskedSize.y > 0.0f) {
		for (int i = 0; i < Input::pointerAmount; i++) {
			InputPointer* pointer = &Input::pointers[i];

			if (pointer->active && pointer->pressed && !pointer->pressConsumed) {
				if (Input::screenToRender(pointer->pressedPosition).isInside(unmaskedPos, unmaskedSize)) {
					ownsPointers[i] = true;
					pointer->pressConsumed = true;
				}
			}

			if (ownsPointers[i]) {
				if (!pointer->down && !pointer->released) {
					ownsPointers[i] = false;
					clickedPointerIndex = -1;
				}
				pointerConsumend = true;
			}
		}
	}

	return pointerConsumend;
}



bool JObject::isPressed() {
	consumePointers();
	bool pressed = false;

	for (int i = 0; i < Input::pointerAmount; i++) {
		if (ownsPointers[i] && Input::pointers[i].pressed) {
			pressed = true;
			pressedPointerIndex = i;
			clickedPointerIndex = i;
		}
	}

	return pressed;
}


bool JObject::isHovered() {
	float x, y, h, w;
	getUnmaskedBounds(&x, &y, &h, &w);
	return Input::isAreaHovered(x, y, w, h);
}


bool JObject::isDown() {
	float x, y, h, w;
	getUnmaskedBounds(&x, &y, &h, &w);
	return Input::isAreaDown(x, y, h, w);
}


bool JObject::isClicked( bool consumePress ) {
	isPressed();
	consumePointers();
	// TODO: Je/ik was bezig met isPressed en co. te verbeteren. Fix nu deze nieuwe methode voor interface dingen

	bool clicked = false;
	Vector2 unmaskedPos, unmaskedSize;
	getUnmaskedBounds(&unmaskedPos.x, &unmaskedPos.y, &unmaskedSize.x, &unmaskedSize.y);

	for (int i = 0; i < Input::pointerAmount; i++) {
		InputPointer* pointer = &Input::pointers[i];
		if (ownsPointers[i]) {
			if (pointer->released) {
				if (Input::screenToRender(pointer->releasedPosition).isInside(unmaskedPos, unmaskedSize)) {
					clicked = true;
				}
			}
		}
	}

	/*
	bool clicked = false;
	if (clickedPointerIndex != -1) {
		if (consumePress) {
			pressedPointerIndex = -1;
		}
		if (Input::pointers[clickedPointerIndex].released) {
			float rpposx = Input::pointers[clickedPointerIndex].releasedPosition.x;
			float rpposy = Input::pointers[clickedPointerIndex].releasedPosition.y;
			if (Input::isScreenPosInRenderRect(rpposx, rpposy, getPosX(), getPosY(), getWidth(), getHeight())) {
				clicked = true;
				clickedPointerIndex = -1;
			}
		}
	}*/
	return clicked;
}


bool JObject::isBeingClicked() {
	consumePointers();

	bool beingClicked = false;
	Vector2 unmaskedPos, unmaskedSize;
	getUnmaskedBounds(&unmaskedPos.x, &unmaskedPos.y, &unmaskedSize.x, &unmaskedSize.y);

	for (int i = 0; i < Input::pointerAmount; i++) {
		InputPointer* pointer = &Input::pointers[i];
		if (ownsPointers[i]) {
			if (pointer->down) {
				if (Input::screenToRender(pointer->position).isInside(unmaskedPos, unmaskedSize)) {
					beingClicked = true;
				}
			}
		}
	}

	return beingClicked;
}



void JObject::addChild( JObject& child, int index ) {
	// Add child if not already added before.
	if (child.parent != this) {
		child.parent = this;
		child.localPosX -= this->getPosX();
		child.localPosY -= this->getPosY();
		child.setLocalPosZ(child.localPosZ);

		if (index == -1) {
			children.push_back(&child);
		}
		else {
			children.insert(children.begin() + index, &child);
		}
	}
}


void JObject::setParent( JObject* parent ) {
	// If object has parent, remove this object from this parents children vector.
	if (this->parent != NULL) {
		for (unsigned int i = 0; i < parent->children.size(); i++) {
			if (parent->children[i] == this) {
				parent->children.erase(parent->children.begin() + i);
				break;
			}
		}
	}

	// Add child to (new) parent.
	if (parent != NULL) {
		this->localPosX -= parent->getPosX();
		this->localPosY -= parent->getPosY();
		parent->children.push_back(this);
	}
	this->parent = parent;
}


