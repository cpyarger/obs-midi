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
#include "obs-controller.h"

////////////////////
// BUTTON ACTIONS //
////////////////////
OBSController::OBSController(MidiAgent *agent)
{
	//Device to agent;
	device = agent;
	this->setParent(agent);
	/*
	*
	* Connect All Actions to handle_obs_event on midi agent
	*
	*/
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(SetCurrentScene(MidiHook, int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(SetPreviewScene(MidiHook, int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(SetCurrentSceneCollection(MidiHook, int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(ResetSceneItem(MidiHook, int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(TransitionToProgram(MidiHook, int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(SetCurrentTransition(MidiHook, int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(SetTransitionDuration(MidiHook,
					   int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(SetSourceVisibility(MidiHook, int))); 
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(ToggleSourceVisibility(MidiHook, int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(ToggleMute(MidiHook, int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(SetMute(MidiHook, int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(StartStopStreaming()));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(StartStreaming()));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(StopStreaming()));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(StartStopRecording()));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(StartRecording()));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(StopRecording()));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(PauseRecording()));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(ResumeRecording()));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(StartStopReplayBuffer()));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(StartReplayBuffer()));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(StopReplayBuffer()));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(SaveReplayBuffer()));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(SetCurrentProfile(MidiHook, int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(SetTextGDIPlusText(MidiHook, int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(SetBrowserSourceURL(MidiHook, int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(ReloadBrowserSource(MidiHook, int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(TakeSourceScreenshot(MidiHook, int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(EnableSourceFilter(MidiHook, int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(DisableSourceFilter(MidiHook, int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(ToggleSourceFilter(MidiHook, int)));

	// CC ACTIONS
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(SetVolume(MidiHook, int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(SetSyncOffset(MidiHook, int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(SetSourcePosition()));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(SetSourceRotation()));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(SetSourceScale()));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(SetGainFilter()));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(SetOpacity()));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(move_t_bar(MidiHook, int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(play_pause_media_source(MidiHook, int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(toggle_studio_mode()));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(reset_stats()));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(restart_media(MidiHook, int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(stop_media(MidiHook, int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(play_media(MidiHook, int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(next_media(MidiHook, int)));
	connect(agent, SIGNAL(handle_obs_event(MidiHook, int)), this,
		SLOT(prev_media(MidiHook, int)));
}
/**
 * Sets the currently active scene
 */
void OBSController::SetCurrentScene(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Set_Current_Scene") {
		return;
	}

	OBSSourceAutoRelease source =
		obs_get_source_by_name(hook->scene.toStdString().c_str());
	obs_frontend_set_current_scene(source);
}

/**
 * Sets the scene in preview. Must be in Studio mode or will throw error
 */
void OBSController::SetPreviewScene(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Set_Preview_Scene") {
		return;
	}
	if (!obs_frontend_preview_program_mode_active()) {
		blog(LOG_DEBUG, "studio mode not enabled");
	}
	OBSScene scene = Utils::GetSceneFromNameOrCurrent(hook->scene);
	if (!scene) {
		blog(LOG_DEBUG, "specified scene doesn't exist");
	}
	OBSSourceAutoRelease source = obs_scene_get_source(scene);

	obs_frontend_set_current_preview_scene(source);
}

/**
 * Change the active scene collection.
 */
void OBSController::SetCurrentSceneCollection(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) !=
	    "Set_Current_Scene_Collection") {
		return;
	}
	// TODO : Check if specified profile exists and if changing is allowed
	obs_frontend_set_current_scene_collection(
		hook->scene_collection.toStdString().c_str());
}

/**
* Reset a scene item.
*/
void OBSController::ResetSceneItem(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Reset_Scene_Item") {
		return;
	}
	OBSScene scene = Utils::GetSceneFromNameOrCurrent(hook->scene);
	if (!scene) {
		throw("requested scene doesn't exist");
	}

	OBSDataAutoRelease params = obs_data_create();
	obs_data_set_string(params, "scene-name",
			    hook->scene.toStdString().c_str());
	OBSDataItemAutoRelease itemField = obs_data_item_byname(params, "item");

	OBSSceneItemAutoRelease sceneItem =
		Utils::GetSceneItemFromRequestField(scene, itemField);
	if (!sceneItem) {
		throw("specified scene item doesn't exist");
	}

	OBSSourceAutoRelease sceneItemSource =
		obs_sceneitem_get_source(sceneItem);

	OBSDataAutoRelease settings = obs_source_get_settings(sceneItemSource);
	obs_source_update(sceneItemSource, settings);
}

/**
 * Transitions the currently previewed scene to the main output.
 */

/**
 * Transitions the currently previewed scene to the main output using specified transition.
 * transitionDuration is optional. (milliseconds)
 */
void OBSController::TransitionToProgram(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Transition_To_Program") {
		return;
	}
	if (!obs_frontend_preview_program_mode_active()) {
		blog(LOG_DEBUG, "studio mode not enabled");
	}

	if (hook->transition.isEmpty()) {
		blog(LOG_DEBUG, "transition name can not be empty");
	}
	bool success = Utils::SetTransitionByName(hook->transition);
	if (!success) {
		blog(LOG_DEBUG, "specified transition doesn't exist");
	}
	obs_frontend_set_transition_duration(hook->duration);

	obs_frontend_preview_program_trigger_transition();
}

/**
 * Set the active transition.
 */
void OBSController::SetCurrentTransition(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Set_Current_Transition") {
		return;
	}
	Utils::SetTransitionByName(hook->transition);
}

/**
 * Set the duration of the currently active transition
 */
void OBSController::SetTransitionDuration(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Set_Transition_Duration") {
		return;
	}
	obs_frontend_set_transition_duration(hook->duration);
}

void OBSController::SetSourceVisibility(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Set_Source_Visibility") {
		return;
	}
	obs_sceneitem_set_visible(
		Utils::GetSceneItemFromName(
			Utils::GetSceneFromNameOrCurrent(hook->scene),
			hook->item),
		midi_value);
} //DOESNT EXIST

void OBSController::ToggleSourceVisibility(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Toggle_Source_Visibility") {
		return;
	}
	if (obs_sceneitem_visible(Utils::GetSceneItemFromName(
		    Utils::GetSceneFromNameOrCurrent(hook->scene), hook->item))) {
		SetSourceVisibility(hook, false);
	} else {
		SetSourceVisibility(hook, true);
	}
} //DOESNT EXIST

/**
* Inverts the mute status of a specified source.
*/
void OBSController::ToggleMute(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Toggle_Mute") {
		return;
	}
	if (hook->source.isEmpty()) {
		throw("sourceName is empty");
	}

	OBSSourceAutoRelease source =
		obs_get_source_by_name(hook->source.toUtf8());
	if (!source) {
		throw("sourceName not found");
	}

	obs_source_set_muted(source, !obs_source_muted(source));
}

/**
 * Sets the mute status of a specified source.
 */
void OBSController::SetMute(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Set_Mute") {
		return;
	}
	if (hook->source.isEmpty()) {
		throw("sourceName is empty");
	}

	OBSSourceAutoRelease source =
		obs_get_source_by_name(hook->source.toUtf8());
	if (!source) {
		throw("specified source doesn't exist");
	}

	obs_source_set_muted(source, midi_value);
}

/**
 * Toggle streaming on or off.
 */
void OBSController::StartStopStreaming(MidiHook *hook,int midi_value)
{
	if (Utils::untranslate(hook->action) != "Start_Stop_Streaming") {
		return;
	}

	if (obs_frontend_streaming_active())
		StopStreaming(hook, midi_value);
	else
		StartStreaming(hook, midi_value);
}

/**
 * Start streaming.
 */
void OBSController::StartStreaming(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Start_Streaming") {
		return;
	}
	if (obs_frontend_streaming_active() == false) {
		obs_frontend_streaming_start();
	}
}

/**
 * Stop streaming.
 */
void OBSController::StopStreaming(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Stop_Streaming") {
		return;
	}
	if (obs_frontend_streaming_active() == true) {
		obs_frontend_streaming_stop();
	}
}

/**
 * Toggle recording on or off.
 */
void OBSController::StartStopRecording(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Start_Stop_Recording") {
		return;
	}
	(obs_frontend_recording_active() ? obs_frontend_recording_stop()
					 : obs_frontend_recording_start());
}

/**
 * Start recording.
 */
void OBSController::StartRecording(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Start_Recording") {
		return;
	}
	if (!obs_frontend_recording_active()) {
		obs_frontend_recording_start();
	}
}

/**
 * Stop recording.
 */
void OBSController::StopRecording(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Stop_Recording") {
		return;
	}
	if (obs_frontend_recording_active()) {
		obs_frontend_recording_stop();
	}
}

/**
* Pause the current recording.
*/
void OBSController::PauseRecording(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Pause_Recording") {
		return;
	}
	if (obs_frontend_recording_active()) {
		obs_frontend_recording_pause(true);
	}
}

/**
* Resume/unpause the current recording (if paused).
*/
void OBSController::ResumeRecording(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Resume_Recording") {
		return;
	}
	if (obs_frontend_recording_active()) {
		obs_frontend_recording_pause(false);
	}
}

/**
* Toggle the Replay Buffer on/off.
*/
void OBSController::StartStopReplayBuffer(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Start_Stop_Replay_Buffer") {
		return;
	}
	if (obs_frontend_replay_buffer_active()) {
		obs_frontend_replay_buffer_stop();
	} else {
		Utils::StartReplayBuffer();
	}
}

/**
* Start recording into the Replay Buffer.
* Will throw an error if "Save Replay Buffer" hotkey is not set in OBS' settings.
* Setting this hotkey is mandatory, even when triggering saves only
* through obs-midi.
*/
void OBSController::StartReplayBuffer(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Start_Replay_Buffer") {
		return;
	}
	if (!Utils::ReplayBufferEnabled()) {
		blog(LOG_DEBUG, "replay buffer disabled in settings");
	}

	if (obs_frontend_replay_buffer_active() == false) {
		Utils::StartReplayBuffer();
	}
}

/**
* Stop recording into the Replay Buffer.
*/
void OBSController::StopReplayBuffer(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Stop_Replay_Buffer") {
		return;
	}
	if (obs_frontend_replay_buffer_active() == true) {
		obs_frontend_replay_buffer_stop();
	}
}

/**
* Flush and save the contents of the Replay Buffer to disk. This is
* basically the same as triggering the "Save Replay Buffer" hotkey.
* Will return an `error` if the Replay Buffer is not active.
*/
void OBSController::SaveReplayBuffer(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Save_Replay_Buffer") {
		return;
	}
	if (!obs_frontend_replay_buffer_active()) {
		throw("replay buffer not active");
	}

	OBSOutputAutoRelease replayOutput =
		obs_frontend_get_replay_buffer_output();

	calldata_t cd = {0};
	proc_handler_t *ph = obs_output_get_proc_handler(replayOutput);
	proc_handler_call(ph, "save", &cd);
	calldata_free(&cd);
}

void OBSController::SetCurrentProfile(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Set_Current_Profile") {
		return;
	}
	if (hook->profile.isEmpty()) {
		throw("profile name is empty");
	}

	// TODO : check if profile exists
	obs_frontend_set_current_profile(hook->profile.toUtf8());
}

void OBSController::SetTextGDIPlusText(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Set_Gdi_Plus_Text") {
		return;
	}
}
void OBSController::SetBrowserSourceURL(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Set_Browser_Source_Url") {
		return;
	}
	OBSSourceAutoRelease source =
		obs_get_source_by_name(hook->source.toStdString().c_str());
	QString sourceId = obs_source_get_id(source);
	if (sourceId != "browser_source" && sourceId != "linuxbrowser-source") {
		return blog(LOG_DEBUG, "Not a browser Source");
	}

	OBSDataAutoRelease settings = obs_source_get_settings(source);
	obs_data_set_string(settings, "url", hook->string_override.toStdString().c_str());
	obs_source_update(source, settings);
}

