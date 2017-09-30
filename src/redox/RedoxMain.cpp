/*
 * redoxmain.cpp
 *
 *  Created on: 13 okt. 2016
 *      Author: jurriaanvandenberg
 */

#include "RedoxMain.hpp"

#include <iomanip>
#include <SDL_ttf.h>
#include <SDL_syswm.h>
#include <JAudio.hpp>
#include <JBezier.hpp>
#include <JProfiler.hpp>
#include <JPlatform.hpp>
#include <JObject.hpp>
#include <JORender.hpp>
#include <JOInterface.hpp>
#include <JOText.hpp>
#include <JOCollision.hpp>
#include <JCursor.hpp>
#include <JWorld.hpp>
#include <JDevelopment.hpp>

using namespace Renderer;


/* XPM Cursor */
static const char *arrowcursor[] = {
  /* width height num_colors chars_per_pixel */
  "    32    32        3            1",
  /* colors */
  "X c #000000",
  ". c #ff0000",
  "  c None",
  /* pixels */
  "X                               ",
  "XXXXXXXXX..XXXXXX..XX           ",
  "X.X                             ",
  "X..X                            ",
  "X...XXXXXXXXXXX                 ",
  "X....X                          ",
  "X.....X            X            ",
  "X......XXXXXX                   ",
  "X.......X               X       ",
  "X........X                      ",
  "X.....XXXXX                     ",
  "X..X..X                         ",
  "X.X X..X             X          ",
  "XX  X..X                        ",
  "X    X..X                       ",
  "     X..X               X       ",
  "      X..X                      ",
  "      X..X     X           X    ",
  "       XX                       ",
  "                                ",
  "                                ",
  "  X                     X       ",
  "                                ",
  "                                ",
  "        X                       ",
  "                                ",
  "                 X              ",
  "                                ",
  "X                               ",
  "         X               X      ",
  "                                ",
  "               X                ",
  "0,0"
};


JTexture portrait;
JTexture portrait3;
JTexture landscape;
JTexture arrow;
JTexture maskTest;


JWindowData windowTemplate;
JTexture cornerTop, cornerBottom, edgeLeft, edgeTop, edgeBottom;
JTexture buttonClose, buttonCollapse1, buttonCollapse2, buttonMinimize1, buttonMinimize2;

JBitmapFont lazyfont;
JBitmapFont pixelfont;
JBitmapFont pixelfontBold;

JTexture grassTileSet;

JFont roboto;

JObject tileMap;
JObject textObject;
JObject debugObject1, debugObject2, debugObject3, debugObject4;
JObject player;


std::string debugString;


