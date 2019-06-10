//
//  AHVFXAVBuilder.h
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/4/16.
//  Copyright © 2019 AH. All rights reserved.
//

#ifdef __APPLE__

#import <AVFoundation/AVFoundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef CF_ENUM(NSUInteger, AHVFXVideoTransition) {
    AHVFXVideoTransition_None,
    AHVFXVideoTransition_Fade,
    AHVFXVideoTransition_Push_Up,
    AHVFXVideoTransition_Push_Left,
    AHVFXVideoTransition_Push_Down,
    AHVFXVideoTransition_Push_Right,
    AHVFXVideoTransition_Custom
};

@interface AHVFXAVBuildTransition : NSObject

@property (nonatomic, assign) NSInteger from;
@property (nonatomic, assign) NSInteger to;
@property (nonatomic, assign) CMTime duration;
@property (nonatomic, assign) AHVFXVideoTransition transition;
@property (nonatomic, assign) uint32_t render_style;
@property (nonatomic, assign) BOOL fromTiled; //defalut YES
@property (nonatomic, assign) BOOL toTiled; //default YES

+ (id)transitionFrom:(NSInteger)from to:(NSInteger)to duration:(CMTime)duration type:(AHVFXVideoTransition)type;
+ (id)customTransitionFrom:(NSInteger)from to:(NSInteger)to duration:(CMTime)duration root:(NSString *)root file:(NSString *)filePath;

- (void *)get;

@end

@interface AHVFXAVBuildArrange : NSObject

@property (nonatomic, assign) NSInteger index;
@property (nonatomic, assign) CMTimeRange timeRange;

@end

@interface AHVFXAVBuildInfo : NSObject

@property (nonatomic, strong) NSArray <AHVFXAVBuildArrange *> *arranges;
@property (nonatomic, strong) AHVFXAVBuildTransition *transition;

@end

@interface AHVFXAVBuilder : NSObject

@property (nonatomic, readonly) AVMutableComposition *composition;
@property (nonatomic, readonly) AVMutableVideoComposition *videoComposition;
@property (nonatomic, readonly) AVMutableAudioMix *audioMix;

@property (nonatomic, strong) NSArray <AVAsset *> *assets;
@property (nonatomic, strong) NSArray <AHVFXAVBuildArrange *> *arranges;
@property (nonatomic, strong) NSArray <AHVFXAVBuildTransition *> *transitions;

- (void)build;

@end

NS_ASSUME_NONNULL_END

#endif
