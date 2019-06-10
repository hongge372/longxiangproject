//
//  ahvfx_drawable.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/19.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_drawable_hpp
#define ahvfx_drawable_hpp

#include <stdio.h>

namespace ahvfx {
    class drawable {
        virtual void draw() = 0;
    };
}

#endif /* ahvfx_drawable_hpp */
