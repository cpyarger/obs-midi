/*
obs-websocket
Copyright (C) 2016-2019	Stéphane Lepin <stephane.lepin@gmail.com>

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
#include "rtmidi17/rtmidi17.hpp"
#else
#include "rtmidi17/rtmidi17.hpp"
#endif
#include <stdio.h>
#include <iostream>
#include <vector>

#include <QtCore/QString>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QSystemTrayIcon>
#include <QMessageBox>
#include <obs.hpp>
#include <obs-module.h>
#include <util/config-file.h>
#include "obs-midi.h"

typedef void (*PauseRecordingFunction)(bool);
typedef bool (*RecordingPausedFunction)();

enum class Pairs {
	Scene,
	Source,
	Item,
	Transition,
	Audio,
	Media,
	Filter,
	String,
	Integer,
	Boolean
};
class ActionsClass : public QObject {
	Q_OBJECT
public:
	enum class Actions {
		Disable_Preview,
		Disable_Source_Filter,
		Enable_Preview,
		Enable_Source_Filter,
		Next_Media,
		Pause_Recording,
		Play_Pause_Media,
		Previous_Media,
		Reset_Scene_Item,
		Reset_Stats,
		Restart_Media,
		Scrub_Media,
		Set_Audio_Monitor_Type,
		Set_Current_Scene,
		Set_Current_Scene_Collection,
		Set_Current_Transition,
		Set_Gain_Filter,
		Set_Media_Time,
		Set_Mute,
		Set_Preview_Scene,
		Set_Scene_Item_Crop,
		Set_Scene_Item_Position,
		Set_Scene_Item_Render,
		Set_Scene_Item_Transform,
		Set_Scene_Transition_Override,
		Set_Source_Filter_Visibility,
		Set_Source_Name,
		Set_Source_Settings,
		Set_Sync_Offset,
		Set_Transition_Duration,
		Set_Volume,
		Start_Recording,
		Start_Replay_Buffer,
		Start_Streaming,
		Stop_Media,
		Stop_Recording,
		Stop_Replay_Buffer,
		Stop_Streaming,
		Studio_Mode,
		Take_Source_Screenshot,
		Toggle_Mute,
		Toggle_Source_Filter,
		Toggle_Start_Stop_Streaming,
		Toggle_Start_Stop_Recording,
		Toggle_Start_Stop_Replay_Buffer,
		Do_Transition,
		Unpause_Recording,
		Resume_Recording,
		Save_Replay_Buffer,
		Set_Current_Profile,
		Set_Text_GDIPlus_Text,
		Set_Browser_Source_URL,
		Reload_Browser_Source,
		Set_Source_Scale,
		Set_Source_Rotation,
		Set_Source_Position,
		Set_Opacity,
		Move_T_Bar,
		Toggle_Source_Visibility
	};
	Q_ENUM(Actions)
	enum class obs_event_type {
		SourceVolumeChanged,
		SwitchScenes,
		TransitionBegin,
		TransitionEnd,
		SourceMuteStateChanged,
		SourceRenamed,
		Exiting,
		SourceDestroyed,
	};
	Q_ENUM(obs_event_type)

	static QString action_to_string(const Actions &enumval);
	static Actions string_to_action(const QString &string);

	static QString event_to_string(const obs_event_type &enumval);
	static obs_event_type string_to_event(const QString &string);
};

typedef void (*PauseRecordingFunction)(bool);
typedef bool (*RecordingPausedFunction)();
namespace Utils {

class OBSActionsWidget;
float mapper(int x);
int mapper2(double x);
int t_bar_mapper(int x);

bool is_number(const QString &s);
bool isJSon(QString val);

QString get_midi_message_type(rtmidi::message message);
QStringList GetMediaSourceNames();
QStringList GetAudioSourceNames();

QString nsToTimestamp(uint64_t ns);
obs_data_array_t *StringListToArray(char **strings, const char *key);
obs_data_array_t *GetSceneItems(obs_source_t *source);
QStringList GetSceneItemsBySource(obs_source_t *source);
obs_data_t *GetSceneItemData(obs_sceneitem_t *item);
OBSDataArrayAutoRelease GetSourceArray();
OBSDataArrayAutoRelease GetSceneArray(QString name = NULL);
// These functions support nested lookup into groups
obs_sceneitem_t *GetSceneItemFromName(obs_scene_t *scene, QString name);
obs_sceneitem_t *GetSceneItemFromId(obs_scene_t *scene, int64_t id);
obs_sceneitem_t *GetSceneItemFromItem(obs_scene_t *scene, obs_data_t *item);
obs_sceneitem_t *GetSceneItemFromRequestField(obs_scene_t *scene,
					      obs_data_item_t *dataItem);

obs_scene_t *GetSceneFromNameOrCurrent(QString sceneName);
obs_data_t *GetSceneItemPropertiesData(obs_sceneitem_t *item);

obs_data_t *GetSourceFilterInfo(obs_source_t *filter, bool includeSettings);
obs_data_array_t *GetSourceFiltersList(obs_source_t *source,
				       bool includeSettings);

bool IsValidAlignment(const uint32_t alignment);

obs_data_array_t *GetScenes();
obs_data_t *GetSceneData(obs_source_t *source);

// TODO contribute a proper frontend API method for this to OBS and remove this hack
int GetTransitionDuration(obs_source_t *transition);
obs_source_t *GetTransitionFromName(QString transitionName);
bool SetTransitionByName(QString transitionName);
obs_data_t *GetTransitionData(obs_source_t *transition);

QString OBSVersionString();

const char *GetRecordingFolder();
bool SetRecordingFolder(const char *path);

QString ParseDataToQueryString(obs_data_t *data);
obs_hotkey_t *FindHotkeyByName(QString name);

bool ReplayBufferEnabled();
void StartReplayBuffer();
bool IsRPHotkeySet();
const char *GetFilenameFormatting();
bool SetFilenameFormatting(const char *filenameFormatting);
bool inrange(int low, int high, int x);
QStringList GetTransitionsList();
QStringList GetSceneItemsList(QString scene);
bool inrange(int low, int high, int x);
QString mtype_to_string(rtmidi::message_type);

int get_midi_note_or_control(rtmidi::message mess);
int get_midi_value(rtmidi::message mess);
QSpinBox *GetTransitionDurationControl();

QStringList TranslateActions();
QStringList get_scene_names();
QStringList get_source_names(QString scene);
QStringList get_filter_names(QString Source);
QStringList get_transition_names();

QString untranslate(QString tstring);
const QList<ActionsClass::Actions> AllActions_raw = {
	ActionsClass::Actions::Disable_Preview,
	ActionsClass::Actions::Disable_Source_Filter,
	ActionsClass::Actions::Enable_Preview,
	ActionsClass::Actions::Enable_Source_Filter,
	ActionsClass::Actions::Next_Media,
	ActionsClass::Actions::Pause_Recording,
	ActionsClass::Actions::Play_Pause_Media,
	ActionsClass::Actions::Previous_Media,
	ActionsClass::Actions::Reset_Scene_Item,
	ActionsClass::Actions::Toggle_Source_Visibility,
	ActionsClass::Actions::Restart_Media,
	ActionsClass::Actions::Set_Current_Scene,
	ActionsClass::Actions::Set_Current_Transition,
	ActionsClass::Actions::Set_Preview_Scene,
	ActionsClass::Actions::Set_Scene_Transition_Override,
	ActionsClass::Actions::Set_Volume,
	ActionsClass::Actions::Start_Recording,
	ActionsClass::Actions::Start_Replay_Buffer,
	ActionsClass::Actions::Start_Streaming,
	ActionsClass::Actions::Stop_Media,
	ActionsClass::Actions::Stop_Recording,
	ActionsClass::Actions::Stop_Replay_Buffer,
	ActionsClass::Actions::Stop_Streaming,
	ActionsClass::Actions::Take_Source_Screenshot,
	ActionsClass::Actions::Toggle_Mute,
	ActionsClass::Actions::Toggle_Source_Filter,
	ActionsClass::Actions::Toggle_Start_Stop_Streaming,
	ActionsClass::Actions::Toggle_Start_Stop_Recording,
	ActionsClass::Actions::Toggle_Start_Stop_Replay_Buffer,
	ActionsClass::Actions::Do_Transition,
	ActionsClass::Actions::Unpause_Recording,
	ActionsClass::Actions::Resume_Recording,
	ActionsClass::Actions::Save_Replay_Buffer,
	ActionsClass::Actions::Reload_Browser_Source,
	ActionsClass::Actions::Move_T_Bar};
const QList<ActionsClass::Actions> not_ready_actions{
	ActionsClass::Actions::Studio_Mode,
	ActionsClass::Actions::Set_Current_Scene_Collection,
	ActionsClass::Actions::Reset_Stats,
	ActionsClass::Actions::Set_Current_Profile,
	ActionsClass::Actions::Set_Source_Scale,
	ActionsClass::Actions::Set_Source_Rotation,
	ActionsClass::Actions::Set_Source_Position,
	ActionsClass::Actions::Set_Source_Filter_Visibility,
	ActionsClass::Actions::Set_Source_Name,
	ActionsClass::Actions::Set_Transition_Duration,
	ActionsClass::Actions::Set_Gain_Filter,
	ActionsClass::Actions::Set_Media_Time,
	ActionsClass::Actions::Set_Source_Settings,
	ActionsClass::Actions::Set_Sync_Offset,
	ActionsClass::Actions::Set_Mute,
	ActionsClass::Actions::Scrub_Media,
	ActionsClass::Actions::Set_Audio_Monitor_Type,
	ActionsClass::Actions::Set_Scene_Item_Crop,
	ActionsClass::Actions::Set_Scene_Item_Position,
	ActionsClass::Actions::Set_Scene_Item_Render,
	ActionsClass::Actions::Set_Scene_Item_Transform,
	ActionsClass::Actions::Set_Text_GDIPlus_Text,
	//ActionsClass::Actions::Set_Opacity,
	ActionsClass::Actions::Set_Browser_Source_URL,
};

void alert_popup(QString message);
QString translate_action(ActionsClass::Actions action);
};

typedef struct MidiMessage {
	MidiMessage(){};
	void set_message(rtmidi::message message)
	{
		this->channel = message.get_channel();
		this->message_type = Utils::get_midi_message_type(message);
		this->NORC = Utils::get_midi_note_or_control(message);
		this->value = Utils::get_midi_value(message);
	}
	QString device_name;
	QString message_type;
	int channel = 0;
	int NORC = 0;
	int value = 0;
	MidiMessage get() { return (MidiMessage) * this; }
} MidiMessage;
Q_DECLARE_METATYPE(MidiMessage);
class MidiHook : public QObject {
	Q_OBJECT

public:
	int channel = -1;     //midi channel
	QString message_type; // Message Type
	int norc = -1;        // Note or Control
	QString action;
	QString scene;
	QString source;
	QString filter;
	QString transition;
	QString item;
	QString audio_source;
	QString media_source;
	int duration = -1;
	QString scene_collection;
	QString profile;
	QString string_override;
	bool bool_override = false;
	int int_override = -1;
	int value = -1;
	MidiMessage *get_message_from_hook()
	{
		MidiMessage *message = new MidiMessage();
		message->channel = this->channel;
		message->message_type = this->message_type;
		message->NORC = this->norc;
		return message;
	}
	MidiHook(){};

	MidiHook(QString jsonString)
	{

		obs_data_t *data = obs_data_create_from_json(
			jsonString.toStdString().c_str());
		channel = obs_data_get_int(data, "channel");
		message_type = obs_data_get_string(data, "message_type");
		norc = obs_data_get_int(data, "norc");
		action = obs_data_get_string(data, "action");
		scene = obs_data_get_string(data, "scene");
		source = obs_data_get_string(data, "source");
		filter = obs_data_get_string(data, "filter");
		transition = obs_data_get_string(data, "transition");
		item = obs_data_get_string(data, "item");
		audio_source = obs_data_get_string(data, "audio_source");
		media_source = obs_data_get_string(data, "media_source");
		duration = obs_data_get_int(data, "duration");
		scene_collection =
			obs_data_get_string(data, "scene_collection");
		profile = obs_data_get_string(data, "profile");
		string_override = obs_data_get_string(data, "string_override");
		bool_override = obs_data_get_bool(data, "bool_override");
		int_override = obs_data_get_int(data, "int_override");
	}

	obs_data_t *GetData()
	{
		obs_data_t *data = obs_data_create();
		obs_data_set_int(data, "channel", channel);
		obs_data_set_string(data, "message_type",
				    message_type.toStdString().c_str());
		obs_data_set_int(data, "norc", norc);
		obs_data_set_string(data, "action",
				    action.toStdString().c_str());
		obs_data_set_string(data, "scene", scene.toStdString().c_str());
		obs_data_set_string(data, "source",
				    source.toStdString().c_str());
		obs_data_set_string(data, "filter",
				    filter.toStdString().c_str());
		obs_data_set_string(data, "transition",
				    transition.toStdString().c_str());
		obs_data_set_string(data, "item", item.toStdString().c_str());
		obs_data_set_string(data, "audio_source",
				    audio_source.toStdString().c_str());
		obs_data_set_string(data, "media_source",
				    media_source.toStdString().c_str());
		obs_data_set_int(data, "duration", duration);
		obs_data_set_string(data, "scene_collection",
				    scene_collection.toStdString().c_str());
		obs_data_set_string(data, "profile",
				    profile.toStdString().c_str());
		obs_data_set_string(data, "string_override",
				    string_override.toStdString().c_str());
		obs_data_set_bool(data, "bool_override", bool_override);
		obs_data_set_int(data, "int_override", int_override);

		return data;
	}

	const char *ToJSON() { return obs_data_get_json(GetData()); }
};
