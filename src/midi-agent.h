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
#include "rtmidi17/rtmidi17.hpp"
#else
#include <obs-frontend-api/obs-frontend-api.h>
#include "rtmidi17/rtmidi17.hpp"
#endif
#include <QtCore/QString>
#include <QtCore/QSharedPointer>
#include <vector>
#include <QObject>
#include <functional>
#include <map>
#include <string>
#include <iostream>
#include "rpc/RpcEvent.h"
#include "utils.h"
#include "obs-controller.h"


class MidiAgent : public QObject {
	Q_OBJECT

public:
	MidiAgent();
	MidiAgent(obs_data_t *data);
	~MidiAgent();
	void Load(obs_data_t *data);

	void open_midi_input_port(int inport);
	void open_midi_output_port(int outport);
	void close_midi_port();
	QString get_midi_input_name();
	QString get_midi_output_name();
	void set_midi_output_name(QString oname);
	int GetPort();
	bool isEnabled();
	bool isConnected();
	bool isBidirectional();
	bool setBidirectional(bool state);
	void set_enabled(bool enabled);
	static void HandleInput(const rtmidi::message &message, void *userData);
	QVector<MidiHook *> GetMidiHooks();
	void set_midi_hooks(QVector<MidiHook *>);
	void add_MidiHook(MidiHook *hook);
	void remove_MidiHook(MidiHook *hook);
	void clear_MidiHooks();
	obs_data_t *GetData();

public slots:
	void handle_obs_event(QString eventType, QString eventData);
signals:
	void broadcast_midi_message(MidiMessage);
	void do_obs_action(MidiHook *, int);

private:
	void send_message_to_midi_device(MidiMessage message);
	rtmidi::midi_in midiin;
	rtmidi::midi_out midiout;
	QString midi_input_name;
	QString midi_output_name;
	bool sending;
	int port;
	int lastscenebtn;
	int last_preview_scene_norc;
	bool enabled=false;
	bool connected=false;
	bool bidirectional = false;
	MidiHook *get_midi_hook_if_exists(MidiMessage *message);
	bool closing = false;
	QVector<MidiHook *> midiHooks;
};
