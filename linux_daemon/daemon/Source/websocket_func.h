#pragma once

#include "common.h"

/* WebSocketハンドシェイク処理main */
BOOL ws_handshake_main(SOCKET soc);

/* WebSocketデータフレーム作成 */
int make_websocket_dataframe(BYTE* buf, const BYTE* data, size_t data_len, BOOL textFlag);

/* WebSocketデータフレーム定数 */
#define  WEBSOCKET_PAYLOAD_DATA_MAX         125
#define  WEBSOCKET_DATA_FLAME_HEADER_SIZE     2
#define  WEBSOCKET_DATA_FLAME_MAX           (WEBSOCKET_DATA_FLAME_HEADER_SIZE + WEBSOCKET_PAYLOAD_DATA_MAX)
