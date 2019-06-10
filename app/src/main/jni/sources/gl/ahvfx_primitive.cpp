//
//  ahvfx_primitive.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/19.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_primitive.hpp"

using namespace ahvfx;

/*
 uint32_t type;
 uint32_t startIdx;
 uint32_t count;
 bool use_indices;
 */

primitive::primitive() {
    type = 0;
    startIdx = 0;
    count = 0;
}

primitive::~primitive() {
    decons_log();
}

void primitive::assembly(bool elems, void *indices) {
    if (elems) {
        glDrawElements(type, count, GL_UNSIGNED_SHORT, indices);
    } else {
        glDrawArrays(type, startIdx, count);
    }
}
