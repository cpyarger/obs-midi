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

#include <iostream>

#include "utils.h"
#include "midi-agent.h"
#include "obs-controller.h"
#if __has_include(<obs-frontend-api.h>)
#include <obs-frontend-api.h>
#else
#include <obs-frontend-api/obs-frontend-api.h>
#endif
using namespace std;

////////////////////
// BUTTON ACTIONS //
////////////////////

/**
 * Sets the currently active scene
 */
void OBSController::SetCurrentScene(QString sceneName)
{
	OBSSourceAutoRelease source =
		obs_get_source_by_name(sceneName.toStdString().c_str());
	obs_frontend_set_current_scene(source);
}

/**
 * Sets the scene in preview. Must be in Studio mode or will throw error
 */
void OBSController::SetPreviewScene(QString sceneName)
{
	if (!obs_frontend_preview_program_mode_active()) {
		blog(LOG_DEBUG, "studio mode not enabled");
	}
	OBSScene scene = Utils::GetSceneFromNameOrCurrent(sceneName);
	if (!scene) {
		blog(LOG_DEBUG, "specified scene doesn't exist");
	}
	OBSSourceAutoRelease source = obs_scene_get_source(scene);

	obs_frontend_set_current_preview_scene(source);
}

/**
 * Change the active scene collection.
 */
void OBSController::SetCurrentSceneCollection(QString sceneCollection)
{
	// TODO : Check if specified profile exists and if changing is allowed
	obs_frontend_set_current_scene_collection(sceneCollection.toUtf8());
}

