/*
 * JOCollision.hpp
 *
 *  Created on: 10 jul. 2017
 *      Author: jurriaanvandenberg
 */

#ifndef OVERTURE_SRC_OBJECT_JOCOLLISION_HPP_
#define OVERTURE_SRC_OBJECT_JOCOLLISION_HPP_

#include <vector>
#include <JObject.hpp>
#include <JWorld.hpp>

struct JOCollider;

namespace Collision {
	void setActiveTileMap( JOTileMap* tilemap );
	void resolveCollisions();

	void resolveWithObjectsX( JObject* object, JOCollider* objectCollider, int objectIndex );
	void resolveWithObjectsY( JObject* object, JOCollider* objectCollider, int objectIndex );

	void resolveWithTileMapX( JObject* object, JOCollider* objectCollider, bool usePrevY );
	void resolveWithTileMapY( JObject* object, JOCollider* objectCollider, bool usePrevX );

	bool resolveWithPolygonX( JObject* object, JOCollider* objectCollider, Vector2 vectorList[], int listLength, bool usePrevY );
	bool resolveWithPolygonY( JObject* object, JOCollider* objectCollider, Vector2 vectorList[], int listLength, bool usePrevX );

	bool resolveCollisionWithLineX( JObject* object, JOCollider* objectCollider, Vector2 v1, Vector2 v2 );
	bool resolveCollisionWithLineY( JObject* object, JOCollider* objectCollider, Vector2 v1, Vector2 v2 );

	bool resolveWithLineX2( JObject* object, JOCollider* objectCollider, Vector2 v1, Vector2 v2, bool usePrevY );
	bool resolveWithLineY2( JObject* object, JOCollider* objectCollider, Vector2 v1, Vector2 v2, bool usePrevX );

}


struct JOCollider : JOComponent {
	JOCollider( bool pushable = false, float weight = 1.0f );

	virtual void update();
	virtual void render() {}

	void ignore( JObject* otherobject );

	float getPrevPos( int axis );

	bool pushable;
	float weight;

	float prevPosX;
	float prevPosY;
	bool teleported;

	//TODO: rotating stuff (whole gameobject)
	//float rotation;
	//Vector2 rotationCenter;

	bool hasCollidedLeft;
	bool hasCollidedRight;
	bool hasCollidedTop;
	bool hasCollidedBottom;

	std::vector<JObject*> ignoredObjects;
};



struct Force {
	float strength;
	float direction;

	bool permanent;
};

struct JOMotion : JOComponent {
	JOMotion( float weight = 1.0f );

	virtual void update();
	virtual void render() {}

	void addForce( float strength, float direction, bool permanent = false );

	float weight;

	float motionX;
	float motionY;

	std::vector<Force> forces;

	float frictionX;
	float frictionY;
};


#endif /* OVERTURE_SRC_OBJECT_JOCOLLISION_HPP_ */
