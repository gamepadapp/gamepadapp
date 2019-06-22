#include "socket_headers.h"

#include "socket_server.h"
#include "sockfunc_parent.h"
#include "socket_child.h"
#include "syslog_output.h"

/* 接続受付ループ */
static void accept_loop(SOCKET soc);
/* 子プロセス終了待機 */
static void wait_child_process();

/* サーバ処理メイン */
BOOL socket_server_main(const char* pPortAccept)
{
	SOCKET soc;		// サーバソケット

	// サーバソケットの準備
	BOOL rtn = init_server_socket(pPortAccept, &soc);
	if (!rtn) {
		syslog_output(LOG_NOTICE, "init_server_socket() failed");
		return FALSE;
	}
	// 準備完了メッセージ
	syslog_output(LOG_NOTICE, "--- ready for accept ---");

	// 接続受付ループ(終了シグナルで終了)
	accept_loop(soc);

	// 子プロセス終了待機
	wait_child_process();
	// サーバソケットを閉じる
	close(soc);
	// 正常終了
	return TRUE;
}

/* 接続受付ループ */
static void accept_loop(SOCKET soc)
{
	SOCKET acc;				// クライアントソケット
	int status;
	BOOL rtn;

	for (;;) {

		// 接続受付可能チェック
		rtn = check_enable_accept(soc);
		if (!rtn) {
			// 終了シグナルによりaccept_loop終了
			break;
		}

		// 接続受付
		{
			struct sockaddr_storage from;		// 接続相手のアドレス情報構造体
			socklen_t from_len;					// 構造体のサイズ

			// accept実行
			{
				// アドレス情報構造体サイズ取得
				from_len = (socklen_t) sizeof(struct sockaddr_storage);

				// 接続受付、クライアントソケットおよび接続情報取得
				acc = accept(soc, (struct sockaddr*) &from, &from_len);
				if (acc == -1) {
					// システムコールによる割り込み以外の場合
					if (errno != EINTR) {
						syslog_output(LOG_NOTICE, "accept() failed:%d", errno);
					}
					// 再試行
					continue;
				}
			}

			// 取得情報から接続相手のホスト名とサービス名を抽出
			{
				char hbuf[NI_MAXHOST];		// ホスト名
				char sbuf[NI_MAXSERV];		// サービス名

				getnameinfo((struct sockaddr*) &from, from_len,
								hbuf, sizeof(hbuf),
								sbuf, sizeof(sbuf),
								NI_NUMERICHOST | NI_NUMERICSERV);

				// ホスト名とサービス名の出力
				syslog_output(LOG_NOTICE, "accepted host %s:%s", hbuf, sbuf);
			}
		}

		// FORKにより処理分岐
		{
			// FORK実行
			pid_t pid  = fork();

			// ERROR/子プロセス/親プロセス
			switch (pid) {
				case -1: {		// ERROR
					// クライアントソケットclose
					close(acc);
					syslog_output(LOG_NOTICE, "fork() failed:%d", errno);
					break;
				}
				case 0: {		// 子プロセス
					// サーバソケットclose
					close(soc);
					// 子プロセス処理メイン
					socket_child_main(acc);
					// 子プロセス終了
					_exit(1);
					break;
				}
				default: {		// 親プロセス
					// クライアントソケットclose
					close(acc);
					// 子プロセスID出力
					syslog_output(LOG_NOTICE, "forked pid:%d", pid);
					break;
				}
			}
		}

		// シグナルでキャッチできなかった子プロセス終了のチェック 待機なし
		waitpid(-1, &status, WNOHANG);
	}
}

/* 子プロセス終了待機 */
static void wait_child_process()
{
	int status;

	for (;;) {
		// 終了シグナルによる子プロセスの終了を待機
		pid_t pid = waitpid(-1, &status, 0);

		// 残りの子プロセスが存在しなければ終了
		if (pid == -1) {
			if (errno == ECHILD) {
				break;
			}
		}
	}
}
