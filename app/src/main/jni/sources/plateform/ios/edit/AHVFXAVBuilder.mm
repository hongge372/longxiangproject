//
//  AHVFXAVBuilder.m
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/4/16.
//  Copyright © 2019 AH. All rights reserved.
//

#ifdef __APPLE__

#import "AHVFXAVBuilder.h"
#include "ahvfx_program.hpp"

extern "C" char *readbuf(const char *, long &);

static inline CMTime convertTime(CMTime time) {
    return CMTimeMakeWithSeconds(CMTimeGetSeconds(time), 240);
}

static inline CMTimeRange convertTimeRange(CMTimeRange timeRange) {
    return CMTimeRangeMake(convertTime(timeRange.start), convertTime(timeRange.duration));
}

NSDictionary * ahvfx_ios_dict_replace_root(NSDictionary *dict, NSString *root, NSString *name) {
    NSMutableDictionary *rs = [NSMutableDictionary dictionaryWithDictionary:dict];
    [dict enumerateKeysAndObjectsUsingBlock:^(id  _Nonnull key, id  _Nonnull obj, BOOL * _Nonnull stop) {
        if ([key hasSuffix:@"path"] || [key hasSuffix:@"Path"]) {
            [rs setValue:[NSString stringWithFormat:@"%@/%@/%@", root, name, obj] forKey:key];
        }
    }];
    return rs;
}


@implementation AHVFXAVBuildInfo

@end

@interface AHVFXAVBuildTransition()

@property (nonatomic, strong) NSString *vsh;
@property (nonatomic, strong) NSString *fsh;
@property (nonatomic, assign) ahvfx::program *program;

@end

@implementation AHVFXAVBuildTransition

- (void)dealloc {
    if (_program) {
        delete _program;
        _program = 0;
    }
}

- (void)loadCustomRoot:(NSString *)root file:(NSString *)file {
    NSString *full = [root stringByAppendingFormat:@"/%@/transit.prog", file];
    if ([[NSFileManager defaultManager] fileExistsAtPath:full]) {
        long sz = 0;
        char *buf = readbuf(full.UTF8String, sz);
        if (buf && sz) {
            NSData *data = [NSData dataWithBytes:buf length:sz];
            NSDictionary *dict = [NSJSONSerialization JSONObjectWithData:data options:kNilOptions error:0];
            dict = ahvfx_ios_dict_replace_root(dict, root, file);
            self.vsh = dict[@"vsh_path"];
            self.fsh = dict[@"fsh_path"];
            self.render_style = [dict[@"render_style"] intValue];
            NSArray *tiles = dict[@"tiles"];
            if (tiles.count == 2) {
                self.fromTiled = [tiles[0] boolValue];
                self.toTiled = [tiles[1] boolValue];
            }
            free(buf);            
        }
    }
}


- (void *)get {
    if (self.vsh || self.fsh) {
        return (void *)self.program;
    }
    return NULL;
}

- (ahvfx::program *)program {
    if (_program == NULL) {
        _program = new ahvfx::program();
        _program->load_file(self.vsh.UTF8String, self.fsh.UTF8String);
    }
    return _program;
}

- (instancetype)init {
    if (self = [super init]) {
        self.fromTiled = YES;
        self.toTiled = YES;
        self.duration = kCMTimeZero;
    }
    return self;
}

+ (id)transitionFrom:(NSInteger)from to:(NSInteger)to duration:(CMTime)duration type:(AHVFXVideoTransition)type {
    AHVFXAVBuildTransition *rs = [AHVFXAVBuildTransition new];
    rs.from = from;
    rs.to = to;
    rs.duration = duration;
    rs.transition = type;
    rs.fromTiled = YES;
    rs.toTiled = YES;
    return rs;
}

+ (id)customTransitionFrom:(NSInteger)from to:(NSInteger)to duration:(CMTime)duration root:(nonnull NSString *)root file:(nonnull NSString *)filePath{
    AHVFXAVBuildTransition *rs = [AHVFXAVBuildTransition new];
    rs.from = from;
    rs.to = to;
    rs.duration = duration;
    rs.transition = AHVFXVideoTransition_Custom;
    rs.fromTiled = YES;
    rs.toTiled = YES;
    [rs loadCustomRoot:root file:filePath];
    return rs;
}

- (void)setDuration:(CMTime)duration {
    _duration = convertTime(duration);
}

