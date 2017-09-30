/*
 * JWorld.cpp
 *
 *  Created on: 5 jul. 2017
 *      Author: jurriaanvandenberg
 */

#include <JWorld.hpp>

#include <vector>
#include <JRenderer.hpp>

using namespace Renderer;


namespace Overture {

	void World_Close() {
		// Delete allocated hitboxes (quadliterals).
		for (int i = 0; i < World::tileInfo.size(); i++) {
			for (int i2 = 0; i2 < World::tileInfo[i].hitboxQuads.size(); i2++) {
				if (World::tileInfo[i].hitboxQuads[i2] != NULL) {
					delete World::tileInfo[i].hitboxQuads[i2];
					World::tileInfo[i].hitboxQuads[i2] = NULL;
				}
			}
		}
	}
}


namespace World {

	std::vector<TileInfo> tileInfo;
	float wallAngle = 45.0f;


	void update() {

	}



	void createTile( JTexture* texture, std::string name, std::string material, bool insidecorners ) {
		if (texture != NULL) {
			bool alreadyAdded = false;
			for (int i = 0; i < tileInfo.size(); i++) {
				if (tileInfo[i].texture == texture) {
					alreadyAdded = true;
				}
			}

			if (!alreadyAdded) {
				TileInfo tileinfo = { texture, insidecorners,  name, material };
				tileInfo.push_back(tileinfo);
			}
		}
	}



	void setTileHitbox( JTexture* texture, TileType type, float p0x, float p0y, float p1x, float p1y, float p2x, float p2y, float p3x, float p3y ) {
		for (int i = 0; i < tileInfo.size(); i++) {
			if (tileInfo[i].texture == texture) {

				Quadrilateral* quad = new Quadrilateral(p0x, p0y, p1x, p1y, p2x, p2y, p3x, p3y);

				if (type == TILE_UNDEFINED) {
					if (tileInfo[i].hitboxQuads.size() == 0) {
						tileInfo[i].hitboxQuads.push_back(quad);
					}
					else {
						tileInfo[i].hitboxQuads[0] = quad;
					}
				}
				else {
					int typeInt = abs(static_cast<int>(type));

					if (typeInt < 0) {
						typeInt = abs(typeInt);
						quad->flipX((float) TILE_SIZE);
					}

					for (int i2 = tileInfo[i].hitboxQuads.size(); i2 <= typeInt + 1 ; i2++) {
						tileInfo[i].hitboxQuads.push_back(NULL);
					}
					tileInfo[i].hitboxQuads[typeInt + 1] = quad;
				}

				break;
			}
		}
	}


	void setTileHitbox( JTexture* texture, TileType type, float pointsX, float pointsY, float pointsWidth, float pointsHeight ) {
		setTileHitbox(texture, type, pointsX, pointsY, pointsX + pointsWidth, pointsY, pointsX, pointsY + pointsHeight, pointsX + pointsWidth, pointsY + pointsHeight);
	}


	void setTileHitboxFull( JTexture* texture, TileType type ) {
		setTileHitbox(texture, type, 0.0f, 0.0f, (float) TILE_SIZE, 0.0f, (float) TILE_SIZE, (float) TILE_SIZE, 0.0f, (float) TILE_SIZE);
	}

}





using namespace World;


/////////////////////// JOTileMap Component ///////////////////////


JOTileMap::JOTileMap() {

}


void JOTileMap::update() {

}


