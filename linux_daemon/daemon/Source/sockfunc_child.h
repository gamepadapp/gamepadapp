#pragma once

#include "common.h"

/* データ送信処理 指定サイズ送信 */
BOOL exec_send(SOCKET soc, const BYTE* buf, size_t sendLen);

/* データ受信処理 指定サイズ受信 */
BOOL exec_recv(SOCKET soc, BYTE* buf, size_t requireLen);
/* データ受信処理 CRLFCRLFまで */
BOOL exec_recv_crlfcrlf(SOCKET soc, BYTE* buf, size_t maxRecvLen, size_t* pReceivedLen);
