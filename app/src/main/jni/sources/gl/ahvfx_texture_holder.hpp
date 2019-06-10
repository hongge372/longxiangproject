//
//  ahvfx_texture_holder.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/4/1.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_texture_holder_hpp
#define ahvfx_texture_holder_hpp

#include "ahvfx_obj.hpp"

class ahvfx_texture_holder_impl;

ahvfx_ns_0

typedef struct texture_t texture_t;
class texture_holder : public obj_base {
public:
    std::shared_ptr<ahvfx_texture_holder_impl> impl;
public:
    ahvfx_des;
    texture_holder() = delete;
    explicit texture_holder(uint32_t capacity = 0);
    ~texture_holder();
    
    void hold(ahvfx::texture_t in, uint32_t fIdx);
    
    const ahvfx::texture_t out() const;
    void begin_next();
    void end_next();
    const ahvfx::texture_t next_out();
    
    bool full() const;
};


ahvfx_ns_1

#endif /* ahvfx_texture_holder_hpp */
