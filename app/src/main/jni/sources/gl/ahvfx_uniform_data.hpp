//
//  ahvfx_uniform_data.hpp
//  AHVideoFX
//
//  Created by longxiang on 2019/6/4.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_uniform_data_hpp
#define ahvfx_uniform_data_hpp

#include <stdio.h>
#include <iostream>
#include <memory.h>

namespace ahvfx {
    class uniform_data {
    public:
        enum type {
            FLOAT1,
            FLOAT2,
            FLOAT3,
            FLOAT4,
            INT1,
            INT2,
            INT3,
            INT4,
            MATRIX22,
            MATRIX33,
            MATRIX44
        };
    public:
        std::string data_name;
        uniform_data() = default;
        uniform_data(ahvfx::uniform_data::type data_type, uint32_t num_elems);
        void pack(uint32_t index, void *value);
        void unpack(int32_t location, uint32_t index);
        void free_data();
#ifdef DEBUG
        void print();
#endif
    private:
        uint32_t num_data = 0;
        ahvfx::uniform_data::type data_type = FLOAT1;
        void *data_ptr = 0;
        size_t data_sz = 0;
        size_t stride = 0;
    };
}


#endif /* ahvfx_uniform_data_hpp */
