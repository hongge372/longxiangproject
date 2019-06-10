//
//  ahvfx_gl_library.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/26.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_gl_library_hpp
#define ahvfx_gl_library_hpp

#include <stdio.h>
#include <iostream>
#include <string>

namespace ahvfx {
    class gl_library {
    public:
        explicit
            gl_library() = delete;
        ~gl_library() = delete;
        static std::string library_inline(std::string &in);
    };
}

#endif /* ahvfx_gl_library_hpp */
