/*
 * JOInterface.cpp
 *
 *  Created on: 8 jun. 2017
 *      Author: jurriaanvandenberg
 */

#include <JOInterface.hpp>

#include <SDL.h>
#include <JRenderer.hpp>
#include <JCursor.hpp>

using namespace Renderer;




////////// JWindow Data holder //////////

void JWindowData::setTextures( JTexture* cornerTop, JTexture* cornerBottom, JTexture* edgeLeft, JTexture* edgeTop, JTexture* edgeBottom ) {
	symetricX = true;

	tCorner0 = cornerTop;
	tCorner1 = cornerTop;
	tCorner2 = cornerBottom;
	tCorner3 = cornerBottom;
	tEdgeLeft = edgeLeft;
	tEdgeRight = edgeLeft;
	tEdgeTop = edgeTop;
	tEdgeBottom = edgeBottom;

	if (edgeTop != NULL) {
		titleBarHeight = edgeTop->height;
	}
}


void JWindowData::setButtonTextures( JTexture* close, JTexture* collapse1, JTexture* collapse2, JTexture* minimize1, JTexture* minimize2 ) {
	tButtonClose = close;
	tButtonCollapse1 = collapse1;
	tButtonCollapse2 = collapse2;
	tButtonMinimize1 = minimize1;
	tButtonMinimize2 = minimize2;
}


void JWindowData::setMargins( float left, float right, float top, float bottom ) {
	marginLeft = left;
	marginRight = right;
	marginTop = top;
	marginBottom = bottom;
}


void JWindowData::setMargins( float margins ) {
	setMargins(margins, margins, margins, margins);
}


void JWindowData::setMarginsX( float margins ) {
	setMargins(margins, margins, marginTop, marginBottom);
}


void JWindowData::setMarginsY( float margins) {
	setMargins(marginLeft, marginRight, margins, margins);
}




////////// JObject JOWindow Component //////////

JOWindow::JOWindow( JWindowData* windowData, JObject* object ) {
	this->windowData = windowData;

	collapsed = false;
	minimized = false;

	maximizedPosX = 0.0f;
	maximizedPosY = 0.0f;
	maximizedWidth = 0.0f;

	if (object != NULL) {
		object->isHUD = true;

		JObject* frameObject = new JObject(true);
		frameObject->addComponent(new JOFrame());
		frameObject->isMask = true;
		frameObject->init(object->getPosX(), object->getPosY(), object->getWidth(), object->getHeight(), "window frame");
		object->addChild(*frameObject);
	}
}


