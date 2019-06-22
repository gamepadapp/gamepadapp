#pragma once

#include "common.h"

/* daemon化 */
BOOL daemonize();

/* pidファイル生成 */
BOOL create_pid_file(const char* pidFilePath);
/* pidファイル削除 */
BOOL delete_pid_file(const char* pidFilePath);