void OBSController::ReloadBrowserSource(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Reload_Browser_Source") {
		return;
	}
	OBSSourceAutoRelease source =
		obs_get_source_by_name(hook->source.toUtf8());
	obs_properties_t *sourceProperties = obs_source_properties(source);
	obs_property_t *property =
		obs_properties_get(sourceProperties, "refreshnocache");
	obs_property_button_clicked(
		property,
		source); // This returns a boolean but we ignore it because the browser plugin always returns `false`.
	obs_properties_destroy(sourceProperties);
}

void OBSController::TakeSourceScreenshot(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Take_Source_Screenshot") {
		return;
	}
}

void OBSController::EnableSourceFilter(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Enable_Source_Filter") {
		return;
	}
	OBSSourceAutoRelease source =
		obs_get_source_by_name(hook->source.toStdString().c_str());
	obs_source_set_enabled(source, true);
}

void OBSController::DisableSourceFilter(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Disable_Source_Filter") {
		return;
	}
	OBSSourceAutoRelease source =
		obs_get_source_by_name(hook->source.toStdString().c_str());
	obs_source_set_enabled(source, false);
}

void OBSController::ToggleSourceFilter(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Toggle_Source_Filter") {
		return;
	}
	OBSSourceAutoRelease source =
		obs_get_source_by_name(hook->source.toStdString().c_str());

	OBSSourceAutoRelease filter = obs_source_get_filter_by_name(
		source, hook->filter.toStdString().c_str());
	if (obs_source_enabled(filter)) {
		obs_source_set_enabled(filter, false);
	} else {
		obs_source_set_enabled(filter, true);
	}
}
////////////////
// CC ACTIONS //
////////////////

