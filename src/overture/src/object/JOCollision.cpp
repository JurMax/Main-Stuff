/*
 * JOCollision.cpp
 *
 *  Created on: 10 jul. 2017
 *      Author: jurriaanvandenberg
 */

#include <JOCollision.hpp>

#include <cmath>
#include <JOverture.hpp>
#include <JTypes.hpp>
#include <JOInterface.hpp>


/**
 * TODO (but only when necesarry):
 *   Slanted hitboxes (tilemap).
 *   Rotation.
 *	 Some other stuff probably idontknow.
 */

std::vector<JObject*> collisionObjects;
JOTileMap* activeTileMap;

namespace Collision {

	void setActiveTileMap( JOTileMap* tilemap ) {
		activeTileMap = tilemap;
	}


	void resolveCollisions() {
		for (unsigned int i = 0; i < collisionObjects.size(); i++) {
			JObject* object = collisionObjects[i];
			JOCollider* objectCollider = object->getComponent<JOCollider>();

			bool checkCollision = true;

			JOResizer* resizer = object->getComponent<JOResizer>();
			if (resizer != NULL && resizer->isBeingDragged()) {
				checkCollision = false;
			}

			if (checkCollision && objectCollider != NULL) {

				resolveWithTileMapX(object, objectCollider, true);
				resolveWithObjectsX(object, objectCollider, i);

				resolveWithTileMapY(object, objectCollider, false);
				resolveWithObjectsY(object, objectCollider, i);
			}
		}

		for (unsigned int i = 0; i < collisionObjects.size(); i++) {
			JObject* object = collisionObjects[i];
			JOCollider* objectCollider = object->getComponent<JOCollider>();

			if (objectCollider != NULL) {
				objectCollider->prevPosX = object->getPosX();
				objectCollider->prevPosY = object->getPosY();
			}
		}

		collisionObjects.clear();
	}



	void resolveWithObjectsX( JObject* object, JOCollider* objectCollider, int objectIndex ) {
		for (unsigned int i2 = objectIndex + 1; i2 < collisionObjects.size(); i2++) {
			JObject* otherObject = collisionObjects[i2];
			JOCollider* otherObjectCollider = otherObject->getComponent<JOCollider>();

			if (otherObjectCollider != NULL) {
				bool checkCollision = true;

				// Don't check collision if one of the objects ignores the other.
				for (unsigned int i3 = 0; i3 < objectCollider->ignoredObjects.size(); i3++) {
					if (objectCollider->ignoredObjects[i3] == otherObject) {
						checkCollision = false;
					}
				}
				for (unsigned int i3 = 0; i3 < otherObjectCollider->ignoredObjects.size(); i3++) {
					if (otherObjectCollider->ignoredObjects[i3] == object) {
						checkCollision = false;
					}
				}

				if (checkCollision) {
					bool collidedX = false;
					bool collidedY = objectCollider->prevPosY + object->getHeight() > otherObjectCollider->prevPosY && objectCollider->prevPosY < otherObjectCollider->prevPosY + otherObject->getHeight();

					// Check collision for the X axis.
					if (collidedY) {
						if (object->getPosX() - objectCollider->prevPosX > otherObject->getPosX() - otherObjectCollider->prevPosX) {
							// The object moves to the right relative to the other object.

							collidedX = object->getPosX() - otherObject->getPosX() + object->getWidth() > 0 && objectCollider->prevPosX - otherObjectCollider->prevPosX < otherObject->getWidth();

							if (collidedX) {
								objectCollider->hasCollidedRight = true;
								otherObjectCollider->hasCollidedLeft = true;

								if (otherObjectCollider->pushable && objectCollider->pushable) {
									//TODO: Use weight to find a average.
								}
								else if (objectCollider->pushable) {
									object->setPosX(otherObject->getPosX() - object->getWidth());

								}
								else if (otherObjectCollider->pushable) {
									otherObject->setPosX(object->getPosX() + object->getWidth());
								}
							}
						}
						else if (object->getPosX() - objectCollider->prevPosX < otherObject->getPosX() - otherObjectCollider->prevPosX) {
							// The object moves to the left relative to the other object.

							collidedX = object->getPosX() - otherObject->getPosX() < otherObject->getWidth() && objectCollider->prevPosX - otherObjectCollider->prevPosX + object->getWidth() > 0;

							if (collidedX) {
								objectCollider->hasCollidedLeft = true;
								otherObjectCollider->hasCollidedRight = true;

								if (otherObjectCollider->pushable && objectCollider->pushable) {
									//TODO: Use weight to find a average.
								}
								else if (objectCollider->pushable) {
									object->setPosX(otherObject->getPosX() + otherObject->getWidth());
								}
								else if (otherObjectCollider->pushable) {
									otherObject->setPosX(object->getPosX() - otherObject->getWidth());
								}
							}
						}
					}
				}
			}
		}
	}


