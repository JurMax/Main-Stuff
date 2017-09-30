/*
 * JOFont.cpp
 *
 *  Created on: 17 jun. 2017
 *      Author: jurriaanvandenberg
 */

#include <JOText.hpp>

#include <math.h>
#include <JRenderer.hpp>

using namespace Renderer;


void toggleFontStyle( FontType& font, FontType newType ) {
	if (font != newType) {
		font = newType;
	}
	else {
		font = FONT_NORMAL;
	}
}


bool charBreaksWord( char c ) {
	if (c == ' ' || c == '-' || c == '_') {
		return true;
	}
	else {
		return false;
	}
}


// How many JOText components are currently selected. If higher than 0, then Input:: will be put in textinput mode.
int selectedTextObjects = 0;

// Global and standard variables.
SDL_Color highlightColor = SDLColor(0x70, 0x80, 0xEA);
SDL_Color highlightInactiveColor = SDLColor(0xDD, 0xDD, 0xDD);
int standardTimeBetweenChars = 0;
int iBeamBlinkTime = 30;  // In miliseconds




////////// JObject JOText Component //////////


JOText::JOText( JBitmapFont* bitmapfont, SDL_Color color, std::string str, bool selectable ) {

	bitmapFontNormal = bitmapfont;
	bitmapFontBold = NULL;
	bitmapFontItalics = NULL;

	buildString = str;

	allowOverflowX = false;
	animationsEnabled = true;
	this->selectable = selectable;
	editable = true;

	baseColor = color;
	charWidthOffset = 0.0f;
	charHeightOffset = 0.0f;
	spaceWidth = bitmapfont->cellWidth / 2;


	animationTicksStart = -1;
	timeBetweenChars = standardTimeBetweenChars;

	selected = false;
	selectionStart = 0;
	selectionEnd = 0;

	textLocked = false;
	textureCreated = false;
}


JOText::~JOText() {
	lockedTexture.destroy();
	stopEditing();
}



void JOText::update() {

	// Text hasn't been build yet, so do that now. charData will always contain one char after rebuild.
	if (charData.size() == 0) {
		rebuildCharData();
	}


	if (selectable && !textLocked) {

		// Start and stop editing the contents of this JOText component when object is pressed.
		if (object->isPressed()) {
			startEditing();
			selected = true;
		}
		else if (Input::isScreenPressed() || Input::isKeyPressed(KEY_ESCAPE) || Input::isKeyPressed(KEY_ESCAPE)) {
			stopEditing();
		}


		if (selected) {

			// Select text and move iBeam using the arrow keys.
			bool shiftPressed = Input::isKeyDown(KEY_LSHIFT) || Input::isKeyDown(KEY_RSHIFT);
			if (Input::isKeyPressed(KEY_LEFT)) {
				if (shiftPressed) {
					// Only move the start of the selection one to the left.
					selectionStart--;
				}
				else if (selectionStart != selectionEnd) {
					selectionEnd = selectionStart;
				}
				else {
					selectionStart--;
					selectionEnd--;
				}
			}
			if (Input::isKeyPressed(KEY_RIGHT)) {
				if (shiftPressed) {
					// Only move the start of the selection one to the left.
					selectionEnd++;
				}
				else if (selectionStart != selectionEnd) {
					selectionStart = selectionEnd;
				}
				else {
					selectionStart++;
					selectionEnd++;
				}
			}

			// Correct if selection is out of bounds.
			if (selectionStart > charData.size() - 1) {
				selectionStart = charData.size() - 1;
			}
			if (selectionStart < 0) {
				selectionStart = 0;
			}
			if (selectionEnd > charData.size() - 1) {
				selectionEnd = charData.size() - 1;
			}
			if (selectionEnd < 0) {
				selectionEnd = 0;
			}


			// Select text and move iBeam using the mouse.
			object->consumePointers();
			for (int i = 0; i < Input::pointerAmount; i++) {
				if (object->ownsPointers[i]) {
					InputPointer* pointer = &Input::pointers[i];
					Vector2 pPPos = Input::screenToRender(pointer->pressedPosition) - object->getPos();
					Vector2 pPos = Input::screenToRender(pointer->position) - object->getPos();

					float rowHeight = bitmapFontNormal->cellHeight + charHeightOffset;
					int pressedRow = floor(pPPos.y / rowHeight);
					int currentRow = floor(pPos.y / rowHeight);
					int pressedChar = -1;
					int currentChar = -1;

					for (int i = 0; i < (int) charData.size(); i++) {
						int charRow = (int) floor(charData[i].offsetY / rowHeight);

						if (pressedChar == -1) {
							if (charRow > pressedRow) {
								pressedChar = i;
							}
							else if (charRow == pressedRow) {
								if (charData[i].offsetX + charData[i].width > pPPos.x) {
									pressedChar = i;
								}
							}
						}
						if (currentChar == -1) {
							if (charRow > currentRow) {
								currentChar = i;
							}
							else if (charRow == currentRow) {
								if (charData[i].offsetX + charData[i].width > pPos.x) {
									currentChar = i;
								}
							}
						}
					}
					if (pressedChar == -1)
						pressedChar = charData.size() - 1;
					if (currentChar == -1)
						currentChar = charData.size() - 1;

					selectionStart = std::min(pressedChar, currentChar);
					selectionEnd = std::max(pressedChar, currentChar);
				}
			}


			// Direct textinput to this JOTextComponent.
			std::string inputString = Input::getInputLineChange();
			if (inputString != "") {
				insertString(inputString, selectionEnd);
			}

			// Forward selected text to input, so it may be copied.
			if (selectionStart < selectionEnd) {
				std::string selectedText = "";
				for (int i = selectionStart; i < selectionEnd; i++) {
					if (i >= 0 && i < charData.size()) {
						selectedText += charData[i].character;
					}
				}
				Input::setSelectedText(selectedText);
			}
		}
	}
}


