
#include <iostream>
#include <obs-module.h>
#if __has_include(<obs-frontend-api.h>)
#include <obs-frontend-api.h>
#include "rtmidi17/rtmidi17.hpp"
#else
#include <obs/obs-frontend-api.h>
#include "rtmidi17/rtmidi17.hpp"
#endif
#include <obs-data.h>
#include <string>
#include <map>
#include <iostream>
#include <utility>
#include "obs-midi.h"

#include "forms/settings-dialog.h"
#include <QtWidgets/QAction>
#include <QtWidgets/QMainWindow>
#include "config.h"
#include "device-manager.h"
#include "utils.h"
#include "midi-agent.h"
#include "events.h"
using namespace std;

void ___source_dummy_addref(obs_source_t *) {}
void ___sceneitem_dummy_addref(obs_sceneitem_t *) {}
void ___data_dummy_addref(obs_data_t *) {}
void ___data_array_dummy_addref(obs_data_array_t *) {}
void ___output_dummy_addref(obs_output_t *) {}

void ___data_item_dummy_addref(obs_data_item_t *) {}
void ___data_item_release(obs_data_item_t *dataItem)
{
	obs_data_item_release(&dataItem);
}

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-midi", "en-US")
ConfigPtr _config;
DeviceManagerPtr _deviceManager;
eventsPtr _eventsSystem;

bool obs_module_load(void)
{
	blog(LOG_INFO, "MIDI LOADED! :)");
	qRegisterMetaType<MidiMessage>();
	// Device Manager Setup
	_deviceManager = DeviceManagerPtr(new DeviceManager());

	// Config Setup
	_config = ConfigPtr(new Config());

	// Signal Router Setup
	_eventsSystem = eventsPtr(new events(_deviceManager));
	_config->Load();
	// UI SETUP
	QMainWindow *mainWindow = (QMainWindow *)obs_frontend_get_main_window();
	PluginWindow *pluginWindow = new PluginWindow(mainWindow);
	const char *menuActionText = obs_module_text("OBS MIDI Settings");
	QAction *menuAction =
		(QAction *)obs_frontend_add_tools_menu_qaction(menuActionText);
	QObject::connect(menuAction, SIGNAL(triggered()), pluginWindow,
			 SLOT(ToggleShowHide()));

	// Setup event handler to start the server once OBS is ready
	auto eventCallback = [](enum obs_frontend_event event, void *param) {
		if (event == OBS_FRONTEND_EVENT_FINISHED_LOADING) {
			obs_frontend_remove_event_callback(
				(obs_frontend_event_cb)param, nullptr);
		}
	};
	obs_frontend_add_event_callback(
		eventCallback, (void *)(obs_frontend_event_cb)eventCallback);

	return true;
}

void obs_module_unload()
{
	_config.reset();
	_eventsSystem.reset();
	_deviceManager.reset();
	blog(LOG_INFO, "goodbye!");
}

ConfigPtr GetConfig()
{
	return _config;
}

DeviceManagerPtr GetDeviceManager()
{
	return _deviceManager;
}

eventsPtr GetEventsSystem()
{
	return _eventsSystem;
}

void reloadEvents()
{
	_eventsSystem.reset();
	_eventsSystem = eventsPtr(new events(_deviceManager));
	// Setup event handler to start the server once OBS is ready
	auto eventCallback = [](enum obs_frontend_event event, void *param) {
		if (event == OBS_FRONTEND_EVENT_FINISHED_LOADING) {
			obs_frontend_remove_event_callback(
				(obs_frontend_event_cb)param, nullptr);
		}
	};
	obs_frontend_add_event_callback(
		eventCallback, (void *)(obs_frontend_event_cb)eventCallback);
}
