//
//  AHVFXGPUDownloader.h
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/27.
//  Copyright © 2019 AH. All rights reserved.
//
#if __APPLE__
#import <AVFoundation/AVFoundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface AHVFXGPUDownloader : NSObject

- (CVPixelBufferRef)downloadTexture:(uint32_t)texture width:(uint32_t)width height:(uint32_t)height;

@end

NS_ASSUME_NONNULL_END

#endif
