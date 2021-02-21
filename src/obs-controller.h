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
#include "midi-agent.h"
#include "obs-midi.h"
#include "qobject.h"

class OBSController: QObject {
	Q_OBJECT
// BUTTON ACTIONS

public:
	OBSController(MidiAgent *agent);
	
public	slots:
	void SetCurrentScene(MidiHook *hook, int midi_value);
	void SetPreviewScene(MidiHook *hook, int midi_value);
	void SetCurrentSceneCollection(MidiHook *hook, int midi_value);
	void ResetSceneItem(MidiHook *hook, int midi_value);
	void TransitionToProgram(MidiHook *hook, int midi_value);
	void SetCurrentTransition(MidiHook *hook, int midi_value);
	void SetTransitionDuration(MidiHook *hook, int midi_value); // can also be used with cc
	void SetSourceVisibility(MidiHook *hook, int midi_value);                   // doesn't exist??
	void ToggleSourceVisibility(MidiHook *hook, int midi_value); //doesn't exist?
	void ToggleMute(MidiHook *hook, int midi_value);
	void SetMute(MidiHook *hook, int midi_value);
	void StartStopStreaming(MidiHook *hook, int midi_value);
	void StartStreaming(MidiHook *hook, int midi_value);
	void StopStreaming(MidiHook *hook, int midi_value);
	void StartStopRecording(MidiHook *hook, int midi_value);
	void StartRecording(MidiHook *hook, int midi_value);
	void StopRecording(MidiHook *hook, int midi_value);
	void PauseRecording(MidiHook *hook, int midi_value);
	void ResumeRecording(MidiHook *hook, int midi_value);
	void StartStopReplayBuffer(MidiHook *hook, int midi_value);
	void StartReplayBuffer(MidiHook *hook, int midi_value);
	void StopReplayBuffer(MidiHook *hook, int midi_value);
	void SaveReplayBuffer(MidiHook *hook, int midi_value);
	void SetCurrentProfile(MidiHook *hook, int midi_value);
	void SetTextGDIPlusText(MidiHook *hook, int midi_value);
	void SetBrowserSourceURL(MidiHook *hook, int midi_value);
	void ReloadBrowserSource(MidiHook *hook, int midi_value);
	void TakeSourceScreenshot(MidiHook *hook, int midi_value);
	void EnableSourceFilter(MidiHook *hook, int midi_value);
	void DisableSourceFilter(MidiHook *hook, int midi_value);
	void ToggleSourceFilter(MidiHook *hook, int midi_value);

	// CC ACTIONS
	void SetVolume(MidiHook *hook, int midi_value);
	void SetSyncOffset(MidiHook *hook, int midi_value);
	void SetSourcePosition(MidiHook *hook, int midi_value);
	void SetSourceRotation(MidiHook *hook, int midi_value);
	void SetSourceScale(MidiHook *hook, int midi_value);
	void SetGainFilter(MidiHook *hook, int midi_value);
	void SetOpacity(MidiHook *hook, int midi_value);
	void move_t_bar(MidiHook *hook, int midi_value);
	void play_pause_media_source(MidiHook *hook, int midi_value);
	void toggle_studio_mode(MidiHook *hook, int midi_value);
	void reset_stats(MidiHook *hook, int midi_value);
	void restart_media(MidiHook *hook, int midi_value);
	void stop_media(MidiHook *hook, int midi_value);
	void play_media(MidiHook *hook, int midi_value);
	void next_media(MidiHook *hook, int midi_value);
	void prev_media(MidiHook *hook, int midi_value);

private:
	MidiAgent *device=NULL;
};
