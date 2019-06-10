//
//  ahvfx_util.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/25.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_util_hpp
#define ahvfx_util_hpp

#include <iostream>

namespace ahvfx {
    template <typename T>
    void foreach(std::vector<T> &vec, std::function<void(T, uint32_t)> func) {
        typename std::vector<T>::iterator it;
        uint32_t index = 0;
        for (it = vec.begin(); it != vec.end(); ++it) {
            func(*it, index++);
        }
    }
}

#endif /* ahvfx_util_hpp */
