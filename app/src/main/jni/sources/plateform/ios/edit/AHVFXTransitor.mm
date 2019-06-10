//
//  AHVFXTransitor.m
//  AHVideoFX
//
//  Created by longxiang on 2019/4/17.
//  Copyright © 2019 AH. All rights reserved.
//

#import "AHVFXTransitor.h"
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import "AHVFXAVBuilder.h"

#include "ahvfx_framebuffer.hpp"
#include "ahvfx_program.hpp"
#include "ahvfx_flat.hpp"
#include "ahvfx_texture_duplicator.hpp"
#include "ahvfx_graph_node.hpp"
extern "C" {
    void ahvfx_GPU_COMMANDS_SYNC(void);
}

#define _STR(x) @#x

@interface AHVFXTransitor() {
    void *renderQueueKey;
    GLuint frameBuffer;
    ahvfx::program *program;
    CVOpenGLESTextureRef renderTexture;
    CGSize frameBufferSize;
    ahvfx::flat *_flat;
}

@property (nonatomic, strong) EAGLContext *glContext;
@property (nonatomic, strong) dispatch_queue_t renderQueue;
@property (nonatomic, assign) CVOpenGLESTextureCacheRef textureCache;
@property (nonatomic, assign) ahvfx::flat *flat;
@property (nonatomic, assign) ahvfx::texture_duplicator *duplicator;
@property (nonatomic, assign) ahvfx::graph_node *tiled_pass_from;
@property (nonatomic, assign) ahvfx::graph_node *tiled_pass_to;

@end

@implementation AHVFXTransitor

- (void)clean {
    if (_flat) {
        delete _flat;
        _flat = 0;
    }
    if (_tiled_pass_from) {
        delete _tiled_pass_from;
        _tiled_pass_from = 0;
    }
    if (_tiled_pass_to) {
        delete _tiled_pass_to;
        _tiled_pass_to = 0;
    }
    if (program) {
        delete program;
        program = 0;
    }
    if (frameBuffer) {
        glDeleteFramebuffers(1, &frameBuffer);
        frameBuffer = 0;
    }
    if (renderTexture) {
        CFRelease(renderTexture);
        renderTexture = 0;
    }
}

- (void)dealloc {
    [self runSyncInRenderQueue:^{
        [self clean];
    }];
}

- (instancetype)init {
    if (self = [super init]) {
        [self commonInit];
    }
    return self;
}

- (dispatch_queue_t)renderQueue {
    if (_renderQueue == 0) {
        _renderQueue = dispatch_queue_create("", 0);
        renderQueueKey = &renderQueueKey;
        dispatch_queue_set_specific(_renderQueue, renderQueueKey, 0, 0);
    }
    return _renderQueue;
}

- (void)runSyncInRenderQueue:(dispatch_block_t)block {
    dispatch_get_specific(renderQueueKey) ? block() : dispatch_sync(self.renderQueue, block);
}

