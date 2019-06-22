#include <stdio.h>
#include <stdarg.h>

#include "syslog_output.h"
#include "common.h"

/* facility=LOG_DAEMONでsyslog出力 */
void syslog_output(int level, const char* format, ...)
{
	// LOG_EMERG
	// LOG_ALERT
	// LOG_CRIT
	// LOG_ERR
	// LOG_WARNING
	// LOG_NOTICE
	// LOG_INFO
	// LOG_DEBUG

#if ENABLE_DAEMON
	// syslogに出力
	va_list list;
	va_start(list, format);
	vsyslog(LOG_DAEMON | level, format, list);
	va_end(list);
#else
	// チェック用に標準出力
	printf("level:%d content:%s\n", level, format);
#endif

}
