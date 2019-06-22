//#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import "PadViewController.h"
#import "CommonFuncs.h"
#import "AppDelegate.h"

// コントローラ画面
@implementation PadViewController {
    BOOL _bgWaiting;
//    SystemSoundID _sound;         // iOSのver変更により不使用
}

#pragma mark - Override

- (id)initWithCoder:(NSCoder*)aDecoder {
    self = [super initWithCoder:aDecoder];
    if (self != nil) {
        // Initialize
        self.inputKeys = [NSMutableArray arrayWithObjects:@NO, @NO, @NO, @NO, @NO, @NO, nil];
        self.manualCloseFlag = [NSNumber numberWithBool:NO];
        _bgWaiting = NO;
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
	// Do any additional setup after loading the view.
    
    // View要素初期作成
    [self initializeViewContents];
    // 効果音初期化
    [self initSound];
    // アプリ閉じる通知ON
    [self resignActiveNotificationWithSetflag:YES];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)viewDidUnload {
    // アプリ閉じる通知OFF
    [self resignActiveNotificationWithSetflag:NO];
    // 効果音後処理
    [self disposeSound];
    
    [super viewDidUnload];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation {
    return (toInterfaceOrientation == UIInterfaceOrientationLandscapeLeft);
}

#pragma mark - Initialize View Contents

// View要素初期作成
- (void)initializeViewContents {
    [self setPadImage];    // パッド画像
    [self setButtons];     // パッド内ボタン
    [self setSwitch];      // 接続スイッチ
}

// パッド画像
- (void)setPadImage {
    UIImageView* imageView = [[UIImageView alloc] init];
    UIImage* image;
    if ([CommonFuncs is4inch]) {
        image = [UIImage imageNamed:@"Pad-568h"];
    } else {
        image = [UIImage imageNamed:@"Pad"];
    }
    [imageView setUserInteractionEnabled:YES];
    [imageView setImage:image];
    [imageView setFrame:CGRectMake(self.view.frame.origin.x, self.view.frame.origin.y, image.size.width, image.size.height)];
    [self.view addSubview:imageView];
    
    [self.view setFrame:CGRectMake(self.view.frame.origin.x, self.view.frame.origin.y, image.size.height - 2.0, image.size.width)];
}

// パッド内ボタン
- (void)setButtons {
    [self setABButton];
    [self setCrossButton];
}

// 接続スイッチ
- (void)setSwitch {
    UISwitch* netSwitch = [[UISwitch alloc] init];
    [netSwitch setTag:NET_SWITCH_TAG];
    CGRect rect = netSwitch.frame;
    rect.origin = CGPointMake(242.0, 230.0);
    netSwitch.frame = rect;
    [netSwitch setOn:NO];
    [netSwitch addTarget:self action:@selector(netSwitchChanged:) forControlEvents:UIControlEventValueChanged];
    [self.view addSubview:netSwitch];
}

#pragma mark - Credit Sound

// 効果音初期化
- (void)initSound {
    NSString* path = [[NSBundle mainBundle] pathForResource:@"button" ofType:@"wav"];
    NSURL* fileUrl = [NSURL fileURLWithPath:path];
//    AudioServicesCreateSystemSoundID(CFBridgingRetain(fileUrl), &_sound);
    self.button_sound = [[AVAudioPlayer alloc] initWithContentsOfURL:fileUrl error:NULL];
}

// 効果音実行
- (void)playButtonSound {
//    AudioServicesPlaySystemSound(_sound);
    [self.button_sound play];
}

// 効果音後処理
- (void)disposeSound {
//    AudioServicesDisposeSystemSoundID(_sound);
}

#pragma mark - Net Switch

// スイッチ切り替えイベント
- (void)netSwitchChanged:(UISwitch*)netSwitch {
    BOOL flag = netSwitch.on;
    
    if (_bgWaiting) {
        // backgroundがスイッチOFF処理に対応前
        netSwitch.on = NO;
        return;
    }
    
    if (flag) {
        // スイッチON
        @synchronized(self.manualCloseFlag) {
            self.manualCloseFlag = [NSNumber numberWithBool:NO];
        }
        // バックグラウンド処理起動
        [self startBackground];
    }
    else {
        // スイッチOFF backgroundに通知
        _bgWaiting = YES;
        @synchronized(self.manualCloseFlag) {
            self.manualCloseFlag = [NSNumber numberWithBool:YES];
        }
    }
}

// スイッチ状態のセット
- (void)setNetSwitchStatusWithFlag:(BOOL)flag {
    UISwitch* netSwitch = (UISwitch*)[self.view viewWithTag:NET_SWITCH_TAG];
    [netSwitch setOn:flag];
}

#pragma mark - App Close Notification

// アプリ閉じる通知のセット
- (void)resignActiveNotificationWithSetflag:(BOOL)set {
    NSNotificationCenter* center = [NSNotificationCenter defaultCenter];
    if (set) {
        [center addObserver:self
                   selector:@selector(applicationWillResignActive)
                       name:UIApplicationWillResignActiveNotification
                     object:nil];
    } else {
        [center removeObserver:self];
    }
}

// アプリ閉じるイベント
- (void)applicationWillResignActive {
    // スイッチOFF
    [self setNetSwitchStatusWithFlag:NO];
    // backgroundに通知
    _bgWaiting = YES;
    @synchronized(self.manualCloseFlag) {
        self.manualCloseFlag = [NSNumber numberWithBool:YES];
    }
}

#pragma mark - Background

// バックグランド処理起動
- (void)startBackground {
    [self performSelectorInBackground:@selector(backgroundMain:) withObject:nil];
}

// バックグラウンド処理停止の通知取得
- (void)backgroundStopped:(NSNumber*)param {
    NSInteger result = [param integerValue];
    
    UIAlertView* alert = nil;
    switch (result) {
        case BG_RESULT_NETWORK_FAILED: {
            // スイッチOFF
            [self setNetSwitchStatusWithFlag:NO];
            alert = [[UIAlertView alloc] initWithTitle:@"ネットワーク" message:@"ネットワーク接続に失敗しました。\n設定を確認してください。" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        }
            break;
        case BG_RESULT_NETWORK_CLOSED: {
            // スイッチOFF
            [self setNetSwitchStatusWithFlag:NO];
            alert = [[UIAlertView alloc] initWithTitle:@"ネットワーク" message:@"ネットワーク接続が切断されました。" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        }
            break;
        case BG_RESULT_NETWORK_UNKNOWN: {
            // スイッチOFF
            [self setNetSwitchStatusWithFlag:NO];
            alert = [[UIAlertView alloc] initWithTitle:@"サーバー接続" message:@"接続先サーバーが不明です。" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        }
            break;
        case BG_RESULT_MANUAL_OFF: {      // 手動OFFへの対応完了通知
            _bgWaiting = NO;
        }
            break;
    }
    
    if (alert != nil) {
        [alert show];
    }
}

@end