- (void)commonInit {
    self.glContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    __unsafe_unretained typeof(self) wself = self;
    [self runSyncInRenderQueue:^
    {
        [EAGLContext setCurrentContext:wself.glContext];
        CVOpenGLESTextureCacheCreate(kCFAllocatorDefault,
                                     0,
                                     wself.glContext,
                                     0,
                                     &wself->_textureCache);
        /*
        ahvfx_glsl(
                   attribute vec4 a_Position;
                   attribute vec4 a_TexCoord;
                   
                   varying vec2 v_TexCoord;
                   
                   uniform vec2 u_FrameBufferSize;
                   uniform vec2 u_TextureSize0;
                   
                   void main() {
                       float scale;
                       if (u_FrameBufferSize.x > u_FrameBufferSize.y) {
                           scale = u_FrameBufferSize.y / u_TextureSize0.y;
                       } else {
                           scale = u_FrameBufferSize.x / u_TextureSize0.x;
                       }
                       gl_Position = mat4(u_TextureSize0.x / u_FrameBufferSize.x * scale, 0.0, 0.0, 0.0,
                                          0.0, u_TextureSize0.y / u_FrameBufferSize.y * scale, 0.0, 0.0,
                                          0.0, 0.0, 1.0, 0.0,
                                          0.0, 0.0, 0.0, 1.0) * a_Position;
                       v_TexCoord = a_TexCoord.st;
                   }
                   )
         */
        
        wself->program = new ahvfx::program(ahvfx_glsl(
                                                       
const int FADE = 1;
const int PUSH_UP = 2;
const int PUSH_LEFT = 3;
const int PUSH_DOWN = 4;
const int PUSH_RIGHT = 5;

attribute vec4 a_Position;
attribute vec4 a_TexCoord;
                                                       

varying vec2 v_TexCoord;

uniform float u_Progress;
uniform highp int u_Transition;
uniform highp int u_InstanceRenderIndex;

mat4 translate(vec2 trans) {
   return mat4(1.0, 0.0, 0.0, 0.0,
                 0.0, 1.0, 0.0, 0.0,
                 0.0, 0.0, 1.0, 0.0,
                 trans.x, trans.y, 0.0, 1.0);
}


void main() {
   mat4 MVP = mat4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
   if (u_Transition > 0) {
       if (u_Transition == FADE) {
           
       } else if (u_Transition <= PUSH_RIGHT) {
           float p = 1.0 - pow((1.0 - u_Progress), 3.2);
           if (u_Transition == PUSH_UP) {
               if (u_InstanceRenderIndex == 0) {
                   MVP = translate(vec2(0.0, -2.0 * p));
               } else {
                   MVP = translate(vec2(0.0, -2.0 * p + 2.0));
               }
           } else if (u_Transition == PUSH_LEFT) {
               if (u_InstanceRenderIndex == 0) {
                   MVP = translate(vec2(-2.0 * p, 0.0));
               } else {
                   MVP = translate(vec2(-2.0 * p + 2.0, 0.0));
               }
           } else if (u_Transition == PUSH_DOWN) {
               if (u_InstanceRenderIndex == 0) {
                   MVP = translate(vec2(0.0, 2.0 * p));
               } else {
                   MVP = translate(vec2(0.0, 2.0 * p - 2.0));
               }
           } else if (u_Transition == PUSH_RIGHT) {
               if (u_InstanceRenderIndex == 0) {
                   MVP = translate(vec2(2.0 * p, 0.0));
               } else {
                   MVP = translate(vec2(2.0 * p - 2.0, 0.0));
               }
           }
       }
   }
   gl_Position = MVP * a_Position;
   v_TexCoord = a_TexCoord.st;
}
                                            ), [NSString stringWithFormat:@"%s\n%@", "#extension GL_EXT_shader_framebuffer_fetch : require", _STR
(
const int FADE = 1;
const int PUSH_UP = 2;
const int PUSH_LEFT = 3;
const int PUSH_DOWN = 4;
const int PUSH_RIGHT = 5;

 precision highp float;
 varying vec2 v_TexCoord;
 uniform sampler2D u_Texture0;
 uniform highp int u_Transition;
 uniform float u_Progress;
 uniform highp int u_InstanceRenderIndex;
 
 
 vec4 fade() {
     vec4 rs = vec4(0.0);
     if (u_InstanceRenderIndex == 0) {
         rs = texture2D(u_Texture0, v_TexCoord);
     } else {
         vec4 last = gl_LastFragData[0];
         rs = mix(last, texture2D(u_Texture0, v_TexCoord), u_Progress);
     }
     return rs;
 }
                                              
 void main() {
     
     vec4 color = vec4(0.0);
     if (u_Transition > 0) {
         if (u_Transition == FADE) {
             color = fade();
         } else if (u_Transition <= PUSH_RIGHT) {
             color = texture2D(u_Texture0, v_TexCoord);
         }
     } else {
         color = texture2D(u_Texture0, v_TexCoord);
     }
     gl_FragColor = color;

 }
                                                                                                     )].UTF8String);
        glGenFramebuffers(1, &wself->frameBuffer);
    }];


}

- (void)uploadPixelBuffer:(CVPixelBufferRef)pixelBuffer texture:(CVOpenGLESTextureRef *)texture size:(CGSize *)size {
    if (texture && size) {
        CGSize osize = CGSizeMake(CVPixelBufferGetWidth(pixelBuffer), CVPixelBufferGetHeight(pixelBuffer));
        CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                     self.textureCache,
                                                     pixelBuffer,
                                                     0,
                                                     GL_TEXTURE_2D,
                                                     GL_RGBA,
                                                     osize.width,
                                                     osize.height,
                                                     GL_BGRA,
                                                     GL_UNSIGNED_BYTE,
                                                     0,
                                                     texture);
        glBindTexture(CVOpenGLESTextureGetTarget(*texture), CVOpenGLESTextureGetName(*texture));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        *size = osize;
        CVOpenGLESTextureCacheFlush(self.textureCache, 0);
    }
}

- (void)bindFrameBuffer:(CVPixelBufferRef)destBuffer {
    size_t width = CVPixelBufferGetWidth(destBuffer);
    size_t height = CVPixelBufferGetHeight(destBuffer);
    frameBufferSize = CGSizeMake(width, height);
//    NSLog(@"dest: %.0f x %.0f", frameBufferSize.width, frameBufferSize.height);
    if (renderTexture) {
        CFRelease(renderTexture);
        renderTexture = 0;
    }
    CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                 self.textureCache,
                                                 destBuffer,
                                                 NULL,
                                                 GL_TEXTURE_2D,
                                                 GL_RGBA,
                                                 (int)width,
                                                 (int)height,
                                                 GL_BGRA,
                                                 GL_UNSIGNED_BYTE,
                                                 0,
                                                 &renderTexture);
    if (renderTexture) {
        uint32_t name = CVOpenGLESTextureGetName(renderTexture);
        glBindTexture(GL_TEXTURE_2D, name);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, CVOpenGLESTextureGetName(renderTexture), 0);
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            NSLog(@"Error at glFramebufferTexture2D");
            return;
        }
        glViewport(0, 0, (int)width, (int)height);
    }
}

