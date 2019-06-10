//
//  ahvfx_vertexarray.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/19.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_vertexarray.hpp"

//#if GL_ES_VERSION_2_0
#if 0
#   define glGenVertexArrays glGenVertexArraysOES
#   define glBindVertexArray glBindVertexArrayOES
#   define glDeleteVertexArrays glDeleteVertexArraysOES
#endif

struct ahvfx_vertexarray_t {
    ahvfx_des_t;
    uint32_t id;
    ahvfx_vertexarray_t()
    : id(0)
    {
        
    }
    ~ahvfx_vertexarray_t() {
        destroy();
        decons_log();
    }
    void create() {
        if (id == 0) {
            glGenVertexArrays(1, &id);
        }
    }
    void destroy() {
        if (id) {
            glDeleteVertexArrays(1, &id);
            id = 0;
        }
    }
    void bind() {
        create();
        glBindVertexArray(id);
    }
    void unbin() {
        glBindVertexArray(0);
    }
};
typedef struct ahvfx_vertexarray_t ahvfx_vertexarray_t;

using namespace ahvfx;

vertexarray::vertexarray() {
    intl_vao = std::shared_ptr<ahvfx_vertexarray_t>(new ahvfx_vertexarray_t());
}

vertexarray::~vertexarray() {
    decons_log();
}

const uint32_t vertexarray::id() const {
    return intl_vao->id;
}

void vertexarray::bind() {
    intl_vao->bind();
}
void vertexarray::unbind() {
    intl_vao->unbin();
}
