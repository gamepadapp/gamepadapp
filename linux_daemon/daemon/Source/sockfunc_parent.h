#pragma once

#include "common.h"

/* サーバソケットの準備 */
BOOL init_server_socket(const char* pPortAccept, SOCKET* soc);
/* 接続受付可能チェック */
BOOL check_enable_accept(SOCKET soc);
