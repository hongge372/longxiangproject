//
//  ahvfx_texture.cpp
//  AHVideoFX
//
//  Created by longxiang on 2019/3/18.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_texture.hpp"

using namespace ahvfx;

void texture::create() {
    if (intl_tex.id == 0) {
        glGenTextures(1, &intl_tex.id);
    }
}

void texture::destroy() {
    if (intl_tex.id != 0) {
        glDeleteTextures(1, &intl_tex.id);
        intl_tex.id = 0;
    }
}

texture::texture(uint32_t fmt) {
    this->fmt = fmt;
}

texture::texture() {
//    intl_tex = ahvfx::texture_t(new ahvfx::texture_t());
}

texture::~texture() {
//    intl_tex->destory();
    destroy();
    decons_log();
}

bool texture::bind() {
    return bind(intl_tex.size);
}

bool texture::bind(ahvfx::vec2<uint32_t> sz2) {
    return bind(ahvfx::vec3<uint32_t>(sz2, 0));
}

bool texture::bind(ahvfx::vec3<uint32_t> sz3) {
    if (intl_tex.size != sz3) {
        destroy();
        create();
        intl_tex.fmt = fmt;
        intl_tex.size = sz3;
        intl_tex.bind();
        intl_tex.alloc();
        intl_tex.default_texture_parameters();
    } else {
        intl_tex.bind();
    }
    return intl_tex.id != 0;
}

const ahvfx::texture_t texture::out() {
    return intl_tex;
}

void texture::upload(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const void *data) {
    intl_tex.upload(x, y, w, h, data);
}