	void resolveWithObjectsY( JObject* object, JOCollider* objectCollider, int objectIndex ) {
		for (unsigned int i2 = objectIndex + 1; i2 < collisionObjects.size(); i2++) {
			JObject* otherObject = collisionObjects[i2];
			JOCollider* otherObjectCollider = otherObject->getComponent<JOCollider>();

			if (otherObjectCollider != NULL) {
				bool checkCollision = true;

				// Don't check collision if one of the objects ignores the other.
				for (unsigned int i3 = 0; i3 < objectCollider->ignoredObjects.size(); i3++) {
					if (objectCollider->ignoredObjects[i3] == otherObject) {
						checkCollision = false;
					}
				}
				for (unsigned int i3 = 0; i3 < otherObjectCollider->ignoredObjects.size(); i3++) {
					if (otherObjectCollider->ignoredObjects[i3] == object) {
						checkCollision = false;
					}
				}

				if (checkCollision) {
					// Check collision for the Y-axis.
					bool collidedX = object->getPosX() + object->getWidth() > otherObject->getPosX() && object->getPosX() < otherObject->getPosX() + otherObject->getWidth();
					bool collidedY = false;

					if (collidedX) {
						if (object->getPosY() - objectCollider->prevPosY >= otherObject->getPosY() - otherObjectCollider->prevPosY) {
							// Object moves down relative to the other object.

							collidedY = object->getPosY() - otherObject->getPosY() + object->getHeight() > 0 && objectCollider->prevPosY - otherObjectCollider->prevPosY < otherObject->getHeight();

							if (collidedY) {
								objectCollider->hasCollidedBottom = true;
								otherObjectCollider->hasCollidedTop = true;

								if (otherObjectCollider->pushable && objectCollider->pushable) {
									//TODO: Use weight to find a average.
								}
								else if (objectCollider->pushable) {
									object->setPosY(otherObject->getPosY() - object->getHeight());
								}
								else if (otherObjectCollider->pushable) {
									otherObject->setPosY(object->getPosY() + object->getHeight());
								}
							}
						}
						else {
							// Object moves up relative to the other object.

							collidedY = object->getPosY() - otherObject->getPosY() < otherObject->getHeight() && objectCollider->prevPosY - otherObjectCollider->prevPosY + object->getHeight() > 0;

							if (collidedY) {
								objectCollider->hasCollidedTop= true;
								otherObjectCollider->hasCollidedBottom = true;

								if (otherObjectCollider->pushable && objectCollider->pushable) {
									//TODO: Use weight to find a average.
								}
								else if (objectCollider->pushable) {
									object->setPosY(otherObject->getPosY() + otherObject->getHeight());
								}
								else if (otherObjectCollider->pushable) {
									otherObject->setPosY(object->getPosY() - otherObject->getHeight());
								}
							}
						}
					}
				}
			}
		}
	}



