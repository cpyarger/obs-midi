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

#if __has_include(<obs-frontend-api.h>)
#include <obs-frontend-api.h>
#include "rtmidi17/rtmidi17.hpp"
#else
#include <obs/obs-frontend-api.h>
#include "rtmidi17/rtmidi17.hpp"
#endif
#include <QtCore/QTime>
//#include <Python.h>
#include <functional>
#include <map>
#include <string>
#include <iostream>
#include "utils.h"
#include "midi-agent.h"
#include "obs-midi.h"
#include "config.h"
#include "obs-controller.h"

#include "device-manager.h"
using namespace std;

///////////////////////
/* MIDI HOOK ROUTES */
//////////////////////


////////////////
// MIDI AGENT //
////////////////
MidiAgent::MidiAgent()
{

	midi_input_name = "Midi Device (uninit)";
	midi_output_name = "Midi Out Device (uninit)";
	midiin = new rtmidi::midi_in();
	midiout = new rtmidi::midi_out();
	midiin->set_callback(
		[this](const auto &message) { HandleInput(message, this); });
	
}

MidiAgent::~MidiAgent()
{
	clear_MidiHooks();
	close_midi_port();
	delete midiin;
	delete midiout;
}

/* Loads information from OBS data. (recalled from Config)
 * This will not enable the MidiAgent or open the port. (and shouldn't)
 */
void MidiAgent::Load(obs_data_t *data)
{
	midi_input_name = obs_data_get_string(data, "name");
	midi_output_name = obs_data_get_string(data, "outname");
	enabled = obs_data_get_bool(data, "enabled");
	bidirectional = obs_data_get_bool(data, "bidirectional");

	obs_data_array_t *hooksData = obs_data_get_array(data, "hooks");
	size_t hooksCount = obs_data_array_count(hooksData);
	for (size_t i = 0; i < hooksCount; i++) {
		obs_data_t *hookData = obs_data_array_item(hooksData, i);
		MidiHook *mh = new MidiHook();
		mh->message_type =
			obs_data_get_string(hookData, "message_type");
		mh->norc = obs_data_get_int(hookData, "norc");
		mh->channel = obs_data_get_int(hookData, "channel");
		mh->action = obs_data_get_string(hookData, "action");
		mh->scene = obs_data_get_string(hookData, "scene");
		mh->source = obs_data_get_string(hookData, "source");
		mh->filter = obs_data_get_string(hookData, "filter");
		mh->transition = obs_data_get_string(hookData, "transition");
		mh->item = obs_data_get_string(hookData, "item");
		mh->audio_source =
			obs_data_get_string(hookData, "audio_source");
		mh->media_source =
			obs_data_get_string(hookData, "media_source");
		mh->duration = obs_data_get_int(hookData, "duration");
		mh->scene_collection =
			obs_data_get_string(hookData, "scene_collection");
		mh->profile = obs_data_get_string(hookData, "profile");
		mh->string_override =
			obs_data_get_string(hookData, "string_override");
		mh->bool_override =
			obs_data_get_bool(hookData, "bool_override");
		mh->int_override = obs_data_get_int(hookData, "int_override");
		add_MidiHook(mh);
	}
}


/* Will open the port and enable this MidiAgent
*/
void MidiAgent::open_midi_input_port(int inport)
{
	try {

		midiin->open_port(inport);

		midi_input_name = QString::fromStdString(midiin->get_port_name(inport));
		enabled = true;
		connected = true;
		blog(LOG_INFO, "MIDI device connected In: [%d] %s", inport,
		     midi_input_name.toStdString().c_str());

	} catch (const rtmidi::midi_exception &error) {

	}
}
void MidiAgent::open_midi_output_port(int outport)
{

	try {
		midiout->open_port(outport);
	} catch (const rtmidi::midi_exception &error) {



	}
	midi_output_name = QString::fromStdString(midiout->get_port_name(outport));
	enabled = true;
	connected = true;

	blog(LOG_INFO, "MIDI device connected Out: [%d] %s", outport,
	     midi_output_name.toStdString().c_str());
}

