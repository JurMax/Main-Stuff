/*
 * JCursor.hpp
 *
 *  Created on: 13 jun. 2017
 *      Author: jurriaanvandenberg
 */

#ifndef OVERTURE_SRC_INPUT_JCURSOR_HPP_
#define OVERTURE_SRC_INPUT_JCURSOR_HPP_


namespace Overture {
	void Cursor_Init();
	void Cursor_Close();
}


// Various Cursor ID's
const int CURSOR_ARROW = 0;     // Arrow
const int CURSOR_IBEAM = 1;     // I-beam
const int CURSOR_WAIT = 2;      // Wait
const int CURSOR_CROSSHAIR = 3; // Crosshair
const int CURSOR_WAITARROW = 4; // Small wait cursor (or Wait if not available)
const int CURSOR_SIZENWSE = 5;  // Double arrow pointing northwest and southeast
const int CURSOR_SIZENESW = 6;  // Double arrow pointing northeast and southwest
const int CURSOR_SIZEWE = 7;    // Double arrow pointing west and east
const int CURSOR_SIZENS = 8;    // Double arrow pointing north and south
const int CURSOR_SIZEALL = 9;   // Four pointed arrow pointing north, south, east, and west
const int CURSOR_NO = 10;       // Slashed circle or crossbones
const int CURSOR_HAND = 11;     // Hand
const int CURSOR_MISC = 12;     // Miscilanous


namespace Cursor {
	// Has the cursor been changed this cycle
	extern bool isSet;

	// Set current cursor. ReSet means that isSet will be ignored.
	void set( int cursor, bool reSet = false );

	// Load cursor image to a cursor, overwrites old cursor.
	int load( int cursor, const char *image[] );

	// Allocate memory for an extra cursor. Returns a new Cursor ID.
	int createCustom();

	void hide();
	void show();
	void toggleVisibility();
}


#endif /* OVERTURE_SRC_INPUT_JCURSOR_HPP_ */
