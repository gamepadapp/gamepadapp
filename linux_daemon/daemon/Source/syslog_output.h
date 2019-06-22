#pragma once

#include <syslog.h>

/* facility=LOG_DAEMONでsyslog出力 */
void syslog_output(int level, const char* format, ...);
