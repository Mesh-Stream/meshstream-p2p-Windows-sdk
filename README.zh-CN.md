# MeshStream Windows SDK

[English](README.md)

[发布维护契约](RELEASE_CONTRACT.md)

MeshStream Windows SDK 通过 C/C++ 接口提供与 CDN 兼容的桌面播放地址，并在可用时
启用 P2P 分发。应用继续负责播放器选择、解码、渲染和播放生命周期。

## MeshStream SDKs

- [在线 Demo](https://demo.meshstream.net/)：查看已部署的 Web 播放和 P2P 分发效果。
- [Web Demo](https://github.com/Mesh-Stream/meshstream-p2p-web-sdk)：使用 npm 发布的 Web SDK。
- [Android SDK](https://github.com/Mesh-Stream/meshstream-p2p-android-sdk)：适用于 Media3 / ExoPlayer。
- [Apple SDK](https://github.com/Mesh-Stream/meshstream-p2p-ios-sdk)：适用于 iOS 和 macOS。
- [Windows SDK](https://github.com/Mesh-Stream/meshstream-p2p-Windows-sdk)：适用于 C/C++ 桌面应用。

## 当前发布

当前版本为 `1.0.0`，支持 Windows 10/11 x64，使用 C/C++ 公共头文件
`meshstream.h`。请安装 Microsoft Visual C++ 2015-2022 Redistributable（x64）。

仓库内的版本包位于：
[`releases/MeshStream-Windows-SDK-1.0.0-win-x64.zip`](releases/MeshStream-Windows-SDK-1.0.0-win-x64.zip)

包内包含：

```text
include/meshstream.h
lib/p2pcdn_core.dll
lib/p2pcdn_core.dll.lib
```

将 `include` 加入编译器头文件搜索路径，链接导入库，并把 DLL 放在应用可执行文件
同级目录。

## 快速接入

用 SDK Key 创建客户端，为每个媒体条目创建播放会话，再把返回的地址交给播放器：

```cpp
meshstream_config_t config{};
meshstream_config_default(&config);
config.key = "your_sdk_key";

meshstream_client_t* client = nullptr;
meshstream_client_create(&config, &client);

meshstream_session_t* session = nullptr;
char* playback_url = nullptr;
meshstream_client_create_playback_session(
    client, origin_url, nullptr, &session, &playback_url);
```

SDK 在后台完成服务配置与 P2P 准备。加速暂不可用时，媒体仍应通过源站/CDN 继续
请求，业务播放器不应因为 P2P 状态而停止。播放结束后销毁会话；应用不再使用 SDK
时销毁客户端。

## 示例与支持

[`Examples/ConsoleDemo`](Examples/ConsoleDemo) 是独立的 CMake 示例，会输出可交给
播放器的播放地址。版本校验信息位于 [`releases/`](releases)。接入、ABI、授权或
分发问题请联系 support@meshstream.net。
