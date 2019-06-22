//#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import "creditViewController.h"
#import "PadViewController.h"
#import "CommonFuncs.h"
#import "CommonDefine.h"

// 初期クレジット画面
@implementation creditViewController {
    PadViewController* _padViewController;
//    SystemSoundID _sound;         // iOSのver変更により不使用
}

// 定数
typedef enum {
    LOGO_VIEW_TAG = VIEWTAG_FIRST
} ENviewtag;

#pragma mark - Override

- (void)viewDidLoad {
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
    
    // splashと同一イメージのセット
    [self setDefaultImage];
    // 効果音初期化
    [self initSound];
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    
    // 効果音実行
    [self playCreditSound];            // COIN sound
    
    // 待機
    [NSThread sleepForTimeInterval:1.4];
    // アニメーションで次のViewへ移行
    UIView* padView = [self createPadView];
    [self logoAnimation:padView];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)viewDidUnload {
    // 効果音後処理
    [self disposeSound];
    _padViewController = nil;
    
    [super viewDidUnload];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation {
    return (toInterfaceOrientation == UIInterfaceOrientationLandscapeLeft);
}

#pragma mark - Set Default Image

// splashと同一イメージのセット
- (void)setDefaultImage {
    [self.view setBackgroundColor:[UIColor blackColor]];
    
    UIImageView* imageView = [[UIImageView alloc] init];
    [imageView setTag:LOGO_VIEW_TAG];
    UIImage* image;
    if ([CommonFuncs is4inch]) {
        image = [UIImage imageNamed:@"Default-568h"];
    } else {
        image = [UIImage imageNamed:@"Default"];
    }
    UIImage* useImage = [UIImage imageWithCGImage:image.CGImage scale:image.scale orientation:UIImageOrientationRight];
    [imageView setImage:useImage];
    [imageView setFrame:CGRectMake(self.view.frame.origin.x, self.view.frame.origin.y, useImage.size.width, useImage.size.height)];
    [self.view addSubview:imageView];
}

#pragma mark - Credit Sound

// 効果音初期化
- (void)initSound {
    NSString* path = [[NSBundle mainBundle] pathForResource:@"credit" ofType:@"wav"];
    NSURL* fileUrl = [NSURL fileURLWithPath:path];
//    AudioServicesCreateSystemSoundID(CFBridgingRetain(fileUrl), &_sound);
    self.credit_sound = [[AVAudioPlayer alloc] initWithContentsOfURL:fileUrl error:NULL];
}

// 効果音実行
- (void)playCreditSound {
//    AudioServicesPlaySystemSound(_sound);
    [self.credit_sound play];
}

// 効果音後処理
- (void)disposeSound {
//    AudioServicesDisposeSystemSoundID(_sound);
}

#pragma mark - Create Next View

// 次のViewを生成
- (UIView*)createPadView {
    PadViewController* vc = [[self storyboard] instantiateViewControllerWithIdentifier:@"PadViewController"];
    _padViewController = vc;
    UIView* childView = vc.view;
    childView.frame = CGRectMake(self.view.frame.origin.x, self.view.frame.origin.y, childView.bounds.size.height, childView.bounds.size.width);
    childView.alpha = 0.0;
    [self.view addSubview:childView];
    return childView;
}

#pragma mark - View Animation

// 移動アニメーション
- (void)logoAnimation:(UIView*)nextView {
    UIView* logoView = [self.view viewWithTag:LOGO_VIEW_TAG];
    CGRect moveRect = logoView.frame;
    moveRect.origin.y += ((moveRect.size.height / 2.0) + 40.0);
    [UIView animateWithDuration:1.4
                     animations:^{
                         logoView.frame = moveRect;
                     }
                     completion:^(BOOL finished) {
                         if (finished){
                             [self nextViewAnimation:nextView];
                         }
                     }];
}

// 表示アニメーション
- (void)nextViewAnimation:(UIView*)nextView {
    [UIView animateWithDuration:0.4
                     animations:^{
                         nextView.alpha = 1.0;
                     }
                     completion:^(BOOL finished) {
                         if (finished){
                             UIView* logoView = [self.view viewWithTag:LOGO_VIEW_TAG];
                             [logoView removeFromSuperview];
                         }
                     }];
}

@end
