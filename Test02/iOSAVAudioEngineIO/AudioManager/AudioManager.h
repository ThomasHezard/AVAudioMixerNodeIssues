//
//  AudioManager.h
//  iOSAVAudioEngineIO
//
//  Created by Thomas HEZARD on 05/12/2018.
//

#import <AVFoundation/AVFoundation.h>
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface AudioManager : NSObject

+ (AudioManager* _Nonnull) instance;

- (void)start;

- (void)log;

/* Player */
- (void)play;
- (void)pause;
- (void)stop;
@end

NS_ASSUME_NONNULL_END
