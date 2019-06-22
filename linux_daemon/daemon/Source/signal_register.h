#pragma once

#include <signal.h>

/* シグナル通知用global */
extern volatile sig_atomic_t g_gotsig;

/* シグナル関連登録メイン */
void signal_register_main();
/* シグナル動作パラメータ更新 */
void sigaction_update(int signum, __sighandler_t handler);
