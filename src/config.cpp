/*
obs-midi
Copyright (C) 2016-2017	Stéphane Lepin <stephane.lepin@gmail.com>

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
#else
#include <obs-frontend-api/obs-frontend-api.h>
#endif

#include <QtCore/QCryptographicHash>
#include <QtCore/QTime>
#include <QtWidgets/QSystemTrayIcon>

#define SECTION_NAME "MidiAPI"
#define PARAM_DEBUG "DebugEnabled"
#define PARAM_ALERT "AlertsEnabled"
#define PARAM_DEVICES "MidiDevices"

#define DEFUALT_DEVICES "{\"devices\": []}"

#include "utils.h"
#include "obs-midi.h"
#include "config.h"
#include "device-manager.h"
#include "forms/settings-dialog.h"
#define QT_TO_UTF8(str) str.toUtf8().constData()

Config::Config()
	: DebugEnabled(false), AlertsEnabled(true), SettingsLoaded(false)
{
	this->setParent(plugin_window);
	qsrand(QTime::currentTime().msec());

	SetDefaults();

	obs_frontend_add_event_callback(on_frontend_event, this);
}

Config::~Config()
{
	obs_frontend_remove_event_callback(on_frontend_event, this);
}

/* Load the configuration from the OBS Config Store
 */
void Config::Load()
{
	SetDefaults();
	config_t *obsConfig = GetConfigStore();

	DebugEnabled = config_get_bool(obsConfig, SECTION_NAME, PARAM_DEBUG);
	AlertsEnabled = config_get_bool(obsConfig, SECTION_NAME, PARAM_ALERT);

	auto deviceManager = GetDeviceManager();
	obs_data_t *deviceManagerData = obs_data_create_from_json(

		config_get_string(obsConfig, SECTION_NAME, PARAM_DEVICES));
	blog(LOG_INFO, "Loaded: \n %s",
	     config_get_string(obsConfig, SECTION_NAME, PARAM_DEVICES));
	deviceManager->Load(deviceManagerData);
	SettingsLoaded = true;
}

/* Save the configuration to the OBS Config Store
 */
void Config::Save()
{
	config_t *obsConfig = GetConfigStore();

	config_set_bool(obsConfig, SECTION_NAME, PARAM_DEBUG, DebugEnabled);
	config_set_bool(obsConfig, SECTION_NAME, PARAM_ALERT, AlertsEnabled);

	auto deviceManager = GetDeviceManager();
	auto data = deviceManager->GetData();
	config_set_string(obsConfig, SECTION_NAME, PARAM_DEVICES,
			  obs_data_get_json(data));
	obs_data_release(data);
	config_save(obsConfig);
}

void Config::SetDefaults()
{
	// OBS Config defaults
	config_t *obsConfig = GetConfigStore();
	if (obsConfig) {
		config_set_default_bool(obsConfig, SECTION_NAME, PARAM_DEBUG,
					DebugEnabled);
		config_set_default_bool(obsConfig, SECTION_NAME, PARAM_ALERT,
					AlertsEnabled);
		config_set_default_string(obsConfig, SECTION_NAME,
					  PARAM_DEVICES, DEFUALT_DEVICES);
	}
}

config_t *Config::GetConfigStore()
{
	return obs_frontend_get_profile_config();
}

void Config::on_frontend_event(obs_frontend_event event, void *param)
{
	if (event == OBS_FRONTEND_EVENT_PROFILE_CHANGED) {
		auto deviceManager = GetDeviceManager();
		deviceManager->Unload();
		auto config = GetConfig();
		config->SetDefaults();
		config->Load();
	}
}
