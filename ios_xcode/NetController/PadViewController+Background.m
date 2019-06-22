#import "PadViewController.h"
#import "CommonFuncs.h"
#import "AppDelegate.h"

// 通信関連
static NSMutableData* receivedData = nil;
static NSInputStream* inStream = nil;;
static NSOutputStream* outStream = nil;
static CFReadStreamRef readStream = NULL;
static CFWriteStreamRef writeStream = NULL;

// ネットワークCloseフラグ
static BOOL networkClosed = NO;

// ヘッダメッセージ
#define  HEADER_MSG_SMPHONE  @"ThisIsNetControllerSMPhoneHeader"
#define  HEADER_MSG_OK       @"HEADER_OK"
// ヘッダメッセージサイズ
#define  SIZE_HEADER_OK  9
// 入力データチェックキャラクタ
#define  CHECK_INPUT_CHARACTER  'Z'

// コントローラ画面 バックグラウンド
@implementation PadViewController (Background)

- (void)dealloc {
    [self networkDisconnect];
    if (readStream != NULL) {
        CFRelease(readStream);
    }
    if (writeStream != NULL) {
        CFRelease(writeStream);
    }
}

- (void)backgroundMain:(id)param {
    
    // 受信済みデータ
    receivedData = [[NSMutableData alloc] init];
    // 進行度
    NSInteger stage = 1;
    
    // 接続開始
    BOOL start = [self networkConnectWithAddress:SERVER_ADDRESS port:SERVER_PORT];
    if (!start) {
        // 失敗を通知
        NSInteger result = BG_RESULT_NETWORK_FAILED;
        [self performSelectorOnMainThread:@selector(backgroundStopped:) withObject:[NSNumber numberWithInteger:result] waitUntilDone:NO];
        return;
    }
    
    // ヘッダ送信
    [self sendHeaderToServer];
    stage = 2;
    
    // データ処理ループ
    while (YES) {
        
        // 0.03秒待機 イベント処理可能
        [[NSRunLoop currentRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.03]];
        
        if (stage == 2) {
            // サーバー応答のチェック
            NSInteger receivedLen = receivedData.length;
            if (receivedLen >= SIZE_HEADER_OK) {
                BOOL header_ok = NO;
                
                // 応答メッセージチェック
                if (receivedLen == SIZE_HEADER_OK) {
                    NSString* checkStr = [[NSString alloc] initWithData:receivedData encoding:NSASCIIStringEncoding];
                    if ([checkStr isEqualToString:HEADER_MSG_OK]) {
                        header_ok = YES;
                    }
                }
                
                // 相手サーバーが不明
                if (!header_ok) {
                    [self networkDisconnect];
                    // 対応を通知
                    NSInteger result = BG_RESULT_NETWORK_UNKNOWN;
                    [self performSelectorOnMainThread:@selector(backgroundStopped:) withObject:[NSNumber numberWithInteger:result] waitUntilDone:NO];
                    return;
                }
                
                NSLog(@"server check OK");
                stage = 3;
            }
        }
        else {
            // キー入力状態取得
            NSArray* inputKeys;
            @synchronized(self.inputKeys) {
                inputKeys = [self.inputKeys copy];
            }
            // キー入力情報送信
            [self sendInputToServerWithInputkeys:inputKeys];
        }
        
        // ネットワークCloseフラグ
        if (networkClosed) {
            networkClosed = NO;
            [self networkDisconnect];
            // 対応を通知
            NSInteger result = BG_RESULT_NETWORK_CLOSED;
            [self performSelectorOnMainThread:@selector(backgroundStopped:) withObject:[NSNumber numberWithInteger:result] waitUntilDone:NO];
            return;
        }
        
        // ユーザー操作での通信Closeを検出
        BOOL closeFlag;
        @synchronized(self.manualCloseFlag) {
            closeFlag = [self.manualCloseFlag boolValue];
        }
        if (closeFlag) {
            [self networkDisconnect];
            // 対応を通知
            NSInteger result = BG_RESULT_MANUAL_OFF;
            [self performSelectorOnMainThread:@selector(backgroundStopped:) withObject:[NSNumber numberWithInteger:result] waitUntilDone:NO];
            return;
        }
    }
}

#pragma mark - Connect/Disconnect

// 接続
- (BOOL)networkConnectWithAddress:(NSString*)address port:(NSInteger)port {
    
    CFStreamCreatePairWithSocketToHost(kCFAllocatorDefault, (__bridge CFStringRef)address, (UInt32)port, &readStream, &writeStream);
    if (readStream == NULL || writeStream == NULL) {
        return NO;
    }
    
    CFReadStreamSetProperty(readStream, kCFStreamPropertyShouldCloseNativeSocket, kCFBooleanTrue);
    CFWriteStreamSetProperty(writeStream, kCFStreamPropertyShouldCloseNativeSocket, kCFBooleanTrue);
    
    inStream = (__bridge NSInputStream*)readStream;
    [inStream setDelegate:self];
    [inStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [inStream open];
    
    outStream = (__bridge NSOutputStream*)writeStream;
    [outStream setDelegate:self];
    [outStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [outStream open];
    
    return YES;
}

// 接続解除
- (void)networkDisconnect {
    if (inStream != nil) {
        [inStream close];
    }
    if (outStream != nil) {
        [outStream close];
    }
}

#pragma mark - Send Data

// ヘッダ送信
- (void)sendHeaderToServer {
    [self writeToServerWithString:HEADER_MSG_SMPHONE];
}

// キー入力情報送信
- (void)sendInputToServerWithInputkeys:(NSArray*)inputKeys {
    uint8_t buf[7];
    
    buf[0] = CHECK_INPUT_CHARACTER;
    
    for (NSInteger i = 0; i < 6; i++) {
        NSNumber* flag = [inputKeys objectAtIndex:i];
        buf[i + 1] = (uint8_t)[flag boolValue];
    }
    
    [outStream write:buf maxLength:7];
}

// サーバへの文字列送信実行
- (void)writeToServerWithString:(NSString*)str {
    const uint8_t* buf = (const uint8_t*)[str cStringUsingEncoding:NSASCIIStringEncoding];
    [outStream write:buf maxLength:strlen((const char*)buf)];
}

#pragma mark - Network Event

// ネットワークイベント発生
- (void)stream:(NSStream*)aStream handleEvent:(NSStreamEvent)eventCode {
    
    if (eventCode == NSStreamEventErrorOccurred) {
        NSLog(@"ERROR");
        // エラー発生
        networkClosed = YES;
        return;
    }
    
    // データ取得の場合以外は処理しない
    if (eventCode != NSStreamEventHasBytesAvailable) {
        return;
    }
    
    // 入力ストリームとしてデータを取得
    NSInputStream* stream = (NSInputStream*)aStream;
    uint8_t buf[1024];
    NSInteger len = 0;
    len = [stream read:buf maxLength:1024];
    if (len == 0 || len == -1) {
        NSLog(@"CLOSE");
        // 取得データなし
        networkClosed = YES;
        return;
    }
    
    // 取得済みデータと結合
    [receivedData appendBytes:buf length:len];
}

@end
