%define _app_orgname ru.erhoof
%define _app_appname scummvm
%define _app_launcher_name ScummVM
%define __requires_exclude ^(libSDL2.*|libFLAC|libfreetype|libfribidi|libjpeg|libtheoradec|libvpx|libbz2)\\.so.*$
%define __provides_exclude_from ^%{_datadir}/%{name}/lib/.*\.so.*$

Name: %{_app_orgname}.%{_app_appname}
Version: 2026.3.1
Release: 5
Summary: ScummVM adventure game interpreter
License: GPLv3+
Group: Amusements/Games
URL: https://www.scummvm.org
Source0: %{name}-%{version}.tar.gz
BuildRequires: cmake
BuildRequires: ninja
BuildRequires: patchelf
BuildRequires: pkgconfig(sdl2)
BuildRequires: pkgconfig(wayland-client)
BuildRequires: pkgconfig(wayland-cursor)
BuildRequires: pkgconfig(wayland-egl)
BuildRequires: pkgconfig(wayland-protocols)
BuildRequires: pkgconfig(wayland-scanner)
BuildRequires: pkgconfig(egl)
BuildRequires: pkgconfig(glesv2)
BuildRequires: pkgconfig(xkbcommon)
BuildRequires: pkgconfig(libpulse)
BuildRequires: pkgconfig(zlib)
BuildRequires: pkgconfig(libpng)
BuildRequires: pkgconfig(libjpeg)
BuildRequires: pkgconfig(freetype2)
BuildRequires: pkgconfig(flac)
BuildRequires: pkgconfig(ogg)
BuildRequires: pkgconfig(vorbis)
BuildRequires: pkgconfig(vorbisfile)
BuildRequires: pkgconfig(theoradec)
BuildRequires: pkgconfig(fribidi)
BuildRequires: pkgconfig(vpx)
BuildRequires: pkgconfig(libcurl)

%description
ScummVM with the OpenGL ES frontend, Aurora rotation and lifecycle support,
and all engines available with this graphics backend, including Gamos for
Pilot Brothers. Game data must be supplied separately by the user.

%prep
%setup -q

%build
export CFLAGS="%{optflags} -O2 -DWL_EGL_PLATFORM"
export CXXFLAGS="%{optflags} -O2 -DAURORA_OS -DWL_EGL_PLATFORM"
export SCUMMVM_AURORA=1
cmake -G Ninja -DCMAKE_MAKE_PROGRAM=/usr/bin/ninja -S libsdl -B build/%{_arch}/libsdl \
  -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$PWD/build/%{_arch}/sdl-install" \
  -DSDL_PULSEAUDIO=ON -DSDL_AUDIO=ON -DSDL_RPATH=OFF -DSDL_STATIC=OFF \
  -DSDL_WAYLAND=ON -DSDL_X11=OFF -DSDL_WAYLAND_LIBDECOR=OFF -DSDL_WAYLAND_QT_TOUCH=ON
cmake --build build/%{_arch}/libsdl -j8
cmake --install build/%{_arch}/libsdl
mkdir -p build/%{_arch}/mad
if [ ! -f build/%{_arch}/mad-install/lib/libmad.a ] || ! grep -q -- '-O2' build/%{_arch}/mad/Makefile; then
tar xf aurora/deps/libmad-0.15.1b.tar.gz -C build/%{_arch}/mad --strip-components=1
# The original Autotools helpers predate aarch64. Use SDL's current helpers.
cp libsdl/build-scripts/config.guess libsdl/build-scripts/config.sub build/%{_arch}/mad/
pushd build/%{_arch}/mad
sed -i 's/-fforce-mem//g' configure
CFLAGS="%{optflags} -O2" ./configure --prefix="$PWD/../mad-install" --disable-shared --enable-static --disable-aso
make clean
make -j8
make install
popd
fi
if [ ! -f build/%{_arch}/mpeg2-install/lib/libmpeg2.a ]; then
mkdir -p build/%{_arch}/mpeg2
tar xf aurora/deps/libmpeg2-0.5.1.tar.gz -C build/%{_arch}/mpeg2 --strip-components=1
cp libsdl/build-scripts/config.guess libsdl/build-scripts/config.sub build/%{_arch}/mpeg2/.auto/
pushd build/%{_arch}/mpeg2
./configure --prefix="$PWD/../mpeg2-install" --disable-shared --enable-static --disable-sdl
make -j8
make install
popd
fi
if [ ! -f build/%{_arch}/gif-install/lib/libgif.a ]; then
mkdir -p build/%{_arch}/giflib
tar xf aurora/deps/giflib-5.2.2.tar.gz -C build/%{_arch}/giflib --strip-components=1
make -C build/%{_arch}/giflib -j8 libgif.a
install -D -m0644 build/%{_arch}/giflib/libgif.a build/%{_arch}/gif-install/lib/libgif.a
install -D -m0644 build/%{_arch}/giflib/gif_lib.h build/%{_arch}/gif-install/include/gif_lib.h
fi
export SDL_CONFIG="$PWD/build/%{_arch}/sdl-install/bin/sdl2-config"
export LDFLAGS="-Wl,-rpath,%{_datadir}/%{name}/lib -Wl,-z,noexecstack -lwayland-client"
mkdir -p build/%{_arch}/scummvm
pushd build/%{_arch}/scummvm
set -- --host=sailfish --prefix=%{_prefix} \
  --datarootdir=%{_datadir}/%{name} --datadir=%{_datadir}/%{name}/data \
  --docdir=%{_datadir}/%{name}/doc --bindir=%{_bindir} \
  --with-sdl-prefix=../sdl-install --with-mad-prefix=../mad-install \
  --with-mpeg2-prefix=../mpeg2-install --with-gif-prefix=../gif-install \
  --enable-all-engines --opengl-mode=gles2 --enable-tinygl --enable-release --disable-debug
