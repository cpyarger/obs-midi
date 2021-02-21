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
#include <obs-frontend-api/obs-frontend-api.h>
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
	this->setParent(GetDeviceManager().get());
	midi_input_name = "Midi Device (uninit)";
	midi_output_name = "Midi Out Device (uninit)";
	midiin.set_callback(
		[this](const auto &message) { HandleInput(message, this); });
}
MidiAgent::MidiAgent(obs_data_t *midiData)
{
	this->setParent(GetDeviceManager().get());
	midiin.set_callback(
		[this](const auto &message) { HandleInput(message, this); });
	this->Load(midiData);
}
MidiAgent::~MidiAgent()
{
	clear_MidiHooks();
	midiin.cancel_callback();
}
/* Loads information from OBS data. (recalled from Config)
 * This will not enable the MidiAgent or open the port. (and shouldn't)
 */
void MidiAgent::Load(obs_data_t *data)
{
	obs_data_set_default_bool(data, "enabled", false);
	obs_data_set_default_bool(data, "bidirectional", false);
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
		midiin.open_port(inport);
	} catch (const rtmidi::midi_exception &error) {
		blog(LOG_DEBUG, "Midi Error %s", error.what());
	}
	midi_input_name = QString::fromStdString(midiin.get_port_name(inport));
	blog(LOG_INFO, "MIDI device connected In: [%d] %s", inport,
	     midi_input_name.toStdString().c_str());
}
void MidiAgent::open_midi_output_port(int outport)
{
	try {
		midiout.open_port(outport);
	} catch (const rtmidi::midi_exception &error) {
		blog(LOG_DEBUG, "Midi Error %s", error.what());
	}
	midi_output_name =
		QString::fromStdString(midiout.get_port_name(outport));
	blog(LOG_INFO, "MIDI device connected Out: [%d] %s", outport,
	     midi_output_name.toStdString().c_str());
}
/* Will close the port and disable this MidiAgent
*/
void MidiAgent::close_midi_port()
{
	if (midiin.is_port_open()) {
		midiin.close_port();
	}
	if (midiout.is_port_open()) {
		midiout.close_port();
	}
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
		midiout.close_port();
		open_midi_output_port(
			GetDeviceManager()->GetOutPortNumberByDeviceName(
				oname));
	}
	midi_output_name = oname;
}
bool MidiAgent::setBidirectional(bool state)
{
	this->bidirectional = state;
	if (!state) {
		midiout.close_port();
	} else {
		if (midiout.is_port_open()) {
			midiout.close_port();
		}
		open_midi_output_port(
			GetDeviceManager()->GetOutPortNumberByDeviceName(
				midi_output_name));
	}
	GetConfig().get()->Save();
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
	if (self->enabled == false) {
		return;
	}
	/*************Get Message parts***********/
	self->sending = true;
	MidiMessage *x = new MidiMessage();
	x->set_message(message);
	/***** Send Messages to emit function *****/
	x->device_name = self->get_midi_input_name();
	emit self->broadcast_midi_message((MidiMessage)*x);
	/** check if hook exists for this note or cc norc and launch it **/
	OBSController *oc =
		new OBSController(self->get_midi_hook_if_exists(x), x->value);
	oc->~OBSController();
	delete (x);
}
/* Get the midi hooks for this device
*/
QVector<MidiHook *> MidiAgent::GetMidiHooks()
{
	return midiHooks;
}
MidiHook *MidiAgent::get_midi_hook_if_exists(MidiMessage *message)
{
	for (int i = 0; i < this->midiHooks.size(); i++) {
		if (this->midiHooks.at(i)->message_type ==
			    message->message_type &&
		    this->midiHooks.at(i)->norc == message->NORC &&
		    this->midiHooks.at(i)->channel == message->channel) {
			return this->midiHooks.at(i);
		}
	}
	return NULL;
}
void MidiAgent::add_MidiHook(MidiHook *hook)
{
	// Add a new MidiHook
	midiHooks.push_back(hook);
}
void MidiAgent::set_enabled(bool state)
{
	this->enabled = state;
}
void MidiAgent::set_midi_hooks(QVector<MidiHook *> mh)
{
	midiHooks = mh;
}
void MidiAgent::remove_MidiHook(MidiHook *hook)
{
	// Remove a MidiHook
	if (midiHooks.contains(hook)) {
		midiHooks.removeOne(hook);
	}
}
/* Clears all the MidiHooks for this device.
*/
void MidiAgent::clear_MidiHooks()
{
	for (int i = 0; i < midiHooks.count(); i++) {
		delete midiHooks.at(i);
	}
	midiHooks.clear();
}
/* Get this MidiAgent state as OBS Data. (includes midi hooks)
* This is needed to Serialize the state in the config.
* https://obsproject.com/docs/reference-settings.html
*/
obs_data_t *MidiAgent::GetData()
{
	obs_data_t *data = obs_data_create();
	obs_data_set_string(data, "name",
			    midi_input_name.toStdString().c_str());
	obs_data_set_string(data, "outname",
			    midi_output_name.toStdString().c_str());
	obs_data_set_bool(data, "enabled", enabled);
	obs_data_set_bool(data, "bidirectional", bidirectional);
	obs_data_array_t *arrayData = obs_data_array_create();
	for (int i = 0; i < midiHooks.size(); i++) {
		obs_data_t *hookData = midiHooks.at(i)->GetData();
		obs_data_array_push_back(arrayData, hookData);
		obs_data_release(hookData);
	}
	obs_data_set_array(data, "hooks", arrayData);
	return data;
}
/*Handle OBS events*/
void MidiAgent::handle_obs_event(QString eventType, QString eventData)
{
	if (!this->sending) {
		MidiMessage *message = new MidiMessage();
		OBSDataAutoRelease data = obs_data_create_from_json(
			eventData.toStdString().c_str());
		// ON EVENT TYPE Find matching hook, pull data from that hook, and do thing.
		if (eventType == QString("SourceVolumeChanged")) {
			double vol = obs_data_get_double(data, "volume");
			uint8_t newvol = Utils::mapper2(cbrt(vol));
			QString source = QString(
				obs_data_get_string(data, "sourceName"));
			for (int i = 0; i < this->midiHooks.size(); i++) {
				if (this->midiHooks.at(i)->action ==
					    Utils::translate_action(
						    ActionsClass::Actions::
							    Set_Volume) &&
				    this->midiHooks.at(i)->audio_source ==
					    source) {
					message->message_type =
						this->midiHooks.at(i)
							->message_type;
					message->channel =
						this->midiHooks.at(i)->channel;
					message->NORC =
						this->midiHooks.at(i)->norc;
					message->value = newvol;
					this->send_message_to_midi_device(
						message->get());
				}
			}
		} else if (eventType == QString("SwitchScenes")) {
			QString source = QString::fromStdString(
				obs_data_get_string(data, "scene-name"));
			for (int i = 0; i < this->midiHooks.size(); i++) {
				if (this->midiHooks.at(i)->action ==
					    Utils::translate_action(
						    ActionsClass::Actions::
							    Set_Current_Scene) &&
				    this->midiHooks.at(i)->scene == source) {
					message->message_type = "Note Off";
					message->channel =
						this->midiHooks.at(i)->channel;
					message->NORC = lastscenebtn;
					message->value = 0;
					this->send_message_to_midi_device(
						message->get());
					message->NORC =
						this->midiHooks.at(i)->norc;
					message->message_type = "Note On";
					message->value = 1;
					this->send_message_to_midi_device(
						message->get());
					lastscenebtn =
						this->midiHooks.at(i)->norc;
				}
			}
		} else if (eventType == QString("PreviewSceneChanged")) {
			QString source = QString::fromStdString(
				obs_data_get_string(data, "scene-name"));
			for (int i = 0; i < this->midiHooks.size(); i++) {
				if (this->midiHooks.at(i)->action ==
					    Utils::translate_action(
						    ActionsClass::Actions::
							    Set_Preview_Scene) &&
				    this->midiHooks.at(i)->scene == source) {
					message->message_type = "Note Off";
					message->channel =
						this->midiHooks.at(i)->channel;
					message->NORC = last_preview_scene_norc;
					message->value = 0;
					this->send_message_to_midi_device(
						message->get());
					message->NORC =
						this->midiHooks.at(i)->norc;
					message->message_type = "Note On";
					message->value = 1;
					this->send_message_to_midi_device(
						message->get());
					last_preview_scene_norc =
						this->midiHooks.at(i)->norc;
				}
			}
		} else if (eventType == QString("TransitionBegin")) {
			QString from = obs_data_get_string(data, "from-scene");
			for (int i = 0; i < this->midiHooks.size(); i++) {
				if (this->midiHooks.at(i)->action ==
					    Utils::translate_action(
						    ActionsClass::Actions::
							    Set_Current_Scene) &&
				    this->midiHooks.at(i)->scene == from) {
					message->channel =
						this->midiHooks.at(i)->channel;
					message->message_type = "Note On";
					message->NORC =
						this->midiHooks.at(i)->norc;
					message->value = 0;
					this->send_message_to_midi_device(
						message->get());
					message->message_type = "Note Off";
					this->send_message_to_midi_device(
						message->get());
				}
			}
		} else if (eventType == QString("SourceMuteStateChanged")) {
			QString from = obs_data_get_string(data, "sourceName");
			for (int i = 0; i < this->midiHooks.size(); i++) {
				if (this->midiHooks.at(i)->action ==
					    Utils::translate_action(
						    ActionsClass::Actions::
							    Toggle_Mute) &&
				    this->midiHooks.at(i)->audio_source ==
					    from) {
					bool muted = obs_data_get_bool(data,
								       "muted");
					message->message_type = "Note On";
					message->channel =
						this->midiHooks.at(i)->channel;
					message->NORC =
						this->midiHooks.at(i)->norc;
					message->value = muted;
					this->send_message_to_midi_device(
						message->get());
				}
			}
		} else if (eventType == QString("SourceRenamed")) {
			QString from =
				obs_data_get_string(data, "previousName");
			for (int i = 0; i < this->midiHooks.size(); i++) {
				if (this->midiHooks.at(i)->source == from) {
					this->midiHooks.at(i)->source =
						obs_data_get_string(data,
								    "newName");
					this->GetData();
				}
			}
		} else if (eventType == QString("Exiting")) {
			closing = true;
		} else if (eventType == QString("SourceDestroyed")) {
			if (!closing) {
				QString from =
					obs_data_get_string(data, "sourceName");
				for (int i = 0; i < this->midiHooks.size();
				     i++) {
					if (this->midiHooks.at(i)->source ==
					    from) {
						this->remove_MidiHook(
							this->midiHooks.at(i));
						this->GetData();
						i--;
					}
				}
				GetConfig()->Save();
			}
		}
		delete (message);
		obs_data_release(data);
	} else {
		this->sending = false;
	}
}
void MidiAgent::send_message_to_midi_device(MidiMessage message)
{
	std::unique_ptr<rtmidi::message> hello =
		std::make_unique<rtmidi::message>();
	if (message.message_type == "Control Change") {
		this->midiout.send_message(hello->control_change(
			message.channel, message.NORC, message.value));
	} else if (message.message_type == "Note On") {
		this->midiout.send_message(hello->note_on(
			message.channel, message.NORC, message.value));
	} else if (message.message_type == "Note Off") {
		this->midiout.send_message(hello->note_off(
			message.channel, message.NORC, message.value));
	}
}