void redoxmain::init() {

	portrait.setAndLoad("hello.bmp");
	portrait3.setAndLoad("hello.bmp");
	landscape.setAndLoad("cap2.png");
	arrow.setAndLoad("doublearrow.png");
	maskTest.setAndLoad("masktest.png");

	cornerTop.setAndLoad("pixelwindow/cornerTop.png");
	cornerBottom.setAndLoad("pixelwindow/cornerBottom.png");
	edgeLeft.setAndLoad("pixelwindow/edgeLeft.png");
	edgeTop.setAndLoad("pixelwindow/edgeTop.png");
	edgeBottom.setAndLoad("pixelwindow/edgeBottom.png");
	buttonClose.setAndLoad("pixelwindow/buttonClose.png");
	buttonCollapse1.setAndLoad("pixelwindow/buttonCollapse1.png");
	buttonCollapse2.setAndLoad("pixelwindow/buttonCollapse2.png");
	buttonMinimize1.setAndLoad("pixelwindow/buttonMinimize1.png");
	buttonMinimize2.setAndLoad("pixelwindow/buttonMinimize2.png");
	lazyfont.setTextureAndLoad("lazyfont.png", 2);
	pixelfont.setTextureAndLoad("pixelfont.png", 1);
	pixelfontBold.setTextureAndLoad("pixelfontBold.png", 1);

	grassTileSet.setAndLoad("grasstileset.png");
	World::createTile(&grassTileSet, "grass", "grass", true);
	World::setTileHitboxFull(&grassTileSet);
	float tileSize = (float) TILE_SIZE;
	World::setTileHitbox(&grassTileSet, TILE_DOWN, 0.0f, 0.0f, tileSize, 8.0f, tileSize, tileSize, 0.0f, tileSize);
	World::setTileHitbox(&grassTileSet, TILE_DOWN_LEFT, 0.0f, 0.0f, 8.0f, tileSize, 0.0f, tileSize, 0.0f, 0.0f);
	World::setTileHitbox(&grassTileSet, TILE_LEFT_UP, 0.0f, 0.0f, tileSize, 0.0f, tileSize, 12.0f, 0.0f, tileSize);

	roboto.set("Roboto-Regular.ttf");


	windowTemplate.backgroundColor = SDLColor(0xFF, 0xFF, 0xFF);
	windowTemplate.titleFont = &pixelfontBold;
	windowTemplate.titleOffsetX = 3;
	windowTemplate.titleOffsetY = 2;
	windowTemplate.setButtonTextures(&buttonClose, &buttonCollapse1, &buttonCollapse2, &buttonMinimize1, &buttonMinimize2);
	windowTemplate.buttonsOffsetX = 3;
	windowTemplate.buttonsOffsetY = 2;
	windowTemplate.buttonDistance = 2;
	windowTemplate.setTextures(&cornerTop, &cornerBottom, &edgeLeft, &edgeTop, &edgeBottom);
	windowTemplate.setMarginsX(4.0f);
	windowTemplate.setMarginsY(2.0f);


	player.setLocalPosZ(100.0f);
	player.init(50.0f, 50.0f, 16.0f, 16.0f, "Player 1");
	player.addComponent(new JOTexture(&portrait));
	player.addComponent(new JOCollider(true));
	player.addComponent(new JOResizer());
	player.addComponent(new JOMotion());

	tileMap.init(-100, -100, 600, 600, "Tile Map");
	tileMap.addComponent(new JOColor(0xFA, 0xF5, 0xA0));
	Renderer::setClearColor(0xFA, 0xF5, 0xA0);
	JOTileMap* tilemap = new JOTileMap();
	tileMap.addComponent(tilemap);
	Collision::setActiveTileMap(tilemap);
	for (int i = 0; i < 30; i++) {
		for (int i2 = 0; i2 < 30; i2++) {
			if (!(i < 6 && i2 < 6)) {
				bool createTile = rand() % 2;
				if (createTile) {
					int tile = rand() % 19;
					if (tile != 12 && tile != 13) {
					}
					tile = 42;
					tileMap.getComponent<JOTileMap>()->setTile(&grassTileSet, i, i2, static_cast<TileType> (tile));
				}
			}
		}
	}
	tileMap.getComponent<JOTileMap>()->setTile(&grassTileSet, 3, 3, TILE_UNDEFINED);
	tileMap.getComponent<JOTileMap>()->setTile(&grassTileSet, 2, 3, TILE_UNDEFINED);
	tileMap.getComponent<JOTileMap>()->setTile(&grassTileSet, 2, 4, TILE_UNDEFINED);
	tileMap.getComponent<JOTileMap>()->setTile(&grassTileSet, 3, 4, TILE_UNDEFINED);
	tileMap.getComponent<JOTileMap>()->connectTile(3, 3);
	tileMap.getComponent<JOTileMap>()->connectTile(2, 4);


	textObject.init(50, 50, 200, 200, "text object");
	textObject.addComponent(new JOColor(230, 125, 140));
	//textObject.addComponent(new JOResizer());
	textObject.addComponent(new JOText(&pixelfont, SDLColor(0xFF, 0xFF, 0xFF), "Dit is een teststring"));


	debugObject1.init(16, 16, 16, 16, "Debug object 1");
	debugObject1.addComponent(new JOColor(65, 240, 50));
	debugObject1.addComponent(new JOResizer(false));
	debugObject1.addComponent(new JOCollider());
	debugObject2.init(16*3, 16, 16, 16, "Debug object 2");
	debugObject2.addComponent(new JOColor(250, 40, 150));
	debugObject2.addComponent(new JOResizer(false));
	debugObject2.addComponent(new JOCollider());
	debugObject3.init(16*3, 16*3, 16, 16, "Debug object 3");
	debugObject3.addComponent(new JOColor(43, 70, 236));
	debugObject3.addComponent(new JOResizer(false));
	debugObject3.addComponent(new JOCollider());

	debugObject4.init(16*-3, 16*6, 16, 16, "Debug object 4");
	debugObject4.addComponent(new JOColor(SDLRandomColor()));
	debugObject4.addComponent(new JOResizer(false));
	debugObject4.addComponent(new JOCollider());

	debugObject1.addChild(debugObject2);
	debugObject1.addChild(debugObject3);
	debugObject2.addChild(debugObject4);

	Overture::cameraPos.x = Renderer::renderTarget->width/2;
	Overture::cameraPos.y = Renderer::renderTarget->height/2;

	Dev::initialize(&windowTemplate, &pixelfont);

	Dev::addDebugValue("Text X", &textObject.localPosX);
	Dev::addDebugValue("Text Y", &textObject.localPosY);
	Dev::addDebugValue("Text active", &textObject.active);
}



