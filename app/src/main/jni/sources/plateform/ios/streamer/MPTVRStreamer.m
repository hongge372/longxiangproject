//
//  MPTVRStreamer.m
//  MPTVideoKit
//
//  Created by Khazmovsky on 15/11/27.
//  Copyright © 2015年 Xuan Yi Xia CO.LTD. All rights reserved.
//

#import "MPTVRStreamer.h"
#import <UIKit/UIKit.h>


@interface MPTVRStreamer () {
    // stream
    AVPlayer *m_player;
    AVPlayerItem *m_playerItem;
    AVPlayerItemVideoOutput *m_item_videoOutput;
    id timeObserver;
    id boundrayTimeObserver;
    
    // play control
    BOOL m_streamer_should_play;
    
    // ctx
    void *player_observe_ctx;
    void *item_playback_observe_ctx;
    void *item_status_observe_ctx;
    
    int faliedCount;
    
    bool status_observer_created;
    
    int fps;
    
    dispatch_semaphore_t sema;
}

@property (nonatomic, strong) NSString *url;
@property (nonatomic, strong) AVPlayerItem *playeritem;
@property (nonatomic, copy) dispatch_block_t seekingBlock;

@property (nonatomic, assign) CGFloat fps;
@property (nonatomic, assign) CGFloat duration;


@end

@implementation MPTVRStreamer

- (void)deallocAction
{
    [self pause];
    NSLog(@"%s %p", __FUNCTION__, self);
    [self removeObserver:timeObserver forPlayer:m_player];
    timeObserver = 0;
    [self removeObserver:boundrayTimeObserver forPlayer:m_player];
    boundrayTimeObserver = 0;
    [self removePlaybackObserverForItem:m_playerItem];
    [self removeNote];
    [self removeStatusObserverForItem:m_playerItem];
    m_player = 0;
    m_playerItem = 0;
}

- (void)dealloc {
    [self deallocAction];
}

- (id)initWithURL:(NSString *)url {
    if (self = [super init]) {
        self.url = url;
        if (self.url) {
            [self commonInit];
            [self initStreamer:nil];
            [self initNote];
        }
    }
    return self;
}

- (id)initWithPlayerItem:(AVPlayerItem *)playeritem {
    return [self initWithPlayerItem:playeritem delegate:nil];
}

- (id)initWithPlayerItem:(AVPlayerItem *)playeritem delegate:(id<MPTVRStreamerDelegate>)delegate {
    if (self = [super init]) {
        self.delegate = delegate;
        [self commonInit];
        [self initStreamer:playeritem];
        [self initNote];
    }
    return self;
}

// note
- (void)initNote {
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didEnterBackground) name:UIApplicationDidEnterBackgroundNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(willResignActive) name:UIApplicationWillResignActiveNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didBecomeActive) name:UIApplicationDidBecomeActiveNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleplayback:) name:AVPlayerItemDidPlayToEndTimeNotification object:nil];
}

- (void)removeNote {
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIApplicationDidEnterBackgroundNotification object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIApplicationWillResignActiveNotification object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIApplicationDidBecomeActiveNotification object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:AVPlayerItemDidPlayToEndTimeNotification object:nil];
}

- (void)didEnterBackground {
    [self pause];
}

- (void)willResignActive {
    //[self pause];
}

- (void)didBecomeActive {
    //[self play];
}

- (void)handleplayback:(NSNotification *)notif {
    if ([notif.object isEqual:self.currentItem])
    {
        NSLog(@"streamer playback seeking...");
        [self reload];
    }
}

- (void)reload
{
    [self seekToTime:self.videoRange.start completionHandler:^{
        NSLog(@"streamer playback seeking done");
        if (self->_playbackCallback)
        {
            self->_playbackCallback(^
                                    {
                                        [self handleDecode:self.videoRange.start];
                                    });
        }
    }];
}

- (BOOL)hasVideo
{
    return [m_playerItem.asset tracksWithMediaType:AVMediaTypeVideo].count;
}

- (void)commonInit
{
    _rate = 1.0;
    _volume = 1.0;
    player_observe_ctx = &player_observe_ctx;
    item_playback_observe_ctx = &item_playback_observe_ctx;
    item_status_observe_ctx = &item_status_observe_ctx;
    m_player = [AVPlayer new];
    sema = dispatch_semaphore_create(0);
}

- (void)createTOB
{
    if (timeObserver) [self destroyTOB];
    __weak typeof(self) wself = self;
    timeObserver = [self addTimeObserverForPlayer:m_player callback:^(CMTime time)
                    {
                        typeof(wself) sself = wself;
                        if (sself.hasVideo)
                        {
                            [sself handleDecode:time isSeek:0];
                        }
                    }];
}