void JOWindow::update() {

	if (Renderer::renderTarget->resized && Renderer::renderTarget->previousWidth != 0) {
		// Move Frame so it remains at the same position on the screen.
		object->localPosX = (object->localPosX / (float) Renderer::renderTarget->previousWidth) * (float) Renderer::renderTarget->width;
		object->localPosY = (object->localPosY / (float) Renderer::renderTarget->previousHeight) * (float) Renderer::renderTarget->height;

		if (minimized) {
			object->localPosY = Renderer::renderTarget->height - windowData->titleBarHeight;
			maximizedPosX = (maximizedPosX / (float) Renderer::renderTarget->previousWidth) * (float) Renderer::renderTarget->width;
			maximizedPosY = (maximizedPosY / (float) Renderer::renderTarget->previousHeight) * (float) Renderer::renderTarget->height;
		}
	}


	// Update buttons.
	if (windowData != NULL) {
		float buttonPosX = object->getPosX() + object->getWidth() - windowData->buttonsOffsetX;
		float buttonPosY = object->getPosY() + windowData->buttonsOffsetY;

		if (windowData->tButtonClose != NULL) {
			buttonPosX -= windowData->tButtonClose->width;
			if (Input::isAreaHovered(buttonPosX, buttonPosY, windowData->tButtonClose->width, windowData->tButtonClose->height)) {
				if (object->isClicked(true)) {
					if (object->destroyable) {
						object->destroy();
					}
					else {
						setMinimized(!minimized);
					}
				}
			}
			buttonPosX -= windowData->buttonDistance;
		}

		if (!collapsed) {
			if (windowData->tButtonCollapse1 != NULL) {
				buttonPosX -= windowData->tButtonCollapse1->width;
				if (Input::isAreaHovered(buttonPosX, buttonPosY, windowData->tButtonCollapse1->width, windowData->tButtonCollapse1->height)) {
					if (object->isClicked(true)) {
						setCollapsed(true);
					}
				}
				buttonPosX -= windowData->buttonDistance;
			}
		}
		else {
			if (windowData->tButtonCollapse2 != NULL) {
				buttonPosX -= windowData->tButtonCollapse2->width;
				if (Input::isAreaHovered(buttonPosX, buttonPosY, windowData->tButtonCollapse2->width, windowData->tButtonCollapse2->height)) {
					if (object->isClicked(true)) {
						setCollapsed(false);
					}
				}
				buttonPosX -= windowData->buttonDistance;
			}
		}

		if (!minimized) {
			if (windowData->tButtonMinimize1 != NULL) {
				buttonPosX -= windowData->tButtonMinimize1->width;
				if (Input::isAreaHovered(buttonPosX, buttonPosY, windowData->tButtonMinimize1->width, windowData->tButtonMinimize1->height)) {
					if (object->isClicked(true)) {
						setMinimized(true);
					}
				}
				buttonPosX -= windowData->buttonDistance;
			}
		}
		else {
			if (windowData->tButtonMinimize2 != NULL) {
				buttonPosX -= windowData->tButtonMinimize2->width;
				if (Input::isAreaHovered(buttonPosX, buttonPosY, windowData->tButtonMinimize2->width, windowData->tButtonMinimize2->height)) {
					if (object->isClicked(true)) {
						setMinimized(false);
					}
				}
				buttonPosX -= windowData->buttonDistance;
			}
		}

		object->minWidth = (object->getPosX() + object->getWidth()) - buttonPosX;
		object->minHeight = windowData->titleBarHeight * 1.5f;
	}

	// Resize frame so it always fits the window.

	if (object->children.size() > 0) {
		JObject* frame = object->children[0];
		frame->localPosX = 0.0f;
		frame->localPosY = 0.0f;
		frame->localWidth = object->getWidth();
		frame->localHeight = object->getHeight();

		if (windowData != NULL) {
			frame->localPosX += windowData->marginLeft;
			frame->localPosY += windowData->titleBarHeight + windowData->marginTop;
			frame->localWidth -= windowData->marginLeft + windowData->marginRight;
			frame->localHeight -= windowData->titleBarHeight + windowData->marginTop + windowData->marginBottom;
		}

		if (frame->localWidth != frame->prevWidth || frame->localHeight != frame->prevHeight) {
			if (frame->getComponent<JOFrame>() != NULL) {
				frame->getComponent<JOFrame>()->updateChildPositions();
			}
		}
	}
}