void JOTileMap::render() {
	resetRenderColor();
	for (int i = 0; i < tileData.size(); i++) {
		for (int i2 = 0; i2 < tileData[i].size(); i2++) {
			TileData tiledata = tileData[i][i2];
			if (tiledata.tileIndex != -1 && tiledata.tileType != TILE_UNDEFINED) {

				float posX = object->getPosX() + (float) (i * TILE_SIZE);
				float posY = object->getPosY() + (float) (i2 * TILE_SIZE);

				setRenderColor(0xFF, 0xFF, 0xFF);
				renderTile(*tileInfo[tiledata.tileIndex].texture, (int) tiledata.tileType, posX, posY);

				bool renderHitbox = false;
				if (renderHitbox) {
					Quadrilateral quad = getHitbox(i, i2);
					quad += Vector2(posX, posY);
					if (quad.hasValues()) {
						setRenderColor(0x00, 0x00, 0x00);
						renderSDLLine(quad.p0.x, quad.p0.y, quad.p1.x, quad.p1.y);
						renderSDLLine(quad.p1.x, quad.p1.y, quad.p2.x, quad.p2.y);
						renderSDLLine(quad.p2.x, quad.p2.y, quad.p3.x, quad.p3.y);
						renderSDLLine(quad.p3.x, quad.p3.y, quad.p0.x, quad.p0.y);
					}
				}
			}
		}
	}
}


int JOTileMap::getTileX( float posX ) {
	return (posX - object->getPosX()) / TILE_SIZE;
}


int JOTileMap::getTileY( float posY ) {
	return (posY - object->getPosY()) / TILE_SIZE;
}



bool JOTileMap::tileExists( int tileX, int tileY) {
	if (tileX >= 0 && tileX < tileData.size() && tileY >= 0 && tileY < tileData[tileX].size()) {
		int tileIndex = tileData[tileX][tileY].tileIndex;
		if (tileIndex >= 0 && tileIndex < tileInfo.size() ) {
			return true;
		}
	}
	return false;
}


Quadrilateral JOTileMap::getHitbox( int tileX, int tileY ) {
	Quadrilateral quad = Quadrilateral();

	if (tileExists(tileX, tileY)) {
		int tileIndex = tileData[tileX][tileY].tileIndex;

		if (tileInfo[tileIndex].hitboxQuads.size() > 0) {
			TileType tileType = tileData[tileX][tileY].tileType;
			int hitboxIndex = abs(static_cast<int>(tileType)) + 1;

			if (hitboxIndex >= tileInfo[tileIndex].hitboxQuads.size()) {
				hitboxIndex = 0;
			}
			else {
				if (tileInfo[tileIndex].hitboxQuads[hitboxIndex] == NULL) {
					hitboxIndex = 0;
				}
			}

			if (tileInfo[tileIndex].hitboxQuads[hitboxIndex] != NULL) {
				quad = *(tileInfo[tileIndex].hitboxQuads[hitboxIndex]);

				if (static_cast<int>(tileType) < 0) {
					// Tile is flipped horizontally, so also flip quad/hitbox horizontally.
					quad.flipX((float) TILE_SIZE);
				}
			}
		}
	}

	return quad;
}



void JOTileMap::setTile( JTexture* texture, int tileX, int tileY, TileType type ) {
	if (tileX >= 0 && tileY >= 0) {
		// Add the necessary colums (keep them empty).
		for (int i = tileData.size(); i <= tileX; i++) {
			std::vector<TileData> emptyTileDataRow;
			tileData.push_back(emptyTileDataRow);
		}
		// Expand the colum to fit the wanted tile.
		for (int i = tileData[tileX].size(); i <= tileY; i++) {
			TileData tiledata = { -1, TILE_UNDEFINED };
			tileData[tileX].push_back(tiledata);
		}

		if (texture != NULL) {
			// Add tile to added or already existing coördinates.
			bool tileTypeExists = false;
			int tileIndex = -1;
			for (int i = 0; i < tileInfo.size(); i++) {
				if (tileInfo[i].texture == texture) {
					tileTypeExists = true;
					tileIndex = i;
					tileData[tileX][tileY].tileIndex = tileIndex;
					tileData[tileX][tileY].tileType = type;
				}
			}

			// Create tiletype if it doesn't exist.
			if (!tileTypeExists) {
				createTile(texture, "[no name]", "undefined");
			}
		}
		else {
			type = TILE_UNDEFINED;
			tileData[tileX][tileY].tileIndex = -1;
			tileData[tileX][tileY].tileType = type;
		}

		if (type == TILE_UNDEFINED) {
			connectTile(tileX, tileY);
		}
	}
}