	void resolveWithTileMapX( JObject* object, JOCollider* objectCollider, bool usePrevY ) {
		if (activeTileMap != NULL && objectCollider->pushable) {
			JOTileMap* tmap = activeTileMap;  // Name shortened for convenience;

			int objectTileWidth = (int) ceilf(object->getWidth() / (float) TILE_SIZE);
			int objectTileHeight = (int) ceilf(object->getHeight() / (float) TILE_SIZE);

			int tileX = activeTileMap->getTileX(objectCollider->prevPosX); // Start at previous X position
			bool leftToRight = object->getPosX() >= objectCollider->prevPosX;  // Check tiles to the right or to the left from starting position (= tileX).

			bool checkingTiles = true;
			while (checkingTiles) {
				for (int i2 = 0; i2 <= objectTileHeight; i2++) {
					int tileY = activeTileMap->getTileY(objectCollider->prevPosY) + i2;
					Quadrilateral quad = tmap->getHitbox(tileX, tileY);

					if (quad.hasValues()) {
						Vector2 offset = Vector2(tmap->object->getPosX() + tileX * TILE_SIZE, tmap->object->getPosY() + tileY * TILE_SIZE);
						Vector2 vectorArray[] = { quad.p0 + offset, quad.p1 + offset, quad.p2 + offset, quad.p3 + offset };

						if (resolveWithPolygonX(object, objectCollider, vectorArray, 4, usePrevY)) {
							// Collided, so no need to check more colums after this one.
							checkingTiles = false;
						}
					}
				}

				if (leftToRight) {
					tileX++;
					if (tileX > activeTileMap->getTileX(object->getPosX()) + objectTileWidth) {
						// Tiles after this are guaranteed not able to collide with this object.
						checkingTiles = false;
					}
				}
				else {
					tileX--;
					if (tileX < activeTileMap->getTileX(object->getPosX())) {
						// Tiles before this are guaranteed not able to collide with this object.
						checkingTiles = false;
					}
				}
			}
		}
	}


	void resolveWithTileMapY( JObject* object, JOCollider* objectCollider, bool usePrevX ) {
		if (activeTileMap != NULL && objectCollider->pushable) {
			JOTileMap* tmap = activeTileMap;  // Name shortened for convenience;

			int objectTileWidth = (int) ceilf(object->getWidth() / (float) TILE_SIZE);
			int objectTileHeight = (int) ceilf(object->getHeight() / (float) TILE_SIZE);

			int tileY = activeTileMap->getTileY(objectCollider->prevPosY);  // Start at previous Y position
			bool topToBottom = object->getPosY() >= objectCollider->prevPosY;  // Check tiles from bottom or to top from starting position (= tileY).

			bool checkingTiles = true;
			while (checkingTiles) {
				for (int i2 = 0; i2 <= objectTileWidth; i2++) {
					int tileX = activeTileMap->getTileX(object->getPosX()) + i2;
					Quadrilateral quad = tmap->getHitbox(tileX, tileY);

					if (quad.hasValues()) {
						Vector2 offset = Vector2(tmap->object->getPosX() + tileX * TILE_SIZE, tmap->object->getPosY() + tileY * TILE_SIZE);
						Vector2 vectorArray[] = { quad.p0 + offset, quad.p1 + offset, quad.p2 + offset, quad.p3 + offset };

						if (resolveWithPolygonY(object, objectCollider, vectorArray, 4, usePrevX)) {
							// Collided, so no need to check more rows after this one.
							checkingTiles = false;
						}
					}
				}

				if (topToBottom) {
					tileY++;
					if (tileY > activeTileMap->getTileY(object->getPosY()) + objectTileHeight) {
						// Tiles after this are guaranteed not able to collide with this object.
						checkingTiles = false;
					}
				}
				else {
					tileY--;
					if (tileY < activeTileMap->getTileY(object->getPosY())) {
						// Tiles before this are guaranteed not able to collide with this object.
						checkingTiles = false;
					}
				}
			}
		}
	}



