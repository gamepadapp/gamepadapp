#include "socket_headers.h"
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#include "websocket_func.h"
#include "sockfunc_child.h"

/* 受け付けるプロトコル名 */
#define  WEBSOCKET_PROTOCOL_NAME    "netcontroller"
/* 必須バージョン */
#define  WEBSOCKET_REQUIRE_VERSION  13

#define  INFO_WEBSOCKET_VERSION   "Sec-WebSocket-Version: "
#define  INFO_WEBSOCKET_KEY       "Sec-WebSocket-Key: "
#define  INFO_WEBSOCKET_PROTOCOL  "Sec-WebSocket-Protocol: "

#define  MAXSIZE_SEC_WEBSOCKET_VERSION    16
#define  MAXSIZE_SEC_WEBSOCKET_KEY        32
#define  MAXSIZE_SEC_WEBSOCKET_PROTOCOL   32

typedef struct {
	char sec_websocket_version[MAXSIZE_SEC_WEBSOCKET_VERSION];             // 接続プロトコルのバージョン
	char sec_websocket_key[MAXSIZE_SEC_WEBSOCKET_KEY];                     // 認証用キー
	char sec_websocket_protocol[MAXSIZE_SEC_WEBSOCKET_PROTOCOL];           // プロトコル指定
} ws_handshake_t;

static BOOL make_ws_handshake_info(char* strHandshake, ws_handshake_t* handshake);
static BOOL check_ws_handshake_info(ws_handshake_t* handshake);
static void make_ws_response(const char* sec_websocket_key, char* buf, size_t bufSize);

/* WebSocketハンドシェイク処理main */
BOOL ws_handshake_main(SOCKET soc) {
	const int SIZE_BUF_HANDSHAKE = 1024;
	const int SIZE_BUF_RESPONSE  =  256;

	BOOL rtn;

	// WebSocketハンドシェイク受信
	BYTE bufHandshake[SIZE_BUF_HANDSHAKE];
	size_t receivedLen;
	rtn = exec_recv_crlfcrlf(soc, bufHandshake, SIZE_BUF_HANDSHAKE - 1, &receivedLen);
	if (!rtn) {
		return FALSE;
	}
	char* strHandshake = (char*)bufHandshake;
	strHandshake[receivedLen] = '\0';

	// ハンドシェイク情報の作成
	ws_handshake_t handshake;
	memset(&handshake, '\0', sizeof(ws_handshake_t));
	rtn = make_ws_handshake_info(strHandshake, &handshake);
	if (!rtn) {
		return FALSE;
	}

	// ハンドシェイク情報のチェック
	rtn = check_ws_handshake_info(&handshake);
	if (!rtn) {
		return FALSE;
	}

	// ハンドシェイク応答の作成
	char response_buf[SIZE_BUF_RESPONSE];
	make_ws_response(handshake.sec_websocket_key, response_buf, SIZE_BUF_RESPONSE);
	// 応答を送信
	rtn = exec_send(soc, (const BYTE*)response_buf, strlen(response_buf));
	if (!rtn) {
		return FALSE;
	}

	return TRUE;
}

static BOOL copy_info_value(char* strHandshake, char* infoStrBuf, size_t bufSize, const char* infoKeyStr);

/* ハンドシェイク情報の作成 */
static BOOL make_ws_handshake_info(char* strHandshake, ws_handshake_t* handshake) {
	BOOL rtn;

	// 接続プロトコルのバージョン
	rtn = copy_info_value(strHandshake, handshake->sec_websocket_version, MAXSIZE_SEC_WEBSOCKET_VERSION, INFO_WEBSOCKET_VERSION);
	if (!rtn) {
		return FALSE;
	}

	// 認証用キー
	rtn = copy_info_value(strHandshake, handshake->sec_websocket_key, MAXSIZE_SEC_WEBSOCKET_KEY, INFO_WEBSOCKET_KEY);
	if (!rtn) {
		return FALSE;
	}

	// プロトコル指定
	rtn = copy_info_value(strHandshake, handshake->sec_websocket_protocol, MAXSIZE_SEC_WEBSOCKET_PROTOCOL, INFO_WEBSOCKET_PROTOCOL);
	if (!rtn) {
		return FALSE;
	}

	return TRUE;
}

/* 情報の値をコピー */
static BOOL copy_info_value(char* strHandshake, char* infoStrBuf, size_t bufSize, const char* infoKeyStr) {

	char* infoPos = strstr(strHandshake, infoKeyStr);
	if (infoPos == NULL) {
		return FALSE;
	}
	char* infoEnd = strchr(infoPos, '\r');
	if (infoEnd == NULL) {
		return FALSE;
	}

	*infoEnd = '\0';

	// 認証用キーのコピー
	infoPos += strlen(infoKeyStr);
	if (strlen(infoPos) > (bufSize - 1)) {
		return FALSE;
	}
	strcpy(infoStrBuf, infoPos);

	*infoEnd = '\r';

	return TRUE;
}