void OBSController::SetVolume(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Set_Volume") {
		return;
	}
	OBSSourceAutoRelease obsSource =
		obs_get_source_by_name(hook->audio_source.toUtf8());
	if (!obsSource) {
		return; // source does not exist
	}

	obs_source_set_volume(obsSource, Utils::mapper(midi_value));
}

/**
 * Set the audio sync offset of a specified source.
 */
void OBSController::SetSyncOffset(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Set_Sync_Offset") {
		return;
	}
	if (hook->source.isEmpty()) {
		throw("source name is empty");
	}

	OBSSourceAutoRelease source =
		obs_get_source_by_name(hook->source.toUtf8());
	if (!source) {
		throw("specified source doesn't exist");
	}

	obs_source_set_sync_offset(source, midi_value);
}

void OBSController::SetSourcePosition(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Set_Source_Position") {
		return;
	}}

void OBSController::SetSourceRotation(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Set_Source_Rotation") {
		return;
	}}

void OBSController::SetSourceScale(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Set_Source_Scale") {
		return;
	}}

void OBSController::SetGainFilter(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Set_Gain_Filter") {
		return;
	}}

void OBSController::SetOpacity(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Set_Opacity") {
		return;
	}}
void OBSController::move_t_bar(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Move_T_Bar") {
		return;
	}
	if (obs_frontend_preview_program_mode_active()) {

		obs_frontend_set_tbar_position(Utils::t_bar_mapper(midi_value));
		obs_frontend_release_tbar();
	}
}
void OBSController::play_pause_media_source(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Play_Pause_Media_Source") {
		return;
	}
	OBSSourceAutoRelease source = obs_get_source_by_name(
		hook->media_source.toStdString().c_str());
	switch (obs_source_media_get_state(source)) {
	case obs_media_state::OBS_MEDIA_STATE_PAUSED:
		obs_source_media_play_pause(source, false);
		break;
	case obs_media_state::OBS_MEDIA_STATE_PLAYING:
		obs_source_media_play_pause(source, true);
		break;
	case obs_media_state::OBS_MEDIA_STATE_ENDED:
		obs_source_media_restart(source);
		break;
	}
}

