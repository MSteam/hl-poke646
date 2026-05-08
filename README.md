# README

## About The Project

This project uses the Half-Life 1 SDK. The code has been modified to work with newer and modern compilers, while preserving the original structure of the default SDK.

## Supported Platforms

- Windows
- Linux (32-bit, see [BUILDING_LINUX.md](BUILDING_LINUX.md))

## Built with

- [CMake](https://cmake.org/) (Windows)
- GNU Make + GCC multilib (Linux)
- [Half-Life 1 SDK](https://github.com/ValveSoftware/halflife)
- [Visual Studio](https://visualstudio.microsoft.com/) (Windows)

## Installation

See [installation instructions](INSTALL.md).

## Building The Code

- Windows: see [BUILDING.md](BUILDING.md).
- Linux: see [BUILDING_LINUX.md](BUILDING_LINUX.md).

To make the Linux server library load on dedicated/Linux clients, the mod's
`liblist.gam` needs a Linux-specific entry next to the existing `gamedll`:

```
gamedll        "dlls\hl.dll"
gamedll_linux  "dlls/hl.so"
```

The client library (`cl_dlls/client.so`) is picked up automatically.

## Custom Console Commands

|Command|Description|
|:--|:--|
|modinfo|Display the mod description and version|

## Contributing

See [contribution guidelines](CONTRIBUTING.md).

## License

See [license](LICENSE.md).

## Acknowledgments

Thanks to the following resources for templates, guides and or general structural inspiration.

- [Apache Licence v2.0](https://www.apache.org/licenses/LICENSE-2.0)
- [Best README Template](https://github.com/othneildrew/Best-README-Template)
- [Keep a Changelog](https://keepachangelog.com/)
