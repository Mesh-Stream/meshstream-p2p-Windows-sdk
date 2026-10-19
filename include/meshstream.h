#pragma once

// Customer-facing MeshStream Windows SDK C ABI.

#include <stdint.h>

#ifdef _WIN32
#  define MESHSTREAM_API __declspec(dllimport)
#else
#  define MESHSTREAM_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum {
  MESHSTREAM_OK = 0,
  MESHSTREAM_INVALID_ARGUMENT = 1,
  MESHSTREAM_RUNTIME_ERROR = 2,
  MESHSTREAM_CREATE_PLAYBACK_SESSION_FAILED = 3,
  MESHSTREAM_PRELOAD_SKIPPED = 4,
  MESHSTREAM_PLAYBACK_OPTIONS_VERSION = 2
};

typedef enum meshstream_unavailable_reason_t {
  MESHSTREAM_REASON_OK = 0,
  MESHSTREAM_REASON_NO_PEERS = 1,
  MESHSTREAM_REASON_TRACKER_ERROR = 2,
  MESHSTREAM_REASON_AUTH_FAILED = 3,
  MESHSTREAM_REASON_ACCOUNT_DISABLED = 4,
  MESHSTREAM_REASON_APP_DISABLED = 5,
  MESHSTREAM_REASON_APP_NOT_FOUND = 6,
  MESHSTREAM_REASON_KEY_DISABLED = 7,
  MESHSTREAM_REASON_PLATFORM_DISABLED = 8,
  MESHSTREAM_REASON_ORIGIN_DENIED = 9,
  MESHSTREAM_REASON_PACKAGE_DENIED = 10,
  MESHSTREAM_REASON_BUNDLE_DENIED = 11,
  MESHSTREAM_REASON_SIGNING_CERT_DENIED = 12,
  MESHSTREAM_REASON_TEAM_DENIED = 13,
  MESHSTREAM_REASON_CONFIG_UNAVAILABLE = 14,
  MESHSTREAM_REASON_INITIALIZATION_FAILED = 15,
  MESHSTREAM_REASON_UNSUPPORTED = 16,
  MESHSTREAM_REASON_DISABLED = 17,
  MESHSTREAM_REASON_KEY_P2P_DISABLED = 18,
  MESHSTREAM_REASON_QUOTA_EXHAUSTED = 19,
  MESHSTREAM_REASON_QUOTA_DISABLED = 20
} meshstream_unavailable_reason_t;

typedef enum meshstream_playback_mode_t {
  MESHSTREAM_PLAYBACK_NATIVE_P2P = 0,
  MESHSTREAM_PLAYBACK_NATIVE_CDN = 1
} meshstream_playback_mode_t;

typedef struct meshstream_client_t meshstream_client_t;
typedef struct meshstream_session_t meshstream_session_t;

typedef struct meshstream_playback_options_t {
  // Set struct_size to sizeof(*this) and version to
  // MESHSTREAM_PLAYBACK_OPTIONS_VERSION. Future SDKs append fields.
  uint32_t struct_size;
  uint32_t version;
  const char* content_id;
  const char* mime_type;
  // JSON object of headers sent only to the origin.
  const char* origin_headers_json;
} meshstream_playback_options_t;

typedef struct meshstream_config_t {
  const char* key;
  int32_t log_level;
} meshstream_config_t;

typedef struct meshstream_stats_t {
  uint64_t http_downloaded;
  uint64_t p2p_downloaded;
  uint64_t p2p_uploaded;
  uint64_t http_segment_download_successes;
  uint64_t http_segment_download_failures;
  uint64_t p2p_segment_download_successes;
  uint64_t p2p_segment_download_failures;
  uint64_t p2p_segment_upload_successes;
  uint64_t p2p_segment_upload_failures;
  uint64_t connected_peers;
  uint64_t peer_connect_errors;
  uint64_t tracker_errors;
} meshstream_stats_t;

typedef struct meshstream_status_t {
  int32_t account_enabled;
  int32_t application_enabled;
  int32_t key_enabled;
  int32_t platform_enabled;
  int32_t effective_p2p_enabled;
  int32_t tracker_pool_ready;
  meshstream_playback_mode_t playback_mode;
  meshstream_unavailable_reason_t reason;
} meshstream_status_t;

