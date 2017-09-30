/*
 * JOUI.hpp
 *
 *  Created on: 8 jun. 2017
 *      Author: jurriaanvandenberg
 */

#ifndef OVERTURE_SRC_OBJECT_JOINTERFACE_HPP_
#define OVERTURE_SRC_OBJECT_JOINTERFACE_HPP_

#include <JObject.hpp>
#include <JInput.hpp>
#include <JOText.hpp>


struct JWindowData;
struct JOWindow;
struct JOResizer;
struct JOFrame;
struct JOAligner;
struct JOButton;


typedef enum {
	ALIGN_LEFT = 0,
	ALIGN_TOP = 0,
	ALIGN_CENTER = 1,
	ALIGN_RIGHT = 2,
	ALIGN_BOTTOM = 2
} JAlignment;



struct JWindowData {
	void setTextures( JTexture* cornerTop, JTexture* cornerBottom, JTexture* edgeLeft, JTexture* edgeTop, JTexture* edgeBottom );
	void setButtonTextures( JTexture* close, JTexture* collapse1, JTexture* collapse2, JTexture* minimize1, JTexture* minimize2 );

	void setMargins( float left, float right, float top, float bottom );
	void setMargins( float margins );
	void setMarginsX( float margins );
	void setMarginsY( float margins );

	JBitmapFont* titleFont;
	int titleOffsetX;
	int titleOffsetY;
	int titleBarHeight;

	JTexture* tButtonClose;
	JTexture* tButtonCollapse1;
	JTexture* tButtonCollapse2;
	JTexture* tButtonMinimize1;
	JTexture* tButtonMinimize2;
	int buttonsOffsetX;
	int buttonsOffsetY;
	int buttonDistance;

	JTexture* tCorner0;  // Top left.
	JTexture* tCorner1;  // Top right.
	JTexture* tCorner2;  // Bottom right.
	JTexture* tCorner3;  // Bottom left.
	JTexture* tEdgeLeft;
	JTexture* tEdgeRight;
	JTexture* tEdgeTop;
	JTexture* tEdgeBottom;
	int textureOffset;  // Offset the texture outward (can be used for shadows).
	bool symetricX;  // Right textures are left textures flipped.

	SDL_Color backgroundColor;

	float marginLeft;
	float marginRight;
	float marginTop;
	float marginBottom;
};



struct JOWindow : JOComponent {
	JOWindow( JWindowData* windowData, JObject* object = NULL );

	virtual void update();
	virtual void render();

	JOFrame* getFrame();

	void setCollapsed( bool collapse );
	void setMinimized( bool minimize );

	bool isCollapsed();
	bool isMinimized();

	JWindowData* windowData;

private:
	bool collapsed;
	bool minimized;

	float maximizedPosX;
	float maximizedPosY;
	float maximizedWidth;
};



struct JOResizer : JOComponent {
	JOResizer( bool dragonly = false );

	virtual void update();
	virtual void render();

	bool isBeingDragged();

	bool dragOnly;
	float lineWidth;

private:
	// Handle resizing per InputPointer that the objects owns.
	void handleResizing( int index );

	short dragState[Input::pointerAmount];
};



struct JOFrame : JOComponent {
	JOFrame();

	virtual void update();
	virtual void render();

	void addRow( int height, bool fillRemainder = false );
	void activeRow( int row );
	void addColumn( int width, bool fillRemainder = false );
	void activeColumn( int column );

	void insertRow( int at, int height, bool fillRemainder = false, bool moveChildren = true );
	void removeRow( int row, bool removeChildren = true );
	void insertColumn( int at, int width, bool fillRemainder = false, bool moveChildren = true );
	void removeColumn( int column, bool removeChildren = true );

	void addObject( JObject* childObject, JAlignment alignmentX = ALIGN_LEFT, JAlignment alignmentY = ALIGN_TOP, float resizeRatioX = 0.0f, float resizeRatioY = 0.0f );
	void removeObjects( int afterRow = 0 );
	std::vector<JObject*>* objects();
	JObject* lastObject( int offset = 0 );

	void updateChildPositions();


	std::vector<int> rows;
	std::vector<int> columns;

	int currentRow;
	int currentColumn;

	bool canScrollX;
	bool canScrollY;
	int scrollRangeX;
	int scrollRangeY;
	float scrollPosX;
	float scrollPosY;

	int lastObjectIndex;
};



struct JOAligner : JOComponent {
	JOAligner( int row = 0, int column = 0, JAlignment alignmentx = ALIGN_LEFT, JAlignment alignmenty = ALIGN_TOP, float resizeratioX = 0.0f, float resizeratioY = 0.0f );
	JOAligner( JOAligner& alignTemplate );

	virtual void update() {}
	virtual void render() {}

	void setMargins( float left, float right, float top, float bottom );
	void setMargins( float margins );
	void setMarginsX( float margins );
	void setMarginsY( float margins );

	int row;
	int column;

	JAlignment alignmentX;
	JAlignment alignmentY;

	float marginRight;
	float marginLeft;
	float marginTop;
	float marginBottom;

	float resizeRatioX;  // Resize this object with parent object.
	float resizeRatioY;
};



struct JOButton : JOComponent {
	// Make sure that boolPointers lifetime exteeds that of this JOComponent, or simply set boolPointer to NULL before.
	JOButton( bool active = false, bool* boolpointer = NULL  );

	virtual void update();
	virtual void render();

	void updatePressedState();
	bool isActivated();
	bool isPressed();
	bool isDown();
	bool isClicked();

	bool activated;
	bool disabled;

	bool* boolPointer;
};




#endif /* OVERTURE_SRC_OBJECT_JOINTERFACE_HPP_ */
