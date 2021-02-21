/*
obs-midi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#pragma once
#if __has_include(<obs-frontend-api.h>)
#include <obs-frontend-api.h>
#else
#include <obs-frontend-api/obs-frontend-api.h>
#endif
#include <iostream>
#include "utils.h"
#include "obs-midi.h"
#include "qobject.h"

class OBSController : public QObject {
	Q_OBJECT
	// BUTTON ACTIONS

public:
	OBSController(MidiHook *incoming_midi_hook, int incoming_midi_value);
	~OBSController();

private:
	/**
	* Variables
	*/
	MidiHook *hook;
	int midi_value;

private:
	/**
	* Actions
	*/
	void SetCurrentScene();
	void SetPreviewScene();
	void SetCurrentSceneCollection();
	void ResetSceneItem();
	void TransitionToProgram();
	void SetCurrentTransition();
	void SetTransitionDuration();  // can also be used with cc
	void SetSourceVisibility();    // doesn't exist??
	void ToggleSourceVisibility(); //doesn't exist?
	void ToggleMute();
	void SetMute();
	void StartStopStreaming();
	void StartStreaming();
	void StopStreaming();
	void StartStopRecording();
	void StartRecording();
	void StopRecording();
	void PauseRecording();
	void ResumeRecording();
	void StartStopReplayBuffer();
	void StartReplayBuffer();
	void StopReplayBuffer();
	void SaveReplayBuffer();
	void SetCurrentProfile();
	void SetTextGDIPlusText();
	void SetBrowserSourceURL();
	void ReloadBrowserSource();
	void TakeSourceScreenshot();
	void EnableSourceFilter();
	void DisableSourceFilter();
	void ToggleSourceFilter();

	// CC ACTIONS
	void SetVolume();
	void SetSyncOffset();
	void SetSourcePosition();
	void SetSourceRotation();
	void SetSourceScale();
	void SetGainFilter();
	void SetOpacity();
	void move_t_bar();
	void play_pause_media_source();
	void toggle_studio_mode();
	void reset_stats();
	void restart_media();
	void stop_media();
	void play_media();
	void next_media();
	void prev_media();
};
