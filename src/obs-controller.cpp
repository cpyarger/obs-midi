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
OBSController::OBSController(MidiHook *incoming_hook, int incoming_midi_value)
{
	hook = incoming_hook;
	midi_value = incoming_midi_value;

	/*
	*
	* Connect All Actions to handle_obs_event on midi agent
	*
	*/
	switch (ActionsClass::string_to_action(
		Utils::untranslate(hook->action))) {
	case ActionsClass::Actions::Set_Current_Scene:
		this->SetCurrentScene();
		break;
	case ActionsClass::Actions::Reset_Scene_Item:
		this->ResetSceneItem();
		break;
	case ActionsClass::Actions::Toggle_Mute:
		this->ToggleMute();
		break;
	case ActionsClass::Actions::Do_Transition:
		this->TransitionToProgram();
		break;
	case ActionsClass::Actions::Set_Current_Transition:
		this->SetCurrentTransition();
		break;
	case ActionsClass::Actions::Set_Mute:
		this->SetMute();
		break;
	case ActionsClass::Actions::Toggle_Start_Stop_Streaming:
		this->StartStopStreaming();
		break;
	case ActionsClass::Actions::Set_Preview_Scene:
		this->SetPreviewScene();
		break;
	case ActionsClass::Actions::Set_Current_Scene_Collection:
		this->SetCurrentSceneCollection();
		break;
	case ActionsClass::Actions::Set_Transition_Duration:
		this->SetTransitionDuration();
		break;
	case ActionsClass::Actions::Start_Streaming:
		this->StartStreaming();
		break;
	case ActionsClass::Actions::Stop_Streaming:
		this->StopStreaming();
		break;
	case ActionsClass::Actions::Start_Recording:
		this->StartRecording();
		break;
	case ActionsClass::Actions::Stop_Recording:
		this->StopRecording();
		break;
	case ActionsClass::Actions::Start_Replay_Buffer:
		this->StartReplayBuffer();
		break;
	case ActionsClass::Actions::Stop_Replay_Buffer:
		this->StopReplayBuffer();
		break;
	case ActionsClass::Actions::Set_Volume:
		this->SetVolume();
		break;
	case ActionsClass::Actions::Take_Source_Screenshot:
		this->TakeSourceScreenshot();
		break;
	case ActionsClass::Actions::Pause_Recording:
		this->PauseRecording();
		break;
	case ActionsClass::Actions::Enable_Source_Filter:
		this->EnableSourceFilter();
		break;
	case ActionsClass::Actions::Disable_Source_Filter:
		this->DisableSourceFilter();
		break;
	case ActionsClass::Actions::Toggle_Start_Stop_Recording:
		this->StartStopRecording();
		break;
	case ActionsClass::Actions::Toggle_Start_Stop_Replay_Buffer:
		this->StartStopReplayBuffer();
		break;
	case ActionsClass::Actions::Resume_Recording:
		this->ResumeRecording();
		break;
	case ActionsClass::Actions::Save_Replay_Buffer:
		this->SaveReplayBuffer();
		break;
	case ActionsClass::Actions::Set_Current_Profile:
		this->SetCurrentProfile();
		break;
	case ActionsClass::Actions::Toggle_Source_Filter:
		this->ToggleSourceFilter();
		break;
	case ActionsClass::Actions::Set_Text_GDIPlus_Text:
		this->SetTextGDIPlusText();
		break;
	case ActionsClass::Actions::Set_Browser_Source_URL:
		this->SetBrowserSourceURL();
		break;
	case ActionsClass::Actions::Reload_Browser_Source:
		this->ReloadBrowserSource();
		break;
	case ActionsClass::Actions::Set_Sync_Offset:
		this->SetSyncOffset();
		break;
	case ActionsClass::Actions::Set_Source_Rotation:
		this->SetSourceRotation();
		break;
	case ActionsClass::Actions::Set_Source_Position:
		this->SetSourcePosition();
		break;
	case ActionsClass::Actions::Set_Gain_Filter:
		this->SetGainFilter();
		break;
	case ActionsClass::Actions::Set_Opacity:
		this->SetOpacity();
		break;
	case ActionsClass::Actions::Set_Source_Scale:
		this->SetSourceScale();
		break;
	case ActionsClass::Actions::Move_T_Bar:
		this->move_t_bar();
		break;
	case ActionsClass::Actions::Play_Pause_Media:
		this->play_pause_media_source();
		break;
	case ActionsClass::Actions::Studio_Mode:
		this->toggle_studio_mode();
		break;
	case ActionsClass::Actions::Reset_Stats:
		this->reset_stats();
		break;
	case ActionsClass::Actions::Restart_Media:
		this->restart_media();
		break;
	case ActionsClass::Actions::Stop_Media:
		this->stop_media();
		break;
	case ActionsClass::Actions::Previous_Media:
		this->prev_media();
		break;
	case ActionsClass::Actions::Next_Media:
		this->next_media();
		break;
	case ActionsClass::Actions::Toggle_Source_Visibility:
		this->ToggleSourceVisibility();
		break;
	};
	this->deleteLater();
}
OBSController::~OBSController()
{
	
}
	/**
 * Sets the currently active scene
 */