/* Will close the port and disable this MidiAgent
*/
void MidiAgent::close_midi_port()
{
	midiin->close_port();
	midiout->close_port();
	enabled = false;
	connected = false;
}

QString MidiAgent::get_midi_input_name()
{
	return midi_input_name;
}
QString MidiAgent::get_midi_output_name()
{
	return midi_output_name;
}
void MidiAgent::set_midi_output_name(QString oname)
{
	if (midi_output_name != oname) {
		midiout->close_port();
		open_midi_output_port(GetDeviceManager()->GetOutPortNumberByDeviceName(
			oname));
	}
	midi_output_name = oname;
}
bool MidiAgent::setBidirectional(bool state)
{
	bidirectional = state;
	if (!state) {
		midiout->close_port();

	} else {
		if (midiout->is_port_open()) {
			midiout->close_port();
		}
		open_midi_output_port(GetDeviceManager()->GetOutPortNumberByDeviceName(
			midi_output_name));
	}
	return state;
}

int MidiAgent::GetPort()
{
	return port;
}
bool MidiAgent::isEnabled()
{
	return enabled;
}
bool MidiAgent::isBidirectional()
{
	return bidirectional;
}
bool MidiAgent::isConnected()
{
	return connected;
}

/* Midi input callback.
 * Extend input handling functionality here.
 * For OBS action triggers, edit the funcMap instead.
 */
void MidiAgent::HandleInput(const rtmidi::message &message, void *userData)
{
	MidiAgent *self = static_cast<MidiAgent *>(userData);
	if (self->enabled == false || self->connected == false) {
		return;
	}

	/*************Get Message parts***********/
	QString mType = Utils::mtype_to_string(message.get_message_type());
	int channelnel = message.get_channel();
	int norc = Utils::get_midi_note_or_control(message);
	int value = Utils::get_midi_value(message);
	self->sending = true;
	/***** Send Messages to emit function *****/

	MidiMessage x;
	x.device_name = self->get_midi_input_name();
	x.message_type = mType;
	x.NORC = norc;
	x.channel = channelnel;
	x.value = value;
	//self->SendMessage(x);
	emit self->broadcast_midi_message(x);

	/** check if hook exists for this note or cc norc and launch it **/
	//Eventually add channel to this check.

	for (unsigned i = 0; i < self->midiHooks.size(); i++) {
		if (self->midiHooks.at(i)->message_type == mType &&
		    self->midiHooks.at(i)->norc == norc &&
		    self->midiHooks.at(i)->channel == channelnel) {
			self->do_obs_action(
				self->midiHooks.at(i), value,
				ActionsClass::string_to_action(
					Utils::untranslate(self->midiHooks.at(i)->action)));
			
		}
	}
}



/* Get the midi hooks for this device
*/
QVector<MidiHook *> MidiAgent::GetMidiHooks()
{
	
	return midiHooks;
}

/* Add a new MidiHook
*/
void MidiAgent::add_MidiHook(MidiHook *hook)
{

	//connect()
	midiHooks.push_back(hook);
}
void MidiAgent::set_enabled(bool state) {
	this->enabled = state;
}
	/* Remove a MidiHook
*/
void MidiAgent::set_midi_hooks(QVector<MidiHook *> mh)
{
	midiHooks = mh;
}
void MidiAgent::remove_MidiHook(MidiHook *hook)
{

	if (midiHooks.contains(hook)) {
		midiHooks.removeOne(hook);
	}
}

/* Clears all the MidiHooks for this device.
*/
void MidiAgent::clear_MidiHooks()
{
	midiHooks.clear();
}

