//
//  ahvfx_uniform_data.cpp
//  AHVideoFX
//
//  Created by longxiang on 2019/6/4.
//  Copyright © 2019 AH. All rights reserved.
//
#include <stdlib.h>

#include "ahvfx_uniform_data.hpp"
#include "ahvfx_plateform.h"
#include "ahvfx_defs.h"

typedef void (*unpack_float)(int, int, const float*);
typedef void (*unpack_int)(int, int, const int*);
typedef void (*unpack_matrix)(int, int, unsigned char, const float*);

using namespace ahvfx;

inline size_t nsize(ahvfx::uniform_data::type data_type, uint32_t num_elems) {
    size_t size = sizeof(float);
//    switch (data_type) {
//        case ahvfx::uniform_data::FLOAT1:
//            break;
//        case ahvfx::uniform_data::FLOAT2:
//            size *= 2;
//            break;
//        case ahvfx::uniform_data::FLOAT3:
//            size *= 3;
//            break;
//        case ahvfx::uniform_data::FLOAT4:
//            size *= 4;
//            break;
//        case ahvfx::uniform_data::INT1:
//            size = sizeof(int);
//            break;
//        case ahvfx::uniform_data::INT2:
//            size = sizeof(int);
//            size *= 2;
//            break;
//        case ahvfx::uniform_data::INT3:
//            size = sizeof(int);
//            size *= 3;
//            break;
//        case ahvfx::uniform_data::INT4:
//            size = sizeof(int);
//            size *= 4;
//            break;
//        case ahvfx::uniform_data::MATRIX22:
//            size *= 4;
//            break;
//        case ahvfx::uniform_data::MATRIX33:
//            size *= 9;
//            break;
//        case ahvfx::uniform_data::MATRIX44:
//            size *= 16;
//            break;
//        default:
//            break;
//    }
    return size * num_elems;
}

inline uint32_t num_elems_2_num_data(ahvfx::uniform_data::type data_type, uint32_t num_elems, size_t &stride) {
    stride = sizeof(float);
    if (data_type == ahvfx::uniform_data::type::FLOAT2 || data_type == ahvfx::uniform_data::type::INT2) {
        stride *= 2;
        return num_elems / 2;
    } else if (data_type == ahvfx::uniform_data::type::FLOAT3 || data_type == ahvfx::uniform_data::type::INT3) {
        stride *= 3;
        return num_elems / 3;
    } else if (data_type == ahvfx::uniform_data::type::FLOAT4 || data_type == ahvfx::uniform_data::type::INT4 || data_type == ahvfx::uniform_data::type::MATRIX22) {
        stride *= 4;
        return num_elems / 4;
    } else if (data_type == ahvfx::uniform_data::type::MATRIX33) {
        stride *= 9;
        return num_elems / 9;
    } else if (data_type == ahvfx::uniform_data::type::MATRIX44) {
        stride *= 16;
        return num_elems / 16;
    } else {
        return num_elems;
    }
    return 0;
}

void uniform_data::pack(uint32_t index, void *value) {
    if (data_ptr && stride) {
        memcpy((float *)data_ptr + index, value, sizeof(float));
    }
}

uniform_data::uniform_data(ahvfx::uniform_data::type data_type, uint32_t num_elems) {
    if (num_elems) {
        this->num_data = num_elems_2_num_data(data_type, num_elems, this->stride);
        this->data_type = data_type;
        this->data_sz = nsize(data_type, num_data);
        if (this->data_sz) {
            this->data_ptr = (void *)malloc(this->data_sz);
        }
    }
}

void uniform_data::unpack(int32_t uloc, uint32_t index) {
    if (uloc == -1 || num_data == 0) {
        return;
    }
    unpack_float unp_float = 0;
    unpack_int unp_int = 0;
    unpack_matrix unp_mat = 0;
    size_t offset = index % num_data;
    switch (data_type) {
#if 0
        case FLOAT1:
            unp_float = glUniform1fv;
            break;
        case FLOAT2:
            unp_float = glUniform2fv;
            offset *= 2;
            break;
        case FLOAT3:
            unp_float = glUniform3fv;
            offset *= 3;
            break;
        case FLOAT4:
            unp_float = glUniform4fv;
            offset *= 4;
            break;
        case INT1:
            unp_int = glUniform1iv;
            break;
        case INT2:
            unp_int = glUniform2iv;
            offset *= 2;
            break;
        case INT3:
            unp_int = glUniform3iv;
            offset *= 3;
            break;
        case INT4:
            unp_int = glUniform4iv;
            offset *= 4;
            break;
        case MATRIX22:
            unp_mat = glUniformMatrix2fv;
            offset *= 4;
            break;
        case MATRIX33:
            unp_mat = glUniformMatrix3fv;
            offset *= 9;
            break;
        case MATRIX44:
            unp_mat = glUniformMatrix4fv;
            offset *= 16;
            break;
#endif
        default:
            break;
    }
    if (unp_float) {
        unp_float(uloc, 1, (const float *)data_ptr + offset);
    } else if (unp_int) {
        unp_int(uloc, 1, (const int *)data_ptr + offset);
    } else if (unp_mat) {
        unp_mat(uloc, 1, 0, (const float *)data_ptr + offset);
    }
}

void uniform_data::free_data() {
    if (data_ptr) {
        ahvfx_log("freed uni data:%s", data_name.c_str());
        free(data_ptr);
        data_ptr = 0;
    }
}

#ifdef DEBUG
void uniform_data::print() {
    if (data_ptr) {
        for (int i = 0; i < num_data; ++i) {
            std::cout << *((float *)data_ptr + i) << std::endl;
        }        
    }
}
#endif
