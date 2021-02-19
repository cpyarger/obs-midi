
#include <QtWidgets/QAction>
#include <QtWidgets/QMainWindow>

#include <iostream>
#include <obs-module.h>
#if __has_include(<obs-frontend-api.h>)
#include <obs-frontend-api.h>
#include "rtmidi17/rtmidi17.hpp"
#else
#include <obs-frontend-api/obs-frontend-api.h>
#include "rtmidi17/rtmidi17.hpp"
#endif
#include <obs-data.h>
#include <string>
#include <map>
#include <iostream>
#include <utility>

#include "obs-midi.h"

#include "src/forms/settings-dialog.h"
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
	plugin_window = new PluginWindow(mainWindow);
	const char *menuActionText = obs_module_text("OBS MIDI Settings");
	QAction *menuAction =
		(QAction *)obs_frontend_add_tools_menu_qaction(menuActionText);
	QObject::connect(menuAction, SIGNAL(triggered()), plugin_window,
			 SLOT(ToggleShowHide()));


	return true;
}

void obs_module_unload()
{

	_config.reset();
	_deviceManager.reset();
	_eventsSystem.reset();
	blog(LOG_DEBUG, "goodbye!");
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
