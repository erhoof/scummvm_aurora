# Aurora ScummVM

Build, sign and validate either architecture:

```sh
./build_sign.sh armv7hl --sign
./build_sign.sh aarch64 --sign
```

Add `--deploy` to install on defaultuser@192.168.1.247. Defaults are in
`port_config.json`; build_sign.sh accepts SDK_ROOT, SFDK, TARGET, DEVICE_HOST,
DEVICE_USER, DEVICE_KEY, SIGNING_KEY, SIGNING_CERT and OUTPUT_DIR overrides.
Validated RPMs are preserved in `artifacts/` so building the second architecture
does not remove the first package.

The launcher name is ScummVM, app ID ru.erhoof.scummvm. Runtime config and
saves are under `~/.local/share/ru.erhoof/scummvm`; logs are under
`~/.cache/ru.erhoof/scummvm/logs`. The default frontend is OpenGL ES, with a
GLES3 context and ScummVM's GLES shaders. SDL_WAYLAND_QT_TOUCH is enabled for
Qt extended surface close notifications. Audio pauses on focus loss and
resumes on focus gain. Compositor close bypasses quit confirmation and
return-to-launcher preferences for that request.

All engines available with the GLES backend are enabled, including experimental
engines. The Watchmaker requires classic desktop OpenGL and is excluded by
configure. Experimental engines may have upstream limitations.

For the supplied original Russian Pilot Brothers ISO, select `~/Documents/PILOTS`
in Add Game. Its PILOTS.EXE checksum matches the Gamos detection entry. The GOG
remake copied to `~/Documents/Pilot Brothers` is marked unsupported by ScummVM
because it uses a different engine.

Rotation preserves native panel buffer dimensions, uses SDL orientation events,
applies ScummVM texture/input rotation and updates the Wayland buffer
transform hint with the inverse quarter turn. Absolute touch and mouse warp coordinates share that mapping.

Reproduce the coordinate invariant check:

```sh
python3 work/check_rotation.py
```

Device checks: launcher and game visible in both landscape directions; taps align
with controls; audio stops when minimized and resumes after restoring; closing
the app card removes the process and permits a fresh launch. Device visual
checks are performed by the user; screenshots require root on this tablet.

Touch shortcuts: a quick three-finger tap opens ScummVM's bundled virtual
keyboard (confirmed on tablet). To open the main menu, use Ctrl+F5 on that
keyboard. This SDL backend has no dedicated touch gesture for the menu.
