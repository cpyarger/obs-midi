cmake_minimum_required(VERSION 3.5)
project(obs-midi VERSION 0.9.0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}" external)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

set(CMAKE_PREFIX_PATH "${QTDIR}")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-DASIO_STANDALONE)

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
    set(CMAKE_CXX_FLAGS "-mfpu=neon")
endif()
##################
# THE GOOD STUFF #
##################
# RTMIDI
add_subdirectory(src/libremidi)
set(obs-midi_DEPS libremidi)
include_directories(src/libremidi)
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/UI/obs-frontend-api")
include_directories(${CMAKE_PACKAGE_PREFIX})

if (((WIN32) AND (NOT COMMAND install_obs_plugin_with_data)) OR APPLE)
	include(external/FindLibObs.cmake)
	find_package(LibObs REQUIRED)
else()
	find_package(LibObs)
endif()

find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
# Version bits
execute_process(
  COMMAND git describe --abbrev=0 --tags 
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_TAG
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/include/version.h.in
  ${CMAKE_CURRENT_SOURCE_DIR}/src/version.h
)

include_directories(
	"${LIBOBS_INCLUDE_DIR}/../UI/obs-frontend-api"
        SYSTEM
	${Qt5Core_INCLUDES}
	${Qt5Widgets_INCLUDES}
        "${CMAKE_SOURCE_DIR}/UI")
set(obs-midi_SOURCES
	src/utils.cpp
	src/config.cpp
	src/midi-agent.cpp
	src/device-manager.cpp
	src/obs-controller.cpp
	src/forms/settings-dialog.cpp
	src/obs-midi.cpp
	src/events.cpp
	src/rpc/RpcEvent.cpp
	src/forms/Macros.cpp
	src/Midi_hook.cpp
	src/Midi_message.cpp)

list(APPEND obs-midi_HEADERS
	src/utils.h
	src/config.h
	src/midi-agent.h
	src/device-manager.h
	src/obs-controller.h
	src/forms/settings-dialog.h
	src/obs-midi.h
	src/events.h
	src/rpc/RpcEvent.h
	src/forms/Macros.h
	src/macro-helpers.h
	src/Midi_hook.h
	src/Midi_message.h)

qt5_wrap_ui(obs_midi_UI_HEADERS src/forms/settings-dialog.ui)

add_library(obs-midi MODULE
	${obs-midi_SOURCES}
	${obs_midi_UI_HEADERS}
	${obs-midi_HEADERS}
	src/forms/settings-dialog.ui)
include_directories(
	"${LIBOBS_INCLUDE_DIR}/../UI/obs-frontend-api"
	${obs_midi_UI_HEADERS}
	${Qt5Core_INCLUDES}
	${Qt5Widgets_INCLUDES}
	"${CMAKE_SOURCE_DIR}/deps/asio/asio/include")
if(LINUX)
	include_directories(/usr/include/obs)
	target_link_libraries(obs-midi
		obs-frontend-api
		${obs-midi_DEPS}
		Qt5::Core
		Qt5::Widgets
		libobs.so)
elseif(NOT DEFINED OBS_FRONTEND_LIB)
	target_link_libraries(obs-midi
		obs-frontend-api
		${obs-midi_DEPS}
		Qt5::Core
		Qt5::Widgets
		libobs)
else()
target_link_libraries(obs-midi
		"${OBS_FRONTEND_LIB}"
		${obs-midi_DEPS}
		Qt5::Core
		Qt5::Widgets
		libobs)
