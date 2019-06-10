#ifdef __APPLE__
#import "AHVFXContextManager.h"
#import "ahvfx_plateform.h"

void on_AHGL_Ctx_CREATE(void *);
void on_AHGL_Ctx_BIND(void *);
void on_AHGL_Ctx_DESTORY(void *);

extern "C" void ahvfx_gl_default_config_texture_2d_parameters(uint32_t texture);

@interface AHVFXContextManager()

@property (nonatomic, assign) EAGLRenderingAPI API;
@property (nonatomic, strong) NSMutableDictionary *bridged_ctxs;

@end

@implementation AHVFXContextManager

+ (AHVFXContextManager *)manager {
    return [self managerWithExistContext:nil];
}

+ (AHVFXContextManager *)managerWithExistContext:(EAGLContext *)ctx {
    static AHVFXContextManager *mng = NULL;
    static dispatch_once_t onceToken;
    _dispatch_once(&onceToken, ^{
        mng = [[self alloc] initWithContext:ctx];
    });
    return mng;
}

- (id)initWithContext:(EAGLContext *)ctx {
    if (self = [super init]) {
        [self commonInitWithContext:ctx];
    }
    return self;
}

- (void)commonInitWithContext:(EAGLContext *)ctx {
    
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3 sharegroup:ctx.sharegroup];
    self.API = kEAGLRenderingAPIOpenGLES3;
    if (self.context == nil) {
        self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2 sharegroup:ctx.sharegroup];
        self.API = kEAGLRenderingAPIOpenGLES2;
    }
    [self makeCurrent];
    CVOpenGLESTextureCacheCreate(kCFAllocatorDefault,
                                 0,
                                 self.context,
                                 0,
                                 &_textureCache);
    self.bridged_ctxs = [NSMutableDictionary new];
}

- (void)makeCurrent {
    [EAGLContext setCurrentContext:self.context];
}

- (EAGLContext *)createSharedContext {
    return [[EAGLContext alloc] initWithAPI:self.API sharegroup:self.context.sharegroup];
}

- (void)onCreateCtx:(void *)ahgl_ctx {
    if (ahgl_ctx) {
        self.bridged_ctxs[[NSString stringWithFormat:@"%p", ahgl_ctx]] = [self createSharedContext];
    }
    NSLog(@"%s", __func__);
}

- (void)onBindCtx:(void *)ahgl_ctx {
    if (ahgl_ctx) {
        [EAGLContext setCurrentContext:self.bridged_ctxs[[NSString stringWithFormat:@"%p", ahgl_ctx]]];
    }
    NSLog(@"%s", __func__);
}

- (void)onDestoryCtx:(void *)ahgl_ctx {
    if (ahgl_ctx) {
        self.bridged_ctxs[[NSString stringWithFormat:@"%p", ahgl_ctx]] = NULL;
    }
    NSLog(@"%s", __func__);
}


@end

void ahvfx_upload_pixelbuffer_ios(CVPixelBufferRef pixelBuffer, void (^callback)(GLuint texture, CGSize textureSize)) {
    if (callback) {
        size_t width = CVPixelBufferGetWidth(pixelBuffer);
        size_t height = CVPixelBufferGetHeight(pixelBuffer);
        CVOpenGLESTextureRef textureRef = 0;
        CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                     [AHVFXContextManager manager].textureCache,
                                                     pixelBuffer,
                                                     0,
                                                     GL_TEXTURE_2D,
                                                     GL_RGBA,
                                                     (GLsizei)width,
                                                     (GLsizei)height,
                                                     GL_BGRA,
                                                     GL_UNSIGNED_BYTE,
                                                     0,
                                                     &textureRef);
        if (textureRef) {
            ahvfx_gl_default_config_texture_2d_parameters(CVOpenGLESTextureGetName(textureRef));
            callback(CVOpenGLESTextureGetName(textureRef), CGSizeMake(width, height));
            CFRelease(textureRef);
        }
        CVOpenGLESTextureCacheFlush([AHVFXContextManager manager].textureCache, 0);
    }
}

void ahvfx_upload_pixelbuffer_ios_manual(CVPixelBufferRef pixelBuffer, CVOpenGLESTextureRef *textureRef, CGSize *size)
{
    if (pixelBuffer && textureRef)
    {
        [[AHVFXContextManager manager] makeCurrent];
        
        CVPixelBufferRef movieFrame = pixelBuffer;
        int bufferWidth = (int)CVPixelBufferGetWidth(movieFrame);
        int bufferHeight = (int)CVPixelBufferGetHeight(movieFrame);
        
        
        CVReturn err;
        err = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                           [AHVFXContextManager manager].textureCache,
                                                           movieFrame,
                                                           NULL,
                                                           GL_TEXTURE_2D,
                                                           GL_RGBA,
                                                           bufferWidth,
                                                           bufferHeight,
                                                           GL_BGRA,
                                                           GL_UNSIGNED_BYTE,
                                                           0,
                                                           textureRef);
        
        if (size)
        {
            size->width = bufferWidth;
            size->height = bufferHeight;
        }
        if (textureRef) {
            ahvfx_gl_default_config_texture_2d_parameters(CVOpenGLESTextureGetName(*textureRef));
        }
        CVOpenGLESTextureCacheFlush([AHVFXContextManager manager].textureCache, 0);
    }
    
}


void on_AHGL_Ctx_CREATE(void *ahgl_ctx) {
    [[AHVFXContextManager manager] onCreateCtx:ahgl_ctx];
}

void on_AHGL_Ctx_BIND(void *ahgl_ctx) {
    [[AHVFXContextManager manager] onBindCtx:ahgl_ctx];
}

void on_AHGL_Ctx_DESTORY(void *ahgl_ctx) {
    [[AHVFXContextManager manager] onDestoryCtx:ahgl_ctx];
}


CGImageRef AHVFX_SampleBuffer2Image(CMSampleBufferRef sampeBuffer) {
    CVPixelBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampeBuffer);
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    void *baseAddress = CVPixelBufferGetBaseAddress(pixelBuffer);
    size_t width = CVPixelBufferGetWidth(pixelBuffer);
    size_t height = CVPixelBufferGetHeight(pixelBuffer);
    size_t bufferSize = CVPixelBufferGetDataSize(pixelBuffer);
    size_t bytesPerRow = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
    CGColorSpaceRef rgbColorSpace = CGColorSpaceCreateDeviceRGB();
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, baseAddress, bufferSize, NULL);
    CGImageRef cgImage = CGImageCreate(width,
                                       height,
                                       8,
                                       32,
                                       bytesPerRow,
                                       rgbColorSpace,
                                       kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst,
                                       provider,
                                       NULL,
                                       true,
                                       kCGRenderingIntentDefault);
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(rgbColorSpace);
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    return cgImage;
}
#endif
