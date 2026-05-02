## Important Note
This part of code is no longer maintained and has not been verified against SDK above 3.1.0 (above API 9)

## Prepare stage
download [DevEco Studio][deveco-studio] and setup up environment variable `HARMONY_NDK_ROOT`.

Required Steps:
Tools > SDK Manager > SDK > 3.1.0 (API 9)

## Prepare stage (tun2proxy)
Required Steps:
```
./scripts/build-rust.sh
./scripts/setup-harmony-rust.sh
```

## Build stage

```
WITH_OS=harmony WITH_CPU=x64 ./scripts/build-tun2proxy.sh
WITH_OS=harmony WITH_CPU=arm ./scripts/build-tun2proxy.sh
WITH_OS=harmony WITH_CPU=arm64 ./scripts/build-tun2proxy.sh
```

```
./tools/build --variant gui --arch x64 --system harmony --cmake-build-type MinSizeRel -build-benchmark -build-test -no-packaging
./tools/build --variant gui --arch arm --system harmony --cmake-build-type MinSizeRel -build-benchmark -build-test -no-packaging
./tools/build --variant gui --arch arm64 --system harmony --cmake-build-type MinSizeRel -build-benchmark -build-test -no-packaging
```

```
cd harmony
make
```

## Codesign
TBD

[deveco-studio]: https://developer.harmonyos.com/cn/develop/deveco-studio
[rustup.rs]: https://rustup.rs/