/* Get this MidiAgent state as OBS Data. (includes midi hooks)
* This is needed to Serialize the state in the config.
* https://obsproject.com/docs/reference-settings.html
*/
obs_data_t *MidiAgent::GetData()
{
	obs_data_t *data = obs_data_create();
	obs_data_set_string(data, "name", midi_input_name.toStdString().c_str());
	obs_data_set_string(data, "outname", midi_output_name.toStdString().c_str());

	obs_data_set_bool(data, "enabled", enabled);
	obs_data_set_bool(data, "bidirectional", bidirectional);

	obs_data_array_t *arrayData = obs_data_array_create();
	for (int i = 0; i < midiHooks.size(); i++) {
		obs_data_t *hookData = midiHooks.at(i)->GetData();
		obs_data_array_push_back(arrayData, hookData);
	}
	obs_data_set_array(data, "hooks", arrayData);
	return data;
}
/*Handle OBS events*/
void MidiAgent::handle_obs_event(QString eventType, QString eventData)
{
	if (!this->sending) {
		MidiMessage message;
		OBSDataAutoRelease data = obs_data_create_from_json(
			eventData.toStdString().c_str());

		MidiAgent *self = static_cast<MidiAgent *>(this);
		if (self->enabled == false || self->connected == false) {
			return;
		}

		// ON EVENT TYPE Find matching hook, pull data from that hook, and do thing.

		if (eventType == QString("SourceVolumeChanged")) {
			double vol = obs_data_get_double(data, "volume");
			uint8_t newvol = Utils::mapper2(cbrt(vol));
			QString source = QString(obs_data_get_string(data, "sourceName"));
			for (unsigned i = 0; i < self->midiHooks.size(); i++) {
				if (self->midiHooks.at(i)->action ==
					    Utils::translate_action(ActionsClass::Actions::Set_Volume) &&
				    self->midiHooks.at(i)->audio_source==
					    source) {
					message.message_type =
						self->midiHooks.at(i)
							->message_type;
					message.channel =
						self->midiHooks.at(i)->channel;
					message.NORC =
						self->midiHooks.at(i)->norc;
					message.value = newvol;
					this->send_message_to_midi_device(message);
				}
			}
		} else if (eventType == QString("SwitchScenes")) {
			QString source = QString::fromStdString(
				obs_data_get_string(data, "scene-name"));
			for (unsigned i = 0; i < self->midiHooks.size(); i++) {
				if (self->midiHooks.at(i)->action ==
				     Utils::translate_action(ActionsClass::Actions::Set_Current_Scene) &&
				    self->midiHooks.at(i)->scene == source) {
					message.message_type = "Note Off";
					message.channel =
						self->midiHooks.at(i)->channel;
					message.NORC = lastscenebtn;
					message.value = 0;
					this->send_message_to_midi_device(message);
					message.message_type = "Note On";
					message.value = 1;
					this->send_message_to_midi_device(message);
					lastscenebtn = self->midiHooks.at(i)->norc;
				}
			}

		} else if (eventType == QString("TransitionBegin")) {
			QString from = obs_data_get_string(data, "from-scene");
			for (unsigned i = 0; i < self->midiHooks.size(); i++) {
				if (self->midiHooks.at(i)->action ==
					    Utils::translate_action(
						    ActionsClass::Actions::
							    Set_Current_Scene) &&
				    self->midiHooks.at(i)->scene == from) {
					message.channel =
						self->midiHooks.at(i)->channel;
					message.message_type = "Note On";
					message.NORC =
						self->midiHooks.at(i)->norc;
					message.value = 0;
					this->send_message_to_midi_device(message);
					message.message_type = "Note Off";
					this->send_message_to_midi_device(message);
					
				}
			}

		} else if (eventType == QString("SourceMuteStateChanged")) {
			QString from = obs_data_get_string(data, "sourceName");
			for (unsigned i = 0; i < self->midiHooks.size(); i++) {
				if (self->midiHooks.at(i)->action ==

				    Utils::translate_action(ActionsClass::Actions::Toggle_Mute) &&
				    self->midiHooks.at(i)->audio_source ==
					    from) {
					bool muted = obs_data_get_bool(data,
								       "muted");
					message.message_type = "Note On";
					message.channel =
						self->midiHooks.at(i)->channel;
					message.NORC =
						self->midiHooks.at(i)->norc;
					message.value = muted;
					this->send_message_to_midi_device(message);
				}
			}
		} else if (eventType == QString("SourceRenamed")) {
			QString from =
				obs_data_get_string(data, "previousName");
			for (unsigned i = 0; i < self->midiHooks.size(); i++) {
				if (self->midiHooks.at(i)->source == from) {
					self->midiHooks.at(i)->source =
						obs_data_get_string(data,
								    "newName");
					self->GetData();
				}
			}
		} else if (eventType == QString("Exiting")) {
			closing = true;

		} else if (eventType == QString("SourceDestroyed")) {
			if (!closing) {
				QString from =
					obs_data_get_string(data, "sourceName");

				for (unsigned i = 0; i < self->midiHooks.size();
				     i++) {
					if (self->midiHooks.at(i)->source ==
					    from) {
						self->remove_MidiHook(
							self->midiHooks.at(i));
						self->GetData();
						i--;
					}
				}
				GetConfig()->Save();
			}
		}

		blog(1, "OBS EVENT- MidiAgent.cpp %s -- %s",
		     eventType.toStdString().c_str(),
		     eventData.toStdString().c_str());

	} else {
		this->sending = false;
	}
}
void MidiAgent::send_message_to_midi_device(MidiMessage message)
{
	rtmidi::message *hello = new rtmidi::message();
	if (message.message_type == "Control Change") {
		this->midiout->send_message(
			hello->control_change(message.channel, message.NORC, message.value));
	} else if (message.message_type == "Note On") {
		this->midiout->send_message(hello->note_on(
			message.channel, message.NORC, message.value));

	} else if (message.message_type == "Note Off") {
		this->midiout->send_message(hello->note_off(
			message.channel, message.NORC, message.value));
	}
}



