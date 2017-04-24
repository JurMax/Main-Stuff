/*
 * JProfiler.hpp
 *
 *  Created on: 27 jan. 2017
 *      Author: jurriaanvandenberg
 */

#ifndef OVERTURE_INCLUDE_JPROFILER_HPP_
#define OVERTURE_INCLUDE_JPROFILER_HPP_

#include <string>
#include <vector>


struct Profile {
	std::string name;
	bool started;
	unsigned int calls;
	std::vector<int> startTime;
	std::vector<int> stopTime;
};


namespace profiler {
	void start( std::string profile );
	void stop( std::string profile );

	Profile *getProfile( std::string profile );
	void printData( std::string profile );

	void printData();
	void clearAllData();

}




#endif /* OVERTURE_INCLUDE_JPROFILER_HPP_ */
