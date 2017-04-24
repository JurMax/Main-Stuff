/*
 * JAudio.hpp
 *
 *  Created on: 27 jan. 2017
 *      Author: jurriaanvandenberg
 */

#ifndef OVERTURE_INCLUDE_JAUDIO_HPP_
#define OVERTURE_INCLUDE_JAUDIO_HPP_

#include <JResources.hpp>

bool Audio_Init();
void Audio_Update();

void playAudio( JAudio *audio, bool unique = false, int loop = 0 );
void pauseAudio( JAudio *audio = NULL );
void stopAudio( JAudio *audio = NULL );

bool audioOnBeat( JAudio *audio = NULL );
bool audioOnOffbeat( JAudio *audio = NULL );
float beatProgress( JAudio *audio = NULL, int *ticks = NULL );
float offbeatProgress( JAudio *audio = NULL, int *ticks = NULL );

void setBeatRange( int before, int after );
void getBeatRange( int *before, int *after );
JAudio *getPlayingMusic();


#endif /* OVERTURE_INCLUDE_JAUDIO_HPP_ */
