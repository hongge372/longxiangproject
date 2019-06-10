//
//  ahvfx_texture.hpp
//  AHVideoFX
//
//  Created by longxiang on 2019/3/18.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_texture_hpp
#define ahvfx_texture_hpp

#include "ahvfx_obj.hpp"
#include "ahvfx_io.hpp"

ahvfx_ns_0

class texture : ahvfx::obj_base {
    void create();
    void destroy();
    uint32_t fmt = GL_RGBA;
public:
    ahvfx::texture_t intl_tex;
public:
    ahvfx_des;
    texture(uint32_t fmt);
    texture();
    ~texture();
    bool bind();
    bool bind(ahvfx::vec2<uint32_t> sz2);
    bool bind(ahvfx::vec3<uint32_t> sz3);
    
    const ahvfx::texture_t out();
    void upload(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const void *data);
};


ahvfx_ns_1
#endif /* ahvfx_texture_hpp */