void OBSController::SetCurrentScene()
{
	OBSSourceAutoRelease source =
		obs_get_source_by_name(hook->scene.toUtf8());
	obs_frontend_set_current_scene(source);
}

/**
 * Sets the scene in preview. Must be in Studio mode or will throw error
 */
void OBSController::SetPreviewScene()
{
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
void OBSController::SetCurrentSceneCollection()
{

	// TODO : Check if specified profile exists and if changing is allowed
	obs_frontend_set_current_scene_collection(
		hook->scene_collection.toUtf8());
}

/**
* Reset a scene item.
*/
void OBSController::ResetSceneItem()
{

	OBSScene scene = Utils::GetSceneFromNameOrCurrent(hook->scene);
	if (!scene) {
		throw("requested scene doesn't exist");
	}

	OBSDataAutoRelease params = obs_data_create();
	obs_data_set_string(params, "scene-name", hook->scene.toUtf8());
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
void OBSController::TransitionToProgram()
{

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
void OBSController::SetCurrentTransition()
{
	Utils::SetTransitionByName(hook->transition);
}

/**
 * Set the duration of the currently active transition
 */
void OBSController::SetTransitionDuration()
{
	obs_frontend_set_transition_duration(hook->duration);
}

void OBSController::SetSourceVisibility()
{
	obs_sceneitem_set_visible(
		Utils::GetSceneItemFromName(
			Utils::GetSceneFromNameOrCurrent(hook->scene),
			hook->item),
		midi_value);
} //DOESNT EXIST

void OBSController::ToggleSourceVisibility()
{
	auto scene = Utils::GetSceneItemFromName(
		Utils::GetSceneFromNameOrCurrent(hook->scene), hook->item);
	if (obs_sceneitem_visible(scene)) {
		obs_sceneitem_set_visible(scene, false);
	} else {
		obs_sceneitem_set_visible(scene, true);
	}
} //DOESNT EXIST

/**
* Inverts the mute status of a specified source.
*/
void OBSController::ToggleMute()
{

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
void OBSController::SetMute()
{

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
void OBSController::StartStopStreaming()
{

	if (obs_frontend_streaming_active())
		obs_frontend_streaming_stop();
	else
		obs_frontend_streaming_start();
}

/**
 * Start streaming.
 */
void OBSController::StartStreaming()
{

	if (obs_frontend_streaming_active() == false) {
		obs_frontend_streaming_start();
	}
}

/**
 * Stop streaming.
 */
void OBSController::StopStreaming()
{

	if (obs_frontend_streaming_active() == true) {
		obs_frontend_streaming_stop();
	}
}

/**
 * Toggle recording on or off.
 */
void OBSController::StartStopRecording()
{

	(obs_frontend_recording_active() ? obs_frontend_recording_stop()
					 : obs_frontend_recording_start());
}

/**
 * Start recording.
 */
void OBSController::StartRecording()
{
	if (!obs_frontend_recording_active()) {
		obs_frontend_recording_start();
	}
}

/**
 * Stop recording.
 */
void OBSController::StopRecording()
{

	if (obs_frontend_recording_active()) {
		obs_frontend_recording_stop();
	}
}

/**
* Pause the current recording.
*/
void OBSController::PauseRecording()
{

	if (obs_frontend_recording_active()) {
		obs_frontend_recording_pause(true);
	}
}

/**
* Resume/unpause the current recording (if paused).
*/
void OBSController::ResumeRecording()
{

	if (obs_frontend_recording_active()) {
		obs_frontend_recording_pause(false);
	}
}

/**
* Toggle the Replay Buffer on/off.
*/
void OBSController::StartStopReplayBuffer()
{

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
void OBSController::StartReplayBuffer()
{

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
void OBSController::StopReplayBuffer()
{

	if (obs_frontend_replay_buffer_active() == true) {
		obs_frontend_replay_buffer_stop();
	}
}

/**
* Flush and save the contents of the Replay Buffer to disk. This is
* basically the same as triggering the "Save Replay Buffer" hotkey.
* Will return an `error` if the Replay Buffer is not active.
*/
void OBSController::SaveReplayBuffer()
{
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

void OBSController::SetCurrentProfile()
{

	if (hook->profile.isEmpty()) {
		throw("profile name is empty");
	}

	// TODO : check if profile exists
	obs_frontend_set_current_profile(hook->profile.toUtf8());
}

void OBSController::SetTextGDIPlusText() {}
void OBSController::SetBrowserSourceURL()
{
	OBSSourceAutoRelease source =
		obs_get_source_by_name(hook->source.toUtf8());
	QString sourceId = obs_source_get_id(source);
	if (sourceId != "browser_source" && sourceId != "linuxbrowser-source") {
		return blog(LOG_DEBUG, "Not a browser Source");
	}

	OBSDataAutoRelease settings = obs_source_get_settings(source);
	obs_data_set_string(settings, "url", hook->string_override.toUtf8());
	obs_source_update(source, settings);
}

void OBSController::ReloadBrowserSource()
{

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

void OBSController::TakeSourceScreenshot() {}

void OBSController::EnableSourceFilter()
{

	OBSSourceAutoRelease source =
		obs_get_source_by_name(hook->source.toUtf8());
	obs_source_set_enabled(source, true);
}

void OBSController::DisableSourceFilter()
{
	OBSSourceAutoRelease source =
		obs_get_source_by_name(hook->source.toUtf8());
	obs_source_set_enabled(source, false);
}

void OBSController::ToggleSourceFilter()
{
	OBSSourceAutoRelease source =
		obs_get_source_by_name(hook->source.toUtf8());

	OBSSourceAutoRelease filter =
		obs_source_get_filter_by_name(source, hook->filter.toUtf8());
	if (obs_source_enabled(filter)) {
		obs_source_set_enabled(filter, false);
	} else {
		obs_source_set_enabled(filter, true);
	}
}
////////////////
// CC ACTIONS //
////////////////

void OBSController::SetVolume()
{
	OBSSourceAutoRelease obsSource =
		obs_get_source_by_name(hook->audio_source.toUtf8());
	obs_source_set_volume(obsSource, pow(Utils::mapper(midi_value), 3.0));
}

/**
 * Set the audio sync offset of a specified source.
 */
void OBSController::SetSyncOffset()
{
	OBSSourceAutoRelease source =
		obs_get_source_by_name(hook->source.toUtf8());
	obs_source_set_sync_offset(source, midi_value);
}

void OBSController::SetSourcePosition() {}

void OBSController::SetSourceRotation() {}

void OBSController::SetSourceScale() {}

void OBSController::SetGainFilter() {}

void OBSController::SetOpacity() {}
void OBSController::move_t_bar()
{
	if (obs_frontend_preview_program_mode_active()) {

		obs_frontend_set_tbar_position(Utils::t_bar_mapper(midi_value));
		obs_frontend_release_tbar();
	}
}
void OBSController::play_pause_media_source()
{

	OBSSourceAutoRelease source =
		obs_get_source_by_name(hook->media_source.toUtf8());
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
void OBSController::toggle_studio_mode()
{

	if (obs_frontend_preview_program_mode_active()) {
		obs_frontend_set_preview_program_mode(false);

	} else {
		obs_frontend_set_preview_program_mode(true);
	}
}
void OBSController::reset_stats() {}
void OBSController::restart_media()
{
	OBSSourceAutoRelease source =
		obs_get_source_by_name(hook->media_source.toUtf8());
	obs_source_media_restart(source);
}
void OBSController::play_media()
{

	OBSSourceAutoRelease source =
		obs_get_source_by_name(hook->media_source.toUtf8());
	obs_source_media_play_pause(source, false);
}
void OBSController::stop_media()
{

	OBSSourceAutoRelease source =
		obs_get_source_by_name(hook->media_source.toUtf8());
	obs_source_media_stop(source);
}
void OBSController::next_media()
{
	OBSSourceAutoRelease source =
		obs_get_source_by_name(hook->media_source.toUtf8());
	obs_source_media_next(source);
}
void OBSController::prev_media()
{
	OBSSourceAutoRelease source =
		obs_get_source_by_name(hook->media_source.toUtf8());
	obs_source_media_previous(source);
}