void JOText::render() {
	if (!textLocked) {
		if (animationTicksStart == -1) {
			animationTicksStart = SDL_GetTicks();
		}

		// Rebuild object in render loop, because otherwise there will be a 1 tick delay in the resizing of the children.
		if (object->sizeChanged && !allowOverflowX) {
			rebuildCharData();
		}


		// Render Characters.
		int ticksPassed = SDL_GetTicks() - animationTicksStart;
		SDL_Color activeColor = baseColor;

		for (int i = 0; i < (int) charData.size() - 1; i++) {
			CharData* chardata = &charData[i];

			if (ticksPassed >= chardata->appearTime) {
				JBitmapFont* activeFont = getActiveFont(chardata->font);
				activeColor = chardata->color;

				float posX = object->getPosX() + chardata->offsetX;
				float posY = object->getPosY() + chardata->offsetY;
				float charWidth = chardata->width;

				// Handle the animations
				for (unsigned int i2 = 0; i2 < chardata->animations.size(); i2++) {
					handleAnimation(chardata->animations[i2], i, posX, posY);
				}

				// Render text highlight.
				if (i >= selectionStart && i < selectionEnd) {
					setRenderColor(highlightColor);
					if (!selected) {
						setRenderColor(highlightInactiveColor);
					}
					renderRect(posX, posY, charWidth + activeFont->charDistance + charWidthOffset, activeFont->cellHeight);
				}

				// Render chararacter.
				setRenderColor(activeColor);
				renderCharacter(*activeFont, (unsigned int) chardata->character, posX, posY, charWidth, activeFont->cellHeight);
			}
		}


		// Render editing dash.
		if (editable && selected && selectionStart == selectionEnd && ticksPassed % iBeamBlinkTime * 2 > iBeamBlinkTime) {
			float posX = object->getPosX();
			float posY = object->getPosY();
			if (selectionEnd >= 0 && selectionEnd < charData.size()) {
				posX += charData[selectionEnd].offsetX;
				posY += charData[selectionEnd].offsetY;
			}
			setRenderColor(activeColor);
			renderRect(posX, posY, 1.0f, 8.0f);
		}
	}
	else {
		if (!textureCreated) {
			textureCreated = true;

			if (object->getWidth() != lockedTexture.width || object->getHeight() != lockedTexture.height) {
				lockedTexture.resize((int) object->getWidth(), (int) object->getHeight());
			}
			else if (lockedTexture.texture == NULL) {
				lockedTexture.create();
			}

			Renderer::setRenderTarget(lockedTexture);
			Renderer::pauseClippingMasks(true);
			setRenderColor(0xFF, 0xFF, 0xFF, 0x00);
			SDL_RenderClear(Renderer::rRenderer);

			Vector2 posOffset = object->getPos() - Overture::cameraPos + Vector2(lockedTexture.width / 2, lockedTexture.height / 2);
			object->localPosX -= posOffset.x;
			object->localPosY -= posOffset.y;
			textLocked = false;
			render();
			textLocked = true;
			object->localPosX += posOffset.x;
			object->localPosY += posOffset.y;

			Renderer::resetRenderTarget();
			Renderer::pauseClippingMasks(false);
		}
		resetRenderColor();
		SDL_Rect renderrect = getRenderRect(object->getPosX(), object->getPosY(), object->getWidth(), object->getHeight());
		renderSDLTexture(lockedTexture.texture, &renderrect);
	}
}



