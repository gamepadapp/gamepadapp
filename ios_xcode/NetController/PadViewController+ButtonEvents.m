#import "PadViewController.h"
#import "CommonFuncs.h"

// コントローラ画面 ボタンイベント
@implementation PadViewController (ButtonEvents)

#pragma mark - Button Push Event

- (void)padButtonPushed:(UIButton*)button {
    NSInteger index = (button.tag - KEYS_TAG_FIRST);
    @synchronized(self.inputKeys) {
        NSNumber* flag = @YES;
        [self.inputKeys replaceObjectAtIndex:index withObject:flag];
    }
    [self playButtonSound];
}

#pragma mark - Button Release Event

- (void)padButtonReleased:(UIButton*)button {
    NSInteger index = (button.tag - KEYS_TAG_FIRST);
    @synchronized(self.inputKeys) {
        NSNumber* flag = @NO;
        [self.inputKeys replaceObjectAtIndex:index withObject:flag];
    }
}

@end
