//
//  ahvfx_flat.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/19.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_flat.hpp"
#include "ahvfx_program.hpp"

extern "C" {
    void ahvfx_gl_enable_attribute(uint32_t attrib_idx, int32_t sz, uint32_t data_type, int32_t normalized, int32_t stride, const void *ptr);
}

using namespace ahvfx;

flat::flat() {
    etnl_shape = std::shared_ptr<ahvfx_shape_t>(new ahvfx_shape_t());
    etnl_shape->VAO = std::shared_ptr<ahvfx::vertexarray>(new ahvfx::vertexarray());
    etnl_shape->VAO->bind();
    static GLfloat vert[] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
        1.0f,  1.0f, 1.0f, 1.0f,
    };
    etnl_shape->VBO = std::shared_ptr<ahvfx::vertexbuffer>(new ahvfx::vertexbuffer(GL_ARRAY_BUFFER));
    etnl_shape->VBO->bind();
    etnl_shape->VBO->buffer(vert, sizeof(vert), GL_STATIC_DRAW);
    ahvfx_gl_enable_attribute(ahvfx::ATTRIBUTE_POSITION, 2, 0x1406, 0, sizeof(GLfloat) * 4, NULL);
    ahvfx_gl_enable_attribute(ahvfx::ATTRIBUTE_TEXCOORD, 2, 0x1406, 0, sizeof(GLfloat) * 4, (GLvoid *)(sizeof(GLfloat) * 2));
    etnl_shape->IBO = std::shared_ptr<ahvfx::vertexbuffer>(new ahvfx::vertexbuffer(GL_ELEMENT_ARRAY_BUFFER));
    etnl_shape->IBO->bind();
    static GLushort indices[] = {
        0, 1, 2, 1, 2, 3
    };
    etnl_shape->IBO->buffer(indices, sizeof(indices), GL_STATIC_DRAW);

    etnl_shape->VAO->unbind();

    etnl_shape->VBO->unbind();
    etnl_shape->IBO->unbind();

    etnl_shape->PRIM = std::shared_ptr<ahvfx::primitive>(new ahvfx::primitive());
    etnl_shape->PRIM->count = sizeof(indices) / sizeof(indices[0]);
    etnl_shape->PRIM->type = GL_TRIANGLES;
}

flat::~flat() {
    decons_log();
}

void flat::draw() {
    if (etnl_shape != NULL) {
        if (true) {
            etnl_shape->VAO->bind();
            etnl_shape->VBO->bind();
            ahvfx_gl_enable_attribute(ahvfx::ATTRIBUTE_POSITION, 2, 0x1406, 0, sizeof(GLfloat) * 4, NULL);
            etnl_shape->IBO->bind();
            ahvfx_gl_enable_attribute(ahvfx::ATTRIBUTE_TEXCOORD, 2, 0x1406, 0, sizeof(GLfloat) * 4, (GLvoid *)(sizeof(GLfloat) * 2));
            etnl_shape->VAO->unbind();
        }
        etnl_shape->VAO->bind();
        etnl_shape->PRIM->assembly(etnl_shape->IBO != NULL, NULL);
        etnl_shape->VAO->unbind();
        etnl_shape->VBO->unbind();
        etnl_shape->IBO->unbind();
        glDisableVertexAttribArray(ahvfx::ATTRIBUTE_POSITION);
        glDisableVertexAttribArray(ahvfx::ATTRIBUTE_TEXCOORD);
    } else {
        ahvfx_log( "ahvfx primitive null.\n");
    }
}
