# ScummVM Aurora research

- Source version: 2026.3.1git. Keep this checkout; Gamos supports original Pilot Brothers releases. The supplied GOG remake is explicitly ADGF_UNSUPPORTED in engines/gamos/detection_tables.h because it uses a different engine.
- Configure/Make builds the existing Sailfish SDL backend. Enable all engines, including experimental engines; report engines excluded by graphics/dependency requirements.
- OpenGLSDL uses GLES shaders and SDL_GL_SwapWindow. 2D games, launcher, overlays, cursor and OSD already render textured quads with rotation. 3D renderers already provide intermediate textures where needed.
- Reuse native ScummVM rotation rather than add a second rotation pass. WindowedGraphicsManager rotates absolute mouse/touch coordinates and cursor warps consistently with the textures.
- SDL supplies native panel dimensions and orientation events. Current Sailfish code incorrectly sorts every display to portrait and fixes rotation at 90 degrees.
- Aurora SDL fork savegame/SDL has qt-extended-surface close support under SDL_WAYLAND_QT_TOUCH. The upstream Sailfish RPM explicitly disables this option, preventing app-switcher close delivery.
- SDL focus loss needs audio pause; SDL close must immediately silence audio and deliver EVENT_QUIT. Default SIGTERM handling already terminates the process.
- Aurora needs the buffer transform hint as well as client content rotation. Map landscape and inverted landscape using the native panel shape.
- Existing OpenGL state/texture caches remain owned by ScummVM. Rotation changes must refresh display rectangles, cursor and texture settings through graphics manager APIs.
- Writes: config, saves, logs, screenshots, DLC and icon caches must use ru.erhoof/scummvm directories under the user's .local/share or .cache. Game data is external, copied to Documents separately.
- Bundle SDL and any system libraries disallowed by the regular RPM validator under /usr/share/ru.erhoof.scummvm/lib with RPATH. Use SDK tools only for target operations.
