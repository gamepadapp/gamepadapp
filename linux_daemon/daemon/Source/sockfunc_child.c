#include "socket_headers.h"

#include "sockfunc_child.h"
#include "syslog_output.h"
#include "signal_register.h"

/* 送信可能チェック */
static BOOL check_enable_send(SOCKET soc);
/* 受信可能チェック */
static BOOL check_enable_recv(SOCKET soc);

/* 通信タイムアウト秒 */
#define SOCKET_TIMEOUT 10

/* データ送信処理 指定サイズ送信 */
BOOL exec_send(SOCKET soc, const BYTE* buf, size_t sendLen)
{
	// １回あたりの送信長MAX
	const int SEND_SIZE_PER_TIME = 768;

	int planSize;						// 今回送信予定長
	ssize_t sendResultSize;			// 今回送信結果長
	int restSize = sendLen;			// 残り送信データ長
	int bufIndex = 0;					// バッファ参照位置

	// 送信完了までループ
	do {
		// 今回送信予定長
		planSize = (restSize > SEND_SIZE_PER_TIME) ? SEND_SIZE_PER_TIME : restSize;

		// 送信可能チェック
		BOOL rtn = check_enable_send(soc);
		if (!rtn) {
			return FALSE;
		}

		// 1回送信
		sendResultSize = send(soc, &buf[bufIndex], planSize, 0);
		if (sendResultSize == -1) {
			syslog_output(LOG_DEBUG, "<%d> send() failed:%d", getpid(), errno);
			return FALSE;
		}

		// 送信されたバイトサイズと送信予定サイズに差異
		if (sendResultSize != planSize) {
			syslog_output(LOG_DEBUG, "<%d> send size error", getpid());
			return FALSE;
		}

		// 残りデータ長
		restSize -= sendResultSize;
		// バッファ参照位置
		bufIndex += sendResultSize;

	} while (restSize != 0);

	// 正常終了
	return TRUE;
}

/* データ受信処理 指定サイズ受信 */
BOOL exec_recv(SOCKET soc, BYTE* buf, size_t requireLen)
{
	const int RECV_BUF_SIZE = 768;
	BYTE recvBuf[RECV_BUF_SIZE];	// recv領域

	int planSize;						// 今回受信予定長
	ssize_t recvResultSize;			// 今回受信結果長
	int restSize = requireLen;		// 残り受信データ長
	int bufIndex = 0;					// バッファ参照位置

	// 受信完了までループ
	do {
		// 今回受信予定長
		planSize = (restSize > RECV_BUF_SIZE) ? RECV_BUF_SIZE : restSize;

		// 受信可能チェック
		BOOL rtn = check_enable_recv(soc);
		if (!rtn) {
			return FALSE;
		}

		// 1回受信
		recvResultSize = recv(soc, recvBuf, planSize, 0);
		if (recvResultSize == -1) {
			syslog_output(LOG_DEBUG, "<%d> recv() failed:%d", getpid(), errno);
			return FALSE;
		}

		// 指定サイズ受信前の受信終了通知
		if (recvResultSize == 0) {
			syslog_output(LOG_DEBUG, "<%d> recv finished although datasize is not reached", getpid());
			return FALSE;
		}

		// 残りデータ長
		restSize -= recvResultSize;
		if (restSize < 0) {
			// 受信サイズオーバー
			syslog_output(LOG_DEBUG, "<%d> recv datasize over", getpid());
			return FALSE;
		}

		// データ追加
		memcpy(&buf[bufIndex], recvBuf, recvResultSize);
		// バッファ参照位置
		bufIndex += recvResultSize;

	} while (restSize != 0);

	// 正常終了
	return TRUE;
}

