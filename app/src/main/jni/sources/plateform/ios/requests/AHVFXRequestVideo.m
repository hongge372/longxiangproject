//
//  AHVFXRequestVideo.m
//  AHVideoFX
//
//  Created by longxiang on 2019/3/20.
//  Copyright © 2019 AH. All rights reserved.
//

#import "AHVFXRequestVideo.h"
#import "MPTVRStreamer.h"
#import "AHVFXContextManager.h"

@interface AHVFXRequestVideo() {
    MPTVRStreamer *streamer;
    CVOpenGLESTextureRef textureRef;
    CGSize textureSize;
}

@end

@implementation AHVFXRequestVideo

- (void)dealloc {
    if (textureRef) {
        CFRelease(textureRef);
        textureRef = 0;
    }
}


- (instancetype)initWithURL:(NSString *)url {
    if (self = [super init]) {
        [self commonInit:url];
    }
    return self;
}

- (void)commonInit:(NSString *)url {
//    req_cpp = std::shared_ptr<ahvfx::request_node>(new ahvfx::request_node());
//    ahvfx::req_context_t ctx;
////    ctx.func = &req_for_ahvfx_video_image;
//    ctx.context = (__bridge void *)self;
//    req_cpp->set_req_context(ctx);
    streamer = [[MPTVRStreamer alloc] initWithURL:url];
}

/*
- (ahvfx::texture_t)get_vid_img:(uint32_t)fIdx {
    CVPixelBufferRef pixelBuffer = [streamer copyAtTime:CMTimeMakeWithSeconds(fIdx / 30.0, 10000.0)];
    if (pixelBuffer) {
        if (textureRef) {
            CFRelease(textureRef);
            textureRef = 0;
        }
        ahvfx_upload_pixelbuffer_ios_manual(pixelBuffer, &textureRef, &textureSize);
        CFRelease(pixelBuffer);
        return ahvfx::texture_t(CVOpenGLESTextureGetName(textureRef), textureSize.width, textureSize.height);
    }
    return ahvfx::texture_t();
}

ahvfx::texture_t req_for_ahvfx_video_image(void *ctx, uint32_t fIdx) {
    AHVFXRequestVideo *req_oc = (__bridge AHVFXRequestVideo *)ctx;
    ahvfx::texture_t rs = [req_oc get_vid_img:fIdx];
    ahvfx_log("req vid request img at %u.", fIdx);
    return rs;
}
 */

- (void)prepareAtTime:(float)time {
    CVPixelBufferRef pixelBuffer = [streamer copyAtTime:CMTimeMakeWithSeconds(fmod(time, streamer.duration), 10000.0)];
    if (pixelBuffer) {
        if (textureRef) {
            CFRelease(textureRef);
            textureRef = 0;
        }
        ahvfx_upload_pixelbuffer_ios_manual(pixelBuffer, &textureRef, &textureSize);
        if (textureRef) {
            self.Id = CVOpenGLESTextureGetName(textureRef);
            self.width = textureSize.width;
            self.height = textureSize.height;
        } else {
            NSLog(@"??????????????????");
        }
        CFRelease(pixelBuffer);
    } else {
        NSLog(@"cant copy pixelbuffer at:%.2f", time);
    }
}

@end
