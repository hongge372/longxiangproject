//
//  ahvfx_flat.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/19.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_flat_hpp
#define ahvfx_flat_hpp

#include "ahvfx_shape.hpp"

ahvfx_ns_0

class flat : public ahvfx::shape {
    
public:
    ahvfx_des;
    flat();
    ~flat();
    void draw();
};

ahvfx_ns_1

#endif /* ahvfx_flat_hpp */
