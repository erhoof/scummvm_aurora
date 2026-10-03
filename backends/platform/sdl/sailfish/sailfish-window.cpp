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

#include "backends/graphics/sdl/sdl-graphics.h"
#include "backends/platform/sdl/sailfish/sailfish-window.h"
#ifdef AURORA_OS
#include "common/config-manager.h"
#include <wayland-client.h>

Common::RotationMode SdlWindow_Sailfish::displayRotation(int displayIndex) {
	SDL_DisplayMode mode;
	if (displayIndex < 0 || SDL_GetCurrentDisplayMode(displayIndex, &mode) != 0)
		return Common::kRotationNormal;
	const bool portraitPanel = mode.h > mode.w;
	// ScummVM's texture rotation uses the opposite landscape direction from
	// the Aurora orientation convention on a portrait panel.
	switch (SDL_GetDisplayOrientation(displayIndex)) {
	case SDL_ORIENTATION_LANDSCAPE_FLIPPED:
	case SDL_ORIENTATION_PORTRAIT:
		return portraitPanel ? Common::kRotation90 : Common::kRotation180;
	case SDL_ORIENTATION_LANDSCAPE:
	case SDL_ORIENTATION_PORTRAIT_FLIPPED:
		return portraitPanel ? Common::kRotation270 : Common::kRotationNormal;
	default:
		return portraitPanel ? Common::kRotation270 : Common::kRotationNormal;
	}
}

void SdlWindow_Sailfish::setBufferRotation(Common::RotationMode rotation) {
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	if (!getSDLWMInformation(&info) || info.subsystem != SDL_SYSWM_WAYLAND || !info.info.wl.surface)
		return;
	int transform = WL_OUTPUT_TRANSFORM_NORMAL;
	// Wayland describes the buffer-to-surface transform; ScummVM rotates
	// scene-to-buffer coordinates. Quarter turns therefore need the inverse.
	switch (rotation) {
	case Common::kRotation90: transform = WL_OUTPUT_TRANSFORM_270; break;
	case Common::kRotation180: transform = WL_OUTPUT_TRANSFORM_180; break;
	case Common::kRotation270: transform = WL_OUTPUT_TRANSFORM_90; break;
	default: break;
	}
	if (wl_proxy_get_version(reinterpret_cast<wl_proxy *>(info.info.wl.surface)) >= WL_SURFACE_SET_BUFFER_TRANSFORM_SINCE_VERSION)
		wl_surface_set_buffer_transform(info.info.wl.surface, transform);
	SDL_Log("Aurora: content rotation %d, buffer transform %d", static_cast<int>(rotation), transform);
}
#endif

/* Setting window size at anything other than full screen is unexpected
   and results in a rectangle without any decorations. So always create
   full screen window.
 */
bool SdlWindow_Sailfish::createOrUpdateWindow(int, int, uint32 flags) {
#ifdef AURORA_OS
	SDL_DisplayMode mode;
	const int display = _window ? getDisplayIndex() : 0;
	if (SDL_GetCurrentDisplayMode(display < 0 ? 0 : display, &mode) != 0)
		return false;
	// Keep the EGL buffer in native panel dimensions; ScummVM rotates its textures.
	const bool result = SdlWindow::createOrUpdateWindow(mode.w, mode.h, flags);
	if (result)
		setBufferRotation(static_cast<Common::RotationMode>(ConfMan.getInt("rotation_mode")));
	return result;
#else
	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0,&dm);
	int width, height;

	/* SDL assumes that composer takes care of rotation and so switches
	   sides in landscape rotation. But Lipstick doesn't handle rotation.
	   So put them back in correct order.
	 */
	if (dm.w < dm.h) {
		width = dm.w;
		height = dm.h;
	} else {
		width = dm.h;
		height = dm.w;
	}
	return SdlWindow::createOrUpdateWindow(width, height, flags);
#endif
}
