/*
 * JDevelopment.hpp
 *
 *  Created on: 29 sep. 2017
 *      Author: jurriaanvandenberg
 */

#ifndef OVERTURE_INCLUDE_JDEVELOPMENT_HPP_
#define OVERTURE_INCLUDE_JDEVELOPMENT_HPP_

#include <string>
#include <JObject.hpp>
#include <JOInterface.hpp>
#include <JResources.hpp>


namespace Dev {
	void initialize( JWindowData* windowTemplate, JBitmapFont* font  );
	void update();

	void addDebugValue( std::string name, int* int_ );
	void addDebugValue( std::string name, bool* bool_ );
	void addDebugValue( std::string name, float* float_ );
	void addDebugValue( std::string name, std::string* string_ );
}


struct JOObjectPointer : JOComponent {
	JOObjectPointer( JObject* object ) {
		pointedObject = object;
	}

	virtual void update() {};
	virtual void render() {};

	JObject* pointedObject;
};


struct DebugValue {

	DebugValue( std::string name_, short type_, void* pointer_ ) {
		name = name_;
		type = type_;
		pointer = pointer_;
	}

	std::string name;
	short type;
	void* pointer;
};


#endif /* OVERTURE_INCLUDE_JDEVELOPMENT_HPP_ */