- (void)destroyTOB
{
    [self removeObserver:timeObserver forPlayer:m_player];
    timeObserver = 0;
}

- (AVPlayerItem *)itemFromFile:(NSURL *)url {
    AVMutableComposition *composition = [AVMutableComposition composition];
    AVURLAsset *asset = [AVURLAsset assetWithURL:url];
    [composition insertTimeRange:CMTimeRangeMake(kCMTimeZero, asset.duration) ofAsset:asset atTime:kCMTimeZero error:0];
    
    AVPlayerItem *item = [AVPlayerItem playerItemWithAsset:composition];
    return item;
}

// item
- (void)initStreamer:(AVPlayerItem *)item
{
    if (m_playerItem) {
        [self removeStatusObserverForItem:m_playerItem];
    }
    
    m_playerItem = item;
    
    BOOL fromRemoteHost = [self.url hasPrefix:@"http://"];
    
    if (self.url) {
        m_playerItem = [self itemFromFile:[NSURL fileURLWithPath:self.url]];
    }

    
    [self addStatusObserverForItem:item];
    
    if (fromRemoteHost == NO) {
        //        __unsafe_unretained typeof(self) wself = self;
        //        boundrayTimeObserver =[m_player addBoundaryTimeObserverForTimes:@[[NSValue valueWithCMTime:CMTimeSubtract(m_playerItem.asset.duration, CMTimeMake(1, 10))]] queue:dispatch_get_main_queue() usingBlock:^{
        ////            [wself pause];
        //            [wself->m_player seekToTime:kCMTimeZero toleranceBefore:kCMTimeZero toleranceAfter:kCMTimeZero completionHandler:^(BOOL finished) {
        //                if (finished) {
        //                    if (wself->_playbackCallback) {
        //                        wself->_playbackCallback();
        //                    }
        //                }
        //            }];
        //        }];
    }
    
    if (self.hasVideo)
    {
        m_item_videoOutput = [[AVPlayerItemVideoOutput alloc] initWithPixelBufferAttributes:@{(id)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_32BGRA),                                                                                           @"IOSurfaceOpenGLESTextureCompatibility": @YES,                                                                                           @"IOSurfaceOpenGLESFBOCompatibility": @YES,
                                                                                              }];
        [m_item_videoOutput setSuppressesPlayerRendering:YES];
        [m_playerItem addOutput:m_item_videoOutput];
        
        fps = [m_playerItem.asset tracksWithMediaType:AVMediaTypeVideo][0].nominalFrameRate;
    } else {
        fps = 30;
    }
    [m_player replaceCurrentItemWithPlayerItem:m_playerItem];

}

- (void)replaceCurrentItemWithPlayerItem:(AVPlayerItem *)playeritem
{
    [self initStreamer:playeritem];
}

- (void)handleDecode:(CMTime)time
{
    [self handleDecode:time isSeek:0];
}

- (void)handleDecode:(CMTime)time isSeek:(bool)isSeek
{
    CVPixelBufferRef buffer = 0;
    CMTime realTime;
    int tryCount = 0;
    do {}
    while (++tryCount < isSeek * 100 && ![self->m_item_videoOutput hasNewPixelBufferForItemTime:time]);
    if ((buffer = [self->m_item_videoOutput copyPixelBufferForItemTime:time itemTimeForDisplay:&realTime]))
    {
        if (self.playingCallback) {
            self.playingCallback(CMTimeGetSeconds(time));
        }
        if (self.decodeReadyCallback && buffer) {
            self.decodeReadyCallback(buffer, time, CMTimeGetSeconds(time) * fps, isSeek);
        }
        if (buffer) {
            CFRelease(buffer);
        }
        faliedCount = 0;
    }
    else if (m_playerItem.status == AVPlayerItemStatusReadyToPlay)
    {
        faliedCount++;
        if (faliedCount >= 30)
        {
//            printf("warning:: cant get decoded video buffer data, use Plan B.\n");
//            [self planB];
            faliedCount = 0;
        }
    }
}

- (void)planB
{
    [self pause];
    CMTimeRange r = self.videoRange;
    CMTime f = CMTimeMakeWithSeconds(0.001, 30);
    CMTime s = CMTimeAdd(r.start, f);
    CMTime d = CMTimeSubtract(r.duration, f);
    self.videoRange = CMTimeRangeMake(s, d);
    [self play];
}

- (id)addTimeObserverForPlayer:(AVPlayer *)player callback:(void (^)(CMTime))callback {
    return [player addPeriodicTimeObserverForInterval:CMTimeMake(1, 30) queue:dispatch_get_main_queue() usingBlock:callback];
}

- (void)removeObserver:(id)observer forPlayer:(AVPlayer *)player {
    if (observer) {
        [player removeTimeObserver:observer];
    }
}

