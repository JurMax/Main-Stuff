/*
 * JObject.hpp
 *
 *  Created on: 25 apr. 2017
 *      Author: jurriaanvandenberg
 */

#ifndef OVERTURE_INCLUDE_JOBJECT_HPP_
#define OVERTURE_INCLUDE_JOBJECT_HPP_

#include <string>
#include <vector>
#include <SDL.h>
#include <JResources.hpp>
#include <JTypes.hpp>
#include <JInput.hpp>

/**
 * TODO
 * 	Sort by Zindex
 * 	More stuff
 * 	Position in relation to children (localposition)
 * 	Scaling
 * 	Rotation
 * 	Hitboxes/colliders
 * 	Free all gameobjects on close
 */

class JObject;


namespace Overture {
	void Object_Close();
}


namespace JObjects {

	extern std::vector<JObject*> objects;

	void Update();
	void Render();
}




// For individual components, see classes starting with "JO" in same folder (except JOverture).
struct JOComponent {
	virtual ~JOComponent() {}

	virtual void update() = 0;
	virtual void render() = 0;

	JObject* object;
	bool active;
};



class JObject {
public:
	JObject( bool canBeDeleted = false );
	~JObject();


public:
	void init( std::string n = "[no name]" );
	void init( float x, float y, float w, float h, std::string n = "[no name]" );
	void destroy();  // Warning: be very careful when using this!


	void update();
	void render();


	void startParentMask();
	void renderParentMask();
	void getUnmaskedBounds( float* x, float* y, float* w, float* h );


	void setPosX( float newPosX );
	void setPosY( float newPosY );
	void setLocalPosZ( float newLocalPosZ );
	void setSize( float width, float height );

	void setActive( bool active );
	void setPressed( int index );


	float getPosX();
	float getPosY();
	float getPosZ();
	Vector2 getPos();

	float getWidth();
	float getHeight();
	Vector2 getSize();

	bool consumePointers();
	bool isPressed();
	bool isHovered();
	bool isDown();
	bool isClicked( bool consumePress = false );
	bool isBeingClicked();


	void addChild( JObject& child, int index = -1 );
	void setParent( JObject* parent );


public:
	std::string name;
	int tag;

	bool initialised;
	bool active;
	bool destroyable;  // Can this JObject be deleted. Should only be true for pointer-only JObjects.

	float localPosX;
	float localPosY;
	float localPosZ;  // Always use setLocalPosZ(), because this shouldn't be changed without updating JObjects vector.

	float localWidth;
	float localHeight;

	float prevPosX;
	float prevPosY;
	float prevPosZ;
	float prevWidth;
	float prevHeight;

	float minWidth;
	float minHeight;

	bool sizeChanged;
	bool posChanged;

	bool independentRender;
	bool independentUpdate;

	bool isHUD;
	bool isMask;

	bool ownsPointers[Input::pointerAmount];

	int pressedPointerIndex;
	int clickedPointerIndex;

	JObject* parent;
	std::vector<JObject*> children;

	std::vector<JOComponent*> components;

private:
	unsigned int vectorIndex;


public:
	template <class C>
	C* getComponent( int* index = NULL ) {
	    for (int i = 0; i < components.size(); i++) {
			C* component = dynamic_cast<C*>(components[i]);

			// Component is specified class/struct type
			if (component != NULL) {
				if (index != NULL) {
					*index = i;
				}
				component->object = this;
				return component;
			}
		}
	    return NULL;
	}

	template <class C>
	void addComponent( C* component ) {
		if (getComponent<C>() == NULL) {
			component->active = true;
			components.push_back(component);
		}
	}

	template <class C>
	void removeComponent() {
		int index;
		if (getComponent<C>(&index) != NULL) {
			delete components[index];
			components.erase(components.begin() + index);
		}
	}

};


#endif /* OVERTURE_INCLUDE_JOBJECT_HPP_ */