- (void)transitFrom:(CVPixelBufferRef)pixelBuffer1 to:(CVPixelBufferRef)pixelBuffer2 destBuffer:(CVPixelBufferRef)destBuffer start:(CMTime)start duration:(CMTime)duration now:(CMTime)now type:(NSUInteger)type object:(id)obj {
    __unsafe_unretained typeof(self) wself = self;
    [self runSyncInRenderQueue:^{
        [EAGLContext setCurrentContext:self.glContext];
        
        CVOpenGLESTextureRef textures[2];
        bzero(textures, sizeof(textures));
        CGSize size[2];
        bzero(size, sizeof(size));
        
        CVPixelBufferRef pixelBuffers[2];
        pixelBuffers[0] = pixelBuffer1;
        pixelBuffers[1] = pixelBuffer2;
        
        ahvfx::graph_node *tiled_pass[2] = {wself.tiled_pass_from, wself.tiled_pass_to};
        
        GLuint iotextures[2];
        BOOL tiles[2] = {YES, YES};
        AHVFXAVBuildTransition *transit = nil;
        if (obj) {
            transit = (AHVFXAVBuildTransition *)obj;
            tiles[0] = transit.fromTiled;
            tiles[1] = transit.toTiled;
        }
        
        for (int i = 0; i < 2; ++i) {
            tiled_pass[i]->set_output_size((uint32_t)CVPixelBufferGetWidth(destBuffer), (uint32_t)CVPixelBufferGetHeight(destBuffer));
            [wself uploadPixelBuffer:pixelBuffers[i] texture:&textures[i] size:&size[i]];
            iotextures[i] = CVOpenGLESTextureGetName(textures[i]);
//            NSLog(@"buffer[%d]: %.0f x %.0f", i, size[i].width, size[i].height);
            if (tiles[i] && !CGSizeEqualToSize(size[i], wself->frameBufferSize)) {
                tiled_pass[i]->in(ahvfx::texture_t::move(iotextures[i], size[i].width, size[i].height), 0, 0);
                iotextures[i] = tiled_pass[i]->out().id;
                size[i] = CGSizeMake(CVPixelBufferGetWidth(destBuffer), CVPixelBufferGetHeight(destBuffer));
            }
        }
        
        ahvfx::program *program_2_use = wself->program;
        int render_style = 0;
        if (transit) {
            program_2_use = (ahvfx::program *)transit.get;
            render_style = transit.render_style;
        }
        if (!program_2_use) {
            program_2_use = wself->program;
        }
        
        [wself bindFrameBuffer:destBuffer];
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        program_2_use->use();
        
        float progress = CMTimeGetSeconds(CMTimeSubtract(now, start)) / CMTimeGetSeconds(duration);
        if (render_style == 0) {
            for (int i = 0; i < 2; ++i) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, iotextures[i]);
                program_2_use->uniform1(program_2_use->uniform.texture[0], 0);
                program_2_use->uniform2(program_2_use->uniform.frameBufferSize, (float)wself->frameBufferSize.width, (float)wself->frameBufferSize.height);
                program_2_use->uniform2(program_2_use->uniform.textureSize[0], (float)size[i].width, (float)size[i].height);
                program_2_use->uniform1(program_2_use->uniform_location("u_Progress"), progress);
                program_2_use->uniform1(program_2_use->uniform_location("u_Transition"), (int)type);
                program_2_use->uniform1(program_2_use->uniform.instanceRenderIndex, i);
                wself.flat->draw();
            }
        } else {
            for (int i = 0; i < 2; ++i) {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, iotextures[i]);
                program_2_use->uniform1(program_2_use->uniform.texture[i], i);
                program_2_use->uniform2(program_2_use->uniform.textureSize[i], (float)size[i].width, (float)size[i].height);
                program_2_use->uniform1(program_2_use->uniform_location("u_Progress"), progress);
                program_2_use->uniform1(program_2_use->uniform_location("u_Transition"), (int)type);
//                program_2_use->uniform1(program_2_use->uniform.instanceRenderIndex, i);
            }
            program_2_use->uniform2(program_2_use->uniform.frameBufferSize, (float)wself->frameBufferSize.width, (float)wself->frameBufferSize.height);
            wself.flat->draw();
        }
        ahvfx_GPU_COMMANDS_SYNC();
