//
//  AHVFXTransitor.h
//  AHVideoFX
//
//  Created by longxiang on 2019/4/17.
//  Copyright © 2019 AH. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface AHVFXTransitor : NSObject

- (void)transitFrom:(CVPixelBufferRef)pixelBuffer1 to:(CVPixelBufferRef)pixelBuffer2 destBuffer:(CVPixelBufferRef)destBuffer start:(CMTime)start duration:(CMTime)duration now:(CMTime)now type:(NSUInteger)type object:(id)obj;

@end

NS_ASSUME_NONNULL_END