- (void)addStatusObserverForItem:(AVPlayerItem *)playerItem {
    if (!status_observer_created) {
        @try {
            [playerItem addObserver:self forKeyPath:@"status" options:NSKeyValueObservingOptionNew context:item_status_observe_ctx];
        } @catch (NSException *exception) {
            
        } @finally {
            
        }
        status_observer_created = 1;
    }
}

- (void)removeStatusObserverForItem:(AVPlayerItem *)playerItem {
    if (status_observer_created) {
        @try {
            [playerItem removeObserver:self forKeyPath:@"status"];
        } @catch (NSException *exception) {
            
        } @finally {
            
        }
        status_observer_created = 0;
    }
}

- (void)addPlaybackObserverForItem:(AVPlayerItem *)playerItem {
    //    [playerItem addObserver:self forKeyPath:@"playbackLikelyToKeepUp" options:NSKeyValueObservingOptionNew context:item_playback_observe_ctx];
    //    [playerItem addObserver:self forKeyPath:@"playbackBufferFull" options:NSKeyValueObservingOptionNew context:item_playback_observe_ctx];
    //    [playerItem addObserver:self forKeyPath:@"playbackBufferEmpty" options:NSKeyValueObservingOptionNew context:item_playback_observe_ctx];
}

- (void)removePlaybackObserverForItem:(AVPlayerItem *)playerItem {
    //    [playerItem removeObserver:self forKeyPath:@"playbackLikelyToKeepUp"];
    //    [playerItem removeObserver:self forKeyPath:@"playbackBufferFull"];
    //    [playerItem removeObserver:self forKeyPath:@"playbackBufferEmpty"];
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary<NSString *,id> *)change context:(void *)context {
    if (context == item_status_observe_ctx) {
        AVPlayerItemStatus status = [change[@"new"] integerValue];
        MPTVRStreamerLoadStatus loadStatus = MPTVRStreamerLoadStatusUnknown;
        switch (status) {
            case AVPlayerItemStatusUnknown:
                break;
            case AVPlayerItemStatusFailed:
                loadStatus = MPTVRStreamerLoadStatusFailed;
                NSLog(@"load player asset failed, error:%@", m_playerItem.error);
                [self removeStatusObserverForItem:m_playerItem];
                break;
            case AVPlayerItemStatusReadyToPlay:
                [self removeStatusObserverForItem:m_playerItem];
                loadStatus = MPTVRStreamerLoadStatusReadyToPlay;
                [self addPlaybackObserverForItem:m_playerItem];
                if (self.playingPreparedCallback)
                {
                    self.playingPreparedCallback();
                    self.playingPreparedCallback = 0;
                }
                if (self.seekingBlock)
                {
                    NSLog(@"now seeking...");
                    self.seekingBlock();
                    self.seekingBlock = 0;
                }
                break;
        }
        if ([self.delegate respondsToSelector:@selector(streamer:handleLoadStatus:)]) {
            [self.delegate streamer:self handleLoadStatus:loadStatus];
        }
    }
    
    else if (context == item_playback_observe_ctx) {
        MPTVRStreamerPlayStatus status = MPTVRStreamerPlayStatusUnknown;
        if ([keyPath isEqualToString:@"playbackLikelyToKeepUp"]) {
            BOOL keepUp = [change[@"new"] boolValue];
            status = keepUp ? MPTVRStreamerPlayStatusPlay : MPTVRStreamerPlayStatusLoad;
            if ([self.delegate respondsToSelector:@selector(streamer:handlePlayStatus:)]) {
                [self.delegate streamer:self handlePlayStatus:(MPTVRStreamerPlayStatus)status];
            }
        }
        //        else if ([keyPath isEqualToString:@"playbackBufferFull"]) {
        //            BOOL bufferFull = [change[@"new"] boolValue];
        //            status = bufferFull ? MPTVRStreamerPlayStatusPlay : MPTVRStreamerPlayStatusPause;
        //        } else if ([keyPath isEqualToString:@"playbackBufferEmpty"]) {
        //            BOOL bufferEmpty = [change[@"new"] boolValue];
        //            status = bufferEmpty ? MPTVRStreamerPlayStatusPause : MPTVRStreamerPlayStatusPlay;
        //        }
    }
}


- (void)open {
    m_streamer_should_play = YES;
    //    [m_player play];
}

- (void)close {
    m_streamer_should_play = NO;
    [m_player pause];
    faliedCount = 0;
}

- (void)play {
    [self handleLoading];
    [self createTOB];
    [m_player setRate:_rate];
    faliedCount = 0;
    self.isPlaying = 1;
}

- (void)pause {
    [m_player setRate:0.0];
    [self destroyTOB];
    faliedCount = 0;
    self.isPlaying = 0;
}

