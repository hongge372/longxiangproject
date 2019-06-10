//
//  ahvfx_fxmodel.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/22.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_fx_model.hpp"

using namespace ahvfx;

fxmodel::fxmodel(std::string root, std::string name) {
    this->root = root;
    this->name = name;
}

const bool fxmodel::isgood() const {
    return true;
}
