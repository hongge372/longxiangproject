#ifdef __APPLE__
#import <AVFoundation/AVFoundation.h>
#import <OpenGLES/EAGL.h>
NS_ASSUME_NONNULL_BEGIN

#ifdef __cplusplus
extern "C" {
#endif
    void ahvfx_upload_pixelbuffer_ios(CVPixelBufferRef pixelBuffer, void (^callback)(GLuint texture, CGSize textureSize));
    void ahvfx_upload_pixelbuffer_ios_manual(CVPixelBufferRef pixelBuffer, CVOpenGLESTextureRef _Nullable * _Nullable textureRef, CGSize *size);
    CGImageRef AHVFX_SampleBuffer2Image(CMSampleBufferRef sampeBuffer);
#ifdef __cplusplus
}
#endif

@interface AHVFXContextManager : NSObject

@property (nonatomic, assign) CVOpenGLESTextureCacheRef textureCache;
@property (nonatomic, strong) EAGLContext *context;

+ (AHVFXContextManager *)manager;
+ (AHVFXContextManager *)managerWithExistContext:(EAGLContext *)ctx;

- (EAGLContext *)createSharedContext;

- (void)makeCurrent;

@end

NS_ASSUME_NONNULL_END


#endif