std::string JOText::getText() {
	std::string text = "";
	for (int i = 0; i < (int) charData.size() - 1; i++) {
		text += charData[i].character;
	}
	return text;
}



void JOText::startEditing() {
	if (!selected) {
		selected = true;
		if (editable) {
			if (selectedTextObjects == 0) {
				Input::startTextInput();
			}
			selectedTextObjects++;
		}
	}
}


void JOText::stopEditing() {
	if (selected) {
		selected = false;
		if (editable) {
			selectedTextObjects--;
			if (selectedTextObjects == 0) {
				Input::stopTextInput();
			}
		}
	}
}



void JOText::setString( std::string str ) {
	if (str != buildString) {
		buildString = str;
		rebuildCharData();
	}
	selectionEnd = charData.size() - 1;
	selectionStart = selectionEnd;
	animationTicksStart = SDL_GetTicks();
}


void JOText::insertString( std::string str, int at ) {
	if (str.length() > 0) {
		int i = 0;
		bool textSelected = selectionStart < selectionEnd;

		// Selected text has to be erased.
		if (textSelected) {
			i -= selectionEnd - selectionStart;
		}

		for (; i < (int) str.length(); i++) {
			bool backspaceConsumed = textSelected && i == 0;
			if ((i < 0 || str[i] == '\1')) {
				// Erase a character, provided that this backspace hasn't been 'used' in deleting the selected text.
				if (!backspaceConsumed) {
					int charindex = charIndexToStringIndex(selectionEnd - 1);
					if (charindex >= 0 && charindex < buildString.length()) {
						buildString.erase(buildString.begin() + charindex);
						selectionEnd--;
					}
				}
			}
			else if (str[i] == '\2') {
				// Delete a character (from back).
				if (!backspaceConsumed) {
					int charindex = charIndexToStringIndex(selectionEnd);
					buildString.erase(buildString.begin() + charindex);
				}
			}
			else {
				// Add a new character.
				int charindex = charIndexToStringIndex(selectionEnd);
				buildString.insert(charindex, 1, str[i]);
				selectionEnd++;
			}
		}

		rebuildCharData();

		if (selectionEnd > charData.size() - 1) {
			selectionEnd = charData.size() - 1;
		}
		if (selectionEnd < 0) {
			selectionEnd = 0;
		}

		selectionStart = selectionEnd;
	}
}


int JOText::charIndexToStringIndex( int charIndex ) {
	int textIndex = 0;
	if (charIndex < 0) {
		textIndex = -1;
	}
	else {
		for (int i = 0; i < charIndex; i++) {
			if (i < buildString.length()) {
				if (buildString[i] == '#') {
					for (int i2 = i; i2 < buildString.length(); i2++) {
						if (buildString[i2] == ' ') {
							break;
						}
						else {
							textIndex++;
						}
					}
				}
				textIndex++;
			}
		}
	}

	return textIndex;
}



