#ifdef __APPLE__
#import "AHVFXView.h"
#import "ahvfx_plateform.h"
#import "AHVFXContextManager.h"
#import "ahvfx_program.hpp"
#import "ahvfx_vertexarray.hpp"
#import "ahvfx_vertexbuffer.hpp"

using namespace ahvfx;

extern "C" CGRect AVMakeRectWithAspectRatioInsideRect(CGSize aspectRatio, CGRect boundingRect);
extern "C" void ahvfx_gl_enable_attribute(uint32_t attrib_idx, int32_t sz, uint32_t data_type, int32_t normalized, int32_t stride, const void *ptr);

@interface AHVFXView() {
    GLuint m_frameBuffer;
    GLuint m_renderBuffer;
    GLuint m_renderBufferWidth;
    GLuint m_renderBufferHeight;
    std::shared_ptr<ahvfx::program> m_program;
    std::shared_ptr<ahvfx::vertexarray> m_VAO;
    std::shared_ptr<ahvfx::vertexbuffer> m_VBO_Vertex, m_VBO_Texcoord, m_IBO;
    GLuint m_renderTextureWidth, m_renderTextureHeight;
}

@property (nonatomic, strong) EAGLContext *context;

@end

@implementation AHVFXView

+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (void)dealloc {
    [self destoryFrameBuffer];
    [self removeObserver:self forKeyPath:@"frame"];
}

- (instancetype)initWithFrame:(CGRect)frame {
    return [self initWithFrame:frame context:0];
}

- (instancetype)initWithFrame:(CGRect)frame context:(EAGLContext *)ctx {
    if (self = [super initWithFrame:frame]) {
        [self commonInit:ctx];
    }
    return self;
}

- (EAGLContext *)context {
    if (_context == nil) {
        _context = [[AHVFXContextManager manager] createSharedContext];
    }
    return _context;
}

- (void)bindContext {
    if ([EAGLContext currentContext] != self.context) {
        glFlush();
    }
    [EAGLContext setCurrentContext:self.context];
}

- (void)commonInit:(EAGLContext *)ctx {
    _context = ctx;
    [self bindContext];
    [self initialVideoFlat];
    m_program = std::shared_ptr<ahvfx::program>(new ahvfx::program(NULL, NULL));
    _fillType = AHVFXView_Fill_Type_Aspect_Ratio;
    if ([self respondsToSelector:@selector(setContentScaleFactor:)]) {
        self.contentScaleFactor = [[UIScreen mainScreen] scale];
    }
    self.opaque = YES;
    self.hidden = NO;
    CAEAGLLayer *layer = (CAEAGLLayer *)self.layer;
    layer.opaque = YES;
    layer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];

    [self addObserver:self forKeyPath:@"frame" options:0 context:NULL];
    
    self.clearColor = AHVFX_FLOAT4{0, 0, 0, 1};
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
//    __weak typeof(self) weakSelf = self;
    if (object == self && [keyPath isEqualToString:@"frame"] && (!CGSizeEqualToSize(self.bounds.size, CGSizeZero)))
    {
        [self bindContext];
        [self destoryFrameBuffer];
        [self createFrameBuffer];
        [self recomputeVertexWithInputTextureWidth:m_renderTextureWidth height:m_renderTextureHeight];
    }
}


- (void)createFrameBuffer {
    [self destoryFrameBuffer];
    if (m_frameBuffer == 0) {
        glGenFramebuffers(1, &m_frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
        glGenRenderbuffers(1, &m_renderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);
        CAEAGLLayer *layer = [self valueForKey:@"layer"];
        [self.context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)layer];
        GLint backingWidth, backingHeight;
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
        if (backingWidth * backingHeight == 0) {
            [self destoryFrameBuffer];
            return;
        }
        m_renderBufferWidth = backingWidth;
        m_renderBufferHeight = backingHeight;
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_renderBuffer);
        NSAssert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, @"render buffer failed.");
        NSLog(@"create renderbuffer %d x %d", backingWidth, backingHeight);
    }
}

- (void)destoryFrameBuffer {
    if (m_frameBuffer) {
        glDeleteFramebuffers(1, &m_frameBuffer);
        m_frameBuffer = 0;
    }
    if (m_renderBuffer) {
        glDeleteRenderbuffers(1, &m_renderBuffer);
        m_renderBuffer = 0;
    }
}

- (void)bindFrameBuffer {
    if (m_frameBuffer == 0) {
        [self createFrameBuffer];
    }
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    glViewport(0, 0, m_renderBufferWidth, m_renderBufferHeight);
}

- (void)presentRenderbuffer {
    if ([[[UIApplication sharedApplication] valueForKey:@"applicationState"] intValue]  == UIApplicationStateActive) {
        glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);
        [self.context presentRenderbuffer:GL_RENDERBUFFER];
        glFlush();
//        NSLog(@"flushed.");
    } else {
        NSLog(@"background render state.");
    }
}

