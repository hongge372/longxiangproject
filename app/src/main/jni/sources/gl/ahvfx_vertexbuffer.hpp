//
//  ahvfx_vertexbuffer.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/19.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_vertexbuffer_hpp
#define ahvfx_vertexbuffer_hpp

#include "ahvfx_obj.hpp"

typedef struct ahvfx_vertexbuffer_t ahvfx_vertexbuffer_t;

ahvfx_ns_0

class vertexbuffer : public ahvfx::obj_base {
public:
    std::shared_ptr<ahvfx_vertexbuffer_t> intl_vbo;
public:
    ahvfx_des;
    vertexbuffer(uint32_t type);
    ~vertexbuffer();
    
    const void bind() const;
    const void unbind() const;
    void buffer(void *datapointer, size_t size, uint32_t usage);
};

ahvfx_ns_1

#endif /* ahvfx_vertexbuffer_hpp */