void JOText::lockText() {
	textLocked = true;
	textureCreated = false;

	/*
	if (object->getWidth() != lockedTexture.width || object->getHeight() != lockedTexture.height) {
		lockedTexture.resize((int) object->getWidth(), (int) object->getHeight());
	}
	else if (lockedTexture.texture == NULL) {
		lockedTexture.create();
	}

	Renderer::setRenderTarget(lockedTexture);
	setRenderColor(0xFF, 0xFF, 0xFF, 0x00);
	SDL_RenderClear(Overture::rRenderer);

	Vector2 posOffset = object->getPos() - Overture::cameraPos + Vector2(lockedTexture.width / 2, lockedTexture.height / 2);
	object->localPosX -= posOffset.x;
	object->localPosY -= posOffset.y;
	textLocked = false;
	render();
	textLocked = true;
	object->localPosX += posOffset.x;
	object->localPosY += posOffset.y;

	Renderer::resetRenderTarget();*/
}


void JOText::unlockText( bool destroyTexture ) {
	textLocked = false;
	textureCreated = false;

	if (destroyTexture) {
		lockedTexture.destroy();
	}
}


//// Private functions ////

void JOText::rebuildCharData() {
	charData.clear();

	for (unsigned int i = 0; i < buildString.length(); i++) {
		if (buildString[i] == '#') {
			unsigned int command;
			std::string args;

			for (int i2 = i + 1; i2 < buildString.length(); i2++) {
				if (buildString[i2] == ' ') {
					break;
				}
				else {
					args += buildString[i2];
					i++;
				}
			}

			command = (unsigned int) args[0];
			args.erase(args.begin());

			applyCharCommand(getCurrentChar(), command, args);
		}
		else {
			addCharData(buildString[i]);
		}
	}
}



CharData* JOText::getCurrentChar() {
	if (charData.size() == 0) {

		// Create base char if it doesn't exist.
		CharData baseChar;
		baseChar.character = 'J';
		baseChar.offsetX = 0;
		baseChar.offsetY = charHeightOffset;
		baseChar.width = 0;
		baseChar.appearTime = 0;
		baseChar.font = FONT_NORMAL;
		baseChar.appearTime = standardTimeBetweenChars;
		baseChar.color = baseColor;
		charData.push_back(baseChar);
	}

	return &charData[charData.size() - 1];
}



void JOText::addCharData( char character ) {
	CharData* currentChar = getCurrentChar();
	JBitmapFont* activefont = getActiveFont(currentChar->font);

	int charWidth = spaceWidth;
	unsigned int charBoundsIndex = (unsigned int) character * 3;
	if (charBoundsIndex < 256 * 3 && activefont->charBounds[charBoundsIndex + 1] != -1) {
		charWidth = activefont->charBounds[charBoundsIndex + 2] - activefont->charBounds[charBoundsIndex + 1];
	}

	currentChar->character = character;
	currentChar->width = charWidth;
	currentChar->appearTime += timeBetweenChars;

	// Check for and fix character overflow (only X-axis).
	if (!allowOverflowX) {
		if (currentChar->offsetX + currentChar->width > object->getWidth()) {
			// Character overflows the object bounds, so move the characters of the last word down a row.

			currentChar->offsetY += activefont->cellHeight + charHeightOffset;
			currentChar->offsetX = 0;

			// Iterate back to find the last space, then iterate forward to rebuild the word.
			for (int i2 = charData.size() - 1; i2 >= 0; i2--) {
				char c2 = charData[i2].character;
				if (charBreaksWord(c2)) {
					for (int i3 = i2 + 1; i3 < charData.size() - 1; i3++) {
						charData[i3].offsetX = currentChar->offsetX;
						charData[i3].offsetY = currentChar->offsetY;
						currentChar->offsetX += charData[i3].width + activefont->charDistance + charWidthOffset;
					}
					break;
				}
			}
		}
	}
	else {
		if (object->getWidth() < currentChar->offsetX + currentChar->width) {
			object->localWidth = currentChar->offsetX + currentChar->width;
		}
	}


	// Create CharData for next chararacter.
	CharData newCharData = *currentChar;

	if (character == '\n') {
		newCharData.offsetX = 0;
		newCharData.offsetY += bitmapFontNormal->cellHeight + charHeightOffset;
	}
	else {
		newCharData.offsetX += currentChar->width + activefont->charDistance + charWidthOffset;
	}
	charData.push_back(newCharData);
}


