/*
 * JProfiler.cpp
 *
 *  Created on: 27 jan. 2017
 *      Author: jurriaanvandenberg
 */


#include <JProfiler.hpp>
#include <JOverture.hpp>
#include <string>
#include <SDL.h>

std::vector<Profile> profiles;


namespace Profiler {
	void printData( Profile *profile );

	Profile *getProfile( std::string profile ) {
		Profile *prfl = NULL;

		for (unsigned int i = 0; i < profiles.size(); i++) {
			if (!profile.compare(profiles.at(i).name)) {
				prfl = &profiles.at(i);
			}
		}

		if (prfl == NULL) {
			Profile newprofile;
			newprofile.name = profile;
			newprofile.started = false;
			newprofile.calls = 0;
			profiles.push_back(newprofile);
			prfl = getProfile(profile);
		}

		return prfl;
	}


	void start( std::string profile ) {
		Profile *prfl= getProfile(profile);
		if (!prfl->started) {
			prfl->started = true;
			prfl->startTime.push_back(SDL_GetTicks());
		}
		else {
			SDL_Log("PROFILER: Profile [%s] has already started", profile.c_str());
		}
	}

	void stop( std::string profile ) {
		Profile *prfl= getProfile(profile);
		if (prfl->started) {
			prfl->started = false;
			prfl->stopTime.push_back(SDL_GetTicks());
			prfl->calls += 1;
		}
		else {
			SDL_Log("PROFILER: Profile [%s] not yet started", profile.c_str());
		}
	}


	void printData( std::string profile ) {
		Profile *prfl = getProfile(profile);
		printData(prfl);
	}


	void printData( Profile *profile ) {

	}


	void printData() {
		SDL_Log(" ");
		SDL_Log("Profiler Data");
		SDL_Log("    #    Profile    Time (ms)        Calls");
		SDL_Log("  ----------------------------------------");

		for (unsigned int i = 0; i < profiles.size(); i++) {
			//printData(&profiles.at(i));
			Profile *profile = &profiles.at(i);
			int total = 0;
			for (unsigned int i2 = 0; i2 < profile->calls; i2++) {
				total += profile->stopTime.at(i2) - profile->startTime.at(i2);
			}
			float average = ((float) total) / ((float) profile->calls);
			SDL_Log("  %3s %10s %12s %12s", toString(i).c_str(), profile->name.c_str(), toString(average).c_str(), toString(profile->calls).c_str());
		}

		SDL_Log(" ");
	}


	void clearAllData() {
		profiles.clear();
	}
}