# Reconfigure when build inputs change. Repackaging bundled libraries should
# reuse the engine objects rather than rerun every configure probe.
if [ ! -f config.mk ] || [ ../../../configure -nt configure.stamp ] || \
   [ "$(sed -n 's/^SAVED_CONFIGFLAGS[[:space:]]*:= //p' config.mk)" != "$*" ] || \
   [ "$(sed -n 's/^SAVED_CXXFLAGS[[:space:]]*:= //p' config.mk)" != "$CXXFLAGS" ] || \
   [ "$(sed -n 's/^SAVED_LDFLAGS[[:space:]]*:= //p' config.mk)" != "$LDFLAGS" ] || \
   [ "$(sed -n 's/^SAVED_SDL_CONFIG[[:space:]]*:= //p' config.mk)" != "$SDL_CONFIG" ]; then
  ../../../configure "$@"
fi
make -j8
popd

%install
make -C build/%{_arch}/scummvm DESTDIR=%{buildroot} install-data
install -D -m0755 build/%{_arch}/scummvm/%{name} %{buildroot}%{_bindir}/%{name}
patchelf --force-rpath --set-rpath %{_datadir}/%{name}/lib %{buildroot}%{_bindir}/%{name}
# Private libraries: exclude their Requires/Provides above and resolve them
# through the application RPATH. Copy only each runtime SONAME.
install -D -m0755 build/%{_arch}/sdl-install/lib/libSDL2-2.0.so.0 -t %{buildroot}%{_datadir}/%{name}/lib/
for lib in libFLAC.so.12 libfreetype.so.6 libfribidi.so.0 libjpeg.so.62 libtheoradec.so.1 libvpx.so.9 libbz2.so.1; do
  install -D -m0755 %{_libdir}/$lib -t %{buildroot}%{_datadir}/%{name}/lib/
  patchelf --force-rpath --set-rpath %{_datadir}/%{name}/lib %{buildroot}%{_datadir}/%{name}/lib/$lib
done
for size in 86 108 128 172; do
  install -D -m0644 aurora/icon_${size}.png %{buildroot}%{_datadir}/icons/hicolor/${size}x${size}/apps/%{name}.png
done
install -D -m0644 aurora/%{name}.desktop %{buildroot}%{_datadir}/applications/%{name}.desktop

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png

%changelog
* Sun Oct 04 2026 erhoof <erhoof@localhost> - 2026.3.1-5
- Update launcher icons and dependency architecture detection for aarch64.
* Sun Oct 04 2026 erhoof <erhoof@localhost> - 2026.3.1-4
- Install user-supplied ScummVM launcher icons in all Aurora sizes.
* Sun Oct 04 2026 erhoof <erhoof@localhost> - 2026.3.1-3
- Use inverse quarter turns for the Wayland buffer rotation hint.
* Sun Oct 04 2026 erhoof <erhoof@localhost> - 2026.3.1-2
- Correct portrait-panel landscape rotation following tablet verification.
* Sat Oct 03 2026 erhoof <erhoof@localhost> - 2026.3.1-1
- Aurora OpenGL ES port with rotation, lifecycle handling and available engines.