@end

@implementation AHVFXAVBuildArrange


@end

typedef NS_ENUM(NSUInteger, AHVFXVideoTransitionType) {
    AHVFXVideoTransitionType_None,
    AHVFXVideoTransitionType_FadeInOut,
};


@interface AHVFXVideoCompositionLayerInstruction : NSObject

@property (nonatomic, assign) NSUInteger trackId;
@property (nonatomic, assign) CGAffineTransform transform;
@property (nonatomic, assign) NSUInteger value;
@property (nonatomic, strong) AHVFXAVBuildTransition *customTransition;

@end

@implementation AHVFXVideoCompositionLayerInstruction

@end

@interface AHVFXVideoCompositionInstruction: NSObject <AVVideoCompositionInstruction>

@property (nonatomic, assign) CMTimeRange timeRange;
@property (nonatomic, assign) BOOL enablePostProcessing;
@property (nonatomic, assign) BOOL containsTweening;
@property (nonatomic, strong, nullable) NSArray<NSValue *> *requiredSourceTrackIDs;
@property (nonatomic, assign) CMPersistentTrackID passthroughTrackID;
@property (nonatomic, strong) NSArray <AHVFXVideoCompositionLayerInstruction *> *layerInstructions;
@property (nonatomic, assign) BOOL composit_flag;

@end

@implementation AHVFXVideoCompositionInstruction

@end


@interface AHVFXVideoComposition : NSObject <AVVideoCompositing>

@property (nonatomic, strong) NSArray <AHVFXVideoCompositionInstruction *> *instructions;

@end

#include "AHVFXTransitor.h"

@interface AHVFXVideoComposition() {
    AHVFXTransitor *trans;
}

@property (nonatomic, assign) AVVideoCompositionRenderContext *renderContext;

@end

@implementation AHVFXVideoComposition

- (void)dealloc {
    trans = 0;
#if DEBUG
    NSLog(@"%s", __FUNCTION__);
#endif
}

- (NSDictionary<NSString *,id> *)sourcePixelBufferAttributes {
    return @{ (NSString *)kCVPixelBufferPixelFormatTypeKey : [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA],
              (NSString*)kCVPixelBufferOpenGLESCompatibilityKey : [NSNumber numberWithBool:YES]};
}

- (NSDictionary<NSString *,id> *)requiredPixelBufferAttributesForRenderContext {
    return @{ (NSString *)kCVPixelBufferPixelFormatTypeKey : [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA],
              (NSString*)kCVPixelBufferOpenGLESCompatibilityKey : [NSNumber numberWithBool:YES]};
}

- (void)renderContextChanged:(AVVideoCompositionRenderContext *)newRenderContext {
    self.renderContext = newRenderContext;
}

- (void)startVideoCompositionRequest:(AVAsynchronousVideoCompositionRequest *)requset {
//    CMTimeShow(requset.compositionTime);
    AHVFXVideoCompositionInstruction *instruction = requset.videoCompositionInstruction;
//    CMTimeRangeShow(instruction.timeRange);
    if (instruction.layerInstructions.count == 2) {

//        NSLog(@"instructions count:%d", instruction.layerInstructions.count);
//        NSLog(@"requset source tracks = %@", requset.sourceTrackIDs);
        int type = 0;
        AHVFXAVBuildTransition *customTransit = NULL;
        CVPixelBufferRef pixelBuffers[instruction.layerInstructions.count];
        bzero(pixelBuffers, sizeof(pixelBuffers));
        for (int i = 0; i < instruction.layerInstructions.count; ++i) {
            AHVFXVideoCompositionLayerInstruction *layerIns = instruction.layerInstructions[i];
            pixelBuffers[i] = [requset sourceFrameByTrackID:(int)layerIns.trackId];
//            NSLog(@"layer track id:%d", layerIns.trackId);
            type = (int)layerIns.value;
            customTransit = layerIns.customTransition;
            if (pixelBuffers[i] == 0) {
                return;
            }
        }
        
        static int c = 0;
        
        CVPixelBufferRef pixelBuffer = pixelBuffers[c++ % instruction.layerInstructions.count];
//        pixelBuffer = [self from:pixelBuffers[0] to:pixelBuffers[1]];
        pixelBuffer = [self.renderContext newPixelBuffer];
        if (!trans) {
            trans = [AHVFXTransitor new];
        }
        [trans transitFrom:pixelBuffers[0] to:pixelBuffers[1] destBuffer:pixelBuffer start:instruction.timeRange.start duration:instruction.timeRange.duration now:requset.compositionTime type:type object:customTransit];
        
        if (pixelBuffer) {
            [requset finishWithComposedVideoFrame:pixelBuffer];
            CFRelease(pixelBuffer);
        } else {
            [requset finishCancelledRequest];
        }
    } else {
#if DEBUG
        NSLog(@"no instruction.layerInstructions.count");
#endif
        [requset finishCancelledRequest];
    }
}