void JOTileMap::connectTile( int tileX, int tileY, bool connectNeighbors ) {
	if (tileX >= 0 && tileY >= 0 && tileX < tileData.size() && tileY < tileData[tileX].size()) {
		int tileIndex = tileData[tileX][tileY].tileIndex;

		// 0001 = left, 0010 = right, 0100 = up, 1000 = down.
		int connectData = 0;

		int x = tileX - 1;
		int y = tileY;
		if (x >= 0 && y < tileData[x].size() && (tileData[x][y].tileIndex == tileIndex || tileIndex == -1)) {
			connectData += 1;
			if (connectNeighbors) {
				connectTile(x, y, false);
			}
		}
		x = tileX + 1;
		y = tileY;
		if (x < tileData.size() && y < tileData[x].size() && (tileData[x][y].tileIndex == tileIndex || tileIndex == -1)) {
			connectData += 10;
			if (connectNeighbors) {
				connectTile(x, y, false);
			}
		}
		x = tileX;
		y = tileY - 1;
		if (y >= 0 && (tileData[x][y].tileIndex == tileIndex || tileIndex == -1)) {
			connectData += 100;
			if (connectNeighbors) {
				connectTile(x, y, false);
			}
		}
		x = tileX;
		y = tileY + 1;
		if (y < tileData[tileX].size() && (tileData[x][y].tileIndex == tileIndex || tileIndex == -1)) {
			connectData += 1000;
			if (connectNeighbors) {
				connectTile(x, y, false);
			}
		}


		// INSIDE CORNERS START //
		bool c1 = true;  // Inside corner top left.
		bool c2 = true;  // Inside corner top right.
		bool c3 = true;  // Inside corner bottom left.
		bool c4 = true;  // Inside corner bottom right.

		x = tileX - 1;
		y = tileY - 1;
		if (x >= 0 && y >= 0 && y < tileData[x].size() && ((tileData[x][y].tileIndex == tileIndex) || tileIndex == -1)) {
			c1 = false;
			if (connectNeighbors) {
				connectTile(x, y, false);
			}
		}
		x = tileX + 1;
		y = tileY - 1;
		if (x < tileData.size() && y >= 0 && y < tileData[x].size() && (tileData[x][y].tileIndex == tileIndex || tileIndex == -1)) {
			c2 = false;
			if (connectNeighbors) {
				connectTile(x, y, false);
			}
		}
		x = tileX - 1;
		y = tileY + 1;
		if (x >= 0 && y < tileData[x].size() && (tileData[x][y].tileIndex == tileIndex || tileIndex == -1)) {
			c3 = false;
			if (connectNeighbors) {
				connectTile(x, y, false);
			}
		}
		x = tileX + 1;
		y = tileY + 1;
		if (x < tileData.size() && y < tileData[x].size() && (tileData[x][y].tileIndex == tileIndex || tileIndex == -1)) {
			c4 = false;
			if (connectNeighbors) {
				connectTile(x, y, false);
			}
		}
		// INSIDE CORNERS END //

		bool insideCorners = tileInfo[tileData[tileX][tileY].tileIndex].insideCorners;

		TileType tp = TILE_UNDEFINED;
		if (!insideCorners) {
			switch (connectData) {
			case 0 : tp = TILE_ISOLATED; break;
			case 1 : tp = TILE_LEFT; break;
			case 10 : tp = TILE_RIGHT; break;
			case 11 : tp = TILE_LEFT_RIGHT; break;
			case 100 : tp = TILE_UP; break;
			case 101 : tp = TILE_LEFT_UP; break;
			case 110 : tp = TILE_UP_RIGHT; break;
			case 111 : tp = TILE_LEFT_UP_RIGHT; break;
			case 1000 : tp = TILE_DOWN; break;
			case 1001 : tp = TILE_DOWN_LEFT; break;
			case 1010 : tp = TILE_RIGHT_DOWN; break;
			case 1011 : tp = TILE_RIGHT_DOWN_LEFT; break;
			case 1100 : tp = TILE_UP_DOWN; break;
			case 1101 : tp = TILE_DOWN_LEFT_UP; break;
			case 1110 : tp = TILE_UP_RIGHT_DOWN; break;
			case 1111 : tp = TILE_MIDDLE; break;
			default : tp = TILE_ISOLATED; break;
			}
		}
		else {
			switch (connectData) {
			case 0 : tp = TILE_ISOLATED; break;
			case 1 : tp = TILE_LEFT; break;
			case 10 : tp = TILE_RIGHT; break;
			case 11 : tp = TILE_LEFT_RIGHT; break;
			case 100 : tp = TILE_UP; break;
			case 101 :
				tp = TILE_LEFT_UP;
				if (c1) tp = TILE_UP_LEFT_IC_1;
				break;
			case 110 :
				tp = TILE_UP_RIGHT;
				if (c2) tp = TILE_RIGHT_UP_IC_2;
				break;
			case 111 :
				tp = TILE_LEFT_UP_RIGHT;
				if (c1 && c2) tp = TILE_LEFT_UP_RIGHT_IC_1_2;
				else if (c1) tp = TILE_LEFT_UP_RIGHT_IC_1;
				else if (c2) tp = TILE_LEFT_UP_RIGHT_IC_2;
				break;
			case 1000 : tp = TILE_DOWN; break;
			case 1001 :
				tp = TILE_DOWN_LEFT;
				if (c3) tp = TILE_DOWN_LEFT_IC_3;
				break;
			case 1010 :
				tp = TILE_RIGHT_DOWN;
				if (c4) tp = TILE_RIGHT_DOWN_IC_4;
				break;
			case 1011 :
				tp = TILE_RIGHT_DOWN_LEFT;
				if (c3 && c4) tp = TILE_RIGHT_DOWN_LEFT_IC_3_4;
				else if (c3) tp = TILE_RIGHT_DOWN_LEFT_IC_3;
				else if (c4) tp = TILE_RIGHT_DOWN_LEFT_IC_4;
				break;
			case 1100 : tp = TILE_UP_DOWN; break;
			case 1101 :
				tp = TILE_DOWN_LEFT_UP;
				if (c1 && c3) tp = TILE_DOWN_LEFT_UP_IC_1_3;
				else if (c1) tp = TILE_DOWN_LEFT_UP_IC_1;
				else if (c3) tp = TILE_DOWN_LEFT_UP_IC_3;
				break;
			case 1110 :
				tp = TILE_UP_RIGHT_DOWN;
				if (c2 && c4) tp = TILE_UP_RIGHT_DOWN_IC_2_4;
				else if (c2) tp = TILE_UP_RIGHT_DOWN_IC_2;
				else if (c4) tp = TILE_UP_RIGHT_DOWN_IC_4;
				break;
			case 1111 :
				tp = TILE_MIDDLE;
				if (c1 && c2 && c3 && c4) tp = TILE_MIDDLE_IC_1_2_3_4;
				else if (c1 && c3 && c4) tp = TILE_MIDDLE_IC_1_3_4;
				else if (c2 && c3 && c4) tp = TILE_MIDDLE_IC_2_3_4;
				else if (c1 && c2 && c4) tp = TILE_MIDDLE_IC_1_2_4;
				else if (c1 && c2 && c3) tp = TILE_MIDDLE_IC_1_2_3;
				else if (c2 && c3) tp = TILE_MIDDLE_IC_2_3;
				else if (c1 && c4) tp = TILE_MIDDLE_IC_1_4;
				else if (c2 && c4) tp = TILE_MIDDLE_IC_2_4;
				else if (c1 && c3) tp = TILE_MIDDLE_IC_1_3;
				else if (c3 && c4) tp = TILE_MIDDLE_IC_3_4;
				else if (c1 && c2) tp = TILE_MIDDLE_IC_1_2;
				else if (c4) tp = TILE_MIDDLE_IC_4;
				else if (c3) tp = TILE_MIDDLE_IC_3;
				else if (c2) tp = TILE_MIDDLE_IC_2;
				else if (c1) tp = TILE_MIDDLE_IC_1;
				break;
			default : tp = TILE_ISOLATED; break;
			}
		}

		tileData[tileX][tileY].tileType = tp;
	}
}