	bool resolveWithPolygonX( JObject* object, JOCollider* objectCollider, Vector2 vectorList[], int listLength, bool usePrevY ) {
		bool collidedX = false;

		for (int i = 0; i < listLength; i++) {

			// Find index for the next point (vector2 at i2), with which this point (vector2 at i) forms a line.
			int i2 = i + 1;
			if (i2 == listLength) {
				i2 = 0;
			}

			if (resolveWithLineX2(object, objectCollider, vectorList[i], vectorList[i2], usePrevY)) {
				collidedX = true;
			}
		}

		return collidedX;
	}


	bool resolveWithPolygonY( JObject* object, JOCollider* objectCollider, Vector2 vectorList[], int listLength, bool usePrevX ) {
		bool collidedY = false;

		for (int i = 0; i < listLength; i++) {

			// Find index for the next point (vector2 at i2), with which this point (vector2 at i) forms a line.
			int i2 = i + 1;
			if (i2 == listLength) {
				i2 = 0;
			}

			if (resolveWithLineY2(object, objectCollider, vectorList[i], vectorList[i2], usePrevX)) {
				collidedY = true;
			}
		}

		return collidedY;
	}



	bool resolveCollisionWithLineX( JObject* object, JOCollider* objectCollider, Vector2 v1, Vector2 v2 ) {
		bool collidedX = false;

		float angle = (v2 - v1).angle();
		bool isWall = fabs(angle) >= World::wallAngle && fabs(angle) <= 180.0f - World::wallAngle;

		if (isWall) {

			bool collidedY = false;

			float minX = std::numeric_limits<float>::max();
			float maxX = -std::numeric_limits<float>::max();

			// Gather data (x coördinates) //

			if (objectCollider->prevPosY > std::min(v1.y, v2.y) && objectCollider->prevPosY < std::max(v1.y, v2.y)) {
				// Collided Y at top.

				float xAtTop = v1.extrapolateX(v2, objectCollider->prevPosY);

				if (!std::isinf(xAtTop)) {
					collidedY = true;

					if (xAtTop < minX) {
						minX = xAtTop;
					}
					if (xAtTop > maxX) {
						maxX = xAtTop;
					}
				}
			}

			if (objectCollider->prevPosY + object->getHeight() > std::min(v1.y, v2.y)
				&& objectCollider->prevPosY + object->getHeight() < std::max(v1.y, v2.y)) {
				// Collided Y at Bottom

				float xAtBottom = v1.extrapolateX(v2, objectCollider->prevPosY);

				if (!std::isinf(xAtBottom)) {
					collidedY = true;

					if (xAtBottom < minX) {
						minX = xAtBottom;
					}
					if (xAtBottom > maxX) {
						maxX = xAtBottom;
					}
				}
			}

			if (v1.y >= objectCollider->prevPosY && v1.y < objectCollider->prevPosY + object->getHeight()) {

				collidedY = true;
				float xInMiddle1 = v1.x;

				if (xInMiddle1 < minX) {
					minX = xInMiddle1;
				}
				if (xInMiddle1 > maxX) {
					maxX = xInMiddle1;
				}
			}

			if (v2.y >= objectCollider->prevPosY && v2.y < objectCollider->prevPosY + object->getHeight()) {

				collidedY = true;
				float xInMiddle2 = v2.x;

				if (xInMiddle2 < minX) {
					minX = xInMiddle2;
				}
				if (xInMiddle2 > maxX) {
					maxX = xInMiddle2;
				}
			}


			// Move the object (if collided) //

			if (object->getPosX() > objectCollider->prevPosX && collidedY) {
				// Objects moves to the right.

				collidedX = objectCollider->prevPosX < minX && object->getPosX() + object->getWidth() > minX;
				if (collidedX) {
					object->setPosX(minX - object->getWidth());
				}
			}
			else if (object->getPosX() < objectCollider->prevPosX && collidedY) {
				// Object moves to the left.

				collidedX = object->getPosX() < maxX && objectCollider->prevPosX + object->getWidth() > maxX;

				if (collidedX) {
					object->setPosX(maxX);
				}
			}


			bool stillCollidedX = object->getPosX() < maxX && object->getPosX() + object->getWidth() > minX;

			if (stillCollidedX && collidedY) {
				// Object didn't move along the x-axis so the collision hasn't been resolved.

				float offsetToLeft = minX - (object->getPosX() + object->getWidth());
				float offsetToRight = maxX - object->getPosX();

				if (fabs(offsetToLeft) <= fabs(offsetToRight)) {
					// The offset to the left is smaller than the offset to the right, so move the object left.
					object->setPosX(minX - object->getWidth());

				}
				else {
					// The offset to the right is smaller than the offset to the left, so move the object right.
					object->setPosX(maxX);
				}

				collidedX = true;
			}
		}

		return collidedX;
	}


