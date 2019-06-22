#include "socket_headers.h"

#include "socket_child.h"
#include "sockfunc_child.h"
#include "websocket_func.h"
#include "syslog_output.h"
#include "mutex_data.h"

/* ヘッダメッセージ */
#define HEADER_MSG_SMPHONE   "ThisIsNetControllerSMPhoneHeader"
#define HEADER_MSG_BROWSER   "GET / HTTP/1.1\r\n"
#define RETURN_MSG_HEADER_OK "HEADER_OK"
/* ヘッダメッセージサイズ */
#define SIZE_HEADER_MSG_SMPHONE    32
#define SIZE_HEADER_MSG_BROWSER    16
#define SIZE_RETURN_MSG_HEADER_OK   9
/* 入力データチェックキャラクタ */
#define CHECK_KEYINPUT_CHARACTER 'Z'

/* クライアントソケットshutdown,close*/
static void socket_shutdown(SOCKET soc);
/* クライアントとヘッダ送受信 */
static BOOL header_exchange_with_client(SOCKET soc);
/* スマートフォンクライアント処理main */
static void mode_smartphone_main(SOCKET soc);
/* ブラウザクライアント処理main */
static void mode_webbrowser_main(SOCKET soc);

/* 接続中クライアント種別 */
#define MODE_SMPHONE 0
#define MODE_BROWSER 1
static int _client_mode;

/* 子プロセス処理メイン */
void socket_child_main(SOCKET soc)
{
	BOOL rtn;

	// クライアントとヘッダ送受信
	rtn = header_exchange_with_client(soc);
	if (!rtn) {
		// クライアントソケットshutdown,close
		socket_shutdown(soc);
		// ログ出力
		syslog_output(LOG_DEBUG, "<%d> data_exchange_with_server() failed", getpid());
		syslog_output(LOG_NOTICE, "<%d> child process NG", getpid());
		return;
	}

	// 各クライアントごとの処理
	if (_client_mode == MODE_SMPHONE) {
		// スマートフォン接続中チェックとフラグセット
		BOOL changed = setSMPhoneFlag(TRUE);
		if (!changed) {
			// クライアントソケットshutdown,close
			socket_shutdown(soc);
			// ログ出力
			syslog_output(LOG_DEBUG, "<%d> SMPhone client is already used", getpid());
			syslog_output(LOG_NOTICE, "<%d> child process NG", getpid());
			return;
		}
		syslog_output(LOG_NOTICE, "<%d> SMPhone client connected", getpid());
		// クライアント処理main
		mode_smartphone_main(soc);
		// キー入力状態をリセット
		reset_inputkeys();
		// 接続中フラグOFF
		setSMPhoneFlag(FALSE);
		syslog_output(LOG_NOTICE, "<%d> SMPhone client close", getpid());
	}
	else {
		// ブラウザ接続中チェックとフラグセット
		BOOL changed = setBrowserFlag(TRUE);
		if (!changed) {
			// クライアントソケットshutdown,close
			socket_shutdown(soc);
			// ログ出力
			syslog_output(LOG_DEBUG, "<%d> Browser client is already used", getpid());
			syslog_output(LOG_NOTICE, "<%d> child process NG", getpid());
			return;
		}
		syslog_output(LOG_NOTICE, "<%d> Browser client connected", getpid());
		// クライアント処理main
		mode_webbrowser_main(soc);
		// 接続中フラグOFF
		setBrowserFlag(FALSE);
		syslog_output(LOG_NOTICE, "<%d> Browser client close", getpid());
	}

	// クライアントソケットshutdown,close
	socket_shutdown(soc);
	// 子プロセス終了へ
	syslog_output(LOG_NOTICE, "<%d> child process EXIT", getpid());
	return;
}


// クライアントソケットshutdown,close
static void socket_shutdown(SOCKET soc) {
	shutdown(soc, SHUT_RDWR);
	close(soc);
}


static BOOL check_header_smphone(const BYTE* header);
static BOOL check_header_browser(const BYTE* header);

