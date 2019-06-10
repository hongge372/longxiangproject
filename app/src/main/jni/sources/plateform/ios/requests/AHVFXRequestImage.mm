//
//  AHVFXRequestImage.m
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/20.
//  Copyright © 2019 AH. All rights reserved.
//

#import "AHVFXRequestImage.h"
#import <CoreGraphics/CoreGraphics.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

extern "C" char *readbuf(const char *, long &);

@interface AHVFXRequestImage() {
    GLint maxTextureSize;
    GLuint outputTexture;
}

@property (nonatomic, assign) CGSize outputTextureSize;

@end

@implementation AHVFXRequestImage

- (void)dealloc {
    if (outputTexture) {
        glDeleteTextures(1, &outputTexture);
    }
}

- (id)initWithImage:(UIImage *)image {
    if (self = [super init]) {
        [self commonInit];
        [self processCGImage:image.CGImage];
    }
    return self;
}

- (id)initWithImagePath:(NSString *)image_path {
    long bufsz = 0;
    unsigned char *buf = (unsigned char *)readbuf(image_path.UTF8String, bufsz);
    NSData *data = [NSData dataWithBytes:buf length:bufsz];
    free(buf);
    return [self initWithImage:[UIImage imageWithData:data]];
}

- (void)commonInit {
}

- (uint32_t)Id {
    return outputTexture;
}

- (uint32_t)width {
    return _outputTextureSize.width;
}

- (uint32_t)height {
    return _outputTextureSize.height;
}

- (void)processCGImage:(CGImageRef)image {
    
    BOOL shouldRedrawUsingCoreGraphics = NO;
    
    CGFloat widthOfImage  = CGImageGetWidth(image);
    CGFloat heightOfImage = CGImageGetHeight(image);
    
    if (heightOfImage == 1.0) {
        heightOfImage *= 3.0;
    }
    
    NSAssert(widthOfImage && heightOfImage, @"Wrong Image!");
    
    
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    
    if (widthOfImage > maxTextureSize || heightOfImage > maxTextureSize) {
        self.outputTextureSize = CGSizeMake(maxTextureSize, maxTextureSize);
        if (widthOfImage > heightOfImage) {
            _outputTextureSize.height = maxTextureSize * (heightOfImage / widthOfImage);
        } else {
            _outputTextureSize.width  = maxTextureSize * (widthOfImage / heightOfImage);
        }
        shouldRedrawUsingCoreGraphics = YES;
    }
    
    self.outputTextureSize = CGSizeMake(widthOfImage, heightOfImage);
    
    GLubyte *imageData = NULL;
    CFDataRef dataFromImageDataProvider = NULL;
    GLenum format = GL_BGRA;
    
    //检查格式
    if (!shouldRedrawUsingCoreGraphics) {
        /* Check that the bitmap pixel format is compatible with GL */
        CGBitmapInfo bitmapInfo = CGImageGetBitmapInfo(image);
        if ((bitmapInfo & kCGBitmapFloatComponents) != 0) {
            /* We don't support float components for use directly in GL */
            shouldRedrawUsingCoreGraphics = YES;
        } else {
            CGBitmapInfo byteOrderInfo = bitmapInfo & kCGBitmapByteOrderMask;
            if (byteOrderInfo == kCGBitmapByteOrder32Little) {
                /* Little endian, for alpha-first we can use this bitmap directly in GL */
                CGImageAlphaInfo alphaInfo = (CGImageAlphaInfo)(bitmapInfo & kCGBitmapAlphaInfoMask);
                if (alphaInfo != kCGImageAlphaPremultipliedFirst && alphaInfo != kCGImageAlphaFirst &&
                    alphaInfo != kCGImageAlphaNoneSkipFirst) {
                    shouldRedrawUsingCoreGraphics = YES;
                }
            } else if (byteOrderInfo == kCGBitmapByteOrderDefault || byteOrderInfo == kCGBitmapByteOrder32Big) {
                /* Big endian, for alpha-last we can use this bitmap directly in GL */
                CGImageAlphaInfo alphaInfo = (CGImageAlphaInfo)(bitmapInfo & kCGBitmapAlphaInfoMask);
                if (alphaInfo != kCGImageAlphaPremultipliedLast && alphaInfo != kCGImageAlphaLast &&
                    alphaInfo != kCGImageAlphaNoneSkipLast) {
                    shouldRedrawUsingCoreGraphics = YES;
                } else {
                    /* Can access directly using GL_RGBA pixel format */
                    format = GL_RGBA;
                }
            }
        }
    }
    
    if (shouldRedrawUsingCoreGraphics) {
        imageData = (GLubyte *)calloc(1, (size_t)_outputTextureSize.width * (size_t)_outputTextureSize.height * 4);
        
        CGColorSpaceRef RGBColorSpace = CGColorSpaceCreateDeviceRGB();
        
        CGContextRef imageContext = CGBitmapContextCreate(imageData,
                                                          (size_t)_outputTextureSize.width,
                                                          (size_t)_outputTextureSize.height,
                                                          8,
                                                          (size_t)_outputTextureSize.width * 4,
                                                          RGBColorSpace,
                                                          kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst);
        CGContextDrawImage(imageContext, CGRectMake(0, 0, _outputTextureSize.width, _outputTextureSize.height), image);
        CGContextRelease(imageContext);
        CGColorSpaceRelease(RGBColorSpace);
    } else {
        dataFromImageDataProvider = CGDataProviderCopyData(CGImageGetDataProvider(image));
        imageData = (GLubyte *)CFDataGetBytePtr(dataFromImageDataProvider);
    }

    glGenTextures(1, &outputTexture);
    glBindTexture(GL_TEXTURE_2D, outputTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 (GLsizei)_outputTextureSize.width,
                 (GLsizei)_outputTextureSize.height,
                 0,
                 format,
                 GL_UNSIGNED_BYTE,
                 imageData);
    glBindTexture(GL_TEXTURE_2D, 0);
        
    
    if (shouldRedrawUsingCoreGraphics) {
        free(imageData);
    } else {
        CFRelease(dataFromImageDataProvider);
    }
}


@end
