//
//  ahvfx_framebuffer.cpp
//  AHVideoFX
//
//  Created by longxiang on 2019/3/18.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_framebuffer.hpp"

struct ahvfx_framebuffer_t {
    ahvfx_des_t;
    uint32_t id;
    ahvfx::vec2<uint32_t> size;
    ahvfx::vec4<int32_t> m_viewport;
    ahvfx::vec4<float> m_clear_color;
    std::shared_ptr<ahvfx::texture> texture;
    ahvfx::texture_t texture_st;
    
    ahvfx_framebuffer_t() {
        id = 0;
        texture = 0;
    }
    
    ~ahvfx_framebuffer_t() {
        destroy();
        decons_log();
    }
    
    void create() {
        if (id == 0) {
            glGenFramebuffers(1, &id);
        }
    }
    
    void destroy() {
        if (id) {
            glDeleteFramebuffers(1, &id);
            id = 0;
        }
        texture = NULL;
    }
    
    void bind(ahvfx::texture_t inTex) {
        if (inTex.valid()) {
            if (texture_st != inTex) {
                size = inTex.size.xy();
                texture_st = inTex;
                create();
                glBindFramebuffer(GL_FRAMEBUFFER, id);
                texture_st.bind();
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture_st.target, texture_st.id, 0);
                assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));
            } else {
                glBindFramebuffer(GL_FRAMEBUFFER, id);
            }
            if (m_viewport.empty()) {
                m_viewport = ahvfx::vec4<int32_t>(0, 0, size.elem.x, size.elem.y);
            }
            viewport(m_viewport);
        }
    }
    
    void bind(ahvfx::vec2<uint32_t> sz) {
        if (sz.valid()) {
            if (sz != size) {
                size = sz;
                destroy();
                create();
                glBindFramebuffer(GL_FRAMEBUFFER, id);
                texture = std::shared_ptr<ahvfx::texture>(new ahvfx::texture());
                if (texture->bind(sz)) {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture->intl_tex.target, texture->intl_tex.id, 0);
                    assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));
                }
            } else {
                glBindFramebuffer(GL_FRAMEBUFFER, id);
            }
            if (m_viewport.empty()) {
                m_viewport = ahvfx::vec4<int32_t>(0, 0, size.elem.x, size.elem.y);
            }
            viewport(m_viewport);
        } else {
            ahvfx_log("framebuffer size is not valid.\n");
        }
    }
    
    void viewport(ahvfx::vec4<int32_t> vp) {
        if (vp != m_viewport) {
            m_viewport = vp;
        }
        glViewport(vp.elem.x, vp.elem.y, vp.elem.z, vp.elem.w);
    }
    
    void unbind() {
        if (id) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }
    
    void ACC_FLOAD() {
        clear_color(m_clear_color);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
    
    void ACC_FSTORE() {
        GLenum attachments[] = {
            GL_COLOR_ATTACHMENT0,
            GL_DEPTH_ATTACHMENT,
            GL_STENCIL_ATTACHMENT };
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
    }
    
    void set_clear_color(ahvfx::vec4<float> color) {
        m_clear_color = color;
    }
    
    void clear_color(ahvfx::vec4<float> color) {
        if (m_clear_color != color) {
            m_clear_color = color;
        }
        glClearColor(color[0], color[1], color[2], color[3]);
    }
    
    const ahvfx::vec4<float> get_clear_color() const {
        return m_clear_color;
    }
};
typedef struct ahvfx_framebuffer_t ahvfx_framebuffer_t;

using namespace ahvfx;

framebuffer::framebuffer() {
    intl_fbo = std::shared_ptr<ahvfx_framebuffer_t>(new ahvfx_framebuffer_t());
}

framebuffer::~framebuffer() {
    decons_log();
}

void framebuffer::bind(ahvfx::vec2<uint32_t> size) {
    intl_fbo->bind(size);
}

void framebuffer::bind(ahvfx::texture_t texture) {
    intl_fbo->bind(texture);
}

const ahvfx::vec2<uint32_t> framebuffer::buffer_size() const {
    return intl_fbo->size;
}

void framebuffer::unbind() {
    
}

const ahvfx::texture_t framebuffer::out() {
    if (intl_fbo->texture) {
        return intl_fbo->texture->out();
    }
    return texture_t(0, 0., 0);
}

void framebuffer::setViewport(ahvfx::vec4<int32_t> vp) {
    intl_fbo->m_viewport = vp;
}

const ahvfx::vec4<int32_t> framebuffer::getViewport() const {
    return intl_fbo->m_viewport;
}

void framebuffer::ACC_FLOAD() {
    intl_fbo->ACC_FLOAD();
}

void framebuffer::ACC_FSTORE() {
    intl_fbo->ACC_FSTORE();
}

const void framebuffer::viewport(ahvfx::vec4<int32_t> viewport) {
    intl_fbo->viewport(viewport);
}

void framebuffer::set_clear_color(ahvfx::vec4<float> color) {
    intl_fbo->set_clear_color(color);
}

void framebuffer::clear_color(ahvfx::vec4<float> color) {
    intl_fbo->clear_color(color);
}

const ahvfx::vec4<float> framebuffer::get_clear_color() const {
    return intl_fbo->get_clear_color();
}