void JOWindow::render() {

	int corner0Width = 0;
	int corner0Height = 0;
	int corner1Width = 0;
	int corner1Height = 0;
	int corner2Width = 0;
	int corner2Height = 0;
	int corner3Width = 0;
	int corner3Height = 0;

	int edgeLeftWidth = 0;
	int edgeRightWidth = 0;
	int edgeTopHeight = 0;
	int edgeBottomHeight = 0;

	resetRenderColor();

	if (windowData != NULL) {
		// Render corners.
		if (windowData->tCorner0 != NULL) {  // Top left.
			corner0Width = windowData->tCorner0->width - windowData->textureOffset;
			corner0Height = windowData->tCorner0->height - windowData->textureOffset;
			renderTexture(*windowData->tCorner0, object->getPosX() - windowData->textureOffset, object->getPosY() + windowData->textureOffset);
		}
		if (windowData->tCorner1 != NULL) {  // Top right.
			corner1Width = windowData->tCorner1->width - windowData->textureOffset;
			corner1Height = windowData->tCorner1->height - windowData->textureOffset;
			SDL_RendererFlip flip = SDL_FLIP_NONE;
			if (windowData->symetricX) {
				flip = SDL_FLIP_HORIZONTAL;
			}
			renderTexture(*windowData->tCorner1, object->getPosX() + object->getWidth() - corner1Width, object->getPosY() - windowData->textureOffset, NAF, NAF, NULL, flip);
		}
		if (!collapsed) {
			if (windowData->tCorner2 != NULL) {  // Bottom right.
				corner2Width = windowData->tCorner2->width - windowData->textureOffset;
				corner2Height = windowData->tCorner2->height - windowData->textureOffset;
				SDL_RendererFlip flip = SDL_FLIP_NONE;
				if (windowData->symetricX) {
					flip = SDL_FLIP_HORIZONTAL;
				}
				renderTexture(*windowData->tCorner2, object->getPosX() + object->getWidth() - corner2Width, object->getPosY() + object->getHeight() - corner2Height, NAF, NAF, NULL, flip);
			}
			if (windowData->tCorner3 != NULL) {  // Bottom left.
				corner3Width = windowData->tCorner3->width - windowData->textureOffset;
				corner3Height = windowData->tCorner3->height - windowData->textureOffset;
				renderTexture(*windowData->tCorner3, object->getPosX() - windowData->textureOffset, object->getPosY() + object->getHeight() - corner3Height);
			}
		}

		// Render edges.
		if (windowData->tEdgeTop != NULL) {
			edgeTopHeight = windowData->tEdgeTop->height - windowData->textureOffset;
			renderTexture(*windowData->tEdgeTop, object->getPosX() + corner0Width, object->getPosY() - windowData->textureOffset, object->getWidth() - corner0Width - corner1Width, NAF);
		}
		if (!collapsed) {
			if (windowData->tEdgeLeft != NULL) {
				edgeLeftWidth = windowData->tEdgeLeft->width - windowData->textureOffset;
				renderTexture(*windowData->tEdgeLeft, object->getPosX() - windowData->textureOffset, object->getPosY() + corner0Height, NAF, object->getHeight() - corner0Height - corner3Height);
			}
			if (windowData->tEdgeRight != NULL) {
				edgeRightWidth = windowData->tEdgeRight->width - windowData->textureOffset;
				SDL_RendererFlip flip = SDL_FLIP_NONE;
				if (windowData->symetricX) {
					flip = SDL_FLIP_HORIZONTAL;
				}
				renderTexture(*windowData->tEdgeRight, object->getPosX() + object->getWidth() - edgeRightWidth, object->getPosY() + corner1Height, NAF, object->getHeight() - corner1Height - corner2Height, NULL, flip);
			}

			if (windowData->tEdgeBottom != NULL) {
				edgeBottomHeight = windowData->tEdgeBottom->height - windowData->textureOffset;
				renderTexture(*windowData->tEdgeBottom, object->getPosX() + corner3Width, object->getPosY() + object->getHeight() - edgeBottomHeight, object->getWidth() - corner3Width - corner2Width, NAF);
			}
		}

		// Render inside of window.
		if (!collapsed) {
			setRenderColor(windowData->backgroundColor);
			renderRect(object->getPosX() + edgeLeftWidth, object->getPosY() + edgeTopHeight, object->getWidth() - edgeLeftWidth - edgeRightWidth, object->getHeight() - edgeTopHeight - edgeBottomHeight);
		}


		// Render buttons.
		float buttonPosX = object->getPosX() + object->getWidth() - windowData->buttonsOffsetX;
		float buttonPosY = object->getPosY() + windowData->buttonsOffsetY;

		resetRenderColor();
		if (windowData->tButtonClose != NULL) {
			buttonPosX -= windowData->tButtonClose->width;
			renderTexture(*windowData->tButtonClose, buttonPosX, buttonPosY, NAF, NAF);
			buttonPosX -= windowData->buttonDistance;
		}
		if (!collapsed) {
			if (windowData->tButtonCollapse1 != NULL) {
				buttonPosX -= windowData->tButtonCollapse1->width;
				renderTexture(*windowData->tButtonCollapse1, buttonPosX, buttonPosY, NAF, NAF);
				buttonPosX -= windowData->buttonDistance;
			}
		}
		else {
			if (windowData->tButtonCollapse2 != NULL) {
				buttonPosX -= windowData->tButtonCollapse2->width;
				renderTexture(*windowData->tButtonCollapse2, buttonPosX, buttonPosY, NAF, NAF);
				buttonPosX -= windowData->buttonDistance;
			}
		}
		if (!minimized) {
			if (windowData->tButtonMinimize1 != NULL) {
				buttonPosX -= windowData->tButtonMinimize1->width;
				renderTexture(*windowData->tButtonMinimize1, buttonPosX, buttonPosY, NAF, NAF);
				buttonPosX -= windowData->buttonDistance;
			}
		}
		else {
			if (windowData->tButtonMinimize2 != NULL) {
				buttonPosX -= windowData->tButtonMinimize2->width;
				renderTexture(*windowData->tButtonMinimize2, buttonPosX, buttonPosY, NAF, NAF);
				buttonPosX -= windowData->buttonDistance;
			}
		}


		// Render title.
		if (windowData->titleFont != NULL) {
			renderString(*windowData->titleFont, object->name, object->getPosX() + windowData->titleOffsetX, object->getPosY() + windowData->titleOffsetY, buttonPosX - object->getPosX() - windowData->titleOffsetX * 2);
		}
	}
}



JOFrame* JOWindow::getFrame() {
	for (int i = 0; i < object->children.size(); i++) {
		JObject* child = object->children[i];
		if (child->getComponent<JOFrame>() != NULL) {
			return child->getComponent<JOFrame>();
		}
	}
	return NULL;
}



void JOWindow::setCollapsed( bool collapse ) {
	if (collapse) {
		collapsed = true;
		for (int i = 0; i < object->children.size(); i++) {
			object->children[i]->setActive(false);
		}

	}
	else {
		collapsed = false;
		for (int i = 0; i < object->children.size(); i++) {
			object->children[i]->setActive(true);
		}
	}
}


