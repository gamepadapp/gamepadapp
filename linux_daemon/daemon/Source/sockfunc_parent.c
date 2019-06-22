#include "socket_headers.h"
#include "sockfunc_parent.h"
#include "syslog_output.h"
#include "signal_register.h"

/* サーバソケットの準備 */
BOOL init_server_socket(const char* pPortAccept, SOCKET* soc)
{
	struct addrinfo *res0;		// アドレス情報
	int tmpsoc;

	// アドレス情報の取得
	{
		// アドレス情報のヒントをクリアし情報セット
		struct addrinfo hints;
		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;

		// アドレス情報の取得
		int errcode = getaddrinfo(NULL, pPortAccept, &hints, &res0);
		if (errcode != 0) {
			syslog_output(LOG_NOTICE, "getaddrinfo() failed:%s", gai_strerror(errcode));
			return FALSE;
		}
	}

	// アドレスから名前への変換
	{
		char hbuf[NI_MAXHOST];		// ホスト名
		char sbuf[NI_MAXSERV];		// サービス
		int errcode = getnameinfo(res0->ai_addr, res0->ai_addrlen,
									hbuf, sizeof(hbuf),
									sbuf, sizeof(sbuf),
									NI_NUMERICHOST | NI_NUMERICSERV);
		if (errcode != 0) {
			syslog_output(LOG_NOTICE, "getnameinfo() failed:%s", gai_strerror(errcode));
			freeaddrinfo(res0);
			return FALSE;
		}

		// ホスト名とサービス名の出力
		syslog_output(LOG_NOTICE, "my host %s:%s", hbuf, sbuf);
	}

	// サーバソケットの生成
	{
		tmpsoc = socket(res0->ai_family, res0->ai_socktype, res0->ai_protocol);
		if (tmpsoc == -1) {
			syslog_output(LOG_NOTICE, "socket() failed:%d", errno);
			freeaddrinfo(res0);
			return FALSE;
		}
	}

	// ソケットオプション（再利用フラグ）設定
	{
		int opt = 1;
		socklen_t opt_len = sizeof(int);

		// SOL_SOCKETレベルのSO_REUSEADDRパラメータに1(BOOL)を設定
		int rtn = setsockopt(tmpsoc, SOL_SOCKET, SO_REUSEADDR, &opt, opt_len);
		if (rtn == -1) {
			syslog_output(LOG_NOTICE, "setsockopt() failed:%d", errno);
			close(tmpsoc);
			freeaddrinfo(res0);
			return FALSE;
		}
	}

	// ソケットにアドレス情報をバインド
	{
		int rtn = bind(tmpsoc, res0->ai_addr, res0->ai_addrlen);
		if (rtn == -1) {
			syslog_output(LOG_NOTICE, "bind() failed:%d", errno);
			close(tmpsoc);
			freeaddrinfo(res0);
			return FALSE;
		}
	}

	// アクセスバックログの設定
	{
		int rtn = listen(tmpsoc, SOMAXCONN);
		if (rtn == -1) {
			syslog_output(LOG_NOTICE, "listen() failed:%d", errno);
			close(tmpsoc);
			freeaddrinfo(res0);
			return FALSE;
		}
	}

	// アドレス情報の解放
	freeaddrinfo(res0);

	// サーバソケット
	*soc = tmpsoc;

	// 正常終了
	return TRUE;
}

/* 接続受付可能チェック */
BOOL check_enable_accept(SOCKET soc)
{
	int rtn;

	// シグナルによる終了フラグを確認しながらループ
	while (!g_gotsig) {

		// select実行
		{
			// マスク設定
			fd_set mask;
			FD_ZERO(&mask);
			FD_SET(soc, &mask);

			// タイムアウト設定(1秒)
			struct timeval timeout;
			timeout.tv_sec = 1;
			timeout.tv_usec = 0;

			// 接続受付可能チェック
			rtn = select(soc + 1, &mask, NULL, NULL, &timeout);
		}

		if (rtn > 0) {
			// 接続受付可能
			return TRUE;
		}
	}

	// シグナルによる終了
	syslog_output(LOG_NOTICE, "accept select() end with signal");
	return FALSE;
}
