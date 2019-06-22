#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import "CommonDefine.h"

// コントローラ画面
@interface PadViewController : UIViewController <NSStreamDelegate>

@property (atomic, strong) NSMutableArray* inputKeys;
@property (atomic, strong) NSNumber* manualCloseFlag;

@property AVAudioPlayer *button_sound;

- (void)playButtonSound;
- (void)backgroundStopped:(NSNumber*)param;

@end

// ボタン生成
@interface PadViewController (CreateButtons)
- (void)setABButton;
- (void)setCrossButton;
@end

// ボタンイベント
@interface PadViewController (ButtonEvents)
- (void)padButtonPushed:(UIButton*)button;
- (void)padButtonReleased:(UIButton*)button;
@end

// バックグラウンド
@interface PadViewController (Background)
- (void)backgroundMain:(id)param;
@end

// 定数
typedef enum {
    NET_SWITCH_TAG = VIEWTAG_PAD,
    KEYS_TAG_FIRST
} ENpadviewtag;

typedef enum {
    KEYS_INDEX_A = 0,
    KEYS_INDEX_B,
    KEYS_INDEX_UP,
    KEYS_INDEX_DOWN,
    KEYS_INDEX_LEFT,
    KEYS_INDEX_RIGHT
} ENkeysindex;

typedef enum {
    BG_RESULT_NETWORK_CLOSED,
    BG_RESULT_NETWORK_FAILED,
    BG_RESULT_NETWORK_UNKNOWN,
    BG_RESULT_MANUAL_OFF
} ENbgresult;