float minimizedWindowsTotalWidth = 0.0f;

void JOWindow::setMinimized( bool minimize ) {

	float minimizedWindowWidth = 60.0f;

	if (minimize) {
		minimized = true;
		maximizedPosX = object->localPosX;
		maximizedPosY = object->localPosY;
		maximizedWidth = object->localWidth;

		object->localWidth = minimizedWindowWidth;
		object->setPosX(minimizedWindowsTotalWidth);
		minimizedWindowsTotalWidth += minimizedWindowWidth;

		int posY = Renderer::renderTarget->height;
		if (windowData != NULL) {
			posY -= windowData->titleBarHeight;
		}
		object->setPosY(posY);

		if (object->getComponent<JOResizer>() != NULL) {
			object->getComponent<JOResizer>()->active = false;
		}
	}
	else {
		minimized = false;

		object->localPosX = maximizedPosX;
		object->localPosY = maximizedPosY;
		object->localWidth = maximizedWidth;

		minimizedWindowsTotalWidth -= minimizedWindowWidth;

		if (object->getComponent<JOResizer>() != NULL) {
			object->getComponent<JOResizer>()->active = true;
		}
	}
}


bool JOWindow::isCollapsed() {
	return collapsed;
}


bool JOWindow::isMinimized() {
	return minimized;
}




////////// JObject UI Resizer Component //////////

JOResizer::JOResizer( bool dragonly ) {
	active = true;

	dragOnly = dragonly;
	lineWidth = 2.0f;

	for (int i = 0; i < Input::pointerAmount; i++) {
		dragState[i] = 0;
	}
}


void JOResizer::update() {
	object->consumePointers();

	Vector2 hitboxPos = Vector2(object->getPosX() - lineWidth, object->getPosY() - lineWidth);
	Vector2 hitboxSize = Vector2(object->getWidth() + lineWidth * 2, object->getHeight() + lineWidth * 2);

	for (int i = 0; i < Input::pointerAmount; i++) {

		// Consume pointers manually, to account for the larger hitbox.
		InputPointer* pointer = &Input::pointers[i];
		if (pointer->active && pointer->pressed && !pointer->pressConsumed) {
			if (Input::screenToRender(pointer->pressedPosition).isInside(hitboxPos, hitboxSize)) {
				object->ownsPointers[i] = true;
				pointer->pressConsumed = true;
			}
		}

		if (object->ownsPointers[i]) {
			handleResizing(i);
		}
	}
}


void JOResizer::render() {
}



void JOResizer::handleResizing( int index ) {

	InputPointer* pointer = &Input::pointers[index];
	Vector2 pPos = Input::screenToRender(pointer->position) - object->getPos();

	// If dragstate has been set, move or resize JObject with pointer (mouse).
	if (dragState[index] != 0) {

		float diffX = Input::screenToRenderX(pointer->position.x) - Input::screenToRenderX(pointer->prevPosition.x);
		float diffY = Input::screenToRenderY(pointer->position.y) - Input::screenToRenderY(pointer->prevPosition.y);

		if ((dragState[index] & 1) != 0) {
			// Drag top side.
			object->localPosY += diffY;
			object->localHeight -= diffY;
			Cursor::set(CURSOR_SIZEALL);
		}
		if ((dragState[index] & 2) != 0) {
			// Drag right side.
			object->localWidth += diffX;
			Cursor::set(CURSOR_SIZEALL);
		}
		if ((dragState[index] & 4) != 0) {
			// Drag bottom side.
			object->localHeight += diffY;
			Cursor::set(CURSOR_SIZEALL);
		}
		if ((dragState[index] & 8) != 0) {
			// Drag left side.
			object->localPosX += diffX;
			object->localWidth -= diffX;
			Cursor::set(CURSOR_SIZEALL);
		}


		if (pointer->released) {
			dragState[index] = 0;

			// If objects width/height was set lower than allowed, correct that.
			if (object->localWidth < object->minWidth) {
				object->localWidth  = object->minWidth;
			}
			if (object->localHeight < object->minHeight) {
				object->localHeight  = object->minHeight;
			}
		}
	}
	else {
		// dragState is set to newDragState when the pointer (the mouse) is pressed.
		short newDragState = 0;
		bool canDrag = true;

		// If JObject has a windowComponent, then this object should only be draggable from his/her titlebar.
		JOWindow* windowComponent = object->getComponent<JOWindow>();
		if (windowComponent != NULL && windowComponent->windowData != NULL && windowComponent->windowData->titleBarHeight != 0) {
			canDrag = canDrag && pPos.y < windowComponent->windowData->titleBarHeight;
		}


		if (dragOnly) {
			if (canDrag && pPos.isInside(0.0f, 0.0f, object->getWidth(), object->getHeight())) {
				// Drag all sides.
				newDragState = 1 | 2 | 4 | 8;
				Cursor::set(CURSOR_HAND);
			}
		}
		else {
			if (pPos.isInside(lineWidth, lineWidth, object->getWidth() - lineWidth * 2, object->getHeight() - lineWidth * 2)) {
				if (canDrag) {
					newDragState = 1 | 2 | 4 | 8;  // Inside.
					Cursor::set(CURSOR_HAND);
				}
			}
			else {
				if (pPos.y < lineWidth) {
					// Top side.
					newDragState = newDragState | 1;
					Cursor::set(CURSOR_SIZENS);
				}
				if (pPos.x >= object->getWidth() - lineWidth) {
					// Right side.
					newDragState = newDragState | 2;
					Cursor::set(CURSOR_SIZEWE);
				}
				if (pPos.y >= object->getHeight() - lineWidth) {
					// Bottom side.
					newDragState = newDragState | 4;
					Cursor::set(CURSOR_SIZENS);
				}
				if (pPos.x < lineWidth) {
					// Left side.
					newDragState = newDragState | 8;
					Cursor::set(CURSOR_SIZEWE);
				}
			}
		}


		// If pointer is pressed, set dragstate to newdragstate, determined above ^.
		if (newDragState != 0 && Input::pointers[index].pressed && !Input::pointers[index].released) {
			dragState[index] = newDragState;

			// Move this object a little bit further to the front, so it will lay on top of other objects with the same base z position.
			//TODO: dit kan beter, maar ik ben nu eigenlijk te lui.
			static float zOffset = 0.0f;
			zOffset += 0.001f;
			object->setLocalPosZ(floor(object->localPosZ) + zOffset);
		}
	}
}



