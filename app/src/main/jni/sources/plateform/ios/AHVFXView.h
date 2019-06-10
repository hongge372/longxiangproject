#ifdef __APPLE__

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>

NS_ASSUME_NONNULL_BEGIN

typedef struct {
    float m[4];
} AHVFX_FLOAT4;

typedef enum {
    AHVFXView_Fill_Type_Stretch,
    AHVFXView_Fill_Type_Aspect_Ratio,
    AHVFXView_Fill_Type_Aspect_Ratio_Fill
} AHVFXView_fill_type;

@interface AHVFXView : UIView

@property (nonatomic, assign) AHVFXView_fill_type fillType;
@property (nonatomic, assign) AHVFX_FLOAT4 clearColor;

- (instancetype)initWithFrame:(CGRect)frame context:(EAGLContext *)ctx;

- (void)bindFrameBuffer;
- (void)presentRenderbuffer;
- (void)drawTexture:(GLuint)texture width:(GLuint)width height:(GLuint)height;

@end

NS_ASSUME_NONNULL_END

#endif
