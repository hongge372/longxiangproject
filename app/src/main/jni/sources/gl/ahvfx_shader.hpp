//
//  ahvfx_shader.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/18.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_shader_hpp
#define ahvfx_shader_hpp

#include "ahvfx_obj.hpp"

typedef struct ahvfx_shader_t ahvfx_shader_t;

ahvfx_ns_0

class shader : public ahvfx::obj_base {
public:
    std::shared_ptr<ahvfx_shader_t> intl_shader;
    ahvfx_des;
    shader(const char *src, uint32_t type);
    ~shader();
    
    const int id() const;
    const bool good() const;
};

ahvfx_ns_1

#endif /* ahvfx_shader_hpp */
