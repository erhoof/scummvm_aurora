# Source archives

These pristine source archives are built statically into ScummVM. They enable
engines whose required libraries are missing or too old in the Aurora SDK.

| Archive | Source | SHA-256 |
| --- | --- | --- |
| libmad-0.15.1b.tar.gz | User-provided `/home/erhoof/Downloads/libmad-0.15.1b.tar.gz`; upstream MAD 0.15.1b | bbfac3ed6bfbc2823d3775ebb931087371e142bb0e9bb1bee51a76a6e0078690 |
| libmpeg2-0.5.1.tar.gz | https://deb.debian.org/debian/pool/main/m/mpeg2dec/mpeg2dec_0.5.1.orig.tar.gz | dee22e893cb5fc2b2b6ebd60b88478ab8556cb3b93f9a0d7ce8f3b61851871d4 |
| giflib-5.2.2.tar.gz | https://deb.debian.org/debian/pool/main/g/giflib/giflib_5.2.2.orig.tar.gz | be7ffbd057cadebe2aa144542fd90c6838c6a083b5e8a9048b8ee3b66b29d5fb |

Each archive contains its upstream license. SDL is the separate `libsdl`
submodule pinned to c27776d4ee8055e9a1e543b21eb43e71983ebea6.
