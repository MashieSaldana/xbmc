# Kodi 21.3 fork: immersive audio detection, badges and stream-details rescan

This is a personal fork of [Kodi 21.3 "Omega"](https://github.com/xbmc/xbmc/tree/Omega) that adds
proper Dolby Atmos / DTS:X handling for local media libraries, plus a way to refresh technical
stream details without re-scraping metadata. It is built and tested on **NVIDIA Shield TV (2017)**
running SHIELD Experience 9 (Android 11), but the code changes are not Shield-specific.

Branch: `feature/immersive-audio-rescan`

## What it adds

### 1. Atmos and DTS:X detection

Kodi 21.x pins FFmpeg 6.0.1, which cannot report the immersive audio profiles. This fork:

- bumps the bundled FFmpeg to 6.1.6, which knows `FF_PROFILE_DTS_HD_MA_X` (DTS:X) and
  `FF_PROFILE_DTS_HD_MA_X_IMAX`, and the E-AC-3 / TrueHD Atmos profiles;
- probes the TrueHD major-sync header directly (`F8 72 6F BA`, four substreams, the
  "16-channel presentation" bit that MediaInfo reads), because FFmpeg's normal probe misses
  some TrueHD Atmos tracks;
- extends the TrueHD probe window for tracks whose audio starts late in the file;
- keeps the work at scan time only — playback opening is unchanged.

Detected codec names: `truehd_atmos`, `eac3_ddp_atmos`, `dtshd_ma_x`, `dtshd_ma_x_imax`.
Legacy streams keep their existing names (`truehd`, `eac3`, `ac3`, `dtshd_ma`, `dts`, ...).

### 2. Badges that separate container from immersive format

The Estuary media-flag row now behaves the way a receiver's display does:

| Stream | Codec badge | Channel badge |
|---|---|---|
| Dolby TrueHD + Atmos | TRUEHD | ATMOS |
| Dolby Digital Plus + Atmos | DD+ | ATMOS |
| DTS-HD Master Audio + DTS:X | DTS-MA | DTS:X |
| DTS-HD MA + DTS:X IMAX | DTS-MA | DTS:X IMAX |
| Dolby TrueHD 5.1/7.1 | TRUEHD | 5.1 / 7.1 |
| Dolby Digital | DD | 5.1 |
| DTS-HD MA / DTS | DTS-MA / DTS | 7.1 / 5.1 |

Atmos and DTS:X therefore appear where the channel layout used to be, and the codec tile always
shows the container. The same mapping is used by the info dialog, library list views and the
`AudioCodecVar` / `AudioChannelsVar` text summaries.

### 3. DTS:X passthrough fix

Kodi's audio sink mapped only profile 60 (DTS-HD MA) to the DTS-HD MA passthrough stream type.
Profiles 61/62 (DTS:X, DTS:X IMAX) fell through to DTS core, so a receiver got plain DTS and
reported DTS instead of DTS:X. Both mapping sites now send DTS:X as DTS-HD MA.

### 4. Rescan stream details

**Settings -> Media -> Library -> Rescan stream details** re-reads video/audio stream information
for every library item from the media files. It does **not** fetch online metadata, artwork or
text, and it leaves titles/plots/fanart untouched. Transactions are batched (commit every 200
files) so other Kodi clients sharing a MySQL library are not blocked by one long transaction.

A full rescan of a ~7,000-file library over SMB takes roughly 25-30 minutes.

## Install

Download the APK from the [Releases page](https://github.com/MashieSaldana/xbmc/releases).

- Architecture: `arm64-v8a` (NVIDIA Shield TV 2017/2019 on SHIELD Experience 9)
- Package id: `org.mashie.kodi` — installs **side by side** with the Play Store Kodi
  (`org.xbmc.kodi`); it does not replace or update it
- Minimum Android: 5.0 (API 21), target SDK 34
- Signed with the standard Android **debug** key. It is a sideload build: if you build your own
  APK, uninstall this one first, because the signatures will not match.

Typical sideload:

```
adb install -r kodiapp-arm64-v8a-release.apk
```

After installing a new APK, launch Kodi from its icon (or
`adb shell monkey -p org.mashie.kodi -c android.intent.category.LAUNCHER 1`). Do not launch
`.Main` directly on Android: the launcher activity is what unpacks the APK's assets, so bypassing
it keeps the previous skin from cache.

## Build

This follows the normal Kodi Omega Android build. The versions that matter:

- Kodi 21.3 `Omega`
- Android NDK **21.4.7075529** (NDK 28c fails in `libcdio` and `gnutls`)
- SDK platform 34, build-tools 34.0.0
- JDK 21 for Gradle

```
cd tools/depends && ./bootstrap
./configure --host=aarch64-linux-android \
  --with-sdk-path=$HOME/android-tools/android-sdk-linux \
  --with-ndk-path=$HOME/android-tools/android-sdk-linux/ndk/21.4.7075529 \
  --prefix=$HOME/android-tools/xbmc-depends-aarch64 --enable-debug=no
make -j$(nproc)

make -C tools/depends/target/cmakebuildsys BUILD_DIR=$HOME/kodi-build-arm64 \
  CMAKE_EXTRA_ARGUMENTS="-DAPP_PACKAGE=org.mashie.kodi"
cd $HOME/kodi-build-arm64 && make -j$(nproc) && make apk
```

`APP_PACKAGE` must be passed through `CMAKE_EXTRA_ARGUMENTS`; it feeds both the Gradle
`applicationId` and the JNI `CompileInfo` package name.

Skin textures are packed into `addons/skin.estuary/media/Textures.xbt`. After changing any PNG in
the Estuary media tree, repack it with the depends `TexturePacker` before `make apk`:

```
$DEPENDS/x86_64-linux-gnu-native/bin/TexturePacker \
  -input addons/skin.estuary/media \
  -output <build>/addons/skin.estuary/media/Textures.xbt -dupecheck
```

## Notes and limitations

- Only the bundled **Estuary** skin is updated. Other skins will keep their own badge artwork;
  the codec names (`truehd_atmos`, ...) are available to them, but they need matching images or
  labels.
- If the library is shared over MySQL, other Kodi clients will see the new codec names too.
  Upgrade those clients as well, or they will show the raw name (e.g. `truehd_atmos`) in skins
  that do not know it.
- The fork is based on the Omega branch; rebasing onto Kodi point releases is manual work.
- No Play Store updates, no official support. Use at your own risk.

## License

Kodi is licensed under the GNU General Public License v2.0 or later. This fork keeps the same
license and is not affiliated with or endorsed by the XBMC Foundation. The corresponding source
for every release is the tagged commit in this repository.