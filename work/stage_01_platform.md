# Platform integration and first build

- Inspected Aurora port guide and jangrybirds2 build/sign example.
- Created AURORA_OS platform branch; existing Sailfish build remains available.
- Added ru.erhoof.scummvm identity, desktop Name=ScummVM, sandbox config,
  save/log/screenshot/DLC/icon paths, PulseAudio media role, GLES3 context with
  ScummVM's existing GLES shader frontend.
- Reused engine texture, overlay, cursor and input rotation. Added native panel
  sizing, SDL orientation event handling and Wayland buffer transform hint.
- Enabled SDL_WAYLAND_QT_TOUCH, which supplies qt-extended-surface close events.
- Added focus-loss audio pause and focus-gain resume. Compositor close suppresses
  quit confirmation and return-to-launcher for that exit request.
- Added spec and build_sign.sh. Signing failures and validation failures are fatal.
- SDK uses rpm _arch=arm for armv7hl; build artifacts currently use build/arm.
- Initial configure option --enable-opengl is not accepted by this ScummVM
  version; changed to --opengl-mode=gles2 and added optimized release build.
- System giflib 4.2 is too old; libmpeg2 is absent. Added static giflib 5.2.2 and
  libmpeg2 0.5.1 so HPL1, Phoenix VR, Alcachofa and QD Engine are included.
- Final configure currently excludes only The Watchmaker, requiring desktop
  classic OpenGL. The GLES frontend cannot provide that API.
- Tablet: Aurora 5.2.0.180; ARM 32-bit userspace, aarch64 kernel. Pilot Brothers
  copied by scp to /home/defaultuser/Documents/Pilot Brothers (451 MiB).
- Git checkpoint could not be committed: repository has no configured author
  identity. Continue work without changing user's Git identity.
- Device screenshots require root according to user; ask user for visual checks.

## Further review

- Found negative signs in upstream convertVirtualToWindow mirror terms, causing
  rotated cursor warps to collapse to viewport edges. Corrected under AURORA_OS.
- Tested actual extracted coordinate methods: 3,686,400 exact round trips across
  all four rotations and scales 1/2/3, with nonzero viewport offsets. Original
  code fails at 90 degrees for the first corner. Reproduce: python3 work/check_rotation.py.
- Added OSD rotation updates and lifecycle/rotation log messages.
- Inspected supplied GOG executable: 474624 bytes; detection table explicitly
  marks this edition unsupported (different engine). Initial assumption of Gamos
  support for this edition was incorrect; informed user and requested original data.
- User supplied unpacked original ISO under Downloads/Bratia_Piloty_Po_Sledam_Polosatogo_Slona_ISO/PILOTS.
  PILOTS.EXE has 48357155 bytes and tail-5000 MD5 82ae05090898af66447bac4f06e910f3,
  matching the supported Russian original. Copied by scp to tablet Documents/PILOTS.

## Build and packaging checkpoint

- Research checkpoint committed as a911bbf4 with command-local author identity;
  user Git configuration remains unchanged.
- Optimized armv7hl build completed with 125 main engines including Gamos.
- Matched jangrybirds2 private-library Provides exclusion escaping. Bundled FLAC,
  FreeType, FriBidi, JPEG, Theora, VPX and FreeType's bzip2 dependency alongside SDL;
  each has a private RPATH and corresponding Requires exclusion.
- SDK regular signature verifies. sfdk check and explicit rpm-validator -p regular
  return 0; non-Silica desktop warning remains. Generic rpmlint reports private
  library layout advisories; SDK treats these as warnings.
- Tablet reconnected; glibc 2.38 matches the SDK, original PILOTS data confirmed.

## First device feedback and release 2

- Release 1 installed and launched successfully according to the user.
- Device command detects original Russian Pilot Brothers as gamos:pilots1;
  installed engine list includes Gamos and QD Engine.
- User reports landscape upside down: swap 90°/270° on portrait panels.
  Updated displayRotation mapping, including inverted direction and fallback.
  The shared mode drives both texture/input rotation and buffer transform.
- Coordinate invariant check still passes all 3,686,400 cases.
- Release 2 built, signed and sfdk check returned success.

## Release 3 — compositor hint convention

- Release 2 installer reported success. Host rpm database does not list sandbox
  packages, so rpm -q is unsuitable for installation verification here. User
  confirms new image direction is correct but hint is now upside down.
- Adjust only setBufferRotation: ScummVM 270° maps to Wayland transform 90°,
  ScummVM 90° maps to transform 270°. Image/touch mappings unchanged.
- Renderer and compositor hint quarter turns must be inverse with ScummVM's
  existing renderer; the guide's equal-angle rule assumes its own FBO convention.

## Launcher icons and both architectures

- Supplied Untitled (55).zip contains exact 86/108/128/172 square PNG sizes.
  Copied unchanged into aurora/icon_SIZE.png; spec installs them as app icons.
- Release 4 armv7hl built, signed, validated and installed successfully on tablet.
- User requested both architectures; aarch64 build/sign/validation started.
  Existing tablet is 32-bit userspace, so only armv7hl is installed there.

## Revised icons / aarch64 dependency detection

- User replaced icon archive with Untitled (56).zip. All four original-size PNGs
  copied unchanged, replacing release 4 icons. Release 5 packages these icons.
- First aarch64 attempt failed in libmad's old config.guess (aarch64 unknown).
  Copy current config.guess/config.sub from pristine SDL into extracted libmad
  and libmpeg2 build trees; upstream dependency tarballs and libsdl unchanged.
- Restarted aarch64 build with corrected detection helpers.

## aarch64 compile and packaging

- All 125 enabled engines compiled and executable linked successfully.
- First install attempt assumed SDL prefix/lib; aarch64 CMake uses prefix/lib64.
  Use RPM %{_lib} for the SDL runtime source path, preserving private package
  destination /usr/share/ru.erhoof.scummvm/lib on both architectures.
- User confirms three-finger tap opens keyboard. Main menu is Ctrl+F5 via
  keyboard; no dedicated menu touch gesture exists in current SDL backend.

## Signed aarch64 RPM / final armv7hl rebuild

- Release 5 aarch64 build completed. SDK signature verified; sfdk check selects
  regular profile and returns success. Same generic rpmlint private-library
  advisories as armv7hl, no Aurora dependency validation errors.
- SDK clears top-level RPMS artifacts when switching targets. Build script now
  preserves validated packages in artifacts/ (OUTPUT_DIR override), before deploy.
- Saved signed aarch64 RPM and started release 5 armv7hl build/sign/check/deploy.

## Release 5 completion

- Both architecture RPMs built, signed, signature verified and validated with
  regular profile. sfdk check returns 0 for each; generic rpmlint advisories
  about private-library placement remain treated as warnings by SDK policy.
- Preserved both signed packages in artifacts/. armv7hl deployed to
  defaultuser@192.168.1.247; sdk-deploy-rpm reports Installation successful.
- Final revised icons from Untitled (56).zip included in both packages.
- aarch64 device execution not tested: supplied tablet has armv7hl userspace.
