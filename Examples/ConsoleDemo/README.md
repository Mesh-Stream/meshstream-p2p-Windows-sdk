# CMake Console Demo

This example creates a playback session and prints the SDK playback URL. It
does not embed a media renderer. Pass the printed URL to VLC, libVLC, mpv, or
your own Windows player.

Build it with Visual Studio 2022 or recent Visual C++ Build Tools:

```powershell
cmake -S . -B out -DMESHSTREAM_SDK_ROOT="C:/SDKs/MeshStream-Windows-SDK-1.0.0-win-x64"
cmake --build out --config Release
```

The post-build step copies `p2pcdn_core.dll` next to the executable:

```powershell
.\out\Release\MeshStreamConsoleDemo.exe "your_sdk_key" "https://cdn.example.com/master.m3u8" "stable-content-id"
```