void updateProfilerFrame();

void redoxmain::update() {

	static bool frameAdvance = true;
	static int autofireDelay = 0;

	if (Input::isKeyPressed(KEY_PERIOD)) {
		frameAdvance = !frameAdvance;
		std::cout << "frameAdvance: " << frameAdvance << "\n";
	}

	if (!frameAdvance) {
		if (Input::isKeyDown(KEY_SLASH)) {
			autofireDelay--;
		}
		else {
			autofireDelay = 50;
		}

		if (!Input::isKeyPressed(KEY_SLASH) && autofireDelay > 0) {
			return;
		}
	}


	updateProfilerFrame();



	if (Input::isKeyPressed(KEY_V)) {
		/*
		JObject* object = new JObject(true);
		object->setLocalPosZ(1000.0f);
		static int windowNumber = 0;
		object->init(0, 0, 60, 45, "Frame " + toString(windowNumber));
		windowNumber++;
		object->addComponent(new JOResizer());
		object->addComponent(new JOFrame(&windowTemplate));


		JObject* child1 = new JObject(true);
		object->addChild(*child1);
		child1->init(10, 10, 10, 10, "child 1");
		child1->addComponent(new JOColor(SDLRandomColor()));
		child1->addComponent(new JOAligner(0, 0, ALIGN_CENTER, ALIGN_CENTER));
		child1->getComponent<JOAligner>()->resizeRatioX = 1.0f;
		child1->getComponent<JOAligner>()->resizeRatioY = 0.7f;


		for (int i = 0; i < 0; i++) {
			JObject* child3 = new JObject(true);
			object->addChild(*child3);
			child3->init(10, 10, 2, 2, "random " + toString(i));
			child3->addComponent(new JOColor(SDLRandomColor()));
			child3->addComponent(new JOAligner(rand() % 3, rand() % 3, static_cast<JAlignment> (rand() % 1 + 1), static_cast<JAlignment> (rand() % 1 + 1)));

		}*/
	}



	if (Input::isKeyPressed(KEY_B)) {
		Platform_LoadDragAndDrop();
	}
	if (Input::isKeyPressed(KEY_N)) {
		Platform_Drag("/Users/jurriaanvandenberg/Desktop/C1akw25UUAARHH0.jpg");
	}



	if (Input::isKeyDown(KEY_Y)) {
		static int misc = Cursor::load(CURSOR_MISC, arrowcursor);
		Cursor::set(misc);
	}
	if (Input::isKeyPressed(KEY_H)) {
		Cursor::toggleVisibility();
	}



	// Player movement
	float speed = 60.0f * dt();
	if (Input::isKeyDown(KEY_LSHIFT)) {
		speed = 2.0f * dt();
	}
	if (Input::isKeyDown(KEY_RSHIFT)) {
		speed = 5000.0f * dt();
	}
	float walkForce = 500.0f;


	bool gravity = false;

	if (gravity) {
		static bool walkingLeft = false;
		static bool walkingRight = false;

		if (Input::isKeyPressed(KEY_A)) {
			walkingLeft = true;
		}
		if (!Input::isKeyDown(KEY_A)) {
			walkingLeft = false;
		}
		if (Input::isKeyPressed(KEY_D)) {
			walkingRight = true;
		}
		if (!Input::isKeyDown(KEY_D)) {
			walkingRight = false;
		}

		if (walkingLeft) {
			player.localPosX -= speed;
			//player.getComponent<JOMotion>()->addForce(walkForce, -180.0f);
			if (player.getComponent<JOMotion>()->motionX > 0.0f)
				player.getComponent<JOMotion>()->motionX = 0.0f;
		}
		if (walkingRight) {
			player.localPosX += speed;
			//player.getComponent<JOMotion>()->addForce(walkForce, 0.0f);
			if (player.getComponent<JOMotion>()->motionX < 0.0f)
				player.getComponent<JOMotion>()->motionX = 0.0f;
		}

		if (Input::isKeyDown(KEY_S)) {
			player.localPosY += speed;
		}

		if (player.getComponent<JOCollider>()->hasCollidedTop || player.getComponent<JOCollider>()->hasCollidedBottom) {
			player.getComponent<JOMotion>()->motionY = 0.0f;
		}
		if (player.getComponent<JOCollider>()->hasCollidedLeft || player.getComponent<JOCollider>()->hasCollidedRight) {
			player.getComponent<JOMotion>()->motionX = 0.0f;
		}


		if ((Input::isKeyPressed(KEY_W) || Input::isKeyPressed(KEY_SPACE))) {
			float jumpForce = 6000.0f;

			float angle = -90.0f;
			if (player.getComponent<JOCollider>()->hasCollidedBottom) {
				//player.getComponent<JOMotion>()->addForce(jumpForce, -90.0f);
				if (Input::isKeyDown(KEY_A)) {
					//angle -= 45.0f;
				}
				if (Input::isKeyDown(KEY_D)) {
				//	angle += 45.0f;
				}
				player.getComponent<JOMotion>()->addForce(jumpForce, angle);
				std::cout << "jump" << std::endl;
			}
			else if (player.getComponent<JOCollider>()->hasCollidedLeft) {
				angle += 25.0f;
				player.getComponent<JOMotion>()->addForce(jumpForce, angle);
				walkingRight = false;
				walkingLeft = false;
				std::cout << "Wall Jump from left" << std::endl;
			}
			else if (player.getComponent<JOCollider>()->hasCollidedRight) {
				angle -= 25.0f;
				player.getComponent<JOMotion>()->addForce(jumpForce, angle);
				walkingRight = false;
				walkingLeft = false;
				std::cout << "Wall Jump from right" << std::endl;
			}
		}

		player.getComponent<JOMotion>()->addForce(160.0f, 90.0f);
	}
	else {
		if (Input::isKeyDown(KEY_A))  player.localPosX -= speed;
		if (Input::isKeyDown(KEY_D))  player.localPosX += speed;
		if (Input::isKeyDown(KEY_W))  player.localPosY -= speed;
		if (Input::isKeyDown(KEY_S))  player.localPosY += speed;
	}


	if (Input::isKeyDown(KEY_C)) {
		if (Input::isKeyDown(KEY_LEFT))
			Overture::cameraPos.x -= speed;
		if (Input::isKeyDown(KEY_RIGHT))
			Overture::cameraPos.x += speed;
		if (Input::isKeyDown(KEY_UP))
			Overture::cameraPos.y -= speed;
		if (Input::isKeyDown(KEY_DOWN))
			Overture::cameraPos.y += speed;
	}

	if (Input::isKeyDown(KEY_ESCAPE)) {
		player.localPosX = 40;
		player.localPosY = 40;
		player.getComponent<JOCollider>()->teleported = true;
		Overture::cameraPos.x = (int) roundf(player.localPosX + player.localWidth/2);
		Overture::cameraPos.y = (int) roundf(player.localPosY + player.localHeight/2);
	}


	Dev::update();
	JObjects::Update();


	float scalespeed = 0.01f;
	if (Input::isKeyDown(KEY_6)) {
		setResolutionScale(getResolutionScale() - scalespeed);
	}
	if (Input::isKeyDown(KEY_7)) {
		setResolutionScale(getResolutionScale() + scalespeed);
	}
	if (Input::isKeyDown(KEY_1)) setResolutionScale(1.0f);
	if (Input::isKeyDown(KEY_2)) setResolutionScale(2.0f);
	if (Input::isKeyDown(KEY_3)) setResolutionScale(3.0f);
	if (Input::isKeyDown(KEY_4)) setResolutionScale(4.0f);
	if (Input::isKeyDown(KEY_5)) setResolutionScale(5.0f);


	JOTileMap* tilemap = tileMap.getComponent<JOTileMap>();
	if (tilemap != NULL) {
		static bool erase = true;
		static bool draw = false;

		int x = tilemap->getTileX(Input::screenToRenderX(Input::getMouse()->position.x));
		int y = tilemap->getTileY(Input::screenToRenderY(Input::getMouse()->position.y));

		if (Input::getMouse()->pressed && !Input::getMouse()->pressConsumed) {
			Input::getMouse()->pressConsumed = true;
			draw = true;
			if (tilemap->tileExists(x, y)) {
				erase = true;
			}
			else {
				erase = false;
			}
		}

		if (draw && (Input::getMouse()->down || Input::getMouse()->pressed)) {
			if (erase) {
				tilemap->setTile(NULL, x, y, TILE_UNDEFINED);
			}
			else {
				tilemap->setTile(&grassTileSet, x, y, TILE_UNDEFINED);
			}
		}
		else {
			draw = false;
		}
	}
}


