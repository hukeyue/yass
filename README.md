# Yet Another Shadow Socket

[YASS] is an efficient forward proxy client supporting http/socks4/socks4a/socks5/socks5h protocol running on PC and mobile devices.

## Releases

[![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/hukeyue/yass)](https://github.com/hukeyue/yass/releases)
[![Language: C++](https://img.shields.io/github/languages/top/hukeyue/yass.svg)](https://github.com/hukeyue/yass/search?l=cpp)
[![GitHub release (latest by SemVer)](https://img.shields.io/github/downloads/hukeyue/yass/latest/total)](https://github.com/hukeyue/yass/releases/latest)

Because we are reusing chromium's network stack directly,
we are following [chromium's release schedule](https://chromiumdash.appspot.com/schedule) and delivering new versions based on its beta branch.

_NEW ROLLING BUILDS:_
- M154's Release (1.30.x) will become Stable Release _from Sep 22, 2026_.
- M153's Release (1.29.x) will become Stable Release _from Sep 8, 2026_.

_PLANNED BUILDS:_
- M152's Release (1.28.x) will become Stable Release _from Aug 25, 2026_ (Extended Support).
- M151's Release (1.27.x) will become Stable Release _from Jul 28, 2026_.
- M150's Release (1.26.x) will become Stable Release _from Jun 30, 2026_ (Extended Support).
- M149's Release (1.25.x) will become Stable Release _from Jun 2, 2026_.

_LANDED BUILDS_(signed against new certificates)_:_
- [M148's Release (1.24.x)](https://github.com/hukeyue/yass/releases/tag/1.24.5) will become Stable Release _from May 5, 2026_ (Extended Support).
- M147's Release (1.23.x) will become Stable Release _from Apr 7, 2026_.
- [M146's Release (1.22.x)](https://github.com/hukeyue/yass/releases/tag/1.22.11) has become Stable Release _from Mar 10, 2026_ (Extended Support).
- M145's Release (1.21.x) has become Stable Release _from Feb 10, 2025_.
- [M144's Release (1.20.x)](https://github.com/hukeyue/yass/releases/tag/1.20.5) has become Stable Release _from Jan 13, 2026_ (Extended Support).
- M143's Release (1.19.x) has become Stable Release _from Dec 16, 2025_.
- M142's Release (1.18.x) has become Stable Release _from Oct 28, 2025_ (Extended Support).
- [M141's Release (1.17.x)](https://github.com/hukeyue/yass/releases/tag/1.17.5) has become Stable Release _from Sep 30, 2025_.
- [REFRESHED M132's Release (1.16.x)](https://github.com/hukeyue/yass/releases/tag/1.16.4) has become Stable Release _since Jan 14, 2025_ (Extended Support).

> Due to the change of certificate, re-installation [YASS] on [Android Phone][android_64_apk_url] and [macOS prebuilts][macos_intel_dmg_url] is necessary.

> From 1.20.4 and 1.17.5, Command Line binaries (similar to naiveproxy binary) for Android and Windows are provided.

_BAKED BUILDS_(signed with baked certificates)_:_
- [Latest M132's Release (1.16.x)](https://github.com/hukeyue/yass/releases/tag/1.16.2) has become Stable Release since _Jan 14, 2025_ (Extended Support).
- [Latest M131's Release (1.15.x)](https://github.com/hukeyue/yass/releases/tag/1.15.4) has become Stable Release since _Nov 12, 2024_.

### Prebuilt binaries (Supported platforms)
- GTK3 for AMD64 [download rpm][gtk3_rpm_url] or [download deb][gtk3_deb_url] (minimum requirement: _openSUSE Leap 15.5_, _CentOS 9_ or _Ubuntu 16.04_)
- GTK3 for i686 [download i686 deb][gtk3_i686_deb_url] (minimum requirement: _Ubuntu 16.04 Xenial_)
- GTK3 for ARM64 [download arm64 deb][gtk3_arm64_deb_url] (minimum requirement: _Ubuntu 18.04 Bionic_)
- GTK3 for RISCV64 [download riscv64 deb][gtk3_riscv64_deb_url] (minimum requirement: _Debian 13 Trixie_)
- CLI for LOONG64 [download loong64 deb][cli_loong64_deb_url] (minimum requirement: _Debian 14 Forky/Sid_)
- Qt5 for AMD64 [download rpm][qt5_rpm_url] or [download deb][qt5_deb_url] (minimum requirement: _openSUSE Leap 15.5_, _CentOS 9_ or _Ubuntu 16.04_)
- GTK4 for AMD64 [download rpm][gtk4_rpm_url] or [download deb][gtk4_deb_url] (minimum requirement: _openSUSE Leap 15.5_, _CentOS 9_ or _Ubuntu 22.04_)
- Qt6 for AMD64 [download rpm][qt6_rpm_url] or [download deb][qt6_deb_url] (minimum requirement: _openSUSE Leap 15.5_, _CentOS 9_ with epel or _Ubuntu 22.04_)

> please download and install yass-common package (list below) as well

- GTK3 Common for AMD64 [download rpm][gtk3_common_rpm_url] or [download deb][gtk3_common_deb_url] (minimum requirement: _openSUSE Leap 15.5_, _CentOS 9_ or _Ubuntu 16.04_)
- GTK3 Common for i686 [download i686 deb][gtk3_i686_common_deb_url] (minimum requirement: _Ubuntu 16.04 Xenial_)
- GTK3 Common for ARM64 [download arm64 deb][gtk3_arm64_common_deb_url] (minimum requirement: _Ubuntu 18.04 Bionic_)
- GTK3 Common for RISCV64 [download riscv64 deb][gtk3_riscv64_common_deb_url] (minimum requirement: _Debian 13 Trixie_)
- CLI Common for LOONG64 [download loong64 deb][cli_loong64_common_deb_url] (minimum requirement: _Debian 14 Forky/Sid_)
- Qt5 Common for AMD64 [download rpm][qt5_common_rpm_url] or [download deb][qt5_common_deb_url] (minimum requirement: _openSUSE Leap 15.5_, _CentOS 9_ or _Ubuntu 16.04_)
- GTK4 Common for AMD64 [download rpm][gtk4_common_rpm_url] or [download deb][gtk4_common_deb_url] (minimum requirement: _openSUSE Leap 15.5_, _CentOS 9_ or _Ubuntu 22.04_)
- GTK4 Common for AMD64 [download rpm][qt6_common_rpm_url] or [download deb][qt6_common_deb_url] (minimum requirement: _openSUSE Leap 15.5_, _CentOS 9_ with epel or _Ubuntu 22.04_)

[![aur yass-proxy-gtk3](https://img.shields.io/aur/version/yass-proxy-gtk3)](https://aur.archlinux.org/packages/yass-proxy-gtk3)
[![aur yass-proxy-qt5](https://img.shields.io/aur/version/yass-proxy-qt5)](https://aur.archlinux.org/packages/yass-proxy-qt5)
[![aur yass-proxy](https://img.shields.io/aur/version/yass-proxy)](https://aur.archlinux.org/packages/yass-proxy)
[![aur yass-proxy-qt6](https://img.shields.io/aur/version/yass-proxy-qt6)](https://aur.archlinux.org/packages/yass-proxy-qt6)

See [Status of Package Store](https://github.com/hukeyue/yass/wiki/Status-of-Package-Store) for more.

- CLI [download tgz for amd64][cli_tgz_amd64_url] or [download tgz for i386][cli_tgz_i386_url] or [download tgz for arm64][cli_tgz_arm64_url] (require glibc >= 2.25)
- CLI [download tgz for loongarch64][cli_tgz_loongarch64_url] (require glibc >= 2.38, _new world_)
- CLI [download tgz for riscv64][cli_tgz_riscv64_url] or [download tgz for riscv32][cli_tgz_riscv32_url] (require glibc >= 2.36)
- CLI(openwrt) [download tgz for amd64][cli_openwrt_amd64_url] or [download tgz for i486][cli_openwrt_i486_url] or [download tgz for aarch64 generic][cli_openwrt_aarch64_url] (static build, no madvise support required)
- CLI(musl) [download tgz for amd64][cli_musl_amd64_url] or [download tgz for i386][cli_musl_i386_url] or [download tgz for aarch64][cli_musl_aarch64_url] or [download tgz for armhf][cli_musl_armhf_url] (static build, no madvise support required)

[![aur yass-proxy-cli](https://img.shields.io/aur/version/yass-proxy-cli)](https://aur.archlinux.org/packages/yass-proxy-cli)

- Flatpak for Linux (Qt6) [download flatpak][qt6_flatpak_x86_64_url] for _x86_64_ only.

[Flathub][flathub_url]: available at latest version (Qt5, [report if you need Qt6 version][frs]), require [Flatpak][flatpak_setup_url] installed).

[![Flathub Version](https://img.shields.io/flathub/v/io.github.chilledheart.yass)][flathub_url]
[![Flathub Downloads](https://img.shields.io/flathub/downloads/io.github.chilledheart.yass)][flathub_url]

### Prebuilt binaries (Supported legacy platforms)

- Legacy Android [download 64-bit apk][android_64_apk_url] or [download 32-bit apk][android_32_apk_url] (require _Android 7.0_ or above)
- Legacy iOS [Continue to accept TestFlight invitation][ios_testflight_invitation] (require [TestFlight][ios_testflight_appstore_url] from _AppStore_, and _iOS 13.0_ or above)
- Legacy WindowsXPSP3/Windows7SP1+MinGW [download 64-bit installer][mingw_64_installer_url] (require [KB2999226] on _windows 7 sp1 or above_) or [download 32-bit installer][mingw_32_installer_url] (require [last vc 2019 runtime supporting xp sp3][vs2019_xp_x86] on _windows xp sp3 or above_) or [download arm64 installer][mingw_arm64_installer_url] (require _windows 11_)
- Legacy Windows10+VS2022 (MSVC) [download 64-bit installer][windows_64_installer_url] (require [vs 2022 X64 Runtime][vs2022_x64]) or [download 32-bit installer][windows_32_installer_url] (require [VC++ X86 Runtime][vs2022_x86]) or [download arm64 installer][windows_arm64_installer_url] (require [VC++ ARM64 Runtime][vs2022_arm64] on _windows 11_)
- Legacy WindowsXPSP3/Windows7SP1+VS2017 (deprecated MSVC) [download 64-bit installer][windows_win7_64_installer_url] (require [KB2999226] on _windows 7 sp1 or above_) or [download 32-bit installer][windows_winxp_32_installer_url] (require [last vc 2019 runtime supporting xp sp3][vs2019_xp_x86] on _windows xp sp3 or above_).
- Legacy macOS [download intel dmg][macos_intel_dmg_url] or [download apple silicon dmg][macos_arm_dmg_url] (require _macOS 10.14_ or above)

> New [iOS Beta Build][ios_testflight_invitation] is provided periodically (1.20.4 FOR NOW).

See [Supporteded Operating System](https://github.com/hukeyue/yass/wiki/Supported-Operating-System) for more.

Special Note: for legacy supported platforms, [bugs][bugs] will be fixed ASAP while [feature request][frs] is a time-consuming task and
it is expected that it would be delayed in a proper and unlimited period.

### Contribute your translation (i18n)

Please submit translations via [Transifex][transifex]

Steps:

1. Create a free account on [Transifex][transifex] (https://www.transifex.com/).
2. Send a request to join the language translation.
3. After accepted by the project maintainer, then you can translate online.

### Build from Source
Take a look at [build instructions](BUILDING.md) and [packaging instructions](PACKAGING.md).

## Highlight Features

### TLS-Compatible Protocol Support
Cipher http over TLS are compatible.

If you need custom Certificate Authority support in GUI or add other TLS-related features, [report here][frs].

### RFC9849: TLS Encrypted Client Hello aka ECH Support (investigating)
It has become [RFC9849](https://www.rfc-editor.org/rfc/rfc9849.html) recently. Chrome/Firefox enables ECH by default from M119.

Without ECH GREASE, it relies DOH configuration. See more at [curl's documentation](https://github.com/curl/curl/blob/master/docs/ECH.md)

### Don't configure ALPS protocols that aren't also configured in ALPN
After M148 Release 1, all ALPN settings including HTTP1.1 and H2 will configure its ALPS protocols. (backported from chromium application stack from M122)

In previous release, SSLSocket currently configures all application_settings values in BoringSSL, however BoringSSL's API docs say not to do this: https://commondatastorage.googleapis.com/chromium-boringssl-docs/ssl.h.html#SSL_add_application_settings.

See also discussion here: https://chromium-review.googlesource.com/c/chromium/src/+/5064051/comment/cfc4cb1b_cda4ca01/

### New ALPS codepoint
Old version Chrome with the existing ALPS codepoint can potentially cause network error due to an arithmetic overflow bug in Chrome ALPS decoder (We already fixed the issues starting from M100 in Chrome).

After M141 Release 0, the default became to use the new codepoint, but you can pass --use_new_alps_codepoint_http2=0 to disable it. (backported from chromium application stack from M135 Release)

### Post Quantum key-agreements for TLS 1.3
[ML-KEM Post Quantum key-agreements][mlkem] (not enabled by default) for TLS 1.3 is supported on all platforms
in place of obsolete [Kyber768 hybrid key-agreements][kyber].

See [Protecting Chrome Traffic with Hybrid Kyber KEM](https://blog.chromium.org/2023/08/protecting-chrome-traffic-with-hybrid.html) for more.

### Socks-Compatible Protocol Support
Cipher socks4 and socks5 are compatible.

### Shadowsocket-Compatible Protocol Support
Ciphers released ahead of 2022 are compatible.

### NaïveProxy-Compatible Protocol Support
Cipher http2 and https are compatible.

See [NaïveProxy](https://github.com/klzgrad/naiveproxy)'s project homepage for support.

## Usages

### Standard Usages
Visit wiki's [Usages](https://github.com/hukeyue/yass/wiki/Usage).

### Server side support (mostly outside this project)

See [Setup forwardproxy caddy service](https://github.com/hukeyue/yass/wiki/Usage:-server-setup#setup-forwardproxy-caddy-service) for more.

See [Window sizes for large bandwidth](https://github.com/hukeyue/yass/wiki/Usage:-server-setup#window-sizes-for-large-bandwidth) for more.

See [Use BBR Congestion Control](https://github.com/hukeyue/yass/wiki/Usage:-server-setup#use-bbr-congestion-control) for more.

See [ChatGPT capable caddy Server](https://github.com/hukeyue/yass/wiki/Usage:-server-setup#chatgpt-capable-caddy-server) for more.

### Debug Guide
Start from wiki's [Guide](https://github.com/hukeyue/yass/wiki/Debug-Guide)

## Build Status

[![CircleCI](https://img.shields.io/circleci/build/github/hukeyue/yass/develop?logo=circleci&&label=Sanitizers%20and%20Ubuntu%20arm)](https://circleci.com/gh/hukeyue/yass/?branch=develop)

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

[Report here if you want to connect to new CI infrastructure][frs]

[YASS]: https://letshack.info
[flathub_url]: https://flathub.org/apps/io.github.chilledheart.yass
[flatpak_setup_url]: https://flatpak.org/setup/
[ios_testflight_invitation]: https://testflight.apple.com/join/6AkiEq09
[ios_testflight_appstore_url]: https://apps.apple.com/us/app/testflight/id899247664
[KB2999226]: https://support.microsoft.com/en-us/topic/update-for-universal-c-runtime-in-windows-c0514201-7fe6-95a3-b0a5-287930f3560c
[vs2010_x86]: https://download.microsoft.com/download/1/6/5/165255E7-1014-4D0A-B094-B6A430A6BFFC/vcredist_x86.exe
[vs2019_xp_x86]: https://download.visualstudio.microsoft.com/download/pr/56f631e5-4252-4f28-8ecc-257c7bf412b8/D305BAA965C9CD1B44EBCD53635EE9ECC6D85B54210E2764C8836F4E9DEFA345/VC_redist.x86.exe
[vs2019_xp_x64]: https://download.visualstudio.microsoft.com/download/pr/722d59e4-0671-477e-b9b1-b8da7d4bd60b/591CBE3A269AFBCC025681B968A29CD191DF3C6204712CBDC9BA1CB632BA6068/VC_redist.x64.exe
[transifex]: https://app.transifex.com/yetanothershadowsocket/yetanothershadowsocket
[vs2022_x64]: https://aka.ms/vs/17/release/vc_redist.x64.exe
[vs2022_x86]: https://aka.ms/vs/17/release/vc_redist.x86.exe
[vs2022_arm64]: https://aka.ms/vs/17/release/vc_redist.arm64.exe

[bugs]: https://github.com/hukeyue/yass/issues/new?assignees=&labels=bug&projects=&template=bug_report.md&title=
[frs]: https://github.com/hukeyue/yass/issues/new?assignees=&labels=feature&projects=&template=feature_request.md&title=

[mlkem]: https://datatracker.ietf.org/doc/draft-connolly-tls-mlkem-key-agreement/
[kyber]: https://datatracker.ietf.org/doc/draft-tls-westerbaan-xyber768d00/

[gtk3_rpm_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-gtk3.lp155.x86_64.1.24.5.rpm
[gtk3_deb_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-gtk3-ubuntu-16.04-xenial_amd64.1.24.5.deb
[gtk3_i686_deb_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-gtk3-ubuntu-16.04-xenial_i386.1.24.5.deb
[gtk3_arm64_deb_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-gtk3-ubuntu-18.04-bionic_arm64.1.24.5.deb
[gtk3_riscv64_deb_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-gtk3-debian-13-trixie_riscv64.1.24.5.deb
[cli_loong64_deb_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-client-debian-sid_loong64.1.24.5.deb
[qt5_rpm_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-qt5.lp155.x86_64.1.24.5.rpm
[qt5_deb_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-qt5-ubuntu-16.04-xenial_amd64.1.24.5.deb
[gtk4_rpm_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-gtk4.lp155.x86_64.1.24.5.rpm
[gtk4_deb_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-gtk4-ubuntu-22.04-jammy_amd64.1.24.5.deb
[qt6_rpm_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-qt6.lp155.x86_64.1.24.5.rpm
[qt6_deb_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-qt6-ubuntu-22.04-jammy_amd64.1.24.5.deb

[gtk3_common_rpm_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-common.lp155.x86_64.1.24.5.rpm
[gtk3_common_deb_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-common-ubuntu-16.04-xenial_amd64.1.24.5.deb
[gtk3_i686_common_deb_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-common-ubuntu-16.04-xenial_i386.1.24.5.deb
[gtk3_arm64_common_deb_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-common-ubuntu-18.04-bionic_arm64.1.24.5.deb
[gtk3_riscv64_common_deb_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-common-debian-13-trixie_riscv64.1.24.5.deb
[cli_loong64_common_deb_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-common-debian-sid_loong64.1.24.5.deb
[qt5_common_rpm_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-common.lp155.x86_64.1.24.5.rpm
[qt5_common_deb_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-common-ubuntu-16.04-xenial_amd64.1.24.5.deb
[gtk4_common_rpm_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-common.lp155.x86_64.1.24.5.rpm
[gtk4_common_deb_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-common-ubuntu-22.04-jammy_amd64.1.24.5.deb
[qt6_common_rpm_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-common.lp155.x86_64.1.24.5.rpm
[qt6_common_deb_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-common-ubuntu-22.04-jammy_amd64.1.24.5.deb

[qt6_flatpak_x86_64_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-x86_64-1.24.5.flatpak

[cli_tgz_amd64_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass_cli-linux-release-amd64-1.24.5.tgz
[cli_tgz_i386_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass_cli-linux-release-amd64-1.24.5.tgz
[cli_tgz_arm64_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass_cli-linux-release-arm64-1.24.5.tgz
[cli_tgz_loongarch64_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass_cli-linux-release-loongarch64-1.24.5.tgz
[cli_tgz_riscv64_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass_cli-linux-release-riscv64-1.24.5.tgz
[cli_tgz_riscv32_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass_cli-linux-release-riscv32-1.24.5.tgz

[cli_openwrt_amd64_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass_cli-linux-openwrt-release-x86_64-1.24.5.tgz
[cli_openwrt_i486_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass_cli-linux-openwrt-release-i486-1.24.5.tgz
[cli_openwrt_aarch64_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass_cli-linux-openwrt-release-aarch64-1.24.5.tgz

[cli_musl_amd64_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass_cli-linux-musl-release-amd64-1.24.5.tgz
[cli_musl_i386_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass_cli-linux-musl-release-i386-1.24.5.tgz
[cli_musl_aarch64_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass_cli-linux-musl-release-aarch64-1.24.5.tgz
[cli_musl_armhf_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass_cli-linux-musl-release-armhf-1.24.5.tgz

[android_64_apk_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-android-release-arm64-1.24.5.apk
[android_32_apk_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-android-release-arm-1.24.5.apk

[mingw_64_installer_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-mingw-win7-release-x86_64-1.24.5-system-installer.exe
[mingw_32_installer_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-mingw-winxp-release-i686-1.24.5-system-installer.exe
[mingw_arm64_installer_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-mingw-release-aarch64-1.24.5-system-installer.exe

[windows_64_installer_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-win-release-x86-dynamic-1.24.5-user-installer.exe
[windows_32_installer_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-win-release-x64-dynamic-1.24.5-user-installer.exe
[windows_arm64_installer_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-win-release-arm64-dynamic-1.24.5-user-installer.exe

[windows_win7_64_installer_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-win7-release-x64-dynamic-1.24.5-user-installer.exe
[windows_winxp_32_installer_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-winxp-release-x86-dynamic-1.24.5-user-installer.exe

[macos_intel_dmg_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-macos-release-x64-1.24.5.dmg
[macos_arm_dmg_url]: https://github.com/hukeyue/yass/releases/download/1.24.5/yass-macos-release-arm64-1.24.5.dmg
[apple_developer]: https://developer.apple.com