typedef struct meshstream_diagnostics_t {
  uint64_t fallback_count;
  uint64_t first_peer_connect_ms;
  uint64_t tracker_errors;
  uint64_t peer_connect_errors;
  uint64_t ice_attempts;
  uint64_t ice_successes;
  uint64_t ice_failures;
  uint64_t ice_duration_ms;
} meshstream_diagnostics_t;

MESHSTREAM_API const char* meshstream_version(void);
MESHSTREAM_API const char* meshstream_result_message(int32_t code);
MESHSTREAM_API int32_t meshstream_config_default(meshstream_config_t* out_config);
/* Creates a local client without bootstrap, Tracker, ICE, or token I/O.
 * meshstream_client_create_playback_session returns a CDN-safe URL immediately and
 * resolves P2P configuration in the background. */
MESHSTREAM_API int32_t meshstream_client_create(
    const meshstream_config_t* config,
    meshstream_client_t** out_client);
MESHSTREAM_API void meshstream_client_destroy(meshstream_client_t* client);
/* Optional best-effort rolling files. Absolute private directory; NULL disables. */
MESHSTREAM_API int32_t meshstream_client_configure_file_logging(meshstream_client_t* client, const char* directory);
/* event is a stable call-site ID: 1..128 ASCII letters/digits or . _ - : /.
 * Invalid IDs return MESHSTREAM_INVALID_ARGUMENT. Repeated errors are limited per event. */
MESHSTREAM_API int32_t meshstream_client_log(meshstream_client_t* client, int32_t level, const char* event, const char* message);
MESHSTREAM_API int32_t meshstream_client_get_status_json(
    const meshstream_client_t* client,
    char** out_json);
MESHSTREAM_API int32_t meshstream_client_create_playback_session(
    meshstream_client_t* client,
    const char* origin_url,
    const meshstream_playback_options_t* options,
    meshstream_session_t** out_session,
    char** out_playback_url);
MESHSTREAM_API const char* meshstream_session_get_playback_url(
    const meshstream_session_t* session);
// Schedules a non-blocking HLS/DASH manifest and first-segment warmup.
// Returns MESHSTREAM_PRELOAD_SKIPPED for direct media or an already-running
// warmup. A session pointer is invalid after meshstream_session_destroy.
MESHSTREAM_API int32_t meshstream_session_preload(const meshstream_session_t* session);
MESHSTREAM_API int32_t meshstream_session_is_live(const meshstream_session_t* session);
MESHSTREAM_API int32_t meshstream_session_is_playback_mode_resolved(const meshstream_session_t* session);
MESHSTREAM_API int32_t meshstream_session_get_stats(
    const meshstream_session_t* session,
    meshstream_stats_t* out_stats);
MESHSTREAM_API int32_t meshstream_session_get_status(
    const meshstream_session_t* session,
    meshstream_status_t* out_status);
MESHSTREAM_API int32_t meshstream_session_get_diagnostics(
    const meshstream_session_t* session,
    meshstream_diagnostics_t* out_diagnostics);
MESHSTREAM_API int32_t meshstream_session_set_p2p_enabled(
    meshstream_session_t* session,
    int32_t enabled);
MESHSTREAM_API int32_t meshstream_session_update_buffered_duration_ms(
    meshstream_session_t* session,
    uint64_t buffered_duration_ms);
MESHSTREAM_API int32_t meshstream_session_update_current_position_ms(
    meshstream_session_t* session,
    uint64_t current_position_ms);
// Optional live telemetry. Pass the player's current distance from the live
// edge in milliseconds; values outside 0..600000 are ignored.
MESHSTREAM_API int32_t meshstream_session_update_live_edge_offset_ms(
    meshstream_session_t* session,
    uint64_t live_edge_offset_ms);
// Releases the session. Do not use this pointer with any API afterwards.
MESHSTREAM_API void meshstream_session_destroy(meshstream_session_t* session);
MESHSTREAM_API void meshstream_string_free(char* value);

#ifdef __cplusplus
}
#endif