bool JOResizer::isBeingDragged() {
	for (int i = 0; i < Input::pointerAmount; i++) {
		if (object->ownsPointers[i] && dragState[i] != 0) {
			return true;
		}
	}
	return false;
}




////////// JObject JOFrame Component //////////

JOFrame::JOFrame() {
	currentRow = 0;
	currentColumn = 0;

	canScrollX = true;
	canScrollY = true;
	scrollRangeX = 0;
	scrollRangeY = 0;
	scrollPosX = 0.0f;
	scrollPosY = 0.0f;

	lastObjectIndex = 0;
}



void JOFrame::update() {

	float scrollX = Input::getMouseScrollDiffX(object->getPosX(), object->getPosY(), object->getWidth(), object->getHeight());
	float scrollY = Input::getMouseScrollDiffY(object->getPosX(), object->getPosY(), object->getWidth(), object->getHeight());

	if (scrollPosX + scrollX > scrollRangeX) {
		scrollX = scrollRangeX - scrollPosX;
	}
	if (scrollPosX + scrollX < 0.0f) {
		scrollX = 0.0f - scrollPosX;
	}
	if (scrollPosY + scrollY > scrollRangeY) {
		scrollY = scrollRangeY - scrollPosY;
	}
	if (scrollPosY + scrollY < 0.0f) {
		scrollY = 0.0f - scrollPosY;
	}

	scrollPosX += scrollX;
	scrollPosY += scrollY;

	if (scrollX != 0 || scrollY != 0) {
		for (int i = 0; i < object->children.size(); i++) {
			object->children[i]->localPosX -= scrollX;
			object->children[i]->localPosY -= scrollY;
		}
	}
}



void JOFrame::render() {
	if (object->sizeChanged) {
		updateChildPositions();
	}
}



void JOFrame::addRow( int height, bool fillRemainder ) {
	int rowHeight = height;
	if (fillRemainder) {
		rowHeight += 1000000;
	}
	rows.push_back(rowHeight);
	currentRow = rows.size() - 1;
}


void JOFrame::activeRow( int row ) {
	currentRow = row;
}


void JOFrame::addColumn( int width, bool fillRemainder ) {
	int columnWidth = width;
	if (fillRemainder) {
		columnWidth += 1000000;
	}
	columns.push_back(columnWidth);
	currentColumn = columns.size() - 1;
}


void JOFrame::activeColumn( int column ) {
	currentColumn = column;
}



void JOFrame::insertRow( int at, int height, bool fillRemainder, bool moveChildren ) {
	int rowHeight = height;
	if (fillRemainder) {
		rowHeight += 1000000;
	}
	if (moveChildren) {
		for (int i = 0; i < object->children.size(); i++) {
			JOAligner* childAligner = object->children[i]->getComponent<JOAligner>();
			if (childAligner != NULL && childAligner->row >= at) {
				// Move child one row down, so they remain in the same row after this new one has been inserted.
				childAligner->row++;
			}
		}
	}

	if (at > rows.size()) {
		at = rows.size();
	}
	rows.insert(rows.begin() + at, rowHeight);
	currentRow = at;
}