- (void)cancelAllPendingVideoCompositionRequests {
    
}

- (BOOL)supportsWideColorSourceFrames {
    return YES;
}


@end

@interface AHVFXAVBuilder()

@property (nonatomic, strong) AVMutableComposition *composition;
@property (nonatomic, strong) AVMutableVideoComposition *videoComposition;
@property (nonatomic, strong) AVMutableAudioMix *audioMix;

@end

@implementation AHVFXAVBuilder

- (AVAssetTrack *)trackWithIndex:(NSUInteger)index type:(AVMediaType)type {
    if (index < self.assets.count) {
        AVAsset *asset = self.assets[index];
        NSArray *tracks = [asset tracksWithMediaType:type];
        if (tracks.count) {
            return tracks[0];
        }
    }
    return nil;
}

static inline CMTimeRange AdaptiveAVRange(CMTimeRange v, CMTimeRange a) {
    CMTime s = CMTimeCompare(v.start, a.start) > 0 ? v.start : a.start;
    CMTime d = CMTimeCompare(v.duration, a.duration) > 0 ? a.duration : v.duration;
    return convertTimeRange(CMTimeRangeMake(s, d));
}

- (CMTimeRange)validTimeRange:(CMTimeRange)timeRange ForTrack:(AVAssetTrack *)track {
    if (CMTIMERANGE_IS_EMPTY(timeRange) || CMTIMERANGE_IS_INVALID(timeRange)) {
        return convertTimeRange(track.timeRange);
    }
    if (CMTimeRangeContainsTimeRange(track.timeRange, timeRange)) {
        return convertTimeRange(timeRange);
    }
    return convertTimeRange(CMTimeRangeMake(CMTimeCompare(timeRange.start, track.timeRange.start) > 0 ? timeRange.start : track.timeRange.start, CMTimeCompare(timeRange.duration, track.timeRange.duration) < 0 ? timeRange.duration : track.timeRange.duration));
}

- (AHVFXAVBuildTransition *)validTransitionAtOperateIndex:(NSInteger)index {
    if (index < self.transitions.count) {
        return self.transitions[index];
    }
    return nil;
}

/*
static inline bool push_back_instruction(NSMutableArray *instructions, AHVFXVideoCompositionInstruction *instruction) {
    for (AHVFXVideoCompositionInstruction *strored in instructions) {
        if (CMTimeRangeContainsTime(strored.timeRange, instruction.timeRange.start) || CMTimeRangeContainsTime(strored.timeRange, CMTimeRangeGetEnd(instruction.timeRange))) {
            return false;
        }
    }
    [instructions addObject:instruction];
    return true;
}
*/

