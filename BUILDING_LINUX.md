# Building on Linux

The Linux build was added by merging the Windows-only Poke646 source with the
build infrastructure from Valve's old `halflife` SDK.  Because the Half-Life
engine on Linux is 32-bit, the mod libraries must also be 32-bit.

## Prerequisites

- A C/C++ toolchain with multilib support (32-bit headers and libraries).
  On Arch / Manjaro:
  ```sh
  sudo pacman -S gcc-multilib lib32-glibc lib32-libstdc++5
  ```
  On Debian / Ubuntu:
  ```sh
  sudo apt install build-essential gcc-multilib g++-multilib
  ```

- 32-bit SDL2 (the `libSDL2-2.0.so.0` shipped in `linux/` is the one Valve
  distributes with the Linux Half-Life client; see `linux/Makefile` for how
  it is consumed at link time).

## Build

```sh
cd linux
make            # builds both libraries
make client     # client.so only
make server     # hl.so only
make clean
```

Output is in `linux/release/`:

- `hl.so`     — the game/server library, goes into `<mod>/dlls/`
- `client.so` — the client library, goes into `<mod>/cl_dlls/`

## Installation

After a successful build, copy the resulting libraries into the mod
directory inside your Half-Life install:

```sh
cp linux/release/hl.so      "<HL_DIR>/poke646/dlls/hl.so"
cp linux/release/client.so  "<HL_DIR>/poke646/cl_dlls/client.so"
```

Then add a Linux-specific gamedll entry to `<HL_DIR>/poke646/liblist.gam`
right next to the existing Windows one:

```
gamedll        "dlls\hl.dll"
gamedll_linux  "dlls/hl.so"
```

The client library is picked up automatically.

## Notes

- This Linux build was tested with GCC 15.2 (multilib).  The Makefile pins
  the language dialect to `gnu11` for C and `gnu++14` for C++ to keep the
  ancient code base compiling cleanly on modern toolchains.
- The bundled `vgui.so` and `libSDL2-2.0.so.0` in `linux/` are the 32-bit
  ones from Valve's Linux SDK and are only used as link-time stubs — at
  run time the engine provides its own copies.

## Differences vs. the Windows build

The Linux port keeps the source identical wherever possible.  A handful of
small changes were unavoidable:

- `dlls/extdll.h`: dropped the legacy `min`/`max` preprocessor macros for
  POSIX builds (they break C++17 standard headers); added `<limits>`. The
  code base already uses `std::min` / `std::max`, so nothing is lost.
- `dlls/util.h`: added `#include <cstring>` for `std::strcmp`.
- `cl_dll/ammo.cpp`: added `#include <limits>` for `std::numeric_limits`.
- `dlls/weapons.cpp`: the disabled `IMPLEMENT_SAVERESTORE(CRpg, …)` and
  `IMPLEMENT_SAVERESTORE(CEgon, …)` blocks are now wrapped in `#if 0` to
  match the rest of the “No RPG/No Egon” code in `rpg.cpp` / `egon.cpp`.
  GCC's "key function" rule would otherwise emit vtables that reference
  the (intentionally undefined) `Spawn`/`Precache`/etc. of those classes.
- `cl_dll/poke646/music.cpp`: when the mod was first released, music files
  shipped under `poke646/sound/mp3/`; the reimplementation expects them
  under `poke646/media/`. The Linux client now probes `sound/<path>` first
  and falls back to `media/<path>`. If neither is present, it logs a
  warning and plays nothing instead of letting the engine pick a random
  file.
