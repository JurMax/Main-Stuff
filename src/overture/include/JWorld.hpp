/*
 * JWorld.hpp
 *
 *  Created on: 5 jul. 2017
 *      Author: jurriaanvandenberg
 */

#ifndef OVERTURE_SRC_JWORLD_HPP_
#define OVERTURE_SRC_JWORLD_HPP_

#include <string>

#include <JResources.hpp>
#include <JObject.hpp>
#include <JTypes.hpp>


namespace Overture {
	void World_Close();
}


#define TILE_SIZE 16

enum TileType {
	TILE_UNDEFINED = 42,  // Type will be based on surrounding tiles.

	TILE_ISOLATED = 0,
	TILE_LEFT = 1,
	TILE_UP = 2,
	TILE_RIGHT = -1,  // = left flipped
	TILE_DOWN = 3,

	TILE_LEFT_UP = 4,
	TILE_UP_RIGHT = -4,  // = left up flipped
	TILE_RIGHT_DOWN = -5, // = down left flipped
	TILE_DOWN_LEFT = 5,
	TILE_LEFT_RIGHT = 6,
	TILE_UP_DOWN = 7,

	TILE_LEFT_UP_RIGHT = 8,
	TILE_UP_RIGHT_DOWN = -10,  // = down left up flipped
	TILE_RIGHT_DOWN_LEFT = 9,
	TILE_DOWN_LEFT_UP = 10,

	TILE_MIDDLE = 11,
	TILE_MISC_1 = 12,
	TILE_MISC_2 = 13,
	TILE_MISC_3 = 14,

	// Inside Corners (IC) extra sprites.
	TILE_UP_LEFT_IC_1 = 16,
	TILE_RIGHT_UP_IC_2 = -16,
	TILE_DOWN_LEFT_IC_3 = 17,
	TILE_RIGHT_DOWN_IC_4 = -17,

	TILE_LEFT_UP_RIGHT_IC_1 = 18,
	TILE_LEFT_UP_RIGHT_IC_2 = -18,
	TILE_LEFT_UP_RIGHT_IC_1_2 = 19,

	TILE_RIGHT_DOWN_LEFT_IC_3 = 20,
	TILE_RIGHT_DOWN_LEFT_IC_4 = -20,
	TILE_RIGHT_DOWN_LEFT_IC_3_4 = 21,

	TILE_DOWN_LEFT_UP_IC_1 = 22,
	TILE_DOWN_LEFT_UP_IC_3 = 23,
	TILE_DOWN_LEFT_UP_IC_1_3 = 24,

	TILE_UP_RIGHT_DOWN_IC_2 = -22,
	TILE_UP_RIGHT_DOWN_IC_4 = -23,
	TILE_UP_RIGHT_DOWN_IC_2_4 = -24,

	TILE_MIDDLE_IC_1 = 25,
	TILE_MIDDLE_IC_2 = -25,
	TILE_MIDDLE_IC_3 = 26,
	TILE_MIDDLE_IC_4 = -26,

	TILE_MIDDLE_IC_1_2 = 27,
	TILE_MIDDLE_IC_3_4 = 28,
	TILE_MIDDLE_IC_1_3 = 29,
	TILE_MIDDLE_IC_2_4 = -29,
	TILE_MIDDLE_IC_1_4 = 30,
	TILE_MIDDLE_IC_2_3 = -30,

	TILE_MIDDLE_IC_1_2_3 = 31,
	TILE_MIDDLE_IC_1_2_4 = -31,
	TILE_MIDDLE_IC_2_3_4 = 32,
	TILE_MIDDLE_IC_1_3_4 = -32,

	TILE_MIDDLE_IC_1_2_3_4 = 33,
};

struct TileInfo;


namespace World {

	extern std::vector<TileInfo> tileInfo;
	// Angle in degrees to determine if a slant is a wall or a floor (related to hitboxes).
	extern float wallAngle;

	void createTile( JTexture* texture, std::string name, std::string material, bool insidecorners = false );

	// Use TILE_UNDEFINED for type to set default hitbox.
	void setTileHitbox( JTexture* texture, TileType type, float p0x, float p0y, float p1x, float p1y, float p2x, float p2y, float p3x, float p3y );
	void setTileHitbox( JTexture* texture, TileType type, float pointsX, float pointsY, float pointsWidth, float pointsHeight );
	void setTileHitboxFull( JTexture* texture, TileType type = TILE_UNDEFINED );

}



struct TileInfo {
	JTexture* texture;
	bool insideCorners;  // Has special sprites for inside corners.

	std::string name;
	std::string material;

	// Pointers and vector to save memory space, just because I can. Also hitbox 0 is default, tiles start at 1.
	std::vector<Quadrilateral*> hitboxQuads;
};


struct TileData {
	int tileIndex;
	TileType tileType;
};


struct JOTileMap : JOComponent {
	JOTileMap();

	virtual void update();
	virtual void render();

	int getTileX( float posX );
	int getTileY( float posY );

	bool tileExists( int tileX, int tileY );
	Quadrilateral getHitbox( int tileX, int tileY );

	void setTile( JTexture* texture, int tileX, int tileY, TileType type = TILE_UNDEFINED );
	void connectTile( int tileX, int tileY, bool connectNeighbors = true );

	std::vector<std::vector<TileData> > tileData;
};

#endif /* OVERTURE_SRC_JWORLD_HPP_ */