- (void)handleLoading {
    if ([self.delegate respondsToSelector:@selector(streamer:handlePlayStatus:)]) {
        [self.delegate streamer:self handlePlayStatus:(MPTVRStreamerPlayStatusLoad)];
    }
}

- (void)setVolume:(CGFloat)volume {
    _volume = volume;
    m_player.volume = volume;
}


- (void)setRate:(CGFloat)rate {
    _rate = rate;
    [m_player setRate:rate];
    [self createTOB];
    faliedCount = 0;
}

- (void)seekToTime:(CMTime)time  {
    [self seekToTime:time withHandleDecode:NO completionHandler:NULL];
}

- (void)seekToTime:(CMTime)time withHanldeDeocde:(BOOL)handleDeocde
{
    [self seekToTime:time withHandleDecode:handleDeocde completionHandler:NULL];
}

- (void)seekToTime:(CMTime)time completionHandler:(void (^)())completionHandler {
    [self seekToTime:time withHandleDecode:NO completionHandler:completionHandler];
}

- (void)seekToTime:(CMTime)time withHandleDecode:(BOOL)decode completionHandler:(void (^)())completionHandler {
    __weak typeof(self) wself = self;
    
    dispatch_block_t seekingBlock = ^
    {
        if (!CMTIME_IS_VALID(time))
        {
            NSLog(@"seek time is invalid.");
            if (completionHandler)
            {
                completionHandler();
            }
            return;
        }
        [m_player seekToTime:time toleranceBefore:kCMTimeZero toleranceAfter:kCMTimeZero completionHandler:^(BOOL finished) {
            if (finished)
            {
                if (decode && finished)
                {
                    [wself handleDecode:time isSeek:1];
                }
                if (completionHandler)
                {
                    completionHandler();
                }
            }
            else
            {
//                if (framCounter++ < 10)
//                {
//                    [self seekToTime:time withHandleDecode:decode completionHandler:completionHandler];
//                }
//                else
                {
                    if (completionHandler)
                    {
                        completionHandler();
                    }
                }
            }
        }];
    };
    
    if (m_playerItem.status != AVPlayerItemStatusReadyToPlay)
    {
//        if (completionHandler) completionHandler();
        self.seekingBlock = seekingBlock;
        return NSLog(@"wait to seek...");
    }
    self.seekingBlock = seekingBlock;
    self.seekingBlock();
    self.seekingBlock = 0;
}

- (AVPlayerItem *)currentItem {
    return m_playerItem;
}


#pragma mark - video range

@synthesize videoRange = _videoRange;

- (CMTimeRange)videoRange
{
    if (CMTIMERANGE_IS_EMPTY(_videoRange) || CMTIMERANGE_IS_INVALID(_videoRange))
    {
        _videoRange = CMTimeRangeMake(kCMTimeZero, self.currentItem.duration);
    }
    return _videoRange;
}

- (void)setVideoRange:(CMTimeRange)videoRange
{
    [self seekToTime:videoRange.start withHandleDecode:1 completionHandler:^
    {
    }];
    _videoRange = videoRange;
    [self addTimeObserverForDriver];
}

- (void)removeTimeObserverForDriver
{
    if (boundrayTimeObserver)
    {
        [m_player removeTimeObserver:boundrayTimeObserver];
        boundrayTimeObserver = 0;
    }
}

- (void)addTimeObserverForDriver
{
    [self removeTimeObserverForDriver];
    
    __unsafe_unretained typeof(self) wself = self;
    boundrayTimeObserver =[m_player addBoundaryTimeObserverForTimes:@[[NSValue valueWithCMTime:CMTimeSubtract(CMTimeAdd(self.videoRange.start, self.videoRange.duration), kCMTimeZero)]] queue:dispatch_get_main_queue() usingBlock:^
    {
        [wself reload];
    }];
}

- (CVPixelBufferRef)copyAtTime:(CMTime)time
{
    __block CVPixelBufferRef buffer = 0;
    [m_player seekToTime:time toleranceBefore:CMTimeMake(0, 30) toleranceAfter:CMTimeMake(0, 30)];
    buffer = [m_item_videoOutput copyPixelBufferForItemTime:time itemTimeForDisplay:0];
    return buffer;
}

- (CMTime)currentTime
{
    return m_player.currentTime;
}

- (CGFloat)fps {
    if (_fps == 0) {
        NSArray<AVAssetTrack *> *trakcs = [self.playeritem.asset tracksWithMediaType:AVMediaTypeVideo];
        if (trakcs.count) {
            _fps = [trakcs[0] nominalFrameRate];
        }
    }
    return _fps;
}

- (CGFloat)duration {
    if (_duration == 0) {
        _duration = CMTimeGetSeconds(self.playeritem.asset.duration);
    }
    return _duration;
}

- (AVPlayerItem *)playeritem {
    return m_playerItem;
}

@end