void JOText::applyCharCommand( CharData* charData, unsigned int command, std::string arguments ) {
	if (charData != NULL) {
		switch (command) {
		// Change font style.
		case 'N' :
			charData->font = FONT_NORMAL;
			break;
		case 'B' :
			toggleFontStyle(charData->font, FONT_BOLD);
			break;
		case 'L' :
			toggleFontStyle(charData->font, FONT_ITALIC);
			break;
		case 'U' :
			toggleFontStyle(charData->font, FONT_UNDERLINE);
			break;
		case 'K' :
			toggleFontStyle(charData->font, FONT_STRIKETHROUGH);
			break;

		// Set animations.
		case 's' : charData->animations.push_back(ANIMATION_SHAKE_1);  break;
		case 'S' : charData->animations.push_back(ANIMATION_SHAKE_2);  break;
		case 'w' : charData->animations.push_back(ANIMATION_WAVE_1);  break;
		case 'W' : charData->animations.push_back(ANIMATION_WAVE_2);  break;
		case 'o' : charData->animations.push_back(ANIMATION_WOBBLE_1);  break;
		case 'O' : charData->animations.push_back(ANIMATION_WOBBLE_2);  break;
		case 'a' : charData->animations.push_back(ANIMATION_SCALE_1);  break;
		case 'A' : charData->animations.push_back(ANIMATION_SCALE_2);  break;
		case 'i' : charData->animations.push_back(ANIMATION_TILT_1);  break;
		case 'I' : charData->animations.push_back(ANIMATION_TILT_2);  break;
		case 'c' : charData->animations.push_back(ANIMATION_COLOR_1);  break;
		case 'C' : charData->animations.push_back(ANIMATION_COLOR_2);  break;
		case 'e' : case 'E' : if (charData->animations.size() > 0) charData->animations.pop_back();  break;

		// Set chararacter appear time.
		case 't' : case 'T' : {
			if (stringIsInt(arguments)) {
				timeBetweenChars = atoi(arguments.c_str());
			}
			else {
				timeBetweenChars = standardTimeBetweenChars;
			}
		}
			break;

		case '#' :
			addCharData('#');
			break;

		default :
			break;
		}
	}
}



JBitmapFont* JOText::getActiveFont( FontType fonttype ) {
	JBitmapFont* activeFont = bitmapFontNormal;
	if (fonttype == FONT_BOLD && bitmapFontBold != NULL) {
		activeFont = bitmapFontBold;
	}
	else if (fonttype == FONT_ITALIC && bitmapFontItalics != NULL) {
		activeFont = bitmapFontItalics;
	}
	return activeFont;
}


void JOText::handleAnimation( CharAnimation ca, int index, float& posX, float& posY ) {

	float secondsPassed = (float) (SDL_GetTicks() - animationTicksStart) / 1000.0f;
	static float range = bitmapFontNormal->cellHeight/2;

	float animationFrequency = 4.0f;
	float waveLengthPerChar = 0.25f;

	switch (ca) {
	case ANIMATION_WOBBLE_1 :
		posY += sin(animationFrequency * secondsPassed) * range;
		break;
	case ANIMATION_WOBBLE_2 :
		posY -= sin(animationFrequency * secondsPassed) * range;
		break;

	case ANIMATION_WAVE_1 :
		posY += sin(animationFrequency * secondsPassed + (float) index * waveLengthPerChar) * range;
		break;
	case ANIMATION_WAVE_2 :
		posY -= sin(animationFrequency * secondsPassed + (float) index * waveLengthPerChar) * range;
		break;

	default :
		break;
	}
}


