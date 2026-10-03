/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#define FORBIDDEN_SYMBOL_ALLOW_ALL

#include "common/scummsys.h"
#include "common/config-manager.h"

#include "backends/platform/sdl/sailfish/sailfish.h"
#include "backends/platform/sdl/sailfish/sailfish-window.h"

#include "backends/fs/posix/posix-fs-factory.h"
#include "backends/fs/posix/posix-fs.h"
#include "backends/saves/default/default-saves.h"
#ifdef AURORA_OS
#include "backends/events/sdl/sdl-events.h"

class AuroraEventSource : public SdlEventSource {
protected:
	void preprocessEvents(SDL_Event *event) override {
		if (event->type == SDL_WINDOWEVENT && _graphicsManager) {
			SDL_Window *window = _graphicsManager->getWindow()->getSDLWindow();
			if (window && event->window.windowID != SDL_GetWindowID(window))
				return;
		}
		if (event->type == SDL_WINDOWEVENT) {
			switch (event->window.event) {
			case SDL_WINDOWEVENT_FOCUS_LOST:
			case SDL_WINDOWEVENT_MINIMIZED:
			case SDL_WINDOWEVENT_HIDDEN:
				SDL_PauseAudio(1);
				SDL_Log("Aurora: audio paused (window event %u)", event->window.event);
				break;
			case SDL_WINDOWEVENT_CLOSE:
				// A compositor close must quit even when in-game quit normally returns
				// to the launcher or asks for confirmation in an invisible window.
				SDL_PauseAudio(1);
				SDL_Log("Aurora: compositor close requested");
				ConfMan.setBool("confirm_exit", false, Common::ConfigManager::kTransientDomain);
				ConfMan.setBool("gui_return_to_launcher_at_exit", false, Common::ConfigManager::kTransientDomain);
				break;
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				SDL_PauseAudio(0);
				SDL_Log("Aurora: audio resumed");
				break;
			default:
				break;
			}
		}
		if (event->type == SDL_QUIT) {
			SDL_PauseAudio(1);
			ConfMan.setBool("confirm_exit", false, Common::ConfigManager::kTransientDomain);
			ConfMan.setBool("gui_return_to_launcher_at_exit", false, Common::ConfigManager::kTransientDomain);
		}
		if (_graphicsManager &&
		    ((event->type == SDL_DISPLAYEVENT && event->display.event == SDL_DISPLAYEVENT_ORIENTATION) ||
		     (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_DISPLAY_CHANGED))) {
			_graphicsManager->setRotationMode(SdlWindow_Sailfish::displayRotation(_graphicsManager->getWindow()->getDisplayIndex()));
			_graphicsManager->notifyVideoExpose();
		}
	}
};
#endif

#include "backends/keymapper/action.h"
#include "backends/keymapper/keymapper-defaults.h"
#include "backends/keymapper/hardware-input.h"
#include "backends/keymapper/keymap.h"
#include "backends/keymapper/keymapper.h"

#ifdef AURORA_OS
#define ORG_NAME        "ru.erhoof"
#else
#define ORG_NAME        "org.scummvm"
#endif
#define APP_NAME        "scummvm"

void OSystem_SDL_Sailfish::init() {
	setenv("SDL_VIDEO_WAYLAND_WMCLASS", ORG_NAME "." APP_NAME, 1);
#ifdef AURORA_OS
	setenv("PULSE_PROP_media.role", "x-maemo", 1);
	setenv("SDL_VIDEODRIVER", "wayland", 1);
	SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
#endif
	// Initialze File System Factory
	_fsFactory = new POSIXFilesystemFactory();

	_window = new SdlWindow_Sailfish();

	_isAuroraOS = false;
	char *line = nullptr;
	size_t n = 0;
	FILE *os_release = fopen("/etc/os-release", "r");
	if (os_release) {
		while (getline(&line, &n, os_release) > 0) {
			if (strncmp(line, "ID=auroraos", sizeof("ID=auroraos") - 1) == 0) {
				_isAuroraOS = true;
			}
		}
		free(line);
		fclose(os_release);
	}

	// Invoke parent implementation of this method
	OSystem_SDL::init();
}

