/*
 * JDevelopment.cpp
 *
 *  Created on: 29 sep. 2017
 *      Author: jurriaanvandenberg
 */

#include <JOverture.hpp>
#include <JDevelopment.hpp>
#include <JOText.hpp>
#include <JORender.hpp>
#include <vector>


void addObjectsToProfiler( std::vector<JObject*>* objects, JOFrame* frame, int& row );
void updateProfilerFrame();
void updateValuesFrame();

JObject profilerWindow;
JObject valuesWindow;
JBitmapFont* windowFont;

std::vector<DebugValue> debugValues;

namespace Dev {

	void initialize( JWindowData* windowTemplate, JBitmapFont* font ) {
		windowFont = font;

		valuesWindow.setLocalPosZ(1000.0f);
		valuesWindow.init(0, 0, 90, 50, "Values");
		valuesWindow.addComponent(new JOResizer());
		valuesWindow.addComponent(new JOWindow(windowTemplate, &valuesWindow));

		profilerWindow.setLocalPosZ(1000.0f);
		profilerWindow.init(0, 0, 90, 50, "Profiler");
		profilerWindow.addComponent(new JOResizer());
		profilerWindow.addComponent(new JOWindow(windowTemplate, &profilerWindow));

	}


	void update() {
		updateProfilerFrame();
		updateValuesFrame();
	}



	void addDebugValue( std::string name, int* int_ ) {
		debugValues.push_back(DebugValue(name, 1, int_));
	}

	void addDebugValue( std::string name, bool* bool_ ) {
		debugValues.push_back(DebugValue(name, 2, bool_));
	}

	void addDebugValue( std::string name, float* float_ ) {
		debugValues.push_back(DebugValue(name, 4, float_));
	}

	void addDebugValue( std::string name, std::string* string_ ) {
		debugValues.push_back(DebugValue(name, 8, string_));
	}

}


void updateProfilerFrame() {
	static bool rebuildContents = true;
	JOFrame* frame = profilerWindow.getComponent<JOWindow>()->getFrame();

	if (rebuildContents) {
		rebuildContents = false;

		frame->rows.clear();
		frame->columns.clear();
		frame->removeObjects();

		frame->addColumn(12);
		frame->addColumn(12);
		frame->addColumn(12);
		frame->addColumn(0, true);
		frame->activeColumn(0);

		frame->addRow(12);
		frame->addObject(NULL, ALIGN_LEFT, ALIGN_CENTER, 1.0f, 1.0f);
		frame->lastObject()->addComponent(new JOText(windowFont, SDLColor(0x20, 0x20, 0x20), "Framerate: ", false));
		frame->lastObject()->getComponent<JOText>()->allowOverflowX = true;

		frame->addRow(12);
		frame->addObject(NULL, ALIGN_LEFT, ALIGN_CENTER, 1.0f, 1.0f);
		frame->lastObject()->addComponent(new JOText(windowFont, SDLColor(0x20, 0x20, 0x20), "Objects: ", false));
		frame->lastObject()->getComponent<JOText>()->allowOverflowX = true;

		frame->addRow(3);  // Horizontal line
		frame->addObject(NULL, ALIGN_LEFT, ALIGN_TOP, 1000.0f, 1.0f);
		frame->lastObject()->getComponent<JOAligner>()->setMarginsY(1.0f);
		frame->lastObject()->addComponent(new JOColor(SDLColor(0x20, 0x20, 0x20)));
	}


	(*frame->objects())[0]->getComponent<JOText>()->setString("Framerate: " + toString(Overture_GetFramerate()));
	(*frame->objects())[1]->getComponent<JOText>()->setString("Objects: " + toString(JObjects::objects.size()));

	std::vector<JObject*> headParents;
	for (int i = 0; i < JObjects::objects.size(); i++) {
		if (JObjects::objects[i]->parent == NULL) {
			headParents.push_back(JObjects::objects[i]);
		}
	}

	int row = 3;
	frame->activeColumn(0);
	addObjectsToProfiler(&headParents, frame, row);

	frame->removeObjects(row);
	frame->updateChildPositions();
}


void addObjectsToProfiler( std::vector<JObject*>* objects, JOFrame* frame, int& row ) {
	for (int i = 0; i < objects->size(); i++) {

		JObject* object = (*objects)[i];
		int frameObjectIndex = (row - 3) * 2 + 3;
		bool addNewRow = false;

		if (row < frame->rows.size()) {
			JObject* rowObject = (*frame->objects())[frameObjectIndex]->getComponent<JOObjectPointer>()->pointedObject;

			if (object != rowObject) {
				addNewRow = true;
			}
		}
		else {
			addNewRow = true;
		}

		if (addNewRow) {
			frame->insertRow(row, 12);

			frame->addObject(NULL, ALIGN_CENTER, ALIGN_CENTER);
			frame->lastObject()->setSize(7.0f, 8.0f);
			frame->lastObject()->addComponent(new JOButton());
			frame->lastObject()->addComponent(new JOObjectPointer(object));

			frame->activeColumn(frame->currentColumn + 1);
			frame->addObject(NULL, ALIGN_LEFT, ALIGN_CENTER);
			frame->lastObject()->setSize(10.0f, 8.0f);
			frame->lastObject()->getComponent<JOAligner>()->setMarginsY(1.0f);
			//frame->lastObject()->addComponent(new JOColor(SDLRandomColor()));
			frame->lastObject()->addComponent(new JOText(windowFont, SDLColor(0x20, 0x20, 0x20), object->name));
			frame->lastObject()->getComponent<JOText>()->allowOverflowX = true;
			frame->activeColumn(frame->currentColumn - 1);
		}

		row++;

		JOButton* button = (*frame->objects())[frameObjectIndex]->getComponent<JOButton>();
		JOText* text = (*frame->objects())[frameObjectIndex + 1]->getComponent<JOText>();


		if (Input::isKeyPressed(KEY_Z)) {
			text->lockText();
		}
		if (Input::isKeyPressed(KEY_X)) {
			text->unlockText();
		}

		object->name = text->getText();
		button->disabled = object->children.size() == 0;

		if (object == frame->object) {
			button->disabled = true;
		}

		if (button->isActivated()) {
			frame->activeColumn(frame->currentColumn + 1);
			addObjectsToProfiler(&object->children, frame, row);
			frame->activeColumn(frame->currentColumn - 1);
		}
	}
}



