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
#include <obs-frontend-api.h>
#else
#include <obs-frontend-api/obs-frontend-api.h>
#endif

#include <util/config-file.h>
#include <QtCore/QString>
#include <set>
#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QSharedPointer>
#include <QtCore/QVariantHash>
#include <QtCore/QThreadPool>
#include <vector>
#include "midi-agent.h"
#include "rpc/RpcEvent.h"
#include "obs-controller.h"

class DeviceManager : public QObject {
	Q_OBJECT
public:
	DeviceManager();
	~DeviceManager();
	void Load(obs_data_t *data);
	void Unload();

	QStringList GetPortsList();
	int GetPortNumberByDeviceName(QString deviceName);
	QStringList GetOutPortsList();
	int GetOutPortNumberByDeviceName(QString deviceName);
	QStringList opl;
	QVector<MidiAgent *> GetActiveMidiDevices();
	MidiAgent *GetMidiDeviceByName(QString deviceName);
	QVector<MidiHook *> GetMidiHooksByDeviceName(QString deviceName);
	MidiAgent * RegisterMidiDevice(int port, int outport);

	obs_data_t *GetData();
	void broadcast_obs_event(const RpcEvent &event);
signals:
	void bcast(QString updateType, QString eventData);

private:
	rtmidi::midi_in rtMidi;
	rtmidi::midi_out MO;

	QVector<MidiAgent *> midiAgents;
};
