# Changelog

All notable changes to the public MeshStream Windows SDK distribution are
documented in this file.

## 1.0.7 - 2026-11-01

- Updates the Windows native runtime to the shared MeshStream SDK 1.0.7 release.
- Refreshes the x64 DLL, import library, public header, and ConsoleDemo validation package.

## 1.0.4 - 2026-10-25

- Aligns the Windows native SDK version with the Android and Apple 1.0.4 releases.
- Renames the version query API to `meshstream_get_sdk_version` for the shared
  `getSdkVersion` product contract. Replace the pre-release
  `meshstream_version` call when migrating from 1.0.0.
- Refreshes the ConsoleDemo and the versioned x64 release archive.

## 1.0.0 - 2026-10-18

- First public Windows x64 release of the C/C++ SDK.
- Provides independent client and playback-session lifecycles.
- Supports HLS, DASH, and eligible byte-range MP4/MP3 delivery through a
  local playback URL, with CDN continuity when acceleration is unavailable.
- Includes a CMake console integration example.
