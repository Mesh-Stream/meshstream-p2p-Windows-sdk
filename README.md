# MeshStream Windows SDK

MeshStream Windows SDK provides a CDN-compatible playback URL for desktop
players. It adds P2P delivery opportunistically while preserving media
continuity. Applications continue to own player selection, rendering, and
playback lifecycle.

## Release

The current release is **1.0.0**.

| Item | Value |
| --- | --- |
| Platform | Windows 10 / 11 x64 |
| Public API | C / C++ through `meshstream.h` |
| Runtime | Microsoft Visual C++ 2015-2022 Redistributable (x64) |
| Distribution | GitHub Releases |

Windows ARM64 and x86 are not part of the current published release. Do not
ship the x64 SDK runtime on those architectures.

## Package contents

The versioned release archive contains:

```text
include/meshstream.h
lib/p2pcdn_core.dll
lib/p2pcdn_core.dll.lib
```

Add `include` to the compiler include paths, link `p2pcdn_core.dll.lib`, and
place `p2pcdn_core.dll` next to the application executable.

## Basic integration

Create one client for the application scope, then create a playback session
for each media item and pass its URL to the player:

```cpp
#include "meshstream.h"

meshstream_config_t config{};
meshstream_config_default(&config);
config.key = "your_sdk_key";

meshstream_client_t* client = nullptr;
meshstream_client_create(&config, &client);

meshstream_playback_options_t options{};
options.struct_size = sizeof(options);
options.version = MESHSTREAM_PLAYBACK_OPTIONS_VERSION;
options.content_id = "stable-asset-id";

meshstream_session_t* session = nullptr;
char* playback_url = nullptr;
if (meshstream_client_create_playback_session(
        client, origin_url, &options, &session, &playback_url) == MESHSTREAM_OK) {
    // Give playback_url to VLC, libVLC, mpv, or your own player.
    meshstream_string_free(playback_url);
}
```

The returned URL is ready promptly. Configuration and P2P preparation continue
in the background. When acceleration is unavailable, media delivery continues
through the configured origin/CDN path.

Destroy each session when its player item ends, then destroy the client when
the owning application scope finishes.

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

## Example

[`Examples/ConsoleDemo`](Examples/ConsoleDemo) is a standalone CMake project.
It locates the SDK through `MESHSTREAM_SDK_ROOT`, copies the runtime DLL after
build, and prints the playback URL for your chosen player.

## Release verification

Expected files and SHA-256 digests are recorded under [`releases/`](releases).
Verify the downloaded archive before distributing it with an application.

## Support

For Windows ABI, distribution, licensing, or integration questions, contact
support@meshstream.net.
