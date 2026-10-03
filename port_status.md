# ScummVM Aurora port

Mode: auto. Targets: AuroraOS-5.2.1.200 armv7hl and aarch64. App: ru.erhoof.scummvm.

## Research

See [render pipeline](research/render_pipeline.md). Existing GLES texture rotation and input transforms can be reused. Native panel size must replace portrait-only assumptions. Enable SDL Qt extended surface integration for compositor close events; pause audio on focus loss. Keep writable paths in application sandbox.

## Checklist

- [x] Inspect platform, graphics, input and packaging.
- [x] Aurora platform changes and RPM/build script; see work/stage_01_platform.md.
- [x] Build 125 available engines, including Gamos; Watchmaker requires desktop OpenGL.
- [x] SDK signature verified; regular rpm-validator and sfdk check return success.
- [x] Release 1 installed; both games copied; device detects gamos:pilots1.
- [ ] Device checks: OpenGL, rotation/touch, background audio, compositor close.

## Device

Tablet reachable. Aurora 5.2.0.180, armv7hl userspace; glibc 2.38 matches SDK.
Both editions copied to Documents. Release 1 launched successfully; release 2 image rotation confirmed correct.
Release 3 corrects the inverse Wayland hint; awaiting tablet confirmation.
Generic rpmlint reports private-library layout advisories; Aurora regular validator
returns success with only the non-Silica desktop warning.

## Game compatibility

The copied GOG Pilot Brothers edition is explicitly unsupported by ScummVM's
Gamos detection table (different engine). User supplied original ISO data;
`PILOTS.EXE` is 48357155 bytes and its tail checksum exactly matches the supported
Russian release. Copied to `~/Documents/PILOTS`; use that directory for gameplay.

## Open device findings

[ROT-002](port_errors.md): image correct, inverse compositor hint fixed in release 3; awaiting confirmation.

## Release 5 icons / architecture builds

Revised icons from Untitled (56).zip installed in all four sizes. Previous
release 4 armv7hl signed, validated and deployed. Release 5 aarch64 building
with refreshed Autotools architecture helpers; armv7hl rebuild/deploy follows.