/* クライアントとヘッダ送受信 */
static BOOL header_exchange_with_client(SOCKET soc)
{
	BOOL rtn;
	// ヘッダ情報領域 大きいスマートフォン用ヘッダを基準
	BYTE bufHeader[SIZE_HEADER_MSG_SMPHONE];

	// ヘッダ情報を受信 1回目
	rtn = exec_recv(soc, bufHeader, SIZE_HEADER_MSG_BROWSER);
	if (!rtn) {
		return FALSE;
	}
	// ヘッダメッセージの確認 ブラウザ
	rtn = check_header_browser(bufHeader);
	if (rtn) {
		return TRUE;
	}

	// ヘッダ情報を受信 2回目
	rtn = exec_recv(soc, &bufHeader[SIZE_HEADER_MSG_BROWSER], (SIZE_HEADER_MSG_SMPHONE - SIZE_HEADER_MSG_BROWSER));
	if (!rtn) {
		return FALSE;
	}
	// ヘッダメッセージの確認 スマートフォン
	rtn = check_header_smphone(bufHeader);
	if (rtn) {
		return TRUE;
	}

	// 一致しない
	syslog_output(LOG_DEBUG, "<%d> client header - unknown", getpid());
	return FALSE;
}

/* ヘッダメッセージの確認 スマートフォン */
static BOOL check_header_smphone(const BYTE* header) {
	int cmp = memcmp(header, HEADER_MSG_SMPHONE, SIZE_HEADER_MSG_SMPHONE);
	if (cmp == 0) {
		_client_mode = MODE_SMPHONE;
		return TRUE;
	}
	return FALSE;
}

/* ヘッダメッセージの確認 ブラウザ */
static BOOL check_header_browser(const BYTE* header) {
	int cmp = memcmp(header, HEADER_MSG_BROWSER, SIZE_HEADER_MSG_BROWSER);
	if (cmp == 0) {
		_client_mode = MODE_BROWSER;
		return TRUE;
	}
	return FALSE;
}


/* スマートフォンクライアント処理main */
static void mode_smartphone_main(SOCKET soc) {
	BOOL rtn;

	const int INPUT_DATA_SIZE = (1 + 6);
	BYTE inputBuf[INPUT_DATA_SIZE];

	// ヘッダOKメッセージを送信
	rtn = exec_send(soc, (const BYTE*)RETURN_MSG_HEADER_OK, SIZE_RETURN_MSG_HEADER_OK);
	if (!rtn) {
		return;
	}

	syslog_output(LOG_NOTICE, "<%d> Header check OK", getpid());

	// 通信ループ
	for (;;) {

		// 入力情報を受信
		rtn = exec_recv(soc, inputBuf, INPUT_DATA_SIZE);
		if (!rtn) {
			return;
		}

		// 情報先頭チェック
		if (inputBuf[0] != CHECK_KEYINPUT_CHARACTER) {
			syslog_output(LOG_DEBUG, "<%d> keyinput check char NG", getpid());
			return;
		}

		// 入力状態データを更新
		update_inputkeys(inputBuf[1], inputBuf[2], inputBuf[3], inputBuf[4], inputBuf[5], inputBuf[6]);

		// 0.01秒待機
		usleep(10000);
	}
}


/* ブラウザクライアント処理main */
static void mode_webbrowser_main(SOCKET soc) {
	BOOL rtn;

	const int LEN_KEYSTR = 20;
	char keyStr[LEN_KEYSTR];

	BYTE dataFrameBuf[WEBSOCKET_DATA_FLAME_MAX];

	// WebSocketハンドシェイク処理main
	rtn = ws_handshake_main(soc);
	if (!rtn) {
		syslog_output(LOG_DEBUG, "<%d> WebSocket Handshake failed", getpid());
		return;
	}

	syslog_output(LOG_NOTICE, "<%d> WebSocket Handshake OK", getpid());

	// 通信ループ とりあえず10分まで 切断ハンドシェイクは不使用
	int i;
	for (i = 0; i < 20000; i++) {
		// 0.03秒待機
		usleep(30000);

		// キー入力状態取得
		BOOL isA, isB, isU, isD, isL, isR;
		get_inputkeys(&isA, &isB, &isU, &isD, &isL, &isR);

		// 情報文字列作成
		int btnA = isA ? 1 : 0;
		int btnB = isB ? 1 : 0;
		int btnU = isU ? 1 : 0;
		int btnD = isD ? 1 : 0;
		int btnL = isL ? 1 : 0;
		int btnR = isR ? 1 : 0;
		snprintf(keyStr, LEN_KEYSTR, "A%dB%dU%dD%dL%dR%d", btnA, btnB, btnU, btnD, btnL, btnR);

		// WebSocketデータフレーム作成
		int dataFrameSize = make_websocket_dataframe(dataFrameBuf, (const BYTE*)keyStr, strlen(keyStr), TRUE);
		// 送信
		rtn = exec_send(soc, dataFrameBuf, dataFrameSize);
		if (!rtn) {
			return;
		}
	}
}