endif()
# --- Windows-specific build settings and tasks ---
if(WIN32)

    if(MSVC)
        # Enable Multicore Builds and disable FH4 (to not depend on VCRUNTIME140_1.DLL)
	    add_definitions(/MP /d2FH4-)
    endif()
	add_definitions(-D_WEBSOCKETPP_CPP11_STL_)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(ARCH_NAME "64bit")
		set(OBS_BUILDDIR_ARCH "build64")
	else()
		set(ARCH_NAME "32bit")
		set(OBS_BUILDDIR_ARCH "build32")
	endif()
	include_directories(
		"${LIBOBS_INCLUDE_DIR}/../${OBS_BUILDDIR_ARCH}/UI"
	)
	if(NOT DEFINED OBS_FRONTEND_LIB)
		target_link_libraries(obs-midi
		obs-frontend-api)
	else()
		target_link_libraries(obs-midi
		"${OBS_FRONTEND_LIB}")
	endif()

	# --- Release package helper ---
	# The "release" folder has a structure similar OBS' one on Windows
	set(RELEASE_DIR "${PROJECT_SOURCE_DIR}/release")
	add_custom_command(TARGET obs-midi POST_BUILD
		# If config is Release, package release files
		COMMAND if $<CONFIG:Release>==1 (
			"${CMAKE_COMMAND}" -E make_directory
			"${RELEASE_DIR}/data/obs-plugins/obs-midi"
			"${RELEASE_DIR}/obs-plugins/${ARCH_NAME}")
		COMMAND if $<CONFIG:Release>==1 ("${CMAKE_COMMAND}" -E copy_directory
			"${PROJECT_SOURCE_DIR}/data"
			"${RELEASE_DIR}/data/obs-plugins/obs-midi")
		COMMAND if $<CONFIG:Release>==1 ("${CMAKE_COMMAND}" -E copy
			"$<TARGET_FILE:obs-midi>"
			"${RELEASE_DIR}/obs-plugins/${ARCH_NAME}")
		# If config is RelWithDebInfo, package release files
		COMMAND if $<CONFIG:RelWithDebInfo>==1 (
			"${CMAKE_COMMAND}" -E make_directory
			"${RELEASE_DIR}/data/obs-plugins/obs-midi"
			"${RELEASE_DIR}/obs-plugins/${ARCH_NAME}")
		COMMAND if $<CONFIG:RelWithDebInfo>==1 ("${CMAKE_COMMAND}" -E copy_directory
			"${PROJECT_SOURCE_DIR}/data"
			"${RELEASE_DIR}/data/obs-plugins/obs-midi")
		COMMAND if $<CONFIG:RelWithDebInfo>==1 ("${CMAKE_COMMAND}" -E copy
			"$<TARGET_FILE:obs-midi>"
			"${RELEASE_DIR}/obs-plugins/${ARCH_NAME}")
		COMMAND if $<CONFIG:RelWithDebInfo>==1 ("${CMAKE_COMMAND}" -E copy
			"$<TARGET_PDB_FILE:obs-midi>"
			"${RELEASE_DIR}/obs-plugins/${ARCH_NAME}")
		# Copy to obs-studio dev environment for immediate testing
		COMMAND if $<CONFIG:Debug>==1 (
			"${CMAKE_COMMAND}" -E copy
				"$<TARGET_FILE:obs-midi>"
				"${LibObs_DIR}/../rundir/$<CONFIG>/obs-plugins/${ARCH_NAME}")
		COMMAND if $<CONFIG:Debug>==1 (
			"${CMAKE_COMMAND}" -E copy
				"$<TARGET_PDB_FILE:obs-midi>"
				"${LibObs_DIR}/../rundir/$<CONFIG>/obs-plugins/${ARCH_NAME}")
		COMMAND if $<CONFIG:Debug>==1 (
			"${CMAKE_COMMAND}" -E copy_directory
				"${PROJECT_SOURCE_DIR}/data"
				"${LibObs_DIR}/../rundir/$<CONFIG>/data/obs-plugins/${ARCH_NAME}")
	)
	# --- End of sub-section ---
endif()
# --- End of section ---
# --- Linux-specific build settings and tasks ---
if(UNIX AND NOT APPLE)
	include(GNUInstallDirs)
	set_target_properties(obs-midi PROPERTIES PREFIX "")
	target_link_libraries(obs-midi obs-frontend-api)

	file(GLOB locale_files data/locale/*.ini)
        set(CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS
		OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	find_program(LSB_RELEASE_EXEC lsb_release)
	execute_process(COMMAND ${LSB_RELEASE_EXEC} -is
	OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT
	OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	if(COMMAND install_obs_plugin_with_data)
	message(STATUS "in tree mode")
	elseif(${LSB_RELEASE_ID_SHORT} STREQUAL "Ubuntu")
		install(TARGETS obs-midi LIBRARY
			DESTINATION "/usr/lib/obs-plugins"
			PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
	endif()
	install(TARGETS obs-midi LIBRARY
		DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/obs-plugins"
		PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
	install(FILES ${locale_files}
		DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/obs/obs-plugins/obs-midi/locale")
endif()
# --- End of section ---
# -- OS X specific build settings and tasks --
if(APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -fvisibility=default")
	set(CMAKE_SKIP_RPATH TRUE)
	set_target_properties(obs-midi PROPERTIES PREFIX "")
	if(NOT DEFINED OBS_FRONTEND_LIB)
		target_link_libraries(obs-midi
		obs-frontend-api)
	else()
		target_link_libraries(obs-midi
		"${OBS_FRONTEND_LIB}")
	endif()
endif()
# -- End of section --
if (COMMAND install_obs_plugin_with_data)
install_obs_plugin_with_data(obs-midi data)
endif()
set_target_properties(obs-midi PROPERTIES FOLDER "plugins/obs-midi")
#set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/external" ${CMAKE_MODULE_PATH})

