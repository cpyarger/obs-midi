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

#include "device-manager.h"

DeviceManager::DeviceManager()
{
	rtMidi = new rtmidi::midi_in();
	MO = new rtmidi::midi_out();
}

DeviceManager::~DeviceManager()
{
	Unload();
	delete rtMidi;
	delete MO;
}

/* Load the Device Manager from saved Config Store data.
 * This method is called from Config. Should only be called once on runtime
 */
void DeviceManager::Load(obs_data_t *data)
{
	QStringList portsList = GetPortsList();
	QStringList outPortsList = GetOutPortsList();
	obs_data_array_t *devicesData = obs_data_get_array(data, "devices");
	size_t deviceCount = obs_data_array_count(devicesData);
	for (size_t i = 0; i < deviceCount; i++) {
		obs_data_t *deviceData = obs_data_array_item(devicesData, i);
		MidiAgent *device = new MidiAgent();
		device->Load(deviceData);
		obs_data_release(deviceData);
		midiAgents.push_back(device);

		connect(this, SIGNAL(bcast(QString, QString)), device,
			SLOT(handle_obs_event(QString, QString)));
		if (device->isEnabled()) {
			int portNumber = GetPortNumberByDeviceName(
				device->get_midi_input_name());
			int outPort = GetOutPortNumberByDeviceName(
				device->get_midi_output_name());

			if (portNumber != -1) {
				device->open_midi_input_port(portNumber);
			}
			if (outPort != -1) {
				device->open_midi_output_port(outPort);
			}
		}
	}

	obs_data_array_release(devicesData);
}

void DeviceManager::Unload()
{
	blog(LOG_INFO, "UNLOADING DEVICE MANAGER");
	midiAgents.clear();
	for (auto agent : midiAgents) {
		agent->clear_MidiHooks();
		delete agent;
	}
}

/* Returns vector list of Port Names 
 */
QStringList DeviceManager::GetPortsList()
{
	QStringList ports;
	int portCount = rtMidi->get_port_count();
	for (int i = 0; i < portCount; i++) {
		ports.append(QString::fromStdString(rtMidi->get_port_name(i)));
	}
	return ports;
}

/* Returns vector list of Port Names 
 */
QStringList DeviceManager::GetOutPortsList()
{
	opl.clear();
	QStringList outports;
	int portCount = MO->get_port_count();
	for (int i = 0; i < portCount; i++) {
		outports.append(QString::fromStdString(MO->get_port_name(i)));
		opl.append(QString::fromStdString(MO->get_port_name(i)));
	}
	return outports;
}

/* Returns the port number of the specified device.
 * If the device isn't found (possibly due to being disconnected), returns -1
 */
int DeviceManager::GetPortNumberByDeviceName(QString deviceName)
{
	QStringList portsList = GetPortsList();

	if (portsList.contains(deviceName)) {
		return portsList.indexOf(deviceName);
	} else {
		return -1;
	}
}

/* Returns the port number of the specified device.
 * If the device isn't found (possibly due to being disconnected), returns -1
 */
int DeviceManager::GetOutPortNumberByDeviceName(QString deviceName)
{

	QStringList portsList = GetOutPortsList();

	if (portsList.contains(deviceName)) {
		return portsList.indexOf(deviceName);
	} else {
		return -1;
	}
}

QVector<MidiAgent *> DeviceManager::GetActiveMidiDevices()
{
	return midiAgents;
}

MidiAgent *DeviceManager::GetMidiDeviceByName(QString deviceName)
{
	for (int i = 0; i < midiAgents.size(); i++) {
		if (midiAgents.at(i)->get_midi_input_name() == deviceName) {
			return midiAgents.at(i);
		}
	}
	return NULL;
}

QVector<MidiHook *> DeviceManager::GetMidiHooksByDeviceName(QString deviceName)
{
	if (deviceName != QString("No Devices Available")) {
		auto device = GetMidiDeviceByName(deviceName);
		if (device != NULL) {
			return device->GetMidiHooks();
		} else {
			QVector<MidiHook *> x;
			return x;
		}
	}
}

/* Registers a midi device.
 * Will create, store and enable a midi device.
*/
MidiAgent * DeviceManager::RegisterMidiDevice(int port, int outport)
{
	MidiAgent * midiA = new MidiAgent();
	midiA->open_midi_input_port(port);
	midiA->open_midi_output_port(outport);

	midiAgents.push_back(midiA);
	return midiA;
}

/* Get this Device Manager state as OBS Data. (includes devices and their midi hooks)
* This is needed to Serialize the state in the config.
* https://obsproject.com/docs/reference-settings.html
*/
obs_data_t *DeviceManager::GetData()
{
	obs_data_t *data = obs_data_create();

	obs_data_array_t *deviceData = obs_data_array_create();
	for (int i = 0; i < midiAgents.size(); i++) {
		obs_data_array_push_back(deviceData,
					 midiAgents.at(i)->GetData());
	}

	obs_data_set_array(data, "devices", deviceData);
	return data;
}

void DeviceManager::broadcast_obs_event(const RpcEvent &event)
{
	OBSDataAutoRelease eventData = obs_data_create();

	QString updateType = event.updateType();
	obs_data_set_string(eventData, "update-type",
			    updateType.toUtf8().constData());

	std::optional<uint64_t> streamTime = event.streamTime();
	if (streamTime.has_value()) {
		QString streamingTimecode =
			Utils::nsToTimestamp(streamTime.value());
		obs_data_set_string(eventData, "stream-timecode",
				    streamingTimecode.toUtf8().constData());
	}

	std::optional<uint64_t> recordingTime = event.recordingTime();
	if (recordingTime.has_value()) {
		QString recordingTimecode =
			Utils::nsToTimestamp(recordingTime.value());
		obs_data_set_string(eventData, "rec-timecode",
				    recordingTimecode.toUtf8().constData());
	}

	OBSData additionalFields = event.additionalFields();
	if (additionalFields) {
		obs_data_apply(eventData, additionalFields);
	}

	//blog(1, "OBS EVENT %s -- %s", event.updateType().toStdString().c_str(),obs_data_get_json(eventData));
	emit bcast(event.updateType(),
		   QString::fromStdString(obs_data_get_json(eventData)));
};
