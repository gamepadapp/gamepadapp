#import "PadViewController.h"
#import "CommonFuncs.h"

// コントローラ画面 ボタン生成
@implementation PadViewController (CreateButtons)

- (void)setABButton {
    UIImage* buttonImage = [UIImage imageNamed:@"button_ab"];
    UIImage* buttonImageHigh = [UIImage imageNamed:@"button_ab_high"];
    
    UIButton* aButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [aButton setTag:(KEYS_TAG_FIRST + KEYS_INDEX_A)];
    [aButton setImage:buttonImage forState:UIControlStateNormal];
    [aButton setImage:buttonImageHigh forState:UIControlStateHighlighted];
    [aButton setFrame:CGRectMake(453.0, 215.0, buttonImage.size.width, buttonImage.size.height)];
    [self.view addSubview:aButton];
    [aButton addTarget:self action:@selector(padButtonPushed:) forControlEvents:UIControlEventTouchDown];
    [aButton addTarget:self action:@selector(padButtonReleased:) forControlEvents:UIControlEventTouchUpInside];
    [aButton addTarget:self action:@selector(padButtonReleased:) forControlEvents:UIControlEventTouchUpOutside];
    
    UIButton* bButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [bButton setTag:(KEYS_TAG_FIRST + KEYS_INDEX_B)];
    [bButton setImage:buttonImage forState:UIControlStateNormal];
    [bButton setImage:buttonImageHigh forState:UIControlStateHighlighted];
    [bButton setFrame:CGRectMake(373.0, 215.0, buttonImage.size.width, buttonImage.size.height)];
    [self.view addSubview:bButton];
    [bButton addTarget:self action:@selector(padButtonPushed:) forControlEvents:UIControlEventTouchDown];
    [bButton addTarget:self action:@selector(padButtonReleased:) forControlEvents:UIControlEventTouchUpInside];
    [bButton addTarget:self action:@selector(padButtonReleased:) forControlEvents:UIControlEventTouchUpOutside];
}

- (void)setCrossButton {
    UIImage* buttonImage = [UIImage imageNamed:@"button_cross"];
    UIImage* buttonImageHigh = [UIImage imageNamed:@"button_cross_high"];
    UIImage* useImage;
    UIImage* useImageHith;
    
    UIButton* upButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [upButton setTag:(KEYS_TAG_FIRST + KEYS_INDEX_UP)];
    [upButton setImage:buttonImage forState:UIControlStateNormal];
    [upButton setImage:buttonImageHigh forState:UIControlStateHighlighted];
    [upButton setFrame:CGRectMake(79.0, 149.0, buttonImage.size.width, buttonImage.size.height)];
    [self.view addSubview:upButton];
    [upButton addTarget:self action:@selector(padButtonPushed:) forControlEvents:UIControlEventTouchDown];
    [upButton addTarget:self action:@selector(padButtonReleased:) forControlEvents:UIControlEventTouchUpInside];
    [upButton addTarget:self action:@selector(padButtonReleased:) forControlEvents:UIControlEventTouchUpOutside];
    
    useImage = [UIImage imageWithCGImage:buttonImage.CGImage scale:buttonImage.scale orientation:UIImageOrientationDown];
    useImageHith = [UIImage imageWithCGImage:buttonImageHigh.CGImage scale:buttonImageHigh.scale orientation:UIImageOrientationDown];
    
    UIButton* downButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [downButton setTag:(KEYS_TAG_FIRST + KEYS_INDEX_DOWN)];
    [downButton setImage:useImage forState:UIControlStateNormal];
    [downButton setImage:useImageHith forState:UIControlStateHighlighted];
    [downButton setFrame:CGRectMake(79.0, 232.0, useImage.size.width, useImage.size.height)];
    [self.view addSubview:downButton];
    [downButton addTarget:self action:@selector(padButtonPushed:) forControlEvents:UIControlEventTouchDown];
    [downButton addTarget:self action:@selector(padButtonReleased:) forControlEvents:UIControlEventTouchUpInside];
    [downButton addTarget:self action:@selector(padButtonReleased:) forControlEvents:UIControlEventTouchUpOutside];
    
    useImage = [UIImage imageWithCGImage:buttonImage.CGImage scale:buttonImage.scale orientation:UIImageOrientationLeft];
    useImageHith = [UIImage imageWithCGImage:buttonImageHigh.CGImage scale:buttonImageHigh.scale orientation:UIImageOrientationLeft];
    
    UIButton* leftButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [leftButton setTag:(KEYS_TAG_FIRST + KEYS_INDEX_LEFT)];
    [leftButton setImage:useImage forState:UIControlStateNormal];
    [leftButton setImage:useImageHith forState:UIControlStateHighlighted];
    [leftButton setFrame:CGRectMake(39.0, 190.0, useImage.size.width, useImage.size.height)];
    [self.view addSubview:leftButton];
    [leftButton addTarget:self action:@selector(padButtonPushed:) forControlEvents:UIControlEventTouchDown];
    [leftButton addTarget:self action:@selector(padButtonReleased:) forControlEvents:UIControlEventTouchUpInside];
    [leftButton addTarget:self action:@selector(padButtonReleased:) forControlEvents:UIControlEventTouchUpOutside];
    
    useImage = [UIImage imageWithCGImage:buttonImage.CGImage scale:buttonImage.scale orientation:UIImageOrientationRight];
    useImageHith = [UIImage imageWithCGImage:buttonImageHigh.CGImage scale:buttonImageHigh.scale orientation:UIImageOrientationRight];
    
    UIButton* rightButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [rightButton setTag:(KEYS_TAG_FIRST + KEYS_INDEX_RIGHT)];
    [rightButton setImage:useImage forState:UIControlStateNormal];
    [rightButton setImage:useImageHith forState:UIControlStateHighlighted];
    [rightButton setFrame:CGRectMake(122.0, 190.0, useImage.size.width, useImage.size.height)];
    [self.view addSubview:rightButton];
    [rightButton addTarget:self action:@selector(padButtonPushed:) forControlEvents:UIControlEventTouchDown];
    [rightButton addTarget:self action:@selector(padButtonReleased:) forControlEvents:UIControlEventTouchUpInside];
    [rightButton addTarget:self action:@selector(padButtonReleased:) forControlEvents:UIControlEventTouchUpOutside];
}

@end