// TODO:: Fix this
void OBSController::toggle_studio_mode(MidiHook *hook,int midi_value)
{
	if (Utils::untranslate(hook->action) != "Toggle_Studio_Mode") {
		return;
	}
	if (obs_frontend_preview_program_mode_active()) {
		obs_frontend_set_preview_program_mode(false);

	} else {
		obs_frontend_set_preview_program_mode(true);
	}
}
void OBSController::reset_stats(MidiHook *hook,int midi_value) {
	if (Utils::untranslate(hook->action) != "Reset_Stats") {
		return;
	}
}
void OBSController::restart_media(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Restart_Media") {
		return;
	}
	OBSSourceAutoRelease source = obs_get_source_by_name(
		hook->media_source.toStdString().c_str());
	obs_source_media_restart(source);
}
void OBSController::play_media(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Play_Media") {
		return;
	}
	OBSSourceAutoRelease source = obs_get_source_by_name(
		hook->media_source.toStdString().c_str());
	play_pause_media_source(hook, false);
	
}
void OBSController::stop_media(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Stop_Media") {
		return;
	}
	OBSSourceAutoRelease source = obs_get_source_by_name(
		hook->media_source.toStdString().c_str());
	obs_source_media_stop(source);
}
void OBSController::next_media(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Next_Media") {
		return;
	}
	OBSSourceAutoRelease source =
		obs_get_source_by_name(hook->media_source.toStdString().c_str());

	obs_source_media_next(source);
}
void OBSController::prev_media(MidiHook *hook, int midi_value)
{
	if (Utils::untranslate(hook->action) != "Prev_Media") {
		return;
	}
	OBSSourceAutoRelease source = obs_get_source_by_name(
		hook->media_source.toStdString().c_str());
	obs_source_media_previous(source);
}
