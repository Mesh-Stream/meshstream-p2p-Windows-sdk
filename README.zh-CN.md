# MeshStream P2P Windows SDK

[English](README.md) · [官网](https://meshstream.net/zh/) · [接入文档](https://meshstream.net/zh/docs/sdk/windows-setup/) · [SDK 下载](releases/) · [在线 Demo](https://demo.meshstream.net/)

MeshStream Windows SDK 面向 Windows 10 / 11 x64 桌面应用，通过 C / C++ API 提供 P2P 视频与音频分发。它通过本地播放地址代理 HLS、MPEG-DASH 及符合 HTTP Range 要求的 MP4/MP3 请求，支持现有播放器接入和 HTTP/CDN 回退。

## 工作方式

把原始媒体地址交给 SDK 创建会话，再把会话的 `playbackUrl` 交给现有播放器：

```text
应用 → SDK 创建播放会话 → 本地播放地址 → 播放器
                            └─ SDK 从 Peer 或源站/CDN 获取媒体
```

播放器继续负责解码、画面和播放控制；SDK 负责媒体传输、缓存及 P2P 调度。服务配置与 P2P 准备在后台执行，不需要等到 Peer 连接后才开始播放。离开播放页或切换内容时，先停止使用旧播放地址，再关闭旧会话。

## 产品能力

- 支持 HLS、MPEG-DASH、HTTP Range MP4/MP3 及源站/CDN 回退。
- 通过 `meshstream.h` 提供 C ABI 与 C++ 接入方式。
- 提供会话预加载、Peer 与流量统计、状态和诊断接口。
- 提供带版本号的 x64 DLL 及独立 CMake 控制台示例。

## 接入准备

- 在 [MeshStream 控制台](https://console.meshstream.net/)创建应用和对应平台的 SDK Key；启用身份校验时，登记实际应用标识。
- 准备一个可播放的 HLS `.m3u8`、DASH `.mpd`、支持 Range 的 MP4 或 MP3 地址，建议使用 HTTPS。
- 选择支持该媒体格式的播放器。SDK 提供传输能力，不增加播放器的解码能力。

希望先看效果？直接跳到[运行 Demo](#运行-demo)，再按下方步骤接入自己的应用。顶部的在线 Demo 是 Web 体验页，各端示例在本仓库中运行。

## 当前发布

当前版本为 `1.0.10`，支持 Windows 10/11 x64，使用 C/C++ 公共头文件
`meshstream.h`。请安装 Microsoft Visual C++ 2015-2022 Redistributable（x64）。

## 安装

下载并解压 [`MeshStream-Windows-SDK-1.0.10-win-x64.zip`](releases/MeshStream-Windows-SDK-1.0.10-win-x64.zip)，包内包含：

```text
include/meshstream.h
lib/p2pcdn_core.dll
lib/p2pcdn_core.dll.lib
```

将 `include` 加入头文件搜索路径，链接 `p2pcdn_core.dll.lib`，并把 `p2pcdn_core.dll` 放在应用可执行文件同级目录。版本及 SHA-256 见 [`releases/`](releases/)。当前发行包面向 x64；不提供 Windows ARM64 或 x86 组件。

## 快速接入

以下为应用内部的接入流程片段。创建客户端和会话，把返回地址复制给播放器；播放结束后再释放会话与客户端。

```cpp
#include "meshstream.h"

meshstream_config_t config{};
meshstream_config_default(&config);
config.key = "your_sdk_key";
const char* origin_url = "https://cdn.example.com/video/master.m3u8";

meshstream_client_t* client = nullptr;
int rc = meshstream_client_create(&config, &client);
if (rc != MESHSTREAM_OK) {
    // 记录 meshstream_result_message(rc)，并处理初始化失败。
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

// 将 playback_url 复制给播放器；播放期间保持 session 存活。
meshstream_string_free(playback_url);

// 播放器不再使用会话地址后：
meshstream_session_destroy(session);
meshstream_client_destroy(client);
```

释放字符串不等于关闭会话；播放器仍在读取本地地址时，必须保持会话和客户端存活。P2P 准备在后台执行，加速状态变化时无需替换播放地址或停止播放器。

## 运行 Demo

需要 Visual Studio 2022（C++ 桌面开发工具）和 CMake 3.21+。在 PowerShell 中执行；将 `C:/SDKs/...` 替换为 SDK 解压后的实际根目录：

```powershell
git clone https://github.com/Mesh-Stream/meshstream-p2p-Windows-sdk.git
cd meshstream-p2p-Windows-sdk/Examples/ConsoleDemo
cmake -S . -B out -DMESHSTREAM_SDK_ROOT="C:/SDKs/MeshStream-Windows-SDK-1.0.10-win-x64"
cmake --build out --config Release
.\out\Release\MeshStreamConsoleDemo.exe "your_sdk_key" "https://cdn.example.com/master.m3u8" "stable-content-id"
```

控制台输出本地播放地址。**保持控制台运行**，在同一台电脑的兼容播放器中打开该地址；按 Enter 会结束会话，地址随后失效。这个示例展示 C ABI 接入，不内置视频窗口或统计面板；应用可用 `meshstream_session_get_stats` 读取统计。

## 格式、统计与诊断

SDK 代理 HLS、DASH 和支持单段字节范围请求、有效 `Content-Range` 的 MP4/MP3，实际解码由播放器完成。签名 URL 变化时使用稳定的 `options.content_id`；无后缀地址通过 `options.mime_type` 声明 `m3u8`、`mpd`、`mp4` 或 `mp3`。

用 `meshstream_session_get_stats` 获取 Peer 与流量，用 `meshstream_session_get_status` 查看加速策略，用 `meshstream_session_get_diagnostics` 排查问题。状态变化只影响加速，不应据此停止正常播放。

## 确认 P2P 生效

1. 在两台设备或两个可同时运行的实例中，用各自有效的 Key 播放同一内容，保持相同清晰度和相近播放进度。签名 URL 会变化时，为同一内容使用相同、稳定的内容 ID；不同内容不要共用 ID。
2. 查看 Demo 的 Peer 数和流量统计。Peer 大于 0 表示已连接节点；**P2P 下载或上传字节持续增加**才说明发生了媒体共享。Windows 控制台示例需自行读取统计接口。
3. 关闭另一个实例，确认剩余实例仍能通过 HTTP/CDN 播放；然后测试拖动、切换内容和退出页面的释放流程。

只有一个观看者或没有可共享分片时，主要走 HTTP/CDN 属于正常情况。发现 Peer 及实际共享还取决于账户配置、网络连通性和双方缓存内容。

## 更多用法与支持

预加载、直播/点播调度、播放器状态回传、多会话及诊断，请参阅[高级用法](https://meshstream.net/zh/docs/sdk/windows-advanced/)。更新记录见 [CHANGELOG](CHANGELOG.md)。接入问题请联系 support@meshstream.net，并附 SDK 版本、系统、播放器和脱敏日志。

## 其他 MeshStream SDK

- [Web](https://github.com/Mesh-Stream/meshstream-p2p-web-sdk): JavaScript / TypeScript、WebRTC 与浏览器播放器.
- [iOS / macOS](https://github.com/Mesh-Stream/meshstream-p2p-ios-sdk): MeshStreamKit、Swift / Objective-C、SPM 与 CocoaPods.
- [Android](https://github.com/Mesh-Stream/meshstream-p2p-android-sdk): Kotlin / Java、Maven 与 Media3 / ExoPlayer.
- [uni-app](https://github.com/Mesh-Stream/meshstream-p2p-uniapp-sdk): 面向 iOS / Android App 的 UTS / JavaScript 插件.
- [Flutter](https://github.com/Mesh-Stream/meshstream-p2p-flutter-sdk): 面向 iOS / Android 的 Dart 插件及 video_player 示例.
- [React Native](https://github.com/Mesh-Stream/meshstream-p2p-react-native-sdk): 面向 iOS / Android 的 TypeScript API 及 react-native-video 示例.
