//
//  ahvfx_font_manager.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/4/9.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_font_manager.hpp"
#include "ft2build.h"
#include "freetype/freetype.h"

namespace ahvfx {
    class font_manager_impl {
    public:
        FT_Library library;
        FT_Face face;
    public:
        font_manager_impl() {
            FT_Error error = FT_Init_FreeType(&library);
            if (error != FT_Err_Ok) {
                assert(0);
            }
        }
        ~font_manager_impl() {
            
        }
        bool load_font(const char *font_path) {
            FT_Error error = FT_New_Memory_Face(library, (const FT_Byte *)font_path, 20, 0, &face);
            return 1;
        }
    };
}

using namespace ahvfx;

bool font_manager::load_font(const char *font_path) {
    return impl->load_font(font_path);
}