void JOFrame::removeRow( int row, bool removeChildren ) {
	if (removeChildren) {
		for (int i = object->children.size() - 1; i >= 0; i--) {
			JOAligner* childAligner = object->children[i]->getComponent<JOAligner>();

			if (childAligner != NULL) {
				if (childAligner->row >= row) {
					// Move all children on rows after this row one row down (so they remain at the same row).
					childAligner->row--;
				}
				else if (childAligner->row == row) {
					// Remove all objects that are on this row.
					if (object->children[i]->destroyable) {
						object->children[i]->destroy();
					}
					else {
						object->children[i]->setParent(NULL);
					}
				}
			}
		}
	}

	rows.erase(rows.begin() + row);
}


void JOFrame::insertColumn( int at, int width, bool fillRemainder, bool moveChildren ) {
	int columnWidth = width;
	if (fillRemainder) {
		columnWidth += 1000000;
	}
	if (moveChildren) {
		for (int i = 0; i < object->children.size(); i++) {
			JOAligner* childAligner = object->children[i]->getComponent<JOAligner>();
			if (childAligner != NULL && childAligner->column >= at) {
				// Move child one column left, so they remain in the same column after this new one has been inserted.
				childAligner->column++;
			}
		}
	}

	if (at > columns.size()) {
		at = columns.size();
	}
	columns.insert(columns.begin() + at, columnWidth);
	currentColumn = at;
}


void JOFrame::removeColumn( int column, bool removeChildren ) {
	if (removeChildren) {
		for (int i = object->children.size() - 1; i >= 0; i--) {
			JOAligner* childAligner = object->children[i]->getComponent<JOAligner>();

			if (childAligner != NULL) {
				if (childAligner->column >= column) {
					// Move all children on columns after this colum one column down (so they remain at the same column).
					childAligner->column--;
				}
				else if (childAligner->column == column) {
					// Remove all objects that are on this column.
					if (object->children[i]->destroyable) {
						object->children[i]->destroy();
					}
					else {
						object->children.erase(object->children.begin() + i);
					}
				}
			}
		}
	}
	columns.erase(columns.begin() + column);
}



void JOFrame::addObject( JObject* childObject, JAlignment alignmentX, JAlignment alignmentY, float resizeRatioX, float resizeRatioY ) {

	// Find right index to place the object, based on row.
	int index = 0;
	for (; index < object->children.size(); index++) {
		JOAligner* childAligner = object->children[index]->getComponent<JOAligner>();
		if (childAligner != NULL && childAligner->row > currentRow) {
			break;
		}
	}

	if (childObject != NULL) {
		JOAligner* childAligner = childObject->getComponent<JOAligner>();
		if (childAligner != NULL) {
			childAligner->alignmentX = alignmentX;
			childAligner->alignmentY = alignmentY;
			childAligner->resizeRatioX = resizeRatioX;
			childAligner->resizeRatioY = resizeRatioY;
		}
		else {
			childObject->addComponent(new JOAligner(currentRow, currentColumn, alignmentX, alignmentY, resizeRatioX, resizeRatioY));
		}
		object->addChild(*childObject, index);
	}
	else {
		childObject = new JObject(true);
		childObject->addComponent(new JOAligner(currentRow, currentColumn, alignmentX, alignmentY, resizeRatioX, resizeRatioY));
		childObject->init(0, 0, 10, 10, "[generated frame object]");
		object->addChild(*childObject, index);
	}

	lastObjectIndex = index;
}


void JOFrame::removeObjects( int afterRow ) {
	for (int i = 0; i < object->children.size(); i++) {
		JOAligner* childAligner = object->children[i]->getComponent<JOAligner>();
		if (childAligner == NULL || (childAligner != NULL && childAligner->row >= afterRow)) {
			if (object->children[i]->destroyable) {
				object->children[i]->destroy();
			}
			else {
				object->children[i]->setParent(NULL);
			}
			i--;
		}
	}

	if (afterRow != 0) {
		rows.erase(rows.begin() + afterRow, rows.end());
	}
}


std::vector<JObject*>* JOFrame::objects() {
	return &object->children;
}


JObject* JOFrame::lastObject( int offset ) {
	if (lastObjectIndex - offset < object->children.size() && lastObjectIndex - offset >= 0) {
		return object->children[lastObjectIndex - offset];
	}
	else {
		return NULL;
	}
}