void MidiAgent::do_obs_action(MidiHook *hook, int MidiVal,
			      ActionsClass::Actions action)
{
	switch (action) {
	case ActionsClass::Actions::Set_Current_Scene:
		OBSController::SetCurrentScene(hook->scene);
		break;
	case ActionsClass::Actions::Reset_Scene_Item:
		OBSController::ResetSceneItem(hook->scene, hook->item);
		break;
	case ActionsClass::Actions::Toggle_Mute:
		OBSController::ToggleMute(hook->audio_source);
		break;
	case ActionsClass::Actions::Do_Transition:
		OBSController::TransitionToProgram();
		break;
	case ActionsClass::Actions::Set_Current_Transition:
		OBSController::SetCurrentTransition(hook->transition);
		break;
	case  ActionsClass::Actions::Set_Mute:
		OBSController::SetMute(hook->audio_source, hook->bool_override);
		break;
	case  ActionsClass::Actions::Toggle_Start_Stop_Streaming:
		OBSController::StartStopStreaming();
		break;
	case  ActionsClass::Actions::Set_Preview_Scene:
		OBSController::SetPreviewScene(hook->scene);
		break;
	case  ActionsClass::Actions::Set_Current_Scene_Collection:
		OBSController::SetCurrentSceneCollection(
			hook->scene_collection);
		break;
	case  ActionsClass::Actions::Set_Transition_Duration:
		if (hook->duration != -1) {
			OBSController::SetTransitionDuration(hook->duration);
		} else {
			OBSController::SetTransitionDuration(MidiVal);
		}
		break;
	case  ActionsClass::Actions::Start_Streaming:
		OBSController::StartStreaming();
		break;
	case  ActionsClass::Actions::Stop_Streaming:
		OBSController::StopStreaming();
		break;
	case  ActionsClass::Actions::Start_Recording:
		OBSController::StartRecording();
		break;
	case  ActionsClass::Actions::Stop_Recording:
		OBSController::StopRecording();
		break;
	case  ActionsClass::Actions::Start_Replay_Buffer:
		OBSController::StartReplayBuffer();
		break;
	case  ActionsClass::Actions::Stop_Replay_Buffer:
		OBSController::StopReplayBuffer();
		break;
	case  ActionsClass::Actions::Set_Volume:
		OBSController::SetVolume(hook->audio_source,
					 pow(Utils::mapper(MidiVal), 3.0));
		break;
	case  ActionsClass::Actions::Take_Source_Screenshot:
		OBSController::TakeSourceScreenshot(hook->source);
		break;
	case  ActionsClass::Actions::Pause_Recording:
		OBSController::PauseRecording();
		break;
	case  ActionsClass::Actions::Enable_Source_Filter:
		OBSController::EnableSourceFilter(obs_get_source_by_name(
			hook->source.toStdString().c_str()));
		break;
	case  ActionsClass::Actions::Disable_Source_Filter:
		OBSController::DisableSourceFilter(obs_get_source_by_name(
			hook->source.toStdString().c_str()));
		break;
	case  ActionsClass::Actions::Toggle_Start_Stop_Recording:
		OBSController::StartStopRecording();
		break;
	case  ActionsClass::Actions::Toggle_Start_Stop_Replay_Buffer:
		OBSController::StartStopReplayBuffer();
		break;
	case  ActionsClass::Actions::Resume_Recording:
		OBSController::ResumeRecording();
		break;
	case  ActionsClass::Actions::Save_Replay_Buffer:
		OBSController::SaveReplayBuffer();
		break;
	case  ActionsClass::Actions::Set_Current_Profile:
		OBSController::SetCurrentProfile(hook->profile);
		break;
	case  ActionsClass::Actions::Toggle_Source_Filter:
		OBSController::ToggleSourceFilter(obs_get_source_by_name(hook->source.toStdString().c_str()));
		break;
	case  ActionsClass::Actions::Set_Text_GDIPlus_Text:
		OBSController::SetTextGDIPlusText(hook->string_override);
		break;
	case  ActionsClass::Actions::Set_Browser_Source_URL:
		OBSController::SetBrowserSourceURL(hook->source, hook->string_override);
		break;
	case  ActionsClass::Actions::Reload_Browser_Source:
		OBSController::ReloadBrowserSource(hook->source);
		break;
	case  ActionsClass::Actions::Set_Sync_Offset:
		OBSController::SetSyncOffset(hook->media_source,
					     (int64_t)MidiVal);
		break;
	case  ActionsClass::Actions::Set_Source_Rotation:
		OBSController::SetSourceRotation();
		break;
	case  ActionsClass::Actions::Set_Source_Position:
		OBSController::SetSourcePosition();
		break;
	case  ActionsClass::Actions::Set_Gain_Filter:
		OBSController::SetGainFilter();
		break;
	case  ActionsClass::Actions::Set_Opacity:
		OBSController::SetOpacity();
		break;
	case  ActionsClass::Actions::Set_Source_Scale:
		OBSController::SetSourceScale();
		break;
	case ActionsClass::Actions::Move_T_Bar:
		OBSController::move_t_bar(MidiVal);
		break;
	case ActionsClass::Actions::Play_Pause_Media:
		OBSController::play_pause_media_source(hook->media_source);
		break;
	case ActionsClass::Actions::Studio_Mode:
		OBSController::toggle_studio_mode();
		break;
	case ActionsClass::Actions::Reset_Stats:
		OBSController::reset_stats();
		break;
	case ActionsClass::Actions::Restart_Media:
		OBSController::restart_media(hook->media_source);
		break;

	case ActionsClass::Actions::Stop_Media:
		OBSController::stop_media(hook->media_source);
		break;
	case ActionsClass::Actions::Previous_Media:
		OBSController::prev_media(hook->media_source);
		break;
	case ActionsClass::Actions::Next_Media:
		OBSController::next_media(hook->media_source);
		break;
	};
}