/**
* Reset a scene item.
*/
void OBSController::ResetSceneItem(QString sceneName, QString itemName)
{
	OBSScene scene = Utils::GetSceneFromNameOrCurrent(sceneName);
	if (!scene) {
		throw("requested scene doesn't exist");
	}

	OBSDataAutoRelease params = obs_data_create();
	obs_data_set_string(params, "scene-name",
			    sceneName.toStdString().c_str());
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
void OBSController::TransitionToProgram()
{
	obs_frontend_preview_program_trigger_transition();
}

/**
 * Transitions the currently previewed scene to the main output using specified transition.
 * transitionDuration is optional. (milliseconds)
 */
void OBSController::TransitionToProgram(QString transitionName,
					int transitionDuration)
{
	if (!obs_frontend_preview_program_mode_active()) {
		blog(LOG_DEBUG, "studio mode not enabled");
	}

	if (transitionName.isEmpty()) {
		blog(LOG_DEBUG, "transition name can not be empty");
	}
	bool success = Utils::SetTransitionByName(transitionName);
	if (!success) {
		blog(LOG_DEBUG, "specified transition doesn't exist");
	}
	obs_frontend_set_transition_duration(transitionDuration);

	obs_frontend_preview_program_trigger_transition();
}

/**
 * Set the active transition.
 */
void OBSController::SetCurrentTransition(QString name)
{
	Utils::SetTransitionByName(name);
}

/**
 * Set the duration of the currently active transition
 */
void OBSController::SetTransitionDuration(int duration)
{
	obs_frontend_set_transition_duration(duration);
}

void OBSController::SetSourceVisibility(QString scene, QString item, bool set)
{
	obs_sceneitem_set_visible(
		Utils::GetSceneItemFromName(
			Utils::GetSceneFromNameOrCurrent(scene), item),
		set);
} //DOESNT EXIST

void OBSController::ToggleSourceVisibility(QString scene, QString item)
{
	if (obs_sceneitem_visible(Utils::GetSceneItemFromName(
		    Utils::GetSceneFromNameOrCurrent(scene), item))) {
		SetSourceVisibility(scene, item, false);
	} else {
		SetSourceVisibility(scene, item, true);
	}
} //DOESNT EXIST

/**
* Inverts the mute status of a specified source.
*/
void OBSController::ToggleMute(QString sourceName)
{
	if (sourceName.isEmpty()) {
		throw("sourceName is empty");
	}

	OBSSourceAutoRelease source =
		obs_get_source_by_name(sourceName.toUtf8());
	if (!source) {
		throw("sourceName not found");
	}

	obs_source_set_muted(source, !obs_source_muted(source));
}

/**
 * Sets the mute status of a specified source.
 */
void OBSController::SetMute(QString sourceName, bool mute)
{
	if (sourceName.isEmpty()) {
		throw("sourceName is empty");
	}

	OBSSourceAutoRelease source =
		obs_get_source_by_name(sourceName.toUtf8());
	if (!source) {
		throw("specified source doesn't exist");
	}

	obs_source_set_muted(source, mute);
}

/**
 * Toggle streaming on or off.
 */
void OBSController::StartStopStreaming()
{
	if (obs_frontend_streaming_active())
		StopStreaming();
	else
		StartStreaming();
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

void OBSController::SetCurrentProfile(QString profileName)
{
	if (profileName.isEmpty()) {
		throw("profile name is empty");
	}

	// TODO : check if profile exists
	obs_frontend_set_current_profile(profileName.toUtf8());
}

void OBSController::SetTextGDIPlusText(QString text) {}

void OBSController::SetBrowserSourceURL(QString sourceName, QString url)
{

	OBSSourceAutoRelease source =
		obs_get_source_by_name(sourceName.toStdString().c_str());
	QString sourceId = obs_source_get_id(source);
	if (sourceId != "browser_source" && sourceId != "linuxbrowser-source") {
		return blog(LOG_DEBUG, "Not a browser Source");
	}

	OBSDataAutoRelease settings = obs_source_get_settings(source);
	obs_data_set_string(settings, "url", url.toStdString().c_str());
	obs_source_update(source, settings);
}

void OBSController::ReloadBrowserSource(QString sourceName)
{
	OBSSourceAutoRelease source =
		obs_get_source_by_name(sourceName.toUtf8());
	obs_properties_t *sourceProperties = obs_source_properties(source);
	obs_property_t *property =
		obs_properties_get(sourceProperties, "refreshnocache");
	obs_property_button_clicked(
		property,
		source); // This returns a boolean but we ignore it because the browser plugin always returns `false`.
	obs_properties_destroy(sourceProperties);
}

void OBSController::TakeSourceScreenshot(QString source) {}

void OBSController::EnableSourceFilter(obs_source_t *source)
{
	obs_source_set_enabled(source, true);
}

void OBSController::DisableSourceFilter(obs_source_t *source)
{
	obs_source_set_enabled(source, false);
}

void OBSController::ToggleSourceFilter(QString sourcename, QString filtername)
{
	OBSSourceAutoRelease source =
		obs_get_source_by_name(sourcename.toStdString().c_str());

	OBSSourceAutoRelease filter = obs_source_get_filter_by_name(
		source, filtername.toStdString().c_str());
	if (obs_source_enabled(filter)) {
		obs_source_set_enabled(filter, false);
	} else {
		obs_source_set_enabled(filter, true);
	}
}
////////////////
// CC ACTIONS //
////////////////

void OBSController::SetVolume(QString source, float volume)
{
	OBSSourceAutoRelease obsSource =
		obs_get_source_by_name(source.toUtf8());
	if (!obsSource) {
		return; // source does not exist
	}

	obs_source_set_volume(obsSource, volume);
}

/**
 * Set the audio sync offset of a specified source.
 */
void OBSController::SetSyncOffset(QString sourceName, int64_t sourceSyncOffset)
{
	if (sourceName.isEmpty()) {
		throw("source name is empty");
	}

	OBSSourceAutoRelease source =
		obs_get_source_by_name(sourceName.toUtf8());
	if (!source) {
		throw("specified source doesn't exist");
	}

	obs_source_set_sync_offset(source, sourceSyncOffset);
}

void OBSController::SetSourcePosition() {}

void OBSController::SetSourceRotation() {}

void OBSController::SetSourceScale() {}

void OBSController::SetGainFilter() {}

void OBSController::SetOpacity() {}
void OBSController::move_t_bar(int move)
{

	if (obs_frontend_preview_program_mode_active()) {

		obs_frontend_set_tbar_position(Utils::t_bar_mapper(move));
		obs_frontend_release_tbar();
	}
}
void OBSController::play_pause_media_source(QString media_source)
{
	OBSSourceAutoRelease source =
		obs_get_source_by_name(media_source.toStdString().c_str());
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
void OBSController::restart_media(QString media_source)
{
	OBSSourceAutoRelease source =
		obs_get_source_by_name(media_source.toStdString().c_str());
	obs_source_media_restart(source);
}

void OBSController::stop_media(QString media_source)
{
	OBSSourceAutoRelease source =
		obs_get_source_by_name(media_source.toStdString().c_str());
	obs_source_media_stop(source);
}
void OBSController::next_media(QString media_source)
{
	OBSSourceAutoRelease source =
		obs_get_source_by_name(media_source.toStdString().c_str());

	obs_source_media_next(source);
}
void OBSController::prev_media(QString media_source)
{
	OBSSourceAutoRelease source =
		obs_get_source_by_name(media_source.toStdString().c_str());
	obs_source_media_previous(source);
}
