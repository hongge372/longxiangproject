//
//  ahvfx_font_manager.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/4/9.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_font_manager_hpp
#define ahvfx_font_manager_hpp

#include <stdio.h>
#include <iostream>

namespace ahvfx {
    class font_manager_impl;
    class font_manager {
        std::shared_ptr<font_manager_impl> impl;
        bool load_font(const char *font_path);
    };
}


#endif /* ahvfx_font_manager_hpp */
