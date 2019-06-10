//
//  ahvfx_io.hpp
//  AHVideoFX
//
//  Created by longxiang on 2019/3/18.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_io_hpp
#define ahvfx_io_hpp

#include "ahvfx_obj.hpp"

ahvfx_ns_0

struct texture_t {
    uint32_t id = 0;
    vec3<uint32_t> size;
    uint32_t target = GL_TEXTURE_2D;
    uint32_t fmt = GL_RGBA;
    uint32_t level = 0;
    bool owner = false;
    bool texture_parameters_initialized = false;
    void clone(texture_t &t) {
        this->id = t.id;
        this->size = t.size;
        this->target = t.target;
        this->fmt = t.fmt;
        this->owner = t.owner;
        this->texture_parameters_initialized = t.texture_parameters_initialized;
    }
    bool valid() {
        return id != 0 && size.valid() && target != 0 && texture_parameters_initialized;
    }
    const uint32_t width() const {
        return size[0];
    }
    const uint32_t height() const {
        return size[1];
    }
    const uint32_t depth() const {
        return size[2];
    }
    ahvfx_des_t;
    texture_t(uint32_t id, float width, float height) {
        this->size = vec3<uint32_t>((uint32_t)width, (uint32_t)height, 0);
        this->id = id;
        target = GL_TEXTURE_2D;
        owner = false;
        texture_parameters_initialized = false;
    }
    texture_t(uint32_t id, double width, double height) {
        this->size = vec3<uint32_t>((uint32_t)width, (uint32_t)height, 0);
        this->id = id;
        target = GL_TEXTURE_2D;
        owner = false;
        texture_parameters_initialized = false;
    }
    texture_t(uint32_t id, uint32_t width, uint32_t height) {
        this->size = vec3<uint32_t>((uint32_t)width, (uint32_t)height, 0);
        this->id = id;
        target = GL_TEXTURE_2D;
        owner = false;
        texture_parameters_initialized = false;
    }
    texture_t(uint32_t id, float width, float height, bool initialized) {
        this->size = vec3<uint32_t>((uint32_t)width, (uint32_t)height, 0);
        this->id = id;
        target = GL_TEXTURE_2D;
        owner = false;
        texture_parameters_initialized = initialized;
    }
    texture_t(uint32_t id, double width, double height, bool initialized) {
        this->size = vec3<uint32_t>((uint32_t)width, (uint32_t)height, 0);
        this->id = id;
        target = GL_TEXTURE_2D;
        owner = false;
        texture_parameters_initialized = initialized;
    }
    texture_t(uint32_t id, uint32_t width, uint32_t height, bool initialized) {
        this->size = vec3<uint32_t>((uint32_t)width, (uint32_t)height, 0);
        this->id = id;
        target = GL_TEXTURE_2D;
        owner = false;
        texture_parameters_initialized = initialized;
    }

    texture_t(uint32_t id, vec3<uint32_t> size) {
        this->id = id;
        this->size = size;
        target = GL_TEXTURE_2D;
        owner = false;
        texture_parameters_initialized = false;
    }
    texture_t() : id(0), size(vec3<uint32_t>()), target(GL_TEXTURE_2D), owner(false), texture_parameters_initialized(false) {}
    ~texture_t() {
        if (owner) {
            decons_log();
        }
    }
    
    void gen() {
        if (id == 0) {
            glGenTextures(1, &id);
        }
    }
    
    void gen_sz(uint32_t w, uint32_t h) {
        if (size.elem.x != w || size.elem.y != h) {
            SLT_DEL();
            gen();
            size = vec3<uint32_t>(w, h, 0);
            target = GL_TEXTURE_2D;
            owner = true;
            texture_parameters_initialized = false;
        }
    }
    
    void alloc(GLvoid *data = NULL) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        if (size.valid()) {
            glTexImage2D(target, level, fmt, size.elem.x, size.elem.y, 0, fmt, GL_UNSIGNED_BYTE, data);            
        }
    }
    void bind() {
        if (id) {
            glBindTexture(target, id);
        }
    }
    void unbind() {
        glBindTexture(target, 0);
    }
    void default_texture_parameters() {
        if (texture_parameters_initialized == false) {
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            texture_parameters_initialized = true;
        }
    }
    
    void SLT_DEL() {
        if (id) {
            glDeleteTextures(1, &id);
            id = 0;
        }
        size.clear();
    }
    
    bool operator== (const texture_t &t2) {
        return
        id == t2.id &&
        size == t2.size &&
        target == t2.target;
    }
    
    bool operator!= (const texture_t &t2) {
        return !(*this == t2);
    }
    
    static inline texture_t move(uint32_t id, uint32_t w, uint32_t h, bool init = true) {
        texture_t rs(id, w, h);
        rs.texture_parameters_initialized = init;
        return rs;
    }
    
    static inline texture_t move(texture_t t) {
        return texture_t(t);
    }
    
    void upload(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const void *data) {
        
//        unsigned char *c = (unsigned char *)data;
//        for (int i = 0; i < h; ++i) {
//            for (int j = 0; j < w; ++j) {
//                printf("%c ", (*(c++) > 0) ? '.' : ' ');
//            }
//            printf("\n");
//        }
//        printf("\n");

        
        glTexSubImage2D(target, 0, x, y, w, h, fmt, GL_UNSIGNED_BYTE, data);
    }
};

static ahvfx::texture_t texture_t_zero;

static inline ahvfx::texture_t ahvfx_texture_move(uint32_t id, uint32_t w, uint32_t h, bool initilized = true) {
    ahvfx::texture_t rs(id, w, h);
    rs.texture_parameters_initialized = true;
    return rs;
}

static inline void ahvfx_active_bind_texture(ahvfx::texture_t &texture, uint32_t index) {
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(texture.target, texture.id);
}

class outputable;

class inputable {
    
public:
    virtual void in(ahvfx::texture_t texture, uint32_t index, uint32_t fIdx) = 0;
    virtual void linked_by(std::weak_ptr<ahvfx::outputable> output, uint32_t index) = 0;
};

class outputable {
    
public:
    virtual const ahvfx::texture_t out() = 0;
    virtual void link(std::weak_ptr<ahvfx::inputable> input, uint32_t index) = 0;
};


ahvfx_ns_1

#endif /* ahvfx_io_hpp */
