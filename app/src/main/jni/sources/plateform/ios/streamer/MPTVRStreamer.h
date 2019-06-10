//
//  MPTVRStreamer.h
//  MPTVideoKit
//
//  Created by Khazmovsky on 15/11/27.
//  Copyright © 2015年 Xuan Yi Xia CO.LTD. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>

typedef void (^MPTVRStreamerStreamingReadyCallback)(CVPixelBufferRef buffer, CMTime time, unsigned int frameIndex, bool isSeek);
typedef enum {
    MPTVRStreamerPlayStatusUnknown,
    MPTVRStreamerPlayStatusLoad,
    MPTVRStreamerPlayStatusPlay,
    MPTVRStreamerPlayStatusPause
} MPTVRStreamerPlayStatus;

typedef enum {
    MPTVRStreamerLoadStatusUnknown,
    MPTVRStreamerLoadStatusFailed,
    MPTVRStreamerLoadStatusReadyToPlay
} MPTVRStreamerLoadStatus;

@class MPTVRStreamer;
@protocol MPTVRStreamerDelegate <NSObject>

- (void)streamer:(MPTVRStreamer *)streamer handleLoadStatus:(MPTVRStreamerLoadStatus)status;
- (void)streamer:(MPTVRStreamer *)streamer handlePlayStatus:(MPTVRStreamerPlayStatus)status;

@end

@interface MPTVRStreamer : NSObject

@property (nonatomic, copy  ) MPTVRStreamerStreamingReadyCallback decodeReadyCallback;
@property (nonatomic, copy  ) void (^playbackCallback)(dispatch_block_t block);
@property (nonatomic, copy  ) void (^playingCallback)(CGFloat);
@property (nonatomic, copy  ) void (^playingPreparedCallback)(void);
@property (nonatomic, weak) id<MPTVRStreamerDelegate> delegate;

@property (nonatomic, readonly) AVPlayerItem *currentItem;
@property (nonatomic, readonly) CMTime currentTime;

@property (nonatomic, assign) CGFloat volume;
@property (nonatomic, assign) CGFloat rate;
@property (nonatomic, assign) CMTimeRange videoRange;
@property (nonatomic, assign) BOOL isPlaying;

@property (nonatomic, readonly) CGFloat fps;
@property (nonatomic, readonly) CGFloat duration;

- (id)initWithURL:(NSString *)url;
- (id)initWithPlayerItem:(AVPlayerItem *)playeritem;
- (id)initWithPlayerItem:(AVPlayerItem *)playeritem delegate:(id<MPTVRStreamerDelegate>)delegate;

- (void)play;
- (void)pause;
- (void)seekToTime:(CMTime)time;
- (void)seekToTime:(CMTime)time withHanldeDeocde:(BOOL)handleDeocde;
- (void)seekToTime:(CMTime)time completionHandler:(void (^)())completionHandler;
- (void)seekToTime:(CMTime)time withHandleDecode:(BOOL)decode completionHandler:(void (^)())completionHandler;

- (CVPixelBufferRef)copyAtTime:(CMTime)time;

- (void)deallocAction;

@end

typedef MPTVRStreamer YXVideoStreamer;
