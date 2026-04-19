# Yet Another Shadow Socket

[YASS] is an efficient forward proxy client supporting http/socks4/socks4a/socks5/socks5h protocol running on PC and mobile devices.

## Releases

[![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/hukeyue/yass)](https://github.com/hukeyue/yass/releases)
[![Language: C++](https://img.shields.io/github/languages/top/hukeyue/yass.svg)](https://github.com/hukeyue/yass/search?l=cpp)
[![GitHub release (latest by SemVer)](https://img.shields.io/github/downloads/hukeyue/yass/latest/total)](https://github.com/hukeyue/yass/releases/latest)

[![aur yass-proxy-gtk3](https://img.shields.io/aur/version/yass-proxy-gtk3)](https://aur.archlinux.org/packages/yass-proxy-gtk3)
[![aur yass-proxy-qt5](https://img.shields.io/aur/version/yass-proxy-qt5)](https://aur.archlinux.org/packages/yass-proxy-qt5)
[![aur yass-proxy](https://img.shields.io/aur/version/yass-proxy)](https://aur.archlinux.org/packages/yass-proxy)
[![aur yass-proxy-qt6](https://img.shields.io/aur/version/yass-proxy-qt6)](https://aur.archlinux.org/packages/yass-proxy-qt6)
[![aur yass-proxy-cli](https://img.shields.io/aur/version/yass-proxy-cli)](https://aur.archlinux.org/packages/yass-proxy-cli)
[![Flathub Version](https://img.shields.io/flathub/v/io.github.chilledheart.yass)][flathub_url]
[![Flathub Downloads](https://img.shields.io/flathub/downloads/io.github.chilledheart.yass)][flathub_url]

Because we are reusing chromium's network stack directly,
we are following [chromium's release schedule](https://chromiumdash.appspot.com/schedule) and delivering new versions based on its beta branch.

### Some Notes on System Requirement including runtime

- Android: Require 7.0 or above
- iOS: NO IPK release. Require 13.0 or above. You should accept [TestFlight invitation][ios_testflight_invitation] (require [TestFlight][ios_testflight_appstore_url] from _AppStore_)
- Windows 10 or above: for x64 install [vs 2022 x64 runtime][vs2022_x64], for x86 install [vs 2022 x86 runtime][vs2022_x86], and for arm64 [vs 2022 arm64 runtime][vs2022_arm64]
- Legacy Windows: for x64 install [KB2999226] on _windows 7 sp1 or above_, and for x86 install [last vc 2019 runtime supporting xp sp3][vs2019_xp_x86] on _windows xp sp3 or above_
- macOS: Require 10.14 or above

See [Status of Package Store](https://github.com/hukeyue/yass/wiki/Status-of-Package-Store) for more.
See [Supporteded Operating System](https://github.com/hukeyue/yass/wiki/Supported-Operating-System) for more.

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

[mlkem]: https://datatracker.ietf.org/doc/draft-connolly-tls-mlkem-key-agreement/
[kyber]: https://datatracker.ietf.org/doc/draft-tls-westerbaan-xyber768d00/

[apple_developer]: https://developer.apple.com
