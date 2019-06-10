//
//  ahvfx_font.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/4/9.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_font_hpp
#define ahvfx_font_hpp

#include <stdio.h>
#include <iostream>

namespace ahvfx {
    class font_impl;
    class font {
    private:
        std::shared_ptr<font_impl> impl;
    public:
        explicit font();
        ~font();
        void face(const char *font_Path, size_t font_Size);
        void drawText(uint32_t x, uint32_t y, const char *text, size_t len = -1);
    };
}

#endif /* ahvfx_font_hpp */
