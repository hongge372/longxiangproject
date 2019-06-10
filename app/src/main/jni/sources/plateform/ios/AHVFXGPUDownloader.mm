//
//  AHVFXGPUDownloader.m
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/27.
//  Copyright © 2019 AH. All rights reserved.
//
#if __APPLE__

#import "AHVFXGPUDownloader.h"
#import "AHVFXContextManager.h"
#import <OpenGLES/ES2/glext.h>
#include "ahvfx_program.hpp"
#include "ahvfx_video_flat.hpp"
#include "ahvfx_io.hpp"

extern "C" {
    void ahvfx_GPU_COMMANDS_SYNC(void);
    void ahvfx_gl_default_config_texture_2d_parameters(uint32_t texture);
}

@interface AHVFXGPUDownloader() {
    GLuint frameBuffer;
    CGSize renderTargetSize;
}

@property (nonatomic, assign) CVPixelBufferRef renderTarget;
@property (nonatomic, assign) CVOpenGLESTextureRef renderTexture;
@property (nonatomic, assign) ahvfx::program *renderProgram;

@end

@implementation AHVFXGPUDownloader

- (void)dealloc {
    [self destroyRenderTexture];
    [self destroyRenderTarget];
    [self destroyFrameBuffer];
    if (_renderProgram) delete _renderProgram;
}

- (void)createFrameBuffer {
    [self destroyFrameBuffer];
    if (!frameBuffer) {
        glGenFramebuffers(1, &frameBuffer);
    }
}

- (void)destroyFrameBuffer {
    if (frameBuffer) {
        glDeleteFramebuffers(1, &frameBuffer);
        frameBuffer = 0;
    }
}

- (void)destroyRenderTarget {
    if (_renderTarget) {
        CFRelease(_renderTarget);
        _renderTarget = nil;
    }
}

- (void)createRenderTarget:(CGSize)size {
    [self destroyRenderTarget];
    renderTargetSize = size;
    NSDictionary *attributes = @{
                                 (id)kCVPixelBufferIOSurfacePropertiesKey: @{
                                         @"IOSurfaceOpenGLESTextureCompatibility": @YES,
                                         @"IOSurfaceOpenGLESFBOCompatibility": @YES,
                                         }
                                 };
    
    CVPixelBufferCreate(kCFAllocatorDefault,
                        size.width,
                        size.height,
                        kCVPixelFormatType_32BGRA,
                        (__bridge CFDictionaryRef _Nullable)(attributes),
                        &_renderTarget);
    
    if (_renderTarget) {
        GLsizei width = (GLsizei)CVPixelBufferGetWidth(_renderTarget);
        GLsizei height = (GLsizei)CVPixelBufferGetHeight(_renderTarget);
        CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                     [AHVFXContextManager manager].textureCache,
                                                     _renderTarget,
                                                     NULL,
                                                     GL_TEXTURE_2D,
                                                     GL_RGBA,
                                                     width,
                                                     height,
                                                     GL_BGRA,
                                                     GL_UNSIGNED_BYTE,
                                                     0,
                                                     &_renderTexture);
        
        if (_renderTexture) {
            ahvfx_gl_default_config_texture_2d_parameters(CVOpenGLESTextureGetName(_renderTexture));
        }
    }

}

- (void)destroyRenderTexture {
    if (_renderTexture) {
        CFRelease(_renderTexture);
        _renderTexture = NULL;
    }
}

- (instancetype)init {
    if (self = [super init]) {
    }
    return self;
}

- (void)bindDownloader:(CGSize)inputSize {
    if (!__CGSizeEqualToSize(renderTargetSize, inputSize) && inputSize.width && inputSize.height) {
        [self createFrameBuffer];
        [self createRenderTarget:inputSize];
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, CVOpenGLESTextureGetName(_renderTexture), 0);
        
    }
}

- (CVPixelBufferRef)downloadTexture:(uint32_t)texture width:(uint32_t)width height:(uint32_t)height {
    [self bindDownloader: CGSizeMake(width, height)];
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, renderTargetSize.width, renderTargetSize.height);
    
    self.renderProgram->use();
    ahvfx::texture_t texture_t(texture, width, height);
    ahvfx_active_bind_texture(texture_t, 0);
    self.renderProgram->uniform1(self.renderProgram->uniform.texture[0], 0);
    ahvfx::video_flat::draw();
    ahvfx_GPU_COMMANDS_SYNC();
    return self.renderTarget;
}

- (ahvfx::program *)renderProgram {
    if (!_renderProgram) {
        _renderProgram = new ahvfx::program(NULL, NULL);
    }
    return _renderProgram;
}

@end

#endif
