//
//  ahvfx_vertexbuffer.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/19.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_vertexbuffer.hpp"

struct ahvfx_vertexbuffer_t {
    uint32_t id;
    uint32_t type;
    ahvfx_des_t;
    ahvfx_vertexbuffer_t(uint32_t type) {
        id = 0;
        this->type = type;
    }
    ~ahvfx_vertexbuffer_t() {
        destroy();
        decons_log();
    }
    void bind() {
        create();
        if (id) {
            glBindBuffer(type, id);
        }
    }
    
    const void unbind() const {
        glBindBuffer(type, 0);
    }
    
    void create() {
        if (id == 0) {
            glGenBuffers(1, &id);
        }
    }
    
    void destroy() {
        if (id) {
            glDeleteBuffers(1, &id);
            id = 0;
        }
    }
    
    void buffer(void *datapointer, size_t size, uint32_t usage) {
        if (size && datapointer) {
            glBufferData(type, size, datapointer, usage);
        }
    }
};
typedef struct ahvfx_vertexbuffer_t ahvfx_vertexbuffer_t;

using namespace ahvfx;

vertexbuffer::vertexbuffer(uint32_t type) {
    intl_vbo = std::shared_ptr<ahvfx_vertexbuffer_t>(new ahvfx_vertexbuffer_t(type));
}

vertexbuffer::~vertexbuffer() {
    decons_log();
}

const void vertexbuffer::bind() const {
    intl_vbo->bind();
}

const void vertexbuffer::unbind() const {
    intl_vbo->unbind();
}

void vertexbuffer::buffer(void *datapointer, size_t size, uint32_t usage) {
    intl_vbo->buffer(datapointer, size, usage);
}