SDL_Joystick* gGameController = NULL;
std::vector<SDL_Texture*> textures;

void redoxmain::render() {

	setRenderColor(100, 244, 255, 0xFF);
	std::string string = "FPS: " + toString(Overture_GetFramerate()) + " | " + toString(getResolutionScale());//+ toString(unloadVector.size());

	static bool bl = true;
	if (bl) {
		bl = false;
		//Overture_LoadResources();
	}
	if (Input::isKeyPressed(KEY_L) || (Overture_IsMobile() && Input::pointers[3].pressed)) {
		static bool stuff = false;
		stuff = !stuff;
		if (stuff) {
			portrait.unloadLater();
			portrait3.unloadLater();
			landscape.unloadLater();
			arrow.unloadLater();
		}
		else {
			portrait.loadLater();
			portrait3.loadLater();
			landscape.loadLater();
			arrow.loadLater();
		}
		roboto.free();
		roboto.load(30);
	    //gGameController = SDL_JoystickOpen(0);
	}


	static bool isMoving = false;
	static float tmovement = 0;

	float speed = 1.0f*dt();
	if (Input::isKeyDown(KEY_G)) {
		tmovement += speed;
		std::cout << tmovement << std::endl;
	}
	if (Input::isKeyDown(KEY_F)) {
		tmovement -= speed;
		std::cout << tmovement << std::endl;
	}
	/*
	if (isMoving) {
		if (fromLeft) {
			tmovement += speed;
			if (tmovement >= 1.0) {
				tmovement = 1.0;
				isMoving = false;
				fromLeft = false;
			}
		}
		else {
			tmovement -= speed;
			if (tmovement <= 0.0) {
				tmovement = 0.0;
				isMoving = false;
				fromLeft = true;
			}
		}
	}*/


	int posX = 200;
	int posY = 50;
	float distance = 1.0f;//300;

	/*
	float nx = (float) (x) / 32767.0f;
	float ny = (float) (y) / 32767.0f;
	float nz = (float) (x) / 32767.0f;

	float angle = -atan(nx/ny)*180.0f/3.14159265f;

	if (ny < 0)
		angle += 180.0f;
	*/

	setClearColor(0x30, 0xCF, 0x63);


	Bezier bezier1 = Bezier(Vector2(1, 0), Vector2(0, 1));
	bezier1.render(10, posY, 190, 190);

	resetRenderColor();
	renderTexture(portrait, posX + tmovement*distance, posY, 128, 128);

	if (Input::isAreaClicked(posX + tmovement*distance, posY, 128, 128)) {
		isMoving = !isMoving;
	}
	if (Input::isAreaBeingClicked(posX + tmovement*distance, posY, 128, 128)) {
		setClearColor(0xCC, 0x33, 0x00);
	}
	else if (Input::isAreaHovered(posX + tmovement*distance, posY, 128, 128)) {
		setClearColor(0x33, 0x33, 0xCC);
	}

	posY += 150;

	activateClippingMask(posX+300, posY-150, 50, 50);
	drawClippingMask();

	setRenderColor( 255, 0, 0 );
	renderTexture(portrait, posX + tmovement*distance + 300, posY);
	if (Input::pointers[0].active) {
		float maskSizeX = 200;
		float maskSizeY = 200;
		float maskPosX = Input::getPointerPosition(0).x - maskSizeX/2;
		float maskPosY = Input::getPointerPosition(0).y  - maskSizeY/2;


		setRenderColor(0x00, 0xFF, 0x00, 0x99);
		renderTexture(portrait, maskPosX + maskSizeX * 2, maskPosY + maskSizeY * 2);

		activateClippingMask(maskPosX, maskPosY, maskSizeX, maskSizeY);
		setRenderColor(0x00, 0xFF, 0xFF, 0x55);
		renderRect(-100, -100, 10000, 10000);

		resetRenderColor();
		renderTexture(landscape, posX + tmovement*distance, posY);

		//activateClippingMask(posX + tmovement*distance+300, posY-150, 50, 50);
		renderTexture(portrait, posX + tmovement*distance+300, posY-150);
		//drawClippingMask();

		drawClippingMask();


	}
	resetRenderColor();
	renderCharacter(lazyfont, 97, 30, 30, 100, 100);



	activateClippingMask(posX + tmovement*distance+500, posY-150, 50, 50);
	renderTexture(portrait, posX + tmovement*distance+500, posY-150);
	drawClippingMask();

	if (Input::isKeyPressed(KEY_E)) {
		time_t t = time(0);
	    tm now = *localtime(&t);
	    std::string str = toString(now.tm_year + 1900) + "-" + toString(now.tm_mon + 1) + "-" + toString(now.tm_mday) +
	    		" " + toString(now.tm_hour) + ":" + toString(now.tm_min) + ":" + toString(now.tm_sec);
	    //JLog(str);
	    //func(6, "dcff", 3, 'a', 1.999, " ", 42.5);
	    //JLog(3, 5, "asdf", 0.2, " ");
	    toString(3);
	}

	setRenderColor( 255, 0, 0 );
	renderTexture(portrait, posX + tmovement*distance, posY);


	if (Input::isAreaDown(posX + bezier1.getVectorValue(tmovement).y*distance, posY, 128, 128)) {
		setClearColor(0x40, 0xA2, 0xBF);
	}

	posY += 250;
	Bezier bezier2 = Bezier(Vector2(0, 0), Vector2(0, 1));
	bezier2.render(10, posY, 190, 190);

	//renderTexture(arrow, posX + bezier2.getFloatValue(tmovement)*distance, posY, NAF, NAF, SDL_FLIP_NONE, angle);


	setRenderColor(255, 255, 255);

	if (Input::isKeyPressed(KEY_T)) {
		textObject.getComponent<JOText>()->setString("#T80\n Dit is een test ik #o hoop #e#O dat #e dit gaat werken haha,"
				" #T200  W H O  #T-200 K N O W S ? #T2400b#T20lablablabla blablablabla blablablabla blablablabla #T80 en nu weer #T-100 normaal#T80 \ndit was een enter, \ndit ook #w woooooooooooww #eJ");
	}



	if (Overture_IsMobile()) {
		for (int i = 0; i < Input::pointerAmount; i++) {
			if (Input::pointers[i].active) {
				float posX = Input::getPointerPosition(i).x - 10;
				float posY = Input::getPointerPosition(i).y - 10;

				setRenderColor(0, 0xFF, 0, 0x88);
				renderTexture(arrow, posX, posY, 20, 20);


				if (i == 0) {
					renderTexture(arrow, posX, posY-5, 20, 20);
				}
			}
		}
	}

	if (Input::isKeyPressed(KEY_P) || (Overture_IsMobile() && Input::isScreenPressed())) {
//
//	    // Init Simple-SDL2-Audio
//	    static bool adsf = true;
//	    if (adsf) {
//	    	Overture::Audio_Init();
//	    	adsf = false;
//	    	Profiler::clearAllData();
//	    }
//	    Profiler::printData();
//
//	    // Play music and a sound
//	    //playAudio();
	}


	JObjects::Render();


	Vector2 mousePos = Vector2(Input::screenToRenderX(Input::getMouse()->position.x), Input::screenToRenderY(Input::getMouse()->position.y));

	float textdistance = 3.0f;
	std::string str = toString(mousePos.x) + ", " + toString(mousePos.y);

	setRenderColor(0x33, 0x33, 0x33);
	renderString(pixelfont, str, mousePos.x + textdistance + 1, mousePos.y + textdistance + 1);
	resetRenderColor();
	renderString(pixelfont, str, mousePos.x + textdistance, mousePos.y + textdistance);
}





