#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#include "common.h"
#include "syslog_output.h"
#include "daemon_tools.h"
#include "signal_register.h"
#include "socket_server.h"
#include "mutex_data.h"

/* 指定可能ポート番号の下限上限 */
#define  PORTNUM_MIN  1024
#define  PORTNUM_MAX  65535

static BOOL option_check_main(int argc, char** argv, char** pPortAccept);
static BOOL is_port_string(const char* string);

/* メイン関数 */
int main (int argc, char* argv[])
{
	BOOL rtn;

	// pidファイルパス
	const char* PID_FILEPATH = "/var/run/netcontrollerd.pid";
	// ポート番号パラメータ参照
	char* pPortAccept = NULL;

	// start message
	syslog_output(LOG_NOTICE, "--- daemon start ---");

	// オプションチェック
	rtn = option_check_main(argc, argv, &pPortAccept);
	if (!rtn) {
		syslog_output(LOG_ERR, "option_check_main() failed");
		syslog_output(LOG_NOTICE, "--- daemon end ---");
		return EX_USAGE;
	}

	// フラグONなら
	if (ENABLE_DAEMON) {
		// daemon化
		rtn = daemonize();
		if (!rtn) {
			syslog_output(LOG_ERR, "daemonize() failed");
			syslog_output(LOG_NOTICE, "--- daemon end ---");
			return EX_UNAVAILABLE;
		}

		// pidファイル生成
		rtn = create_pid_file(PID_FILEPATH);
		if (!rtn) {
			syslog_output(LOG_ERR, "create_pid_file() failed");
			syslog_output(LOG_NOTICE, "--- daemon end ---");
			return EX_UNAVAILABLE;
		}
	}

	// シグナル関連登録
	signal_register_main();

	// プロセス間共有start
	rtn = start_mutex_data();
	if (!rtn) {
		syslog_output(LOG_ERR, "start_mutex_data() failed");
		syslog_output(LOG_NOTICE, "--- daemon end ---");
		if (ENABLE_DAEMON) {
			// pidファイル削除
			delete_pid_file(PID_FILEPATH);
		}
		return EX_UNAVAILABLE;
	}

	// サーバ処理(main)
	rtn = socket_server_main(pPortAccept);
	if (!rtn) {
		syslog_output(LOG_ERR, "socket_server_main() failed");
	}

	// プロセス間共有end
	end_mutex_data();

	// 終了
	syslog_output(LOG_NOTICE, "--- daemon end ---");
	if (ENABLE_DAEMON) {
		// pidファイル削除
		delete_pid_file(PID_FILEPATH);
	}
	return rtn ? EX_OK : EX_UNAVAILABLE;
}


/* オプションチェックメイン */
static BOOL option_check_main(int argc, char** argv, char** pPortAccept)
{
	BOOL rtn;

	// パラメータは1つ限定
	if (argc != 2) {
		syslog_output(LOG_NOTICE, "Usage: %s <accept port number>", argv[0]);
		return FALSE;
	}

	char* accept = argv[1];

	// 待ち受けポート番号チェック
	rtn = is_port_string(accept);
	if (!rtn) {
		syslog_output(LOG_NOTICE, "accept port number NG:%s", accept);
		return FALSE;
	}

	// パラメータをセット
	*pPortAccept = accept;

	// 正常終了
	return TRUE;
}

/* ポート番号チェック */
static BOOL is_port_string(const char* string)
{
	// 文字数(4桁or5桁)チェック
	const int len = strlen(string);
	if (len < 4 || len > 5) {
		return FALSE;
	}

	// 数字のみで構成されているかチェック
	int i;
	for (i=0; i<len; i++) {
		if (string[i] < '0' || string[i] > '9') {
			return FALSE;
		}
	}

	// 数値範囲チェック
	int num = atoi(string);
	if (num < PORTNUM_MIN || num > PORTNUM_MAX) {
		return FALSE;
	}

	// OK
	return TRUE;
}
