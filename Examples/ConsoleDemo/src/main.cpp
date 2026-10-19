#include <cstdlib>
#include <iostream>

#include "meshstream.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: MeshStreamConsoleDemo <sdk-key> <origin-url> [content-id]\\n";
        return EXIT_FAILURE;
    }

    meshstream_config_t config{};
    meshstream_config_default(&config);
    config.key = argv[1];

    meshstream_client_t* client = nullptr;
    int32_t result = meshstream_client_create(&config, &client);
    if (result != MESHSTREAM_OK) {
        std::cerr << "Client creation failed: " << meshstream_result_message(result) << '\\n';
        return EXIT_FAILURE;
    }

    meshstream_playback_options_t options{};
    options.struct_size = sizeof(options);
    options.version = MESHSTREAM_PLAYBACK_OPTIONS_VERSION;
    options.content_id = argc >= 4 ? argv[3] : nullptr;

    meshstream_session_t* session = nullptr;
    char* playback_url = nullptr;
    result = meshstream_client_create_playback_session(client, argv[2], &options, &session, &playback_url);
    if (result != MESHSTREAM_OK) {
        std::cerr << "Session creation failed: " << meshstream_result_message(result) << '\\n';
        meshstream_client_destroy(client);
        return EXIT_FAILURE;
    }

    std::cout << "SDK version: " << meshstream_version() << "\\n";
    std::cout << "Playback URL: " << playback_url << "\\n";
    std::cout << "Pass this URL to your media player. Press Enter to stop.\\n";
    meshstream_string_free(playback_url);
    std::cin.get();

    meshstream_session_destroy(session);
    meshstream_client_destroy(client);
    return EXIT_SUCCESS;
}
