//
//  ahvfx_global_callback.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/4/11.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_global_callback.hpp"
#include "ahvfx_program.hpp"

using namespace ahvfx;

global_callback * global_callback::shared() {
    static global_callback *rs = NULL;
#if 0
    static std::once_flag oc;
    std::call_once(oc, [&]{
        rs = new global_callback();
    });
#endif
    return rs;
}
