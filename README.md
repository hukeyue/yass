# Yet Another Shadow Socket

yass is an efficient forward proxy client supporting http/socks4/socks4a/socks5/socks5h protocol running on PC and mobile devices.

If you want to visit [old version of this](README.github.md).

## Releases

Because we are reusing chromium's network stack directly,
we are following [chromium's release schedule](https://chromiumdash.appspot.com/schedule) and delivering new versions based on its beta branch.

- [Latest M132's Release (1.16.x)](https://gitee.com/keeyou/yass/releases/tag/1.16.2) will become Stable Release since _Jan 14, 2025_ (Extended Support).
- [Latest M131's Release (1.15.x)](https://gitee.com/keeyou/yass/releases/tag/1.15.4) has become Stable Release since _Nov 12, 2024_.

### Prebuilt binaries (Supported platforms)
- GTK3 [download rpm][gtk3_rpm_url] or [download deb][gtk3_deb_url] (minimum requirement: _CentOS 8_ or _Ubuntu 16.04_)
- Qt5 [download rpm][qt5_rpm_url] or [download deb][qt5_deb_url] (minimum requirement: _CentOS 8_ or _Ubuntu 16.04_)
- GTK4 [download rpm][gtk4_rpm_url] or [download deb][gtk4_deb_url] (minimum requirement: _openSUSE Leap 15.5_, _CentOS 9_ or _Ubuntu 22.04_)
- Qt6 [download rpm][qt6_rpm_url] or [download deb][qt6_deb_url] (minimum requirement: _openSUSE Leap 15.5_, _CentOS 9_ with epel or _Ubuntu 22.04_)

[![aur yass-proxy-gtk3](https://img.shields.io/aur/version/yass-proxy-gtk3)](https://aur.archlinux.org/packages/yass-proxy-gtk3)
[![aur yass-proxy-qt5](https://img.shields.io/aur/version/yass-proxy-qt5)](https://aur.archlinux.org/packages/yass-proxy-qt5)
[![aur yass-proxy](https://img.shields.io/aur/version/yass-proxy)](https://aur.archlinux.org/packages/yass-proxy)
[![aur yass-proxy-qt6](https://img.shields.io/aur/version/yass-proxy-qt6)](https://aur.archlinux.org/packages/yass-proxy-qt6)

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
- Legacy Windows [download 64-bit installer][windows_64_installer_url] (require [KB2999226] on _windows 7/8/8.1_) or [download 32-bit installer][windows_32_installer_url] (require [vc 2010 runtime][vs2010_x86] on _windows xp sp3_) or [download arm64 installer][windows_arm64_installer_url] (require _windows 10/11_)
- Legacy macOS [download intel dmg][macos_intel_dmg_url] or [download apple silicon dmg][macos_arm_dmg_url] (require _macOS 10.14_ or above)

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

[flathub_url]: https://flathub.org/apps/io.github.chilledheart.yass
[flatpak_setup_url]: https://flatpak.org/setup/
[ios_testflight_invitation]: https://testflight.apple.com/join/6AkiEq09
[ios_testflight_appstore_url]: https://apps.apple.com/us/app/testflight/id899247664
[KB2999226]: https://support.microsoft.com/en-us/topic/update-for-universal-c-runtime-in-windows-c0514201-7fe6-95a3-b0a5-287930f3560c
[vs2010_x86]: https://download.microsoft.com/download/1/6/5/165255E7-1014-4D0A-B094-B6A430A6BFFC/vcredist_x86.exe
[transifex]: https://app.transifex.com/yetanothershadowsocket/yetanothershadowsocket

[mlkem]: https://datatracker.ietf.org/doc/draft-connolly-tls-mlkem-key-agreement/
[kyber]: https://datatracker.ietf.org/doc/draft-tls-westerbaan-xyber768d00/

[gtk3_rpm_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass-gtk3.el8.x86_64.1.16.2.rpm
[gtk3_deb_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass-gtk3-ubuntu-16.04-xenial_amd64.1.16.2.deb
[qt5_rpm_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass-qt5.el8.x86_64.1.16.2.rpm
[qt5_deb_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass-qt5-ubuntu-16.04-xenial_amd64.1.16.2.deb
[gtk4_rpm_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass-gtk4.lp155.x86_64.1.16.2.rpm
[gtk4_deb_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass-gtk4-ubuntu-22.04-jammy_amd64.1.16.2.deb
[qt6_rpm_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass-qt6.lp155.x86_64.1.16.2.rpm
[qt6_deb_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass-qt6-ubuntu-22.04-jammy_amd64.1.16.2.deb

[qt6_flatpak_x86_64_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass-x86_64-1.16.2.flatpak

[cli_tgz_amd64_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass_cli-linux-release-amd64-1.16.2.tgz
[cli_tgz_i386_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass_cli-linux-release-amd64-1.16.2.tgz
[cli_tgz_arm64_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass_cli-linux-release-arm64-1.16.2.tgz
[cli_tgz_loongarch64_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass_cli-linux-release-loongarch64-1.16.2.tgz
[cli_tgz_riscv64_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass_cli-linux-release-riscv64-1.16.2.tgz
[cli_tgz_riscv32_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass_cli-linux-release-riscv32-1.16.2.tgz

[cli_openwrt_amd64_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass_cli-linux-openwrt-release-x86_64-1.16.2.tgz
[cli_openwrt_i486_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass_cli-linux-openwrt-release-i486-1.16.2.tgz
[cli_openwrt_aarch64_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass_cli-linux-openwrt-release-aarch64-1.16.2.tgz

[cli_musl_amd64_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass_cli-linux-musl-release-amd64-1.16.2.tgz
[cli_musl_i386_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass_cli-linux-musl-release-i386-1.16.2.tgz
[cli_musl_aarch64_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass_cli-linux-musl-release-aarch64-1.16.2.tgz
[cli_musl_armhf_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass_cli-linux-musl-release-armhf-1.16.2.tgz

[android_64_apk_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass-android-release-arm64-1.16.2.apk
[android_32_apk_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass-android-release-arm-1.16.2.apk

[windows_64_installer_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass-mingw-win7-release-x86_64-1.16.2-system-installer.exe
[windows_32_installer_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass-mingw-winxp-release-i686-1.16.2-system-installer.exe

[windows_arm64_installer_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass-mingw-release-aarch64-1.16.2-system-installer.exe
[macos_intel_dmg_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass-macos-release-x64-1.16.2.dmg
[macos_arm_dmg_url]: https://gitee.com/keeyou/yass/releases/download/1.16.2/yass-macos-release-arm64-1.16.2.dmg
