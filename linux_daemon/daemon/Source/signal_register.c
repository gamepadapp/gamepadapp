#include <stdio.h>
#include <sys/wait.h>

#include "signal_register.h"

/* 子プロセス終了シグナル(SIGCHLD)ハンドラ */
static void sig_chld_handler(int signum);
/* プロセス終了命令シグナル(SIGINT,SIGTERM)ハンドラ */
static void sig_int_term_handler(int signum);

/* 終了シグナル通知用global */
volatile sig_atomic_t g_gotsig = 0;

/* シグナル関連登録メイン */
void signal_register_main()
{
	/* SIGKILLとSIGSTOPは補足不可 */

	/* とりあえず無視指定 */
	sigaction_update(SIGALRM, SIG_IGN);
	sigaction_update(SIGHUP, SIG_IGN);

	/* シグナル無視指定 */
	sigaction_update(SIGPIPE, SIG_IGN);
	sigaction_update(SIGUSR1, SIG_IGN);
	sigaction_update(SIGUSR2, SIG_IGN);
	sigaction_update(SIGTTIN, SIG_IGN);
	sigaction_update(SIGTTOU, SIG_IGN);

	/* 子プロセス終了シグナル 独自ハンドラ */
	sigaction_update(SIGCHLD, sig_chld_handler);

	/* プロセス終了命令シグナル 独自ハンドラ */
	sigaction_update(SIGINT, sig_int_term_handler);
	sigaction_update(SIGTERM, sig_int_term_handler);
}

/* シグナル動作パラメータ更新 */
void sigaction_update(int signum, __sighandler_t handler)
{
	struct sigaction stSigAct;

	// シグナル動作パラメータの取得
	sigaction(signum, NULL, &stSigAct);
	// パラメータの変更
	stSigAct.sa_handler = handler;		// 指定ハンドラ
	stSigAct.sa_flags = SA_NODEFER;		// シグナル動作をマスクしない
	// シグナル動作パラメータのセット
	sigaction(signum, &stSigAct, NULL);
}

/* 子プロセス終了シグナル(SIGCHLD)ハンドラ */
static void sig_chld_handler(int signum)
{
	// 子プロセスの終了を待つ
	int status;
	wait(&status);
}

/* プロセス終了命令シグナル(SIGINT,SIGTERM)ハンドラ */
static void sig_int_term_handler(int signum)
{
	// 終了シグナルの通知フラグON
	g_gotsig = 1;
}