	bool resolveCollisionWithLineY( JObject* object, JOCollider* objectCollider, Vector2 v1, Vector2 v2 ) {
		bool collidedY = false;

		float angle = (v2 - v1).angle();
		bool isntWall = fabs(angle) >= World::wallAngle && fabs(angle) <= 180.0f - World::wallAngle;

		if (!isntWall) {

			bool collidedX = false;

			float minY = std::numeric_limits<float>::max();
			float maxY = -std::numeric_limits<float>::max();

			// Gather data (y coördinates) //

			if (object->getPosX() > std::min(v1.x, v2.x) && object->getPosX() < std::max(v1.x, v2.x)) {
				// Collided X at left.

				float yAtLeft = v1.extrapolateY(v2, object->getPosX());

				if (!std::isinf(yAtLeft)) {
					collidedX = true;

					if (yAtLeft < minY) {
						minY = yAtLeft;
					}
					if (yAtLeft > maxY) {
						maxY = yAtLeft;
					}
				}
			}

			if (object->getPosX() + object->getWidth() > std::min(v1.x, v2.x)
				&& object->getPosX() + object->getWidth() < std::max(v1.x, v2.x)) {
				// Collided X at right.

				float yAtRight = v1.extrapolateY(v2, object->getPosX());

				if (!std::isinf(yAtRight)) {
					collidedX = true;

					if (yAtRight < minY) {
						minY = yAtRight;
					}
					if (yAtRight > maxY) {
						maxY = yAtRight;
					}
				}
			}

			if (v1.x >= object->getPosX() && v1.x < object->getPosX() + object->getWidth()) {

				collidedX = true;
				float yInMiddle1 = v1.y;

				if (yInMiddle1 < minY) {
					minY = yInMiddle1;
				}
				if (yInMiddle1 > maxY) {
					maxY = yInMiddle1;
				}
			}

			if (v2.x >= object->getPosX() && v2.x < object->getPosX() + object->getWidth()) {

				collidedX = true;
				float yInMiddle2 = v2.y;

				if (yInMiddle2 < minY) {
					minY = yInMiddle2;
				}
				if (yInMiddle2 > maxY) {
					maxY = yInMiddle2;
				}
			}


			// Move the object (if collided) //

			if (object->getPosY() > objectCollider->prevPosY && collidedX) {
				// Objects moves to the bottom.

				collidedY = objectCollider->prevPosY < minY && object->getPosY() + object->getHeight() > minY;

				if (collidedY) {
					object->setPosY(minY - object->getHeight());
				}
			}
			else if (object->getPosY() < objectCollider->prevPosY && collidedX) {
				// Object moves to the top.

				collidedY = object->getPosY() < maxY && objectCollider->prevPosY + object->getHeight() > maxY;


				if (collidedY) {
					object->setPosY(maxY);
				}
			}


			bool stillCollidedY = object->getPosY() < maxY && object->getPosY() + object->getHeight() > minY;

			if (stillCollidedY && collidedX) {
				// Object didn't move along the y-axis so the collision hasn't been resolved.

				float offsetToTop = minY - (object->getPosY() + object->getHeight());
				float offsetToBottom = maxY - object->getPosY();

				if (fabs(offsetToTop) <= fabs(offsetToBottom)) {
					// The offset to the top is smaller than the offset to the bottom, so move the object top.
					object->setPosY(minY - object->getHeight());

				}
				else {
					// The offset to the bottom is smaller than the offset to the top, so move the object bottom.
					object->setPosY(maxY);
				}

				collidedY = true;
			}
		}

		return collidedY;
	}



