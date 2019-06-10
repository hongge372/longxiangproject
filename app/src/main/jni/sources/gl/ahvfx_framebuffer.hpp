//
//  ahvfx_framebuffer.hpp
//  AHVideoFX
//
//  Created by longxiang on 2019/3/18.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_framebuffer_hpp
#define ahvfx_framebuffer_hpp

#include "ahvfx_texture.hpp"

typedef struct ahvfx_framebuffer_t ahvfx_framebuffer_t;

ahvfx_ns_0

class framebuffer : public ahvfx::obj_base {
public:
    std::shared_ptr<ahvfx_framebuffer_t> intl_fbo;
public:
    ahvfx_des;
    framebuffer();
    ~framebuffer();
    void bind(ahvfx::vec2<uint32_t> size);
    void bind(ahvfx::texture_t texture);
    void unbind();
    const ahvfx::texture_t out();

    void setViewport(ahvfx::vec4<int32_t> viewport);
    const ahvfx::vec4<int32_t> getViewport() const;
    const void viewport(ahvfx::vec4<int32_t> viewport);
    
    const ahvfx::vec2<uint32_t> buffer_size() const;
    
    void set_clear_color(ahvfx::vec4<float> color);
    void clear_color(ahvfx::vec4<float> color);
    const ahvfx::vec4<float> get_clear_color() const;
    
    void ACC_FLOAD();
    void ACC_FSTORE();
    
};

ahvfx_ns_1

#endif /* ahvfx_framebuffer_hpp */
