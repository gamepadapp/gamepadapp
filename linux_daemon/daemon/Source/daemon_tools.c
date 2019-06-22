#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <unistd.h>

#include <errno.h>
#include <paths.h>

#include "daemon_tools.h"
#include "syslog_output.h"
#include "signal_register.h"

/* closeするファイルディスクリプタ数 */
#define MAX_CLOSE_FD 64

/* daemon化 */
BOOL daemonize()
{
	pid_t pid;			// プロセスID

	// 1回目のFORK実行(子プロセス生成)
	// バックグラウンドに移行
	// プロセスグループのリーダーでないことを保証
	pid = fork();
	if (pid == -1) {
		syslog_output(LOG_NOTICE, "fork() failed:%d", errno);
		return FALSE;
	}

	// 子プロセスを残し親プロセスは終了
	if (pid != 0) {
		_exit(0);
	}

	// セッショングループのリーダーになって
	// 制御端末を切り離す
	pid_t sid = setsid();
	if (sid == -1) {
		syslog_output(LOG_NOTICE, "setsid() failed:%d", errno);
		return FALSE;
	}

	// HUPシグナルを無視するように
	sigaction_update(SIGHUP, SIG_IGN);

	// 2回目のFORK実行(孫プロセス生成)
	// セッショングループのリーダーでないことを保証
	pid = fork();
	if (pid == -1) {
		syslog_output(LOG_NOTICE, "fork() failed:%d", errno);
		return FALSE;
	}

	// 孫プロセスを残し子プロセスは終了
	if (pid != 0) {
		_exit(0);
	}

	// ルートディレクトリに移動
	// シャットダウン時のアンマウント不可を防ぐ
	int rtn = chdir("/");
	if (rtn == -1) {
		syslog_output(LOG_NOTICE, "chdir() failed:%d", errno);
		return FALSE;
	}

	// ファイルディスクリプタ
	{
		int i;

		// ファイルディスクリプタを指定数分クローズ
		for (i = 0; i < MAX_CLOSE_FD; i++) {
			close(i);
		}

		// ファイルディスクリプタ複製元として/dev/nullをオープン
		int fd = open(_PATH_DEVNULL, O_RDWR, 0);
		if (fd == -1) {
			syslog_output(LOG_NOTICE, "open() failed:%d", errno);
			return FALSE;
		}

		// 複製をstdin,stdout,stderrに割り当て、使用時にエラーとならないようにする
		{
			dup2(fd, STDIN_FILENO);
			dup2(fd, STDOUT_FILENO);
			dup2(fd, STDERR_FILENO);
		}

		// 複製元クローズ
		if (fd > STDERR_FILENO) {
			close(fd);
		}
	}

	// 正常終了
	return TRUE;
}


/* pidファイル生成 */
BOOL create_pid_file(const char* pidFilePath)
{
	// ファイルを書き込みモードで開く
	FILE* fp = fopen(pidFilePath, "w");
	if (fp == NULL) {
		syslog_output(LOG_NOTICE, "fopen() failed:%d", errno);
		return FALSE;
	}

	// pid出力
	int rtn = fprintf(fp, "%ld\n", (long)getpid());
	if (rtn < 0) {
		fclose(fp);
		remove(pidFilePath);
		syslog_output(LOG_NOTICE, "fprintf() failed");
		return FALSE;
	}

	// ファイルを閉じる
	fclose(fp);

	// 正常終了
	return TRUE;
}

/* pidファイル削除 */
BOOL delete_pid_file(const char* pidFilePath)
{
	// ファイル削除
	int rtn = remove(pidFilePath);
	if (rtn == -1) {
		syslog_output(LOG_NOTICE, "remove() failed:%d", errno);
		return FALSE;
	}

	// 正常終了
	return TRUE;
}
