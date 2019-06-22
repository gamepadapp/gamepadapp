#pragma once

#include "common.h"

/* プロセス間共有start */
BOOL start_mutex_data();
/* プロセス間共有end */
void end_mutex_data();

/* 接続種別フラグ確認・更新 */
BOOL setSMPhoneFlag(BOOL flag);
BOOL setBrowserFlag(BOOL flag);

/* キー入力状態のセット */
void update_inputkeys(BOOL a, BOOL b, BOOL up, BOOL down, BOOL left, BOOL right);
/* キー入力状態のリセット */
void reset_inputkeys();
/* キー入力状態の取得 */
void get_inputkeys(BOOL* a, BOOL* b, BOOL* up, BOOL* down, BOOL* left, BOOL* right);
