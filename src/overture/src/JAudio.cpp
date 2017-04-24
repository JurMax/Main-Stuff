/*
 * JAudio.cpp
 *
 *  Created on: 27 jan. 2017
 *      Author: jurriaanvandenberg
 */

#include <JAudio.hpp>

#include <cmath>
#include <string>
#include <stdio.h>
#include <vector>
#include <SDL_mixer.h>
#include <SDL.h>
#include <JOverture.hpp>


struct AudioData {
	int channel;
	Uint32 playtime;
	bool paused;
	bool started;
	Uint32 startticks;
	int bpm;
	JAudio *audio;
};

std::vector<AudioData> audioData;
Uint32 prefTicks;

// The range to used in audioOnBeat()
int beatRangeFor = 0;
int beatRangeAfter = 0;


bool Audio_Init() {
	bool success = true;

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		success = false;
	}

	prefTicks = SDL_GetTicks();

	return success;
}


void Audio_Update() {
	for (unsigned int i = 0; i < audioData.size(); i++) {
		int i2 = audioData.size() - 1 - i;
		AudioData *ad = &audioData.at(i2);

		if (!Mix_Playing(ad->channel)) {
			audioData.erase(audioData.begin() + i2);
		}
		else {
			if (!ad->paused) {
				if (!ad->started) {
					ad->playtime += SDL_GetTicks() - ad->startticks;
					ad->started = true;
				}
				else {
					ad->playtime += SDL_GetTicks() - prefTicks;
				}
			}
		}
	}

	prefTicks = SDL_GetTicks();
}


void playAudio( JAudio *audio, bool unique, int loop ) {
	if (audio != NULL && audio->loaded) {
		int play = true;

		if (unique) {
			for (unsigned int i = 0; i < audioData.size(); i++) {
				if (audioData.at(i).audio == audio) {
					play = false;
				}
			}
		}

		if (play) {
			AudioData ad;
			ad.channel = Mix_PlayChannel(-1, audio->chunk, loop);
			ad.playtime = 0;
			ad.paused = false;
			ad.started = false;
			ad.startticks = SDL_GetTicks();
			ad.audio = audio;
			audioData.push_back(ad);
		}
	}
	else {
		SDL_Log("WARNING: Tried to play an unloaded audio file!");
	}
}


void pauseAudio( JAudio *audio ) {
	for (unsigned int i = 0; i < audioData.size(); i++) {
		AudioData *ad = &audioData.at(i);

		if (ad->audio == audio || audio == NULL) {
			if (ad->paused) {
				ad->paused = false;
				Mix_Resume(ad->channel);
				if (!ad->started) {
					ad->startticks = SDL_GetTicks();
				}
			}
			else {
				ad->paused = true;
				Mix_Pause(ad->channel);
			}
		}
	}
}


void stopAudio( JAudio *audio ) {
	for (unsigned int i = 0; i < audioData.size(); i++) {
		AudioData *ad = &audioData.at(i);
		if (ad->audio == audio || audio == NULL) {
			Mix_HaltChannel(ad->channel);
		}
	}
}


bool audioOnBeat( JAudio *audio ) {
	for (unsigned int i = 0; i < audioData.size(); i++) {
		AudioData *ad = &audioData.at(i);
		if (ad->audio->bmp != -1) {
			if (ad->audio == audio || audio == NULL) {
				int msPerBeat = round(ad->audio->beatLength);
				int offset1 = (ad->playtime + ad->audio->offset) % msPerBeat;
				int offset2 = msPerBeat - offset1;

				if (offset1 <= beatRangeAfter || offset2 <= beatRangeFor) {
					return true;
				}
			}
		}
	}
	return false;
}


bool audioOnOffbeat( JAudio *audio ) {
	for (unsigned int i = 0; i < audioData.size(); i++) {
		AudioData *ad = &audioData.at(i);
		if (ad->audio->bmp != -1) {
			if (ad->audio == audio || audio == NULL) {
				int msPerBeat = round(ad->audio->beatLength);
				int offset1 = (ad->playtime + ad->audio->offset + msPerBeat/2) % msPerBeat;
				int offset2 = msPerBeat - offset1;

				if (offset1 <= beatRangeAfter || offset2 <= beatRangeFor) {
					return true;
				}
			}
		}
	}
	return false;
}


float beatProgress( JAudio *audio, int *ticks ) {
	float progress = -1.0f;
	for (unsigned int i = 0; i < audioData.size(); i++) {
		AudioData *ad = &audioData.at(i);
		if (ad->audio->bmp != -1) {
			if (ad->audio == audio || audio == NULL) {
				int msPerBeat = round(ad->audio->beatLength);
				int ticksTillNextBeat = msPerBeat - ((ad->playtime + ad->audio->offset) % msPerBeat);
				float newprogress = (float) (ticksTillNextBeat) / ad->audio->beatLength;

				if (progress == -1.0f || newprogress < progress) {
					progress = newprogress;
					if (ticks != NULL) {
						*ticks = ticksTillNextBeat;
					}
					// The progress of the song that started playing first is returned.
					return progress;
				}
			}
		}
	}
	return progress;
}


float offbeatProgress( JAudio *audio, int *ticks ) {
	float progress = -1.0f;
	for (unsigned int i = 0; i < audioData.size(); i++) {
		AudioData *ad = &audioData.at(i);
		if (ad->audio->bmp != -1) {
			if (ad->audio == audio || audio == NULL) {
				int msPerBeat = round(ad->audio->beatLength);
				int ticksTillNextBeat = msPerBeat - ((ad->playtime + ad->audio->offset + msPerBeat/2) % msPerBeat);
				float newprogress = (float) (ticksTillNextBeat) / ad->audio->beatLength;

				if (progress == -1.0f || newprogress < progress) {
					progress = newprogress;
					if (ticks != NULL) {
						*ticks = ticksTillNextBeat;
					}
					// The progress of the song that started playing first is returned.
					return progress;
				}
			}
		}
	}
	return progress;
}


void setBeatRange( int before, int after ) {
	beatRangeFor = before;
	beatRangeAfter = after;
}

void getBeatRange( int *before, int *after ) {
	if (before != NULL)
		*before = beatRangeFor;
	if (after != NULL)
		*after = beatRangeAfter;
}

JAudio *getPlayingMusic() {
	JAudio *jaudio;
	for (unsigned int i = 0; i < audioData.size(); i++) {
		AudioData *ad = &audioData.at(i);
		if (ad->audio->bmp != -1) {
			jaudio = ad->audio;
		}
	}
	return jaudio;
}


