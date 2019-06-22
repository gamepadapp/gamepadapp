#import <Foundation/Foundation.h>

// システム共通メソッド
@interface CommonFuncs : NSObject

+ (BOOL)is4inch;                                   // 端末が4inchか
+ (BOOL)isRetina;                                  // Retinaタイプか
+ (CGFloat)screenHeight;                           // Screen領域 縦

@end