void JOFrame::updateChildPositions() {

	std::vector<JObject*> unplacedChildren = object->children;

	float frameHeight = object->getHeight();
	float frameWidth = object->getWidth();

	int staticRowsSize = 0;
	int fillerRowsAmount = 0;
	int staticColumnsSize = 0;
	int fillerColumnsAmount = 0;

	for (int i = 0; i < rows.size(); i++) {
		if (rows[i] < 1000000) {
			staticRowsSize += rows[i];
		}
		else {
			fillerRowsAmount++;
		}
	}
	for (int i = 0; i < columns.size(); i++) {
		if (columns[i] < 1000000) {
			staticColumnsSize += columns[i];
		}
		else {
			fillerColumnsAmount++;
		}
	}


	float rowStartPosY = 0.0f;
	float columnStartPosX = 0.0f;

	for (int r = 0; r < rows.size() + 1; r++) {
		int rowHeight = 0;
		if (r < rows.size()) {
			rowHeight = rows[r];
			if (rowHeight >= 1000000) {
				rowHeight = (frameHeight - (float) staticRowsSize) / (float) fillerRowsAmount;
				if (rowHeight < rows[r] - 1000000) {
					rowHeight = rows[r] - 1000000;
				}
			}
		}
		else if (frameHeight > rowStartPosY) {
			// Fill remainder of frame with one last row, and put al the remaining JObjects in this row.
			rowHeight = frameHeight - rowStartPosY;
		}

		float rowEndPosY = rowStartPosY + (float) rowHeight;
		columnStartPosX = 0.0f;

		for (int c = 0; c < columns.size() + 1; c++) {
			int columnWidth = 0;
			if (c < columns.size()) {
				columnWidth = columns[c];
				if (columnWidth >= 1000000) {
					columnWidth = (frameWidth - (float) staticColumnsSize) / (float) fillerColumnsAmount;
					if (columnWidth < columns[c] - 1000000) {
						columnWidth = columns[c] - 1000000;
					}
				}
			}
			else if (frameWidth > columnStartPosX) {
				// Fill remainder of frame with one last column, and put al the remaining JObjects in this column.
				columnWidth = frameWidth - columnStartPosX;
			}

			float columnEndPosX = columnStartPosX + (float) columnWidth;
			float columnCenterPosX = (columnStartPosX + columnEndPosX) / 2.0f;

			float centerWidth = 0.0f;
			float* centerChildrenPossX[unplacedChildren.size()];
			int centerChildrenAmount = 0;


			for (unsigned int i = 0; i < unplacedChildren.size(); i++) {
				JObject* child = unplacedChildren[i];
				JOAligner* al = child->getComponent<JOAligner>();

				if (al != NULL) {
					bool placeObject = ((al->row == r || r == rows.size()) && (al->column == c || c == columns.size()));
					if (placeObject) {
						unplacedChildren.erase(unplacedChildren.begin() + i);
						i--;

						if (al->resizeRatioX > 0.0f) {
							child->localWidth = (columnEndPosX - columnStartPosX) * al->resizeRatioX - al->marginLeft - al->marginRight;
						}

						if (al->resizeRatioY > 0.0f) {
							child->localHeight = (rowEndPosY- rowStartPosY) * al->resizeRatioY - al->marginTop - al->marginBottom;
						}

						switch (al->alignmentX) {
						default :
						case ALIGN_LEFT :
							columnStartPosX += al->marginLeft;
							child->localPosX = columnStartPosX;
							columnStartPosX += child->getWidth() + al->marginRight;
							break;
						case ALIGN_RIGHT :
							columnEndPosX -= child->getWidth() + al->marginRight;
							child->localPosX = columnEndPosX;
							columnEndPosX -= al->marginLeft;
							break;
						case ALIGN_CENTER :
							float offset = (child->getWidth() + al->marginLeft + al->marginRight) / 2.0f;
							columnCenterPosX -= offset;
							child->localPosX = columnCenterPosX + al->marginLeft + centerWidth;
							centerWidth += offset * 2.0f;
							for (int i = 0; i < centerChildrenAmount; i++) {
								*centerChildrenPossX[i] -= offset;
							}
							centerChildrenPossX[centerChildrenAmount] = &child->localPosX;
							centerChildrenAmount++;
							break;
						}

						switch (al->alignmentY) {
						default :
						case ALIGN_TOP :
							child->localPosY = rowStartPosY + al->marginTop;
							break;
						case ALIGN_BOTTOM :
							child->localPosY = rowEndPosY - child->getHeight() - al->marginBottom;
							break;
						case ALIGN_CENTER :
							child->localPosY = (rowStartPosY + rowEndPosY - child->getHeight()) / 2.0f;
							break;
						}
					}
				}
			}

			columnStartPosX = columnEndPosX;
		}

		rowStartPosY = rowEndPosY;
	}


	if (columnStartPosX > frameWidth) {
		scrollRangeX = columnStartPosX - frameWidth;
	}
	else {
		scrollPosX = 0.0f;
	}

	if (scrollPosX > scrollRangeX) {
		scrollPosX = scrollRangeX;
	}

	if (rowStartPosY > frameHeight) {
		scrollRangeY = rowStartPosY - frameHeight;
	}
	else {
		scrollRangeY = 0.0f;
	}
	if (scrollPosY > scrollRangeY) {
		scrollPosY = scrollRangeY;
	}

	for (int i = 0; i < object->children.size(); i++) {
		object->children[i]->localPosX -= scrollPosX;
		object->children[i]->localPosY -= scrollPosY;
	}
}




