//
//  ahvfx_ texture_duplicator.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/29.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_texture_duplicator.hpp"
#include "ahvfx_program.hpp"
#include "ahvfx_flat.hpp"
#include "ahvfx_framebuffer.hpp"

#define use_fbo_obj 1

class ahvfx_texture_duplicator_intl {
public:
    std::shared_ptr<ahvfx::framebuffer> frame_buffer;
    uint32_t framebuffer = 0;
    ahvfx::vec2<uint32_t> framebuffer_size;
    std::shared_ptr<ahvfx::program> split_program;
    std::shared_ptr<ahvfx::flat> flat;
public:
    explicit
    ahvfx_texture_duplicator_intl() {
        
    }
    ~ahvfx_texture_duplicator_intl() {
        destory();
    }
    
    void destory() {
        if (framebuffer) {
            glDeleteFramebuffers(1, &framebuffer);
            framebuffer = 0;
        }
    }
    
    void copy(ahvfx::texture_t in, ahvfx::texture_t &rs) {
#if !use_fbo_obj
        ahvfx::vec2<uint32_t> size = in.size.xy();
        if (framebuffer_size != size && size.valid()) {
            framebuffer_size = size;
            if (!framebuffer) {
                glGenFramebuffers(1, &framebuffer);
            }
        }
#endif
        
        if (rs.valid() == false) {
            rs.size = in.size;
            rs.gen();
            rs.bind();
            rs.default_texture_parameters();
            rs.alloc();
        }
        
#if GL_APPLE_framebuffer_multisample_CH
        glBindFramebuffer(GL_READ_FRAMEBUFFER_APPLE, framebuffer);
        glFramebufferTexture2D(GL_READ_FRAMEBUFFER_APPLE, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, in.id, 0);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        rs.bind();
        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, in.width(), in.height(), 0);
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
#else
#if !use_fbo_obj
        rs.bind();
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, rs.target, rs.id, 0);
        glViewport(0, 0, size[0], size[1]);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        
        lazy_load(split_program,
                  std::shared_ptr<ahvfx::program>(new ahvfx::program(NULL, NULL));
                  );
        split_program->use();
        ahvfx_active_bind_texture(in, 0);
        split_program->uniform1(split_program->uniform.texture[0], 0);
        ahvfx::video_flat::draw();
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
#endif
        lazy_load(frame_buffer, std::shared_ptr<ahvfx::framebuffer>(new ahvfx::framebuffer()));
        frame_buffer->bind(rs);
        frame_buffer->set_clear_color(ahvfx::vec4<float>(0, 0, 0, 1));
        frame_buffer->setViewport(ahvfx::vec4<int32_t>(0, 0, in.width(), in.height()));
        frame_buffer->ACC_FLOAD();
        lazy_load(split_program,
                  std::shared_ptr<ahvfx::program>(new ahvfx::program(NULL, NULL));
                  );
        split_program->use();
        ahvfx_active_bind_texture(in, 0);
        split_program->uniform1(split_program->uniform.texture[0], 0);
        lazy_load(flat, std::shared_ptr<ahvfx::flat>(new ahvfx::flat()));
        flat->draw();
        frame_buffer->ACC_FSTORE();
#endif
        rs.unbind();
    }
};

using namespace ahvfx;

 texture_duplicator:: texture_duplicator() {
    intl_texture_duplicator = std::shared_ptr<ahvfx_texture_duplicator_intl>(new ahvfx_texture_duplicator_intl());
}

 texture_duplicator::~ texture_duplicator() {
    decons_log();
}

void  texture_duplicator::copy(ahvfx:: texture_t in, ahvfx::texture_t &rs) {
    if (intl_texture_duplicator) {
        intl_texture_duplicator->copy(in, rs);
    }
}
