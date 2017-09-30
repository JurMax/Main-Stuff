/*
 * JOptions.hpp
 *
 *  Created on: 16 apr. 2017
 *      Author: jurriaanvandenberg
 */

#ifndef OVERTURE_INCLUDE_JSETTINGS_HPP_
#define OVERTURE_INCLUDE_JSETTINGS_HPP_

#include <string>

namespace Overture {
	void Settings_Init();
}

namespace Settings {
	extern bool IsMultithreaded;
	extern bool VSync;
}

/*





class JSetting {
public:
	JSetting ();

	std::string name;

};

class JSettingBool : JSetting {
};


class JSettingInt : JSetting {
};


class JSettingFloat : JSetting {
};


namespace Option {
	bool get( *JSettingBool );
	bool get( *JSettingInt );
	bool get( *JSettingFloat );
}*/



#endif /* OVERTURE_INCLUDE_JSETTINGS_HPP_ */
