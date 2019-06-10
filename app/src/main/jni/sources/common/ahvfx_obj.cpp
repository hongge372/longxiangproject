//
//  ahvfx_object.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/18.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_obj.hpp"

using namespace ahvfx;

obj_base::obj_base() {
    char n[32];
    sprintf(n, "%p", this);
    name = std::string(n);
}

obj_base::~obj_base() {
//    decons_log();
}

void obj_base::set_name(std::string s) {
    name = s;
}
const std::string obj_base::get_name() const {
    return name;
}
