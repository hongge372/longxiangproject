//
//  ahvfx_png.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/4/11.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_png.hpp"
#include "ahvfx_png_handler.hpp"
#include <stdio.h>
#include <unistd.h>

namespace ahvfx {
    class png_imlp {
        std::string name;
        ahvfx_des;
        std::vector<texture_t> textures;
        std::vector<std::string> valid_path;
    public:
        uint32_t interval : 8;
    public:
        explicit png_imlp() {
            interval = 1;
            char s[256];
            sprintf(s, "%p", this);
            name = std::string(s);
        }
        ~png_imlp() {
            clean();
            decons_log();
        }
        void load(const char *path) {
            if (access(path, 0) != -1) {
//                png_handler handler(path);
//                texture_t t(0, handler.width(), handler.height());
//                t.gen();
//                t.bind();
//                t.default_texture_parameters();
//                t.alloc((void *)handler.byte());
//                t.unbind();
//                textures.push_back(t);
                valid_path.push_back(path);
            } else {
                ahvfx_log("file %s not exist.", path);
            }
        }
        
        void load(std::vector<std::string> &pathes) {
            for (std::vector<std::string>::iterator it = pathes.begin(); it != pathes.end(); ++it) {
                load(it->c_str());
            }
            if (valid_path.size()) {
                textures.resize(valid_path.size());
            }
        }
        
        void clean() {
            for (std::vector<texture_t>::iterator it = textures.begin(); it != textures.end(); ++it) {
                it->SLT_DEL();
            }
            std::vector<texture_t>().swap(textures);
        }
        void set_interval(uint32_t v) {
            interval = v;
        }
        
        const uint32_t get_interval() const {
            return interval;
        }
        
        const texture_t out(uint32_t fIdx)  {
            
            if (valid_path.size()) {
                uint32_t idx = (fIdx / interval) % valid_path.size();
                if (textures[idx].id == 0) {
                    png_handler handler(valid_path[idx]);
                    texture_t t(0, handler.width(), handler.height());
                    t.gen();
                    t.bind();
                    t.default_texture_parameters();
                    if (handler.component() == 1) {
                        t.fmt = GL_LUMINANCE;
                    } else if (handler.component() == 3) {
                        t.fmt = GL_RGB;
                    }
                    t.alloc((void *)handler.byte());
                    t.unbind();
                    textures[idx] = t;
                    return t;
                }
                if (textures.size()) {
                    return textures[idx];
                }
            }
            return ahvfx::texture_t_zero;
//            ahvfx_log("idx: %d is requesting.", (fIdx / interval) % textures.size());
        }
    };
}


using namespace ahvfx;

png::png() {
    impl = std::shared_ptr<png_imlp>(new png_imlp());
}

png::~png() {
    decons_log();
}

void png::load(const char *path) {
    impl->load(path);
}

void png::load(std::vector<std::string> &pathes) {
    impl->load(pathes);
}

void png::set_interval(uint32_t v) {
    impl->set_interval(v);
}

const uint32_t png::get_interval() const {
    return impl->get_interval();
}

const texture_t png::out(uint32_t fIdx) const {
    return impl->out(fIdx);
}
