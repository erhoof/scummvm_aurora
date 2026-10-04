# ScummVM Aurora port

Mode: auto. Targets: AuroraOS-5.2.1.200 armv7hl and aarch64. App: ru.erhoof.scummvm.

## Research

See [render pipeline](research/render_pipeline.md). Existing GLES texture rotation and input transforms can be reused. Native panel size must replace portrait-only assumptions. Enable SDL Qt extended surface integration for compositor close events; pause audio on focus loss. Keep writable paths in application sandbox.

## Checklist

- [x] Inspect platform, graphics, input and packaging.
- [x] Aurora platform changes and RPM/build script; see work/stage_01_platform.md.
- [x] Build 125 available engines, including Gamos; Watchmaker requires desktop OpenGL.
- [x] Release 5 signatures verified; regular validation and sfdk check succeed for both architectures.
- [x] Release 5 armv7hl installed; both games copied; device detects gamos:pilots1.
- [ ] Device checks: OpenGL, rotation/touch, background audio, compositor close.

## Device

Tablet reachable. Aurora 5.2.0.180, armv7hl userspace; glibc 2.38 matches SDK.
Both editions copied to Documents. Release 1 launched successfully; release 2 image rotation confirmed correct.
Release 3 corrected the inverse Wayland hint. User subsequently reported everything looks good.
Generic rpmlint reports private-library layout advisories; Aurora regular validator
returns success with only the non-Silica desktop warning.

## Game compatibility

The copied GOG Pilot Brothers edition is explicitly unsupported by ScummVM's
Gamos detection table (different engine). User supplied original ISO data;
`PILOTS.EXE` is 48357155 bytes and its tail checksum exactly matches the supported
Russian release. Copied to `~/Documents/PILOTS`; use that directory for gameplay.

## Open device findings

No remaining reported rotation faults; user reports everything looks good.
Release 6 menu gesture and keyboard scaling await device feedback.

## Release 5 icons / architecture builds

Revised icons from Untitled (56).zip installed in all four sizes. Previous
release 4 armv7hl signed, validated and deployed. Release 5 aarch64 signed and validated, preserved in artifacts/.
Release 5 armv7hl signed, validated and installed successfully. Both RPMs
are preserved in artifacts/. Virtual keyboard three-finger gesture confirmed
by user; main menu uses Ctrl+F5. User reports the port looks good overall; audio/card-close behavior has not
been separately recorded as confirmed.

## Release 6 touch improvements

Four-finger menu gesture and enlarged virtual keyboard implemented. Existing
three-finger keyboard gesture preserved. Both release 6 RPMs built, signed and
validated, preserved in artifacts/. armv7hl installed successfully on tablet.
User confirms armv7hl release 6 works.

## aarch64 device deployment

Release 6 installed successfully on defaultuser@192.168.1.133 (aarch64, 64-bit
userspace, glibc 2.38). Original Pilot Brothers copied to ~/Documents/PILOTS
and added with --add --game=gamos:pilots1. --list-targets confirms pilots1-win-ru.
CLI runtime verified on aarch64; graphics/gameplay await device use.
