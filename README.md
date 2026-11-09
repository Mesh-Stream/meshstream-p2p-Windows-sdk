# MeshStream P2P Windows SDK

[简体中文](README.zh-CN.md) · [Website](https://meshstream.net/) · [Documentation](https://meshstream.net/docs/sdk/windows-setup/) · [SDK downloads](releases/) · [Live demo](https://demo.meshstream.net/)

MeshStream Windows SDK provides a C / C++ API for P2P video and audio delivery in Windows 10 / 11 x64 desktop apps. It proxies HLS, MPEG-DASH and eligible HTTP Range MP4/MP3 requests through a local playback URL, with HTTP/CDN fallback for your existing media player.

## How it works

Create a session from your media URL, then give its `playbackUrl` to your existing player:

```text
App → SDK playback session → Local playback URL → Player
                              └─ SDK fetches from peers or origin/CDN
```

Your player handles decoding, rendering and playback controls. The SDK handles media delivery, caching and P2P scheduling. Configuration and P2P preparation run in the background; playback need not wait for a peer connection. Stop using the old playback URL before closing its session when leaving the page or switching content.

## Features

- HLS, MPEG-DASH and HTTP Range MP4/MP3 delivery with origin/CDN fallback.
- C ABI and C++ integration through `meshstream.h`.
- Session preload, peer and traffic statistics, status and diagnostics.
- Versioned x64 DLL distribution and a standalone CMake console example.

## Before you start

- Create an application and a platform-specific SDK Key in the [MeshStream console](https://console.meshstream.net/). If application identity validation is enabled, register your actual application identity.
- Have a playable HLS `.m3u8`, DASH `.mpd`, or HTTP Range-capable MP4 / MP3 URL. Prefer HTTPS.
- Choose a player that supports your source format. The SDK adds delivery capabilities, not codecs.

To try it first, jump to [Run the demo](#run-the-demo). The online demo linked above is a Web experience; run this repository's example for this platform.

## Release

The current release is **1.0.10**.

| Item | Value |
| --- | --- |
| Platform | Windows 10 / 11 x64 |
| Public API | C / C++ through `meshstream.h` |
| Runtime | Microsoft Visual C++ 2015-2022 Redistributable (x64) |
| Distribution | Versioned archive in this repository |

Windows ARM64 and x86 are not part of the current published release. Do not
ship the x64 SDK runtime on those architectures.

## Package contents

The versioned release archive is committed at
[`releases/MeshStream-Windows-SDK-1.0.10-win-x64.zip`](releases/MeshStream-Windows-SDK-1.0.10-win-x64.zip)
and contains:

```text
include/meshstream.h
lib/p2pcdn_core.dll
lib/p2pcdn_core.dll.lib
```

Add `include` to the compiler include paths, link `p2pcdn_core.dll.lib`, and
place `p2pcdn_core.dll` next to the application executable.

## Basic integration

Inside your playback owner, create one client for the application scope, then create a playback session
for each media item and pass its URL to the player:

```cpp
#include "meshstream.h"

meshstream_config_t config{};
meshstream_config_default(&config);
config.key = "your_sdk_key";
const char* origin_url = "https://cdn.example.com/video/master.m3u8";

meshstream_client_t* client = nullptr;
int rc = meshstream_client_create(&config, &client);
if (rc != MESHSTREAM_OK) {
    // Record meshstream_result_message(rc). The application may explicitly
    // choose a direct origin fallback for this local setup failure.
    return;
}

meshstream_playback_options_t options{};
options.struct_size = sizeof(options);
options.version = MESHSTREAM_PLAYBACK_OPTIONS_VERSION;
options.content_id = "stable-asset-id";

meshstream_session_t* session = nullptr;
char* playback_url = nullptr;
rc = meshstream_client_create_playback_session(
    client, origin_url, &options, &session, &playback_url);
if (rc != MESHSTREAM_OK) {
    meshstream_client_destroy(client);
    return;
}

// Copy playback_url into the player while the session remains alive.
meshstream_string_free(playback_url);

// When the player no longer uses the session URL:
meshstream_session_destroy(session);
meshstream_client_destroy(client);
```

The returned URL is ready promptly. Configuration and P2P preparation continue
in the background. P2P availability affects acceleration only; it is not a
signal to replace the session URL or stop the player.

Destroy each session when its player item ends, then destroy the client when
the owning application scope finishes.

## Run the demo

Use Visual Studio 2022 with C++ desktop tools and CMake 3.21+. Run in PowerShell, replacing `C:/SDKs/...` with the actual extracted SDK root:

```powershell
git clone https://github.com/Mesh-Stream/meshstream-p2p-Windows-sdk.git
cd meshstream-p2p-Windows-sdk/Examples/ConsoleDemo
cmake -S . -B out -DMESHSTREAM_SDK_ROOT="C:/SDKs/MeshStream-Windows-SDK-1.0.10-win-x64"
cmake --build out --config Release
.\out\Release\MeshStreamConsoleDemo.exe "your_sdk_key" "https://cdn.example.com/master.m3u8" "stable-content-id"
```

The console prints a local playback URL. **Keep the console running** and open that URL in a compatible player on the same computer. Pressing Enter closes the session and invalidates the URL. This C ABI example does not render video or include a statistics panel; read `meshstream_session_get_stats` in your app.

## Supported media paths

The SDK can proxy HLS, DASH, and eligible byte-range MP4/MP3 requests. It does
not decode or render media. Your player must support the media format and play
the SDK-provided URL.

MP4/MP3 acceleration requires origin support for single byte-range requests
with a valid `Content-Range` response. Otherwise playback can continue without
acceleration.

For rotating signed URLs, use a stable `content_id` for each media item. For
extensionless URLs, set `options.mime_type` to `m3u8`, `mpd`, `mp4`, or `mp3`.
Use `options.origin_headers_json` only for origin authentication headers and do
not expose those values in logs or telemetry.

## Observability

Use `meshstream_session_get_stats` for traffic and peer statistics,
`meshstream_session_get_status` for the acceleration policy snapshot, and
`meshstream_session_get_diagnostics` for troubleshooting. These values describe
acceleration state only and must not be used to stop player playback.

## Verify P2P delivery

1. Play the same content in two devices or independently running instances, each with a valid Key. Use the same rendition and nearby playback positions. For rotating signed URLs, use the same stable content ID for the same content, and different IDs for different content.
2. Watch peers and traffic in the demo. A nonzero peer count means a connection exists; **increasing P2P download or upload bytes** confirms media sharing. For the Windows console example, read the statistics API in your application.
3. Close the other instance and check that HTTP/CDN playback continues. Also check seeking, switching content and cleanup when leaving playback.

HTTP/CDN delivery is normal with one viewer or no shareable segments. Discovery and sharing also depend on account configuration, network connectivity and the content cached by both viewers.

## More examples and support

See [advanced usage](https://meshstream.net/docs/sdk/windows-advanced/) for preload, live/VOD scheduling, player feedback, multiple sessions and diagnostics. See [CHANGELOG](CHANGELOG.md) for updates. Contact support@meshstream.net with the SDK version, OS, player and redacted logs.

## Other MeshStream SDKs

- [Web](https://github.com/Mesh-Stream/meshstream-p2p-web-sdk): JavaScript / TypeScript, WebRTC and browser players.
- [iOS / macOS](https://github.com/Mesh-Stream/meshstream-p2p-ios-sdk): MeshStreamKit, Swift / Objective-C, SPM and CocoaPods.
- [Android](https://github.com/Mesh-Stream/meshstream-p2p-android-sdk): Kotlin / Java, Maven and Media3 / ExoPlayer.
- [uni-app](https://github.com/Mesh-Stream/meshstream-p2p-uniapp-sdk): UTS / JavaScript plugin for iOS and Android apps.
- [Flutter](https://github.com/Mesh-Stream/meshstream-p2p-flutter-sdk): Dart plugin for iOS / Android with video_player examples.
- [React Native](https://github.com/Mesh-Stream/meshstream-p2p-react-native-sdk): TypeScript API for iOS / Android with react-native-video examples.