/* データ受信処理 CRLFCRLFまで */
BOOL exec_recv_crlfcrlf(SOCKET soc, BYTE* buf, size_t maxRecvLen, size_t* pReceivedLen)
{
	const char* ENDOFDATA_CODE = "\r\n\r\n";
	const int SIZE_ENDOFDATA_CODE = 4;

	const int RECV_BUF_SIZE = 768;
	BYTE recvBuf[RECV_BUF_SIZE];	// recv領域

	int planSize;						// 今回受信予定長
	ssize_t recvResultSize;			// 今回受信結果長
	int restSize = maxRecvLen;		// 残り受信可能長
	int bufIndex = 0;					// バッファ参照位置

	// 受信完了までループ
	for(;;) {
		// 今回受信予定長
		planSize = (restSize > RECV_BUF_SIZE) ? RECV_BUF_SIZE : restSize;

		// 受信可能チェック
		BOOL rtn = check_enable_recv(soc);
		if (!rtn) {
			return FALSE;
		}

		// 1回受信
		recvResultSize = recv(soc, recvBuf, planSize, 0);
		if (recvResultSize == -1) {
			syslog_output(LOG_DEBUG, "<%d> recv() failed:%d", getpid(), errno);
			return FALSE;
		}

		// 終端コード受信前の受信終了通知
		if (recvResultSize == 0) {
			syslog_output(LOG_DEBUG, "<%d> recv finished although dataend is not reached", getpid());
			return FALSE;
		}

		// 残りデータ長
		restSize -= recvResultSize;
		if (restSize < 0) {
			// 受信サイズオーバー
			syslog_output(LOG_DEBUG, "<%d> recv datasize over", getpid());
			return FALSE;
		}

		// データ追加
		memcpy(&buf[bufIndex], recvBuf, recvResultSize);
		// バッファ参照位置
		bufIndex += recvResultSize;

		// 終端コードチェック
		const int savedSize = (maxRecvLen - restSize);
		if (savedSize >= SIZE_ENDOFDATA_CODE) {
			int cmp = memcmp(&buf[savedSize - SIZE_ENDOFDATA_CODE], ENDOFDATA_CODE, SIZE_ENDOFDATA_CODE);
			if (cmp == 0) {
				break;
			}
		}

		if (restSize == 0) {
			// 受信可能サイズ到達
			syslog_output(LOG_DEBUG, "<%d> max recv datasize is reached", getpid());
			return FALSE;
		}
	};

	// 受信済みデータサイズ
	*pReceivedLen = (maxRecvLen - restSize);

	// 正常終了
	return TRUE;
}

/* 送信可能チェック */
static BOOL check_enable_send(SOCKET soc)
{
	int rtn;
	int timeCount = 0;

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

			// 送信可能チェック
			rtn = select(soc + 1, NULL, &mask, NULL, &timeout);
		}

		switch (rtn) {
			case -1: {
				// システムコールによる割り込み以外の場合
				if (errno != EINTR) {
					// ERROR
					syslog_output(LOG_DEBUG, "<%d> send select() failed:%d", getpid(), errno);
					return FALSE;
				}
				break;
			}
			case 0: {
				// 1秒加算
				if (++timeCount >= SOCKET_TIMEOUT) {
					// 指定時間タイムアウト
					syslog_output(LOG_DEBUG, "<%d> send select() timeout", getpid());
					return FALSE;
				}
				break;
			}
			default: {
				// 送信可能
				return TRUE;
			}
		}
	}

	// シグナルによる終了
	syslog_output(LOG_NOTICE, "<%d> send select() end with signal", getpid());
	return FALSE;
}

/* 受信可能チェック */
static BOOL check_enable_recv(SOCKET soc)
{
	int rtn;
	int timeCount = 0;

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

			// 受信可能チェック
			rtn = select(soc + 1, &mask, NULL, NULL, &timeout);
		}

		switch (rtn) {
			case -1: {
				// システムコールによる割り込み以外の場合
				if (errno != EINTR) {
					// ERROR
					syslog_output(LOG_DEBUG, "<%d> recv select() failed:%d", getpid(), errno);
					return FALSE;
				}
				break;
			}
			case 0: {
				// 1秒加算
				if (++timeCount >= SOCKET_TIMEOUT) {
					// 指定時間タイムアウト
					syslog_output(LOG_DEBUG, "<%d> recv select() timeout", getpid());
					return FALSE;
				}
				break;
			}
			default: {
				// 受信可能
				return TRUE;
			}
		}
	}

	// シグナルによる終了
	syslog_output(LOG_NOTICE, "<%d> recv select() end with signal", getpid());
	return FALSE;
}