	bool resolveWithLineX2( JObject* object, JOCollider* objectCollider, Vector2 v1, Vector2 v2, bool usePrevY ) {
		bool movedX = false;

		float angle = (v2 - v1).angle();
		float objectPosY;

		if (usePrevY) {
			objectPosY = objectCollider->prevPosY;
		}
		else {
			objectPosY = object->getPosY();
		}


		if (angle >= World::wallAngle && angle <= 180.0f - World::wallAngle) {
			// This line is a wall that points left.

			float wallX = NAF;

			if (angle > 90.0f) {
				// Wall leans left.
				if (objectPosY >= v2.y && objectPosY < v1.y) {
					// Top of object touches wall.
					wallX = v1.extrapolateX(v2, objectPosY);
				}
				else if (v2.y > objectPosY && v2.y <= objectPosY + object->getHeight()) {
					// Top of wall touches the object.
					wallX = v2.x;
				}
			}
			else if (angle < 90.0f) {
				// Wall leans right.
				if (objectPosY + object->getHeight() > v2.y && objectPosY + object->getHeight() < v1.y) {
					// Bottom of object touches wall.
					wallX = v1.extrapolateX(v2, objectPosY + object->getHeight());
				}
				else if (v1.y > objectPosY && v1.y <= objectPosY + object->getHeight()) {
					// Bottom of wall touches the object.
					wallX = v1.x;
				}
			}
			else {
				// Wall stands perpendicular to the x-axis.
				if (objectPosY + object->getHeight() > v2.y && objectPosY < v1.y) {
					// Object touches wall.
					wallX = v1.x; // = v2.x
				}
			}

			bool collidedX = objectCollider->prevPosX + object->getWidth() <= wallX && object->getPosX() + object->getWidth() > wallX;

			if (collidedX && wallX != NAF) {
				object->setPosX(wallX - object->getWidth());
				movedX = true;
			}
		}
		else if (angle >= 180.0f + World::wallAngle && angle <= 360.0f - World::wallAngle) {
			// This line is a wall that points right.

			float wallX = NAF;

			if (angle > 270.0f) {
				// Wall leans left.
				if (objectPosY + object->getHeight() > v1.y && objectPosY + object->getHeight() < v2.y) {
					// Bottom of object touches wall.
					wallX = v1.extrapolateX(v2, objectPosY + object->getHeight());
				}
				else if (v2.y > objectPosY && v2.y <= objectPosY + object->getHeight()) {
					// Bottom of wall touches the object.
					wallX = v2.x;
				}
			}
			else if (angle < 270.0f) {
				// Wall leans right.
				if (objectPosY >= v1.y && objectPosY < v2.y) {
					// Top of object touches wall.
					wallX = v1.extrapolateX(v2, objectPosY);
				}
				else if (v1.y > objectPosY && v1.y <= objectPosY + object->getHeight()) {
					// Top of wall touches the object.
					wallX = v1.x;
				}
			}
			else {
				// Wall stands perpendicular to the x-axis.
				if (objectPosY + object->getHeight() > v1.y && objectPosY < v2.y) {
					// Object touches wall.
					wallX = v1.x; // = v2.x
				}
			}

			bool collidedX = objectCollider->prevPosX >= wallX && object->getPosX() < wallX;

			if (collidedX && wallX != NAF) {
				object->setPosX(wallX);
				movedX = true;
			}
		}
		else {
			// Line is floor or ceiling, so slide up/down.
		}

		return movedX;
	}

	

