# Yet Another Shadow Socket

[![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/hukeyue/yass)](https://github.com/hukeyue/yass/releases)
[![Language: C++](https://img.shields.io/github/languages/top/hukeyue/yass.svg)](https://github.com/hukeyue/yass/search?l=cpp)
[![GitHub release (latest by SemVer)](https://img.shields.io/github/downloads/hukeyue/yass/latest/total)](https://github.com/hukeyue/yass/releases/latest)

## Contribute your translation (i18n)

Please submit translations via [Transifex][transifex]

Steps:

1. Create a free account on [Transifex][transifex] (https://www.transifex.com/).
2. Send a request to join the language translation.
3. After accepted by the project maintainer, then you can translate online.

[![aur yass-proxy-gtk3](https://img.shields.io/aur/version/yass-proxy-gtk3)](https://aur.archlinux.org/packages/yass-proxy-gtk3)
[![aur yass-proxy-qt5](https://img.shields.io/aur/version/yass-proxy-qt5)](https://aur.archlinux.org/packages/yass-proxy-qt5)
[![aur yass-proxy](https://img.shields.io/aur/version/yass-proxy)](https://aur.archlinux.org/packages/yass-proxy)
[![aur yass-proxy-qt6](https://img.shields.io/aur/version/yass-proxy-qt6)](https://aur.archlinux.org/packages/yass-proxy-qt6)
[![aur yass-proxy-cli](https://img.shields.io/aur/version/yass-proxy-cli)](https://aur.archlinux.org/packages/yass-proxy-cli)
[![Flathub Version](https://img.shields.io/flathub/v/io.github.chilledheart.yass)][flathub_url]
[![Flathub Downloads](https://img.shields.io/flathub/downloads/io.github.chilledheart.yass)][flathub_url]

## Build Status

[![MSVC+VS2022 Build](https://github.com/hukeyue/yass/actions/workflows/releases-windows.yml/badge.svg)](https://github.com/hukeyue/yass/actions/workflows/releases-windows.yml)
[![MinGW Build](https://github.com/hukeyue/yass/actions/workflows/releases-mingw-new.yml/badge.svg)](https://github.com/hukeyue/yass/actions/workflows/releases-mingw-new.yml)
[![Linux Build](https://github.com/hukeyue/yass/actions/workflows/releases-linux-binary.yml/badge.svg)](https://github.com/hukeyue/yass/actions/workflows/releases-linux-binary.yml)
[![macOS Build](https://github.com/hukeyue/yass/actions/workflows/releases-macos.yml/badge.svg)](https://github.com/hukeyue/yass/actions/workflows/releases-macos.yml)
[![Android Build](https://github.com/hukeyue/yass/actions/workflows/releases-android-binary.yml/badge.svg)](https://github.com/hukeyue/yass/actions/workflows/releases-android-binary.yml)
[![iOS Build](https://github.com/hukeyue/yass/actions/workflows/releases-ios.yml/badge.svg)](https://github.com/hukeyue/yass/actions/workflows/releases-ios.yml)
[![OpenWRT Build](https://github.com/hukeyue/yass/actions/workflows/releases-openwrt-binary.yml/badge.svg)](https://github.com/hukeyue/yass/actions/workflows/releases-openwrt-binary.yml)
[![FreeBSD Build](https://github.com/hukeyue/yass/actions/workflows/releases-freebsd-binary.yml/badge.svg)](https://github.com/hukeyue/yass/actions/workflows/releases-freebsd-binary.yml)
[![RPM Build](https://github.com/hukeyue/yass/actions/workflows/releases-rpm.yml/badge.svg)](https://github.com/hukeyue/yass/actions/workflows/releases-rpm.yml)
[![DEB Build](https://github.com/hukeyue/yass/actions/workflows/releases-deb.yml/badge.svg)](https://github.com/hukeyue/yass/actions/workflows/releases-deb.yml)
[![Flatpak Build](https://github.com/hukeyue/yass/actions/workflows/releases-flatpak.yml/badge.svg)](https://github.com/hukeyue/yass/actions/workflows/releases-flatpak.yml)
[![MSVC+XPSP3 Build](https://github.com/hukeyue/yass/actions/workflows/releases-windows-vs2017.yml/badge.svg)](https://github.com/hukeyue/yass/actions/workflows/releases-windows-vs2017.yml)
[![MSVC+VS2026 Build](https://github.com/hukeyue/yass/actions/workflows/releases-windows-vs2026.yml/badge.svg)](https://github.com/hukeyue/yass/actions/workflows/releases-windows-vs2026.yml)
[![Clang Tidy](https://github.com/hukeyue/yass/actions/workflows/clang-tidy.yml/badge.svg)](https://github.com/hukeyue/yass/actions/workflows/clang-tidy.yml)
[![CircleCI](https://img.shields.io/circleci/build/github/hukeyue/yass/develop?logo=circleci&&label=Sanitizers%20and%20Ubuntu%20arm)](https://circleci.com/gh/hukeyue/yass/?branch=develop)
[![MUSL Build](https://github.com/hukeyue/yass/actions/workflows/releases-musl.yml/badge.svg)](https://github.com/hukeyue/yass/actions/workflows/releases-musl.yml)

## Build from Source
Take a look at [build instructions](BUILDING.md) and [packaging instructions](PACKAGING.md).

## Debug Guide
Start from wiki's [Guide](https://github.com/hukeyue/yass/wiki/Debug-Guide)

## Notes on System Requirement

- Android: Require 7.0 or above
- iOS: NO IPK release. Require 13.0 or above. You should accept [TestFlight invitation][ios_testflight_invitation] (require [TestFlight][ios_testflight_appstore_url] from _AppStore_)
- Legacy Windows (missing ucrtbase.dll): for Windows 7 install [KB3118401] or [KB2999226] first, and for Windows XP SP3 try [last vs 2019 runtime supporting xp sp3][vs2019_xp_x86] (VS 2019 version 14.28.29213.0 is the last version compatible with Windows XP SP3).
- macOS: Require 10.14 or above
- Windows (missing VCRUNTIME140.DLL and etc): for x64 install [vs 2022 x64 runtime][vs2022_x64], for x86 install [vs 2022 x86 runtime][vs2022_x86], and for arm64 [vs 2022 arm64 runtime][vs2022_arm64] (VS 2022 will be the last v14 for Windows 7/8/8.1 and their Windows Server equivalents).

See [Status of Package Store](https://github.com/hukeyue/yass/wiki/Status-of-Package-Store) for more.
See [Supporteded Operating System](https://github.com/hukeyue/yass/wiki/Supported-Operating-System) for more.

## Usage
Visit wiki's [Usage](https://github.com/hukeyue/yass/wiki/Usage).

## Server side support (mostly outside this project)

See [Setup forwardproxy caddy service](https://github.com/hukeyue/yass/wiki/Usage:-server-setup#setup-forwardproxy-caddy-service) for more.

See [Window sizes for large bandwidth](https://github.com/hukeyue/yass/wiki/Usage:-server-setup#window-sizes-for-large-bandwidth) for more.

See [Use BBR Congestion Control](https://github.com/hukeyue/yass/wiki/Usage:-server-setup#use-bbr-congestion-control) for more.

See [ChatGPT capable caddy Server](https://github.com/hukeyue/yass/wiki/Usage:-server-setup#chatgpt-capable-caddy-server) for more.

[YASS]: https://letshack.info
[flathub_url]: https://flathub.org/apps/io.github.chilledheart.yass
[flatpak_setup_url]: https://flatpak.org/setup/
[ios_testflight_invitation]: https://testflight.apple.com/join/6AkiEq09
[ios_testflight_appstore_url]: https://apps.apple.com/us/app/testflight/id899247664
[KB3118401]: https://www.catalog.update.microsoft.com/Search.aspx?q=kb3118401
[KB2999226]: https://support.microsoft.com/en-us/topic/update-for-universal-c-runtime-in-windows-c0514201-7fe6-95a3-b0a5-287930f3560c
[vs2010_x86]: https://download.microsoft.com/download/1/6/5/165255E7-1014-4D0A-B094-B6A430A6BFFC/vcredist_x86.exe
[vs2019_xp_x86]: https://download.visualstudio.microsoft.com/download/pr/56f631e5-4252-4f28-8ecc-257c7bf412b8/D305BAA965C9CD1B44EBCD53635EE9ECC6D85B54210E2764C8836F4E9DEFA345/VC_redist.x86.exe
[vs2019_xp_x64]: https://download.visualstudio.microsoft.com/download/pr/722d59e4-0671-477e-b9b1-b8da7d4bd60b/591CBE3A269AFBCC025681B968A29CD191DF3C6204712CBDC9BA1CB632BA6068/VC_redist.x64.exe
[transifex]: https://app.transifex.com/yetanothershadowsocket/yetanothershadowsocket
[vs2022_x64]: https://aka.ms/vs/17/release/vc_redist.x64.exe
[vs2022_x86]: https://aka.ms/vs/17/release/vc_redist.x86.exe
[vs2022_arm64]: https://aka.ms/vs/17/release/vc_redist.arm64.exe

[mlkem]: https://datatracker.ietf.org/doc/draft-connolly-tls-mlkem-key-agreement/
[kyber]: https://datatracker.ietf.org/doc/draft-tls-westerbaan-xyber768d00/

[apple_developer]: https://developer.apple.com