void updateValuesFrame() {
	JOFrame* frame = valuesWindow.getComponent<JOWindow>()->getFrame();

	if (frame->columns.size() == 0) {
		frame->addColumn(60, true);
	}

	for (int i = 0; i < debugValues.size(); i++) {

		DebugValue debugvalue = debugValues[i];

		if (i >= frame->rows.size()) {
			// Add new value / row.

			std::string name = debugvalue.name;
			std::string value;
			if ((debugvalue.type & 1) != 0) {
				name += " [i]: ";
				value = toString(*((int*) debugvalue.pointer));
			}
			else if ((debugvalue.type & 2) != 0) {
				name += " [b]: ";
				bool isTrue = *((bool*) debugvalue.pointer);
				if (isTrue)  value = "true";
				else  value = "false";
			}
			else if ((debugvalue.type & 4) != 0) {
				name += " [f]: ";
				value = toString(*((float*) debugvalue.pointer));
			}
			else if ((debugvalue.type & 8) != 0) {
				name += " [s]: ";
				value = *(static_cast<std::string*>(debugvalue.pointer));
			}

			frame->addRow(12);

			frame->addObject(NULL, ALIGN_LEFT, ALIGN_CENTER);
			frame->lastObject()->setSize(10.0f, 8.0f);
			frame->lastObject()->addComponent(new JOText(windowFont, SDLColor(0x20, 0x20, 0x20), name));
			frame->lastObject()->getComponent<JOText>()->allowOverflowX = true;
			frame->lastObject()->getComponent<JOText>()->selectable = false;
			frame->lastObject()->getComponent<JOText>()->editable = false;
			frame->lastObject()->getComponent<JOAligner>()->setMarginsY(1.0f);
			frame->lastObject()->getComponent<JOText>()->lockText();

			frame->addObject(NULL, ALIGN_LEFT, ALIGN_CENTER);
			frame->lastObject()->setSize(10.0f, 8.0f);
			frame->lastObject()->addComponent(new JOText(windowFont, SDLColor(0x20, 0x20, 0x20), value));
			frame->lastObject()->getComponent<JOAligner>()->setMarginsY(1.0f);
			frame->lastObject()->getComponent<JOText>()->allowOverflowX = true;
		}

		JObject* nameObject = (*frame->objects())[i*2];
		JOText* text = (*frame->objects())[i*2 + 1]->getComponent<JOText>();

		nameObject->consumePointers();
		std::string str = text->getText();

		// Value is an integer.
		if ((debugvalue.type & 1) != 0) {
			int* intValue = (int*) debugvalue.pointer;

			if (text->selected) {
				if (stringIsInt(str)) {
					*intValue = atoi(str.c_str());
				}
			}
			else {
				for (int i2 = 0; i2 < Input::pointerAmount; i2++) {
					if (nameObject->ownsPointers[i2]) {
						int p = (int) Input::screenToRender(Input::pointers[i2].position).x;
						int pp = (int) Input::screenToRender(Input::pointers[i2].prevPosition).x;
						*intValue += p - pp;
					}
				}
				text->setString(toString(*intValue));
			}
		}
		// Value is a boolean
		else if ((debugvalue.type & 2) != 0) {
			bool* boolValue = (bool*) debugvalue.pointer;

			if (text->selected) {
				if (str == "true" || str == "True"|| str == "1") {
					*boolValue = true;
				}
				else {
					*boolValue = false;
				}
			}
			else {
				if (nameObject->isPressed()) {
					*boolValue = !*boolValue;
				}

				if (*boolValue) {
					text->setString("true");
				}
				else {
					text->setString("false");
				}
			}
		}
		// Value is a float.
		else if ((debugvalue.type & 4) != 0) {
			float* floatValue = (float*) debugvalue.pointer;

			if (text->selected) {
				if (stringIsFloat(str)) {
					*floatValue = stof(str);
				}
			}
			else {
				for (int i2 = 0; i2 < Input::pointerAmount; i2++) {
					if (nameObject->ownsPointers[i2]) {
						float p = Input::screenToRender(Input::pointers[i2].position).x;
						float pp = Input::screenToRender(Input::pointers[i2].prevPosition).x;
						*floatValue += p - pp;
					}
				}
				text->setString(toString(*floatValue));
			}
		}
		// Value is a string.
		else if ((debugvalue.type & 8) != 0) {
			std::string* stringValue = static_cast<std::string*>(debugvalue.pointer);

			if (text->selected) {
				*stringValue = str;
			}
			else {
				text->setString(*stringValue);
			}
		}
	}
}