////////// JObject UI Aligner Component //////////

JOAligner::JOAligner( int row, int column, JAlignment alignmentx, JAlignment alignmenty, float resizeratioX, float resizeratioY ) {
	this->row = row;
	this->column = column;
	alignmentX = alignmentx;
	alignmentY = alignmenty;
	marginLeft = 0.0f;
	marginRight = 0.0f;
	marginTop = 0.0f;
	marginBottom = 0.0f;

	resizeRatioX = resizeratioX;
	resizeRatioY = resizeratioY;
}


JOAligner::JOAligner( JOAligner& alignTemplate ) {
	row = alignTemplate.row;
	column = alignTemplate.row;
	alignmentX = alignTemplate.alignmentX;
	alignmentY = alignTemplate.alignmentY;
	marginLeft = alignTemplate.marginLeft;
	marginRight = alignTemplate.marginRight;
	marginTop = alignTemplate.marginTop;
	marginBottom = alignTemplate.marginBottom;
	resizeRatioX = alignTemplate.resizeRatioX;
	resizeRatioY = alignTemplate.resizeRatioY;
}


void JOAligner::setMargins( float left, float right, float top, float bottom ) {
	marginLeft = left;
	marginRight = right;
	marginTop = top;
	marginBottom = bottom;
}


void JOAligner::setMargins( float margins ) {
	setMargins(margins, margins, margins, margins);
}


void JOAligner::setMarginsX( float margins ) {
	setMargins(margins, margins, marginTop, marginBottom);
}


void JOAligner::setMarginsY( float margins) {
	setMargins(marginLeft, marginRight, margins, margins);
}




////////// JObject UI JOButton Component //////////

JOButton::JOButton( bool active, bool* boolpointer  ) {
	activated = active;
	disabled = false;

	boolPointer = boolpointer;

	if (boolPointer != NULL) {
		*boolPointer = activated;
	}
}



void JOButton::update() {
	if (isClicked()) {
		activated = !activated;
	}

	if (boolPointer != NULL) {
		*boolPointer = activated;
	}
}


void JOButton::render() {
	int lineWidth = 1;
	int posYOffset = 0;
	if (isDown()) {
		posYOffset = 1;
	}

	setRenderColor(0xFF, 0xFF, 0xFF);
	renderRect(object->getPosX() + lineWidth, object->getPosY() + lineWidth + posYOffset, object->getWidth() - lineWidth * 2, object->getHeight() - lineWidth * 2);

	if (!disabled) {
		setRenderColor(0x20, 0x20, 0x20);
	}
	else {
		setRenderColor(0xAA, 0xAA, 0xAA);
	}

	renderRect(object->getPosX(), object->getPosY() + posYOffset, object->getWidth() - lineWidth, lineWidth);
	renderRect(object->getPosX() + object->getWidth() - lineWidth, object->getPosY() + posYOffset, lineWidth, object->getHeight() - lineWidth * 2);
	renderRect(object->getPosX(), object->getPosY() + object->getHeight() - lineWidth * 2 + posYOffset, object->getWidth(), lineWidth * 2 - posYOffset);
	renderRect(object->getPosX(), object->getPosY() + lineWidth + posYOffset, lineWidth, object->getHeight() - lineWidth * 2);

	if (isActivated()) {
		renderRect(object->getPosX() + lineWidth * 2, object->getPosY() + lineWidth * 2 + posYOffset, object->getWidth() - lineWidth * 4, object->getHeight() - lineWidth * 5);
	}
}



bool JOButton::isActivated() {
	return activated;
}


bool JOButton::isPressed() {
	return !disabled && object->isPressed();
}


bool JOButton::isDown() {
	return !disabled && object->isBeingClicked();
}


bool JOButton::isClicked() {
	return !disabled && object->isClicked();
}



