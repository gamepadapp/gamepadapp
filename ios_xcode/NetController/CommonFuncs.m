#import "CommonFuncs.h"
#import "CommonDefine.h"

// システム共通メソッド
@implementation CommonFuncs

#pragma mark - Application Info

// 端末が4inchか
+ (BOOL)is4inch {
    CGSize screenSize = [[UIScreen mainScreen] bounds].size;
    return (screenSize.width == 568.0 /*APP_SCREEN_WIDTH*/ && screenSize.height == 320.0 /*APP_SCREEN_HEIGHT_40INCH*/);
}

// Retinaタイプか
+ (BOOL)isRetina {
    return ([[UIScreen mainScreen] scale] == 2.0);
}

// Screen領域 縦
+ (CGFloat)screenHeight {
    return [[UIScreen mainScreen] bounds].size.height;
}

@end
