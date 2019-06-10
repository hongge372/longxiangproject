//
//  ahvfx_global_callback.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/4/11.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_global_callback_hpp
#define ahvfx_global_callback_hpp

#include <stdio.h>
#include <iostream>
#include "ahvfx_program.hpp"

namespace ahvfx {
    class program;
    class global_callback {
    private:
        global_callback() = default;
    public:
        std::vector<std::function<void(program *)>> funcs;
        std::function<void(program *)> func = NULL;
        std::function<void(program *)> landmark_func = NULL;
        std::function<void(program *)> device_roll_func = NULL;
        static global_callback * shared();
        void clean() {
            func = NULL;
            landmark_func = NULL;
            device_roll_func = NULL;
            std::vector<std::function<void(program *)>>().swap(funcs);
        }
    };
}

#endif /* ahvfx_global_callback_hpp */