//        glFinish();
        
        for (int i = 0; i < 2; ++i) {
            if (textures[i]) {
                CFRelease(textures[i]);
            }
        }
    }];
}

- (ahvfx::flat *)flat {
    if (_flat == 0) {
        _flat = new ahvfx::flat();
    }
    return _flat;
}



- (ahvfx::graph_node *)tiled_pass_from {
    if (!_tiled_pass_from) {
        _tiled_pass_from = new ahvfx::graph_node();
        _tiled_pass_from->always_update = true;
        _tiled_pass_from->clearColor = ahvfx::vec4<float>(0.0, 0.0, 0.0, 1.0);
        _tiled_pass_from->program = std::shared_ptr<ahvfx::program>(new ahvfx::program(ahvfx_glsl(
                                                                                                  attribute vec4 a_Position;
                                                                                                  attribute vec4 a_TexCoord;
                                                                                                  
                                                                                                  varying vec2 v_TexCoord;
                                                                                                  
                                                                                                  uniform vec2 u_FrameBufferSize;
                                                                                                  uniform vec2 u_TextureSize0;
                                                                                                  
                                                                                                  void main() {
                                                                                                      float scale = 1.0;
                                                                                                      if (u_FrameBufferSize.x > u_FrameBufferSize.y) {
                                                                                                          scale = u_FrameBufferSize.y / u_TextureSize0.y;
                                                                                                      } else {
                                                                                                          scale = u_FrameBufferSize.x / u_TextureSize0.x;
                                                                                                      }
                                                                                                      gl_Position = mat4(u_TextureSize0.x / u_FrameBufferSize.x * scale, 0.0, 0.0, 0.0,
                                                                                                                         0.0, u_TextureSize0.y / u_FrameBufferSize.y * scale, 0.0, 0.0,
                                                                                                                         0.0, 0.0, 1.0, 0.0,
                                                                                                                         0.0, 0.0, 0.0, 1.0) * a_Position;
                                                                                                      v_TexCoord = a_TexCoord.st;
//                                                                                                      gl_Position = a_Position;
                                                                                                  }
                                                                                       ), ahvfx_glsl(
                                                                                                     precision highp float;
                                                                                                     varying vec2 v_TexCoord;
                                                                                                     
                                                                                                     uniform sampler2D u_Texture0;
                                                                                                     void main() {
                                                                                                         gl_FragColor = texture2D(u_Texture0, v_TexCoord);
//                                                                                                         gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
                                                                                                     }
                                                                                       )));
    }
    return _tiled_pass_from;
}

- (ahvfx::graph_node *)tiled_pass_to {
    if (!_tiled_pass_to) {
        _tiled_pass_to = new ahvfx::graph_node();
        _tiled_pass_to->always_update = true;
        _tiled_pass_to->clearColor = ahvfx::vec4<float>(0.0, 0.0, 0.0, 1.0);
        _tiled_pass_to->program = std::shared_ptr<ahvfx::program>(new ahvfx::program(ahvfx_glsl(
                                                                                                attribute vec4 a_Position;
                                                                                                attribute vec4 a_TexCoord;
                                                                                                
                                                                                                varying vec2 v_TexCoord;
                                                                                                
                                                                                                uniform vec2 u_FrameBufferSize;
                                                                                                uniform vec2 u_TextureSize0;
                                                                                                
                                                                                                void main() {
                                                                                                    float scale = 1.0;
                                                                                                    if (u_FrameBufferSize.x > u_FrameBufferSize.y) {
                                                                                                        scale = u_FrameBufferSize.y / u_TextureSize0.y;
                                                                                                    } else {
                                                                                                        scale = u_FrameBufferSize.x / u_TextureSize0.x;
                                                                                                    }
                                                                                                    gl_Position = mat4(u_TextureSize0.x / u_FrameBufferSize.x * scale, 0.0, 0.0, 0.0,
                                                                                                                       0.0, u_TextureSize0.y / u_FrameBufferSize.y * scale, 0.0, 0.0,
                                                                                                                       0.0, 0.0, 1.0, 0.0,
                                                                                                                       0.0, 0.0, 0.0, 1.0) * a_Position;
//                                                                                                    gl_Position = a_Position;
                                                                                                    v_TexCoord = a_TexCoord.st;
                                                                                                }
                                                                                     ), ahvfx_glsl(
                                                                                                   precision highp float;
                                                                                                   varying vec2 v_TexCoord;
                                                                                                   
                                                                                                   uniform sampler2D u_Texture0;
                                                                                                     void main() {
                                                                                                         gl_FragColor = texture2D(u_Texture0, v_TexCoord);
//                                                                                                         gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);
                                                                                                   }
                                                                                       )));
    }
    return _tiled_pass_to;
}

@end
