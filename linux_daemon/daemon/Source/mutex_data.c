#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include <sys/mman.h>
#include <pthread.h>

#include "mutex_data.h"
#include "syslog_output.h"

/* 共有メモリオブジェクト */
static void*            _mmap_addr        = NULL;
static pthread_mutex_t* _mutex_ptr        = NULL;
static BOOL*            _use_smphone_flag = NULL;
static BOOL*            _use_browser_flag = NULL;
static BOOL*            _input_keys       = NULL;

/* 共有メモリ内データ位置 */
#define  POS_MUTEX         0
#define  POS_SMPHONE_FLAG  (POS_MUTEX        + sizeof(pthread_mutex_t))
#define  POS_BROWSER_FLAG  (POS_SMPHONE_FLAG + sizeof(BOOL))
#define  POS_INPUT_KEYS    (POS_BROWSER_FLAG + sizeof(BOOL))

static void wait_mutex();
static void release_mutex();
static void default_values(void* addr);
static void reset_keyarray();

/* プロセス間共有start */
BOOL start_mutex_data() {
	// プロセス間共有メモリ
	void* addr = mmap(NULL, sysconf(_SC_PAGE_SIZE), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	if (addr == MAP_FAILED) {
		syslog_output(LOG_NOTICE, "mmap() failed");
		return FALSE;
	}

	// デフォルト値セット
	default_values(addr);

	// mutex属性の指定 プロセス間で共有
	pthread_mutexattr_t mattr;
	pthread_mutexattr_init(&mattr);
	int rtn = pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
	if (rtn == EINVAL) {
		syslog_output(LOG_NOTICE, "pthread_mutexattr_setpshared() failed");
		munmap(_mmap_addr, sysconf(_SC_PAGE_SIZE));
		return FALSE;
	}

	// mutexを属性付きで初期化
	pthread_mutex_init(_mutex_ptr, &mattr);

	return TRUE;
}

/* プロセス間共有end */
void end_mutex_data() {
	if (_mmap_addr != NULL) {
		// mutex解放
		pthread_mutex_destroy(_mutex_ptr);
		// 共有メモリ解放
		munmap(_mmap_addr, sysconf(_SC_PAGE_SIZE));
	}
}


/* スマートフォン接続中フラグ確認・更新 */
BOOL setSMPhoneFlag(BOOL flag) {
	BOOL check, result;
	// Mutex待機
	wait_mutex();

	// 現在値確認
	check = *_use_smphone_flag;

	if (check != flag) {
		result = TRUE;
		// 状態更新
		*_use_smphone_flag = flag;
	} else {
		/* 切断処理が完全ではないため複数接続を一時的に許可する */
		result = TRUE;
//		result = FALSE;
	}

	// Mutex解放
	release_mutex();
	return result;
}

/* ブラウザ接続中フラグ確認・更新 */
BOOL setBrowserFlag(BOOL flag) {
	BOOL check, result;
	// Mutex待機
	wait_mutex();

	// 現在値確認
	check = *_use_browser_flag;

	if (check != flag) {
		result = TRUE;
		// 状態更新
		*_use_browser_flag = flag;
	} else {
		/* 切断処理が完全ではないため複数接続を一時的に許可する */
		result = TRUE;
//		result = FALSE;
	}

	// Mutex解放
	release_mutex();
	return result;
}


/* キー入力状態の更新 */
void update_inputkeys(BOOL a, BOOL b, BOOL up, BOOL down, BOOL left, BOOL right) {
	// Mutex待機
	wait_mutex();
	// 状態更新
	_input_keys[0] = a;
	_input_keys[1] = b;
	_input_keys[2] = up;
	_input_keys[3] = down;
	_input_keys[4] = left;
	_input_keys[5] = right;
	// Mutex解放
	release_mutex();
}

/* キー入力状態のリセット */
void reset_inputkeys() {
	// Mutex待機
	wait_mutex();
	// 状態更新 すべてFALSE
	reset_keyarray();
	// Mutex解放
	release_mutex();
}

/* キー入力状態の取得 */
void get_inputkeys(BOOL* a, BOOL* b, BOOL* up, BOOL* down, BOOL* left, BOOL* right) {
	// Mutex待機
	wait_mutex();
	// 状態更新
	*a     = _input_keys[0];
	*b     = _input_keys[1];
	*up    = _input_keys[2];
	*down  = _input_keys[3];
	*left  = _input_keys[4];
	*right = _input_keys[5];
	// Mutex解放
	release_mutex();
}


/* ------------------- static ------------------- */

// Mutex待機
static void wait_mutex() {
	pthread_mutex_lock(_mutex_ptr);
}

// Mutex解放
static void release_mutex() {
	pthread_mutex_unlock(_mutex_ptr);
}

/* デフォルト値セット */
static void default_values(void* addr) {
	// 共有メモリ領域
	_mmap_addr = addr;

	// mutexポインタ
	_mutex_ptr = (addr + POS_MUTEX);

	// スマートフォン接続中フラグ
	_use_smphone_flag = (addr + POS_SMPHONE_FLAG);
	*_use_smphone_flag = FALSE;

	// ブラウザ接続中フラグ
	_use_browser_flag = (addr + POS_BROWSER_FLAG);
	*_use_browser_flag = FALSE;

	// キー入力状態
	_input_keys = (addr + POS_INPUT_KEYS);
	reset_keyarray();
}

// キー入力状態リセット
static void reset_keyarray() {
	_input_keys[0] = FALSE;
	_input_keys[1] = FALSE;
	_input_keys[2] = FALSE;
	_input_keys[3] = FALSE;
	_input_keys[4] = FALSE;
	_input_keys[5] = FALSE;
}