	bool resolveWithLineY2( JObject* object, JOCollider* objectCollider, Vector2 v1, Vector2 v2, bool usePrevX ) {
		bool movedY = false;

		float angle = (v2 - v1).angle();
		float objectPosX;

		if (usePrevX) {
			objectPosX = objectCollider->prevPosX;
		}
		else {
			objectPosX = object->getPosX();
		}

		if (angle <= World::wallAngle || angle >= 360.0f - World::wallAngle) {
			// This line is a floor.

			float floorY = NAF;

			bool floorLeansRight = angle < 360.0f && angle >= 360.0f - World::wallAngle;
			bool floorLeansLeft = angle > 0.0f && angle <= World::wallAngle;

			if (floorLeansRight) {
				// Floor leans right (rotated clockwise).
				if (objectPosX >= v1.x && objectPosX < v2.x) {
					// Left of object touches floor.
					floorY = v1.extrapolateY(v2, objectPosX);
				}
				else if (v1.x > objectPosX && v1.x <= objectPosX + object->getWidth()) {
					// Top left of floor touches the object.
					floorY = v1.y;
				}
			}
			else if (floorLeansLeft) {
				// Floor leans left (rotated counter-clockwise).
				if (objectPosX + object->getWidth() > v1.x && objectPosX + object->getWidth() < v2.x) {
					// Right of object touches floor.
					floorY = v1.extrapolateY(v2, objectPosX + object->getWidth());
				}
				else if (v2.x > objectPosX && v2.y <= objectPosX + object->getWidth()) {
					// Top right of floor touches the object.
					floorY = v2.y;
				}
			}
			else {
				// Floor lies perpendicular to the y-axis.
				if (objectPosX + object->getWidth() > v1.x && objectPosX < v2.x) {
					// Object touches floor.
					floorY = v1.y; // = v2.y
				}
			}

			bool collidedY = objectCollider->prevPosY + object->getHeight() <= floorY && object->getPosY() + object->getHeight() > floorY;

			if (collidedY && floorY != NAF) {
				object->setPosY(floorY - object->getHeight());
				movedY = true;
			}


			// Move/slide object alongside a slanted floor.
			if (object->getPosX() > objectCollider->prevPosX) {
				// Object moved to the right.

				if (floorLeansLeft) {
					float floorX = v1.extrapolateX(v2, object->getPosY() + object->getHeight());

					if (floorX > objectCollider->prevPosX && floorX < object->getPosX()) {
						float floorY = v1.extrapolateY(v2, object->getPosX() + object->getWidth());
						object->setPosY(floorY - object->getHeight());
					}
				}
			}
			else if (object->getPosX() < objectCollider->prevPosX) {
				// Object moved to the left.
				if (floorLeansRight) {
					float floorX = v1.extrapolateX(v2, object->getPosY() + object->getHeight());

					if (floorX > object->getPosX() && floorX <= objectCollider->prevPosX) {
						float floorY = v1.extrapolateY(v2, object->getPosX());


						object->setPosY(floorY - object->getHeight());
					}
				}
			}
		}
		else if (angle >= 180.0f - World::wallAngle && angle <= 180.0f + World::wallAngle) {
			// This line is a ceiling.

			float ceilingY = NAF;
			bool ceilingLeansRight = angle < 180.0f;
			bool ceilingLeansLeft = angle > 180.0f;

			if (ceilingLeansRight) {
				// Ceiling leans right (rotated clockwise).
				if (objectPosX + object->getWidth() > v2.x && objectPosX + object->getWidth() < v1.x) {
					// Top right of object touches ceiling.
					ceilingY = v1.extrapolateY(v2, objectPosX + object->getWidth());
				}
				else if (v1.x > objectPosX && v1.x <= objectPosX + object->getWidth()) {
					// Bottom right of ceiling touches the object.
					ceilingY = v1.x;
				}
			}
			else if (ceilingLeansLeft) {
				// Ceiling leans left (rotated counter-clockwise).
				if (objectPosX >= v2.x && objectPosX < v1.x) {
					// Top left of object touches ceiling.
					ceilingY = v1.extrapolateY(v2, objectPosX);
				}
				else if (v2.x > objectPosX && v2.x <= objectPosX + object->getHeight()) {
					// Bottom left of ceiling touches the object.
					ceilingY = v2.y;
				}
			}
			else {
				// Ceiling lies perpendicular to the y-axis.
				if (objectPosX + object->getWidth() > v2.x && objectPosX < v1.x) {
					// Object touches ceiling.
					ceilingY = v1.y; // = v2.y
				}
			}

			bool collidedY = objectCollider->prevPosY >= ceilingY && object->getPosY() < ceilingY;

			if (collidedY && ceilingY != NAF) {
				object->setPosY(ceilingY);
				movedY = true;
			}
		}

		return movedY;
	}

}