Common::String OSystem_SDL_Sailfish::getAppSuffix() {
#ifdef AURORA_OS
	return ORG_NAME "/" APP_NAME;
#else
	if (_isAuroraOS) {
		return ORG_NAME "/" APP_NAME;
	} else {
		return ORG_NAME "." APP_NAME;
	}
#endif
}

void OSystem_SDL_Sailfish::initBackend() {
#ifdef AURORA_OS
	_eventSource = new AuroraEventSource();
	ConfMan.setInt("rotation_mode", SdlWindow_Sailfish::displayRotation(0));
	if (!ConfMan.hasKey("gfx_mode"))
		ConfMan.set("gfx_mode", "opengl");
	ConfMan.setBool("opengl_discrete_window_resolutions", false);
	if (!ConfMan.hasKey("touchpad_mouse_mode"))
		ConfMan.setBool("touchpad_mouse_mode", false);
	const char *home = getenv("HOME");
	if (home) {
		const char *keys[] = {"screenshotpath", "dlcspath", "iconspath"};
		const char *dirs[] = {"screenshots", "dlc", "icons"};
		for (unsigned i = 0; i < ARRAYSIZE(keys); ++i) {
			Common::String relative = ".local/share/" + getAppSuffix() + "/" + dirs[i];
			if (!ConfMan.hasKey(keys[i]) && Posix::assureDirectoryExists(relative, home)) {
				Common::Path path(home);
				path.joinInPlace(relative);
				ConfMan.setPath(keys[i], path);
			}
		}
	}
#else
	if (!ConfMan.hasKey("rotation_mode")) {
		ConfMan.setInt("rotation_mode", 90);
	}
#endif

	if (!ConfMan.hasKey("savepath")) {
		ConfMan.setPath("savepath", getDefaultSavePath());
	}

	// Create the savefile manager
	if (_savefileManager == nullptr) {
		_savefileManager = new DefaultSaveFileManager(getDefaultSavePath());
	}

	OSystem_SDL::initBackend();
}

Common::Path OSystem_SDL_Sailfish::getDefaultSavePath() {
	Common::String saveRelPath;

	const char *prefix = getenv("HOME");
	if (prefix == nullptr) {
		return Common::Path();
	}

	saveRelPath = ".local/share/" + getAppSuffix() + "/saves";

	if (!Posix::assureDirectoryExists(saveRelPath, prefix)) {
		return Common::Path();
	}

	Common::Path savePath(prefix);
	savePath.joinInPlace(saveRelPath);

	return savePath;
}

Common::Path OSystem_SDL_Sailfish::getDefaultConfigFileName() {
	Common::String configPath;

	const char *prefix = getenv("HOME");
	if (prefix == nullptr) {
		return Common::Path();
	}

	configPath = ".local/share/" + getAppSuffix();

	if (!Posix::assureDirectoryExists(configPath, prefix)) {
		return Common::Path();
	}

	Common::Path configFile(prefix);
	configFile.joinInPlace(configPath);
	configFile.joinInPlace("scummvm.ini");

	return configFile;
}

Common::Path OSystem_SDL_Sailfish::getDefaultLogFileName() {
	Common::String logFile;

	const char *prefix = getenv("HOME");
	if (prefix == nullptr) {
		return Common::Path();
	}

	logFile = ".cache/" + getAppSuffix() + "/logs";

	if (!Posix::assureDirectoryExists(logFile, prefix)) {
		return Common::Path();
	}

	Common::Path logPath(prefix);
	logPath.joinInPlace(logFile);
	logPath.joinInPlace("scummvm.log");

	return logPath;
}


bool OSystem_SDL_Sailfish::hasFeature(Feature f) {
	switch (f) {
	case kFeatureFullscreenMode:
		return false;
	case kFeatureTouchpadMode:
		return true;
	default:
		return OSystem_SDL::hasFeature(f);
	}
}
