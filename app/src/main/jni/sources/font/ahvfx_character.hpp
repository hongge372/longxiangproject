//
//  ahvfx_character.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/4/9.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_character_hpp
#define ahvfx_character_hpp

#include <stdio.h>
#include <iostream>

namespace ahvfx {
    class character {
    public:
        character() { x0 = y0 = x1 = y1 = offset_x = offset_y = mip_index = 0;}
    public:
        unsigned int x0:10;
        unsigned int y0:10;
        unsigned int x1:10;
        unsigned int y1:10;
        unsigned int offset_x:8;
        unsigned int offset_y:8;
        unsigned int mip_index:8;
        
        bool valid() {
            return !invalid();
        }
        
        bool invalid() {
            return x0 == 0 && y0 == 0 && x1 == 0 && y1 == 0;
        }
    };
}

#endif /* ahvfx_character_hpp */