JOCollider::JOCollider( bool pushable, float weight ) {
	this->pushable = pushable;
	this->weight = weight;
	prevPosX = 0.0f;
	prevPosY = 0.0f;
	teleported = true;

	hasCollidedLeft = false;
	hasCollidedRight = false;
	hasCollidedTop = false;
	hasCollidedBottom = false;
}


void JOCollider::update() {
	if (teleported) {
		prevPosX = object->getPosX();
		prevPosY = object->getPosY();
		teleported = false;
	}
	hasCollidedLeft = false;
	hasCollidedRight = false;
	hasCollidedTop = false;
	hasCollidedBottom = false;
	collisionObjects.push_back(object);
}


void JOCollider::ignore( JObject* otherobject ) {
	bool alreadyAdded = false;
	for (int i = 0; i < ignoredObjects.size(); i++) {
		if (otherobject == ignoredObjects[i]) {
			alreadyAdded = true;
		}
	}
	if (!alreadyAdded) {
		ignoredObjects.push_back(otherobject);
	}
}


float JOCollider::getPrevPos( int axis ) {
	if (axis == 0 ) {
		return prevPosX;
	}
	else {
		return prevPosY;
	}
}




JOMotion::JOMotion( float weight ) {
	this->weight = weight;

	motionX = 0.0f;
	motionY = 0.0f;

	frictionX = 40.0f;
	frictionY = 40.0f;
}


void JOMotion::update() {

	for (int i = forces.size() - 1; i >= 0; i--) {

		motionX += (forces[i].strength * cos(forces[i].direction / 180.0f * M_PI)) / weight * dt();
		motionY += (forces[i].strength * sin(forces[i].direction / 180.0f * M_PI)) / weight * dt();

		if (!forces[i].permanent) {
			forces.erase(forces.begin() + i);
		}
	}

	if (motionX > 0.0f) {
		motionX -= frictionX * dt();
		if (motionX < 0.0f) {
			motionX = 0.0f;
		}
	}
	else if (motionX < 0.0f) {
		motionX += frictionX * dt();
		if (motionX > 0.0f) {
			motionX = 0.0f;
		}
	}

	if (motionY > 0.0f) {
		motionY -= frictionY * dt();
		if (motionY < 0.0f) {
			motionY = 0.0f;
		}
	}
	else if (motionY < 0.0f) {
		motionY += frictionY * dt();
		if (motionY > 0.0f) {
			motionY = 0.0f;
		}
	}


	JOResizer* resizer = object->getComponent<JOResizer>();
	if (resizer != NULL && resizer->isBeingDragged()) {
		motionX = 0.0f;
		motionY = 0.0f;
	}

	object->localPosX += motionX * dt();
	object->localPosY += motionY * dt();
}


void JOMotion::addForce( float strength, float direction, bool permanent ) {
	Force force;
	force.strength = strength;
	force.direction = direction;
	force.permanent = permanent;
	forces.push_back(force);
}