- (void)drawTexture:(GLuint)texture width:(GLuint)width height:(GLuint)height {
    
    if (texture) {
        [self bindContext];
        
        [self recomputeVertexWithInputTextureWidth:width height:height];
        
        [self bindFrameBuffer];
        glClearColor(_clearColor.m[0], _clearColor.m[1], _clearColor.m[2], _clearColor.m[3]);
        glClear(GL_COLOR_BUFFER_BIT);
        m_program->use();
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(m_program->uniform_location("u_Texture0"), 0);
        
        m_VAO->bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
        m_VAO->unbind();
        GLenum attachments[] = {GL_COLOR_ATTACHMENT0,
            GL_DEPTH_ATTACHMENT,
            GL_STENCIL_ATTACHMENT};
        static int numberOfAttachments = sizeof(attachments) / sizeof(attachments[0]);
#if GL_ES_VERSION_3_0
        glInvalidateFramebuffer(GL_FRAMEBUFFER,
                                numberOfAttachments,
                                attachments);
#elif GL_ES_VERSION_2_0
        glDiscardFramebufferEXT(GL_FRAMEBUFFER,
                                numberOfAttachments,
                                attachments);
#endif
        [self presentRenderbuffer];
    } else {
        ahvfx_log("???");
    }
}

- (void)setFillType:(AHVFXView_fill_type)fillType {
    if (_fillType != fillType) {
        _fillType = fillType;
        [self recomputeVertexWithInputTextureWidth:m_renderTextureWidth height:m_renderTextureHeight];
    }
}

- (void)recomputeVertexWithInputTextureWidth:(GLuint)inputTextureWidth height:(GLuint)inputTextureHeight {
    
    if (!inputTextureWidth * inputTextureHeight) {
        return;
    }
    
    if (m_renderTextureWidth != inputTextureWidth || m_renderTextureHeight != inputTextureHeight) {
        m_renderTextureWidth = inputTextureWidth;
        m_renderTextureHeight = inputTextureHeight;
    }
    
    CGFloat heightScaling, widthScaling;
    CGRect bounds = [[self valueForKey:@"bounds"] CGRectValue];
    CGSize currentViewSize = bounds.size;
    CGRect insetRect = AVMakeRectWithAspectRatioInsideRect(CGSizeMake(inputTextureWidth, inputTextureHeight), bounds);
    
    switch (_fillType) {
        case AHVFXView_Fill_Type_Stretch:
            heightScaling = widthScaling = 1.0;
            break;
        case AHVFXView_Fill_Type_Aspect_Ratio:
            widthScaling = insetRect.size.width / currentViewSize.width;
            heightScaling = insetRect.size.height / currentViewSize.height;
            break;
        case AHVFXView_Fill_Type_Aspect_Ratio_Fill:
            widthScaling = currentViewSize.height / insetRect.size.height;
            heightScaling = currentViewSize.width / insetRect.size.width;
            break;
        default:
            break;
    }
    GLfloat vertices[8];
    vertices[0] = -widthScaling;
    vertices[1] = -heightScaling;
    vertices[2] = widthScaling;
    vertices[3] = -heightScaling;
    vertices[4] = -widthScaling;
    vertices[5] = heightScaling;
    vertices[6] = widthScaling;
    vertices[7] = heightScaling;
    m_VBO_Vertex->bind();
    m_VBO_Vertex->buffer((void *)vertices, sizeof(vertices), GL_STREAM_DRAW);
    m_VBO_Vertex->unbind();
}

- (void)initialVideoFlat {
    m_VAO = std::shared_ptr<ahvfx::vertexarray>(new ahvfx::vertexarray());
    m_VAO->bind();
    m_VBO_Vertex = std::shared_ptr<ahvfx::vertexbuffer>(new ahvfx::vertexbuffer(GL_ARRAY_BUFFER));
    m_VBO_Vertex->bind();
    
    GLfloat widthScaling = 1, heightScaling = 1;
    GLfloat vertices[8];
    vertices[0] = -widthScaling;
    vertices[1] = -heightScaling;
    vertices[2] = widthScaling;
    vertices[3] = -heightScaling;
    vertices[4] = -widthScaling;
    vertices[5] = heightScaling;
    vertices[6] = widthScaling;
    vertices[7] = heightScaling;
    m_VBO_Vertex->buffer((void *)vertices, sizeof(vertices), GL_STREAM_DRAW);
    ahvfx_gl_enable_attribute(ahvfx::ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);

    static const GLfloat texCoords[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
    };
    
    m_VBO_Texcoord =std::shared_ptr<ahvfx::vertexbuffer>(new ahvfx::vertexbuffer(GL_ARRAY_BUFFER));
    m_VBO_Texcoord->bind();
    m_VBO_Texcoord->buffer((void *)texCoords, sizeof(texCoords), GL_STATIC_DRAW);
    ahvfx_gl_enable_attribute(ahvfx::ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, 0);    
    
    m_IBO = std::shared_ptr<ahvfx::vertexbuffer>(new ahvfx::vertexbuffer(GL_ELEMENT_ARRAY_BUFFER));
    m_IBO->bind();
    
    GLushort indices[] = {
        0, 1, 2, 1, 2, 3
    };
    m_IBO->buffer(indices, sizeof(indices), GL_STATIC_DRAW);
    
    m_VAO->unbind();
    m_IBO->unbind();
    m_VBO_Vertex->unbind();
    m_VBO_Texcoord->unbind();
}

@end

#endif
