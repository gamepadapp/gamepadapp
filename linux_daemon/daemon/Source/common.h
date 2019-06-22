#pragma once
/* 共通ヘッダ */

/* socket */
typedef int SOCKET;
/* 1byte */
typedef unsigned char BYTE;
/* boolean */
typedef int BOOL;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* daemon化フラグ */
#define  ENABLE_DAEMON  TRUE