- (void)build {
    if (self.assets.count) {
        
        AVMutableComposition *composition = [AVMutableComposition composition];
        AVMutableVideoComposition *videoComposition = [AVMutableVideoComposition videoComposition];
        AVMutableAudioMix *audioMix = [AVMutableAudioMix audioMix];
        
        NSMutableArray *instructions = [NSMutableArray new];
        NSMutableArray *audioMixParams = [NSMutableArray new];
        videoComposition.customVideoCompositorClass = NSClassFromString(@"AHVFXVideoComposition");
        AVMutableCompositionTrack *videoCompositionTrack[1024];
        AVMutableCompositionTrack *audioCompositionTrack[1024];
        bzero(videoCompositionTrack, sizeof(videoCompositionTrack));
        bzero(audioCompositionTrack, sizeof(audioCompositionTrack));
        NSUInteger compositionTrackIndex = 0;
        NSError *error;
        CMTime insertTime = kCMTimeZero;
        
        NSInteger operateIndex = 0;
        for (AHVFXAVBuildArrange *arrange in self.arranges) {
            AVAssetTrack *videoTrack = [self trackWithIndex:arrange.index type:AVMediaTypeVideo];
            CMTimeRange insertTimeRange = [self validTimeRange:arrange.timeRange ForTrack:videoTrack];
            videoCompositionTrack[compositionTrackIndex] = [composition addMutableTrackWithMediaType:AVMediaTypeVideo preferredTrackID:kCMPersistentTrackID_Invalid];
            if (![videoCompositionTrack[compositionTrackIndex] insertTimeRange:insertTimeRange ofTrack:videoTrack atTime:insertTime error:&error]) {
#if DEBUG
                assert(0);
#endif
            }
#if DEBUG
            NSLog(@"insert video: %f - %f at %f", CMTimeGetSeconds(insertTimeRange.start), CMTimeGetSeconds(insertTimeRange.duration), CMTimeGetSeconds(insertTime));
#endif
            AHVFXVideoCompositionInstruction *instruction = [AHVFXVideoCompositionInstruction new];
            instruction.passthroughTrackID = videoCompositionTrack[compositionTrackIndex].trackID;
            [instructions addObject:instruction];
            AHVFXVideoCompositionLayerInstruction *layerInstruction = [AHVFXVideoCompositionLayerInstruction new];
            instruction.layerInstructions = @[layerInstruction];
            layerInstruction.trackId = instruction.passthroughTrackID;
            instruction.timeRange = CMTimeRangeMake(insertTime, insertTimeRange.duration);
            compositionTrackIndex++;
            AVAssetTrack *audioTrack = [self trackWithIndex:arrange.index type:AVMediaTypeAudio];
            if (audioTrack) {
                audioCompositionTrack[compositionTrackIndex] = [composition addMutableTrackWithMediaType:AVMediaTypeAudio preferredTrackID:kCMPersistentTrackID_Invalid];
                CMTimeRange audioTrackRange = [self validTimeRange:arrange.timeRange ForTrack:audioTrack];
                audioTrackRange = AdaptiveAVRange(insertTimeRange, audioTrackRange);
                if (![audioCompositionTrack[compositionTrackIndex] insertTimeRange:audioTrackRange ofTrack:audioTrack atTime:insertTime error:&error]) {
#if DEBUG
                    assert(0);
#endif
                }
                AVMutableAudioMixInputParameters *param = [AVMutableAudioMixInputParameters audioMixInputParametersWithTrack:audioCompositionTrack[compositionTrackIndex]];
                [param setVolume:1.0 atTime:insertTime];
                [audioMixParams addObject:param];
                compositionTrackIndex++;
            }
            insertTime = CMTimeAdd(insertTime, insertTimeRange.duration);
            AHVFXAVBuildTransition *transition = [self validTransitionAtOperateIndex:operateIndex++];
            if (transition) {
                insertTime = CMTimeSubtract(insertTime, transition.duration);
            }
        }
        
#if DEBUG
        NSLog(@"before ----------:::");
        for (AHVFXVideoCompositionInstruction *ins in instructions) {
            NSLog(@"instruction: %f - %f", CMTimeGetSeconds(ins.timeRange.start), CMTimeGetSeconds(CMTimeRangeGetEnd(ins.timeRange)));
        }
#endif
        
        if (self.transitions.count) {
//            NSMutableArray *newInstuctions = [NSMutableArray new];
            for (AHVFXAVBuildTransition *transition in self.transitions) {
                AHVFXVideoCompositionInstruction *from = instructions[transition.from];
                AHVFXVideoCompositionInstruction *to = instructions[transition.to];
                CMTime tranitDuration = transition.duration;
                CMTimeRange fromTimeRange = CMTimeRangeMake(from.timeRange.start, CMTimeSubtract(from.timeRange.duration, tranitDuration));
                
                AHVFXVideoCompositionInstruction *newFrom = [AHVFXVideoCompositionInstruction new];
                newFrom.timeRange = fromTimeRange;
                newFrom.layerInstructions = [NSArray arrayWithArray:from.layerInstructions];
                
                
                from.timeRange = convertTimeRange(fromTimeRange);
                CMTimeRange toTimeRange = CMTimeRangeMake(CMTimeAdd(to.timeRange.start, tranitDuration), CMTimeSubtract(to.timeRange.duration, tranitDuration));
                to.timeRange = convertTimeRange(toTimeRange);
                
                AHVFXVideoCompositionInstruction *newTo = [AHVFXVideoCompositionInstruction new];
                newTo.timeRange = toTimeRange;
                newTo.layerInstructions = [NSArray arrayWithArray:to.layerInstructions];

                
                CMTimeRange transitTimeRange = CMTimeRangeMake(CMTimeRangeGetEnd(fromTimeRange), transition.duration);
                AHVFXVideoCompositionInstruction *transitInstruction = [AHVFXVideoCompositionInstruction new];
                transitInstruction.composit_flag = YES;
                transitInstruction.timeRange = convertTimeRange(transitTimeRange);
                AHVFXVideoCompositionLayerInstruction *fromLayerInstruction = [AHVFXVideoCompositionLayerInstruction new];
                fromLayerInstruction.trackId = from.passthroughTrackID;
                AHVFXVideoCompositionLayerInstruction *toLayerInstruction = [AHVFXVideoCompositionLayerInstruction new];
                toLayerInstruction.trackId = to.passthroughTrackID;
                transitInstruction.layerInstructions = @[fromLayerInstruction, toLayerInstruction];
                
                fromLayerInstruction.value = transition.transition;
                toLayerInstruction.value = transition.transition;
                
                fromLayerInstruction.customTransition = transition;
                toLayerInstruction.customTransition = transition;
//                static int i = 0;
//                int v = (i++ % 4 + 2);
//                fromLayerInstruction.value = v;
//                toLayerInstruction.value = v;
//                
//                [newInstuctions addObject:newFrom];
                [instructions addObject:transitInstruction];
//                [newInstuctions addObject:newTo];
                
//                push_back_instruction(newInstuctions, newFrom);
//                push_back_instruction(newInstuctions, transitInstruction);
//                push_back_instruction(newInstuctions, newTo);
            }
            
            [instructions sortUsingComparator:^NSComparisonResult(id  _Nonnull obj1, id  _Nonnull obj2) {
                AHVFXVideoCompositionInstruction *ins1 = obj1;
                AHVFXVideoCompositionInstruction *ins2 = obj2;
                return (NSComparisonResult)(CMTimeCompare(CMTimeRangeGetEnd(ins1.timeRange), CMTimeRangeGetEnd(ins2.timeRange)) > 0);
            }];
//            instructions = newInstuctions;
            
            NSMutableArray *toremove = [NSMutableArray new];
            for (AHVFXVideoCompositionInstruction *ins in instructions) {
                if (CMTimeCompare(CMTimeRangeGetEnd(ins.timeRange), ins.timeRange.start) <= 0) {
                    [toremove addObject:ins];
                }
            }
            [instructions removeObjectsInArray:toremove];
        }
        
#if DEBUG
        NSLog(@"after ----------:::");
        for (AHVFXVideoCompositionInstruction *ins in instructions) {
            NSLog(@"instruction: %f - %f: is added:%d", CMTimeGetSeconds(ins.timeRange.start), CMTimeGetSeconds(CMTimeRangeGetEnd(ins.timeRange)), ins.composit_flag);
            for (AHVFXVideoCompositionLayerInstruction *layerins in ins.layerInstructions) {
                NSLog(@"layer ins track:%d", layerins.trackId);
            }
        }
        
        for (AVAssetTrack *track in composition.tracks) {
            CMTimeRangeShow(track.timeRange);
        }
#endif
     
        
        videoComposition.instructions = instructions;
        videoComposition.frameDuration = CMTimeMake(1, 30);
        videoComposition.renderSize = [self trackWithIndex:0 type:AVMediaTypeVideo].naturalSize;
        
        
        audioMix.inputParameters = audioMixParams;
        
        self.composition = composition;
        self.videoComposition = videoComposition;
        self.audioMix = audioMix;
    } else {
        
    }
}

- (CMTime)durationAtIndex:(NSUInteger)index {
    return [self trackWithIndex:index type:(AVMediaTypeVideo)].timeRange.duration;
}

- (NSArray<AHVFXAVBuildArrange *> *)arranges {
    if (_arranges.count == 0) {
        NSMutableArray *rs = [NSMutableArray new];
        for (int i = 0; i < self.assets.count; ++i) {
            AHVFXAVBuildArrange *arrange = [AHVFXAVBuildArrange new];
            arrange.index = i;
            [rs addObject:arrange];
        }
        return rs;
    }
    return _arranges;
}

@end

#endif
