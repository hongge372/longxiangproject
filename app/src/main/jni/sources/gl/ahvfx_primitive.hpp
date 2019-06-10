//
//  ahvfx_primitive.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/19.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_primitive_hpp
#define ahvfx_primitive_hpp

#include "ahvfx_obj.hpp"

ahvfx_ns_0

class primitive : public ahvfx::obj_base {
public:
    uint32_t type;
    uint32_t startIdx;
    uint32_t count;
public:
    ahvfx_des;
    primitive();
    ~primitive();
    void assembly(bool elems = false, void *indices = NULL);
};

ahvfx_ns_1

#endif /* ahvfx_primitive_hpp */