/* ハンドシェイク情報のチェック */
static BOOL check_ws_handshake_info(ws_handshake_t* handshake) {
	const char* checkValue;

	// 接続プロトコルのバージョン
	checkValue = handshake->sec_websocket_version;
	int ver = atoi(checkValue);
	if (ver < WEBSOCKET_REQUIRE_VERSION) {
		return FALSE;
	}

	// プロトコル指定
	checkValue = handshake->sec_websocket_protocol;
	char* pos = strstr(checkValue, WEBSOCKET_PROTOCOL_NAME);
	if (pos == NULL) {
		return FALSE;
	}

	return TRUE;
}

static void calc_accept_field(const char* sec_websocket_key, char* buf, size_t bufSize);

/* ハンドシェイク応答の作成 */
static void make_ws_response(const char* sec_websocket_key, char* buf, size_t bufSize) {
	const int ACCEPT_BUFSIZE = 32;
	const char* RESPONSE_LINE_1 = "HTTP/1.1 101 Switching Protocols";
	const char* RESPONSE_LINE_2 = "Upgrade: websocket";
	const char* RESPONSE_LINE_3 = "Connection: Upgrade";
	const char* RESPONSE_LINE_4 = "Sec-WebSocket-Accept: ";
	const char* RESPONSE_LINE_5 = "Sec-WebSocket-Protocol: ";

	// 認証用キーからACCEPT情報を生成
	char accept_buf[ACCEPT_BUFSIZE];
	memset(accept_buf, '\0', ACCEPT_BUFSIZE);
	calc_accept_field(sec_websocket_key, accept_buf, ACCEPT_BUFSIZE);

	// 複数行の応答文字列を作成
	snprintf(buf, bufSize, "%s\r\n%s\r\n%s\r\n%s%s\r\n%s%s\r\n\r\n", RESPONSE_LINE_1, RESPONSE_LINE_2, RESPONSE_LINE_3,
																				RESPONSE_LINE_4, accept_buf,
																				RESPONSE_LINE_5, WEBSOCKET_PROTOCOL_NAME);
}

/* 認証用キーからACCEPT情報を生成 */
static void calc_accept_field(const char* sec_websocket_key, char* buf, size_t bufSize) {

	// WebSocketキー付加文字列
	const char* ADD_KEY_STRING = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	const int LEN_ADD_KEY_STRING = 36;
	char combineKey[MAXSIZE_SEC_WEBSOCKET_KEY + LEN_ADD_KEY_STRING];
	strcpy(combineKey, sec_websocket_key);
	strcat(combineKey, ADD_KEY_STRING);

	// SHA1
	BYTE md[SHA_DIGEST_LENGTH];
	{
		SHA_CTX sha_ctx;
		SHA1_Init(&sha_ctx);
		SHA1_Update(&sha_ctx, combineKey, strlen(combineKey));
		SHA1_Final(md, &sha_ctx);
	}

	// BASE64
	{
		BIO* b64 = BIO_new(BIO_f_base64());
		BIO* bmem = BIO_new(BIO_s_mem());

		b64 = BIO_push(b64, bmem);
		BIO_write(b64, md, SHA_DIGEST_LENGTH);
		(void)BIO_flush(b64);

		BUF_MEM *bptr;
		BIO_get_mem_ptr(b64, &bptr);
		memcpy(buf, bptr->data, bptr->length -1);

		BIO_free_all(b64);
	}
}

/* WebSocketデータフレーム作成 */
int make_websocket_dataframe(BYTE* buf, const BYTE* data, size_t data_len, BOOL textFlag) {

	BYTE header_fin_opcode = 0x00;
	BYTE header_mask_length = 0x00;

	// FINフラグ: ON
	header_fin_opcode |= (0x01 << 7);
	// opcode: バイナリデータ or テキストデータ
	header_fin_opcode |= (textFlag ? 0x01 : 0x02);

	// Payloadデータマスク: マスクしない
	// Payloadデータ長
	header_mask_length |= data_len;

	// データフレーム作成
	buf[0] = header_fin_opcode;
	buf[1] = header_mask_length;
	memcpy(&buf[2], data, data_len);

	// 作成済みバイト数
	return (data_len + WEBSOCKET_DATA_FLAME_HEADER_SIZE);
}
