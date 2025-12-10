#!/bin/bash
set -x
set -e
PWD=$(dirname "${BASH_SOURCE[0]}")
cd $PWD/..

if [ -z $1 ]; then
  echo "Usage $0 <tag>"
  exit -1
fi

function notarize {
  TAG=$1
  ARCH=$2
  UNSIGNED_DMG=yass-macos-release-$ARCH-$TAG-unsigned.dmg
  SIGNED_DMG=yass-macos-release-$ARCH-$TAG.dmg
  DEBUGINFO_TARBALL=yass-macos-release-$ARCH-$TAG-debuginfo.zip
  if [ ! -f $UNSIGNED_DMG ]; then
    echo 'Skip missing file:' $UNSIGNED_DMG
    return
  fi
  #curl -L https://github.com/hukeyue/yass/releases/download/$TAG/$UNSIGNED_DMG -o $SIGNED_DMG
  mv -v $UNSIGNED_DMG $SIGNED_DMG
  # xcrun notarytool store-credentials "notary" --apple-id "your-apple-id@example.com" --team-id "TEAMID12345" --password "AppSpecificPassword"
  # Team ID: W5PW377ZKW
  xcrun notarytool submit $SIGNED_DMG --keychain-profile notary --wait
  xcrun stapler staple $SIGNED_DMG
  gh release upload $TAG $SIGNED_DMG
  gh release upload $TAG $DEBUGINFO_TARBALL
  #gh release delete-asset -y $TAG $UNSIGNED_DMG
}

function checksum {
  TAG=$1
  ARCH=$2
  SIGNED_DMG=yass-macos-release-$ARCH-$TAG.dmg
  DEBUGINFO_TARBALL=yass-macos-release-$ARCH-$TAG-debuginfo.zip
  sha256sum $SIGNED_DMG
  sha256sum $DEBUGINFO_TARBALL
}

notarize $1 x64
notarize $1 arm64

checksum $1 x64
checksum $1 arm64
