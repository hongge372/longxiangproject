//
//  ahvfx_shape.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/19.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_shape_hpp
#define ahvfx_shape_hpp

#include "ahvfx_obj.hpp"
#include "ahvfx_drawable.hpp"
#include "ahvfx_primitive.hpp"
#include "ahvfx_vertexbuffer.hpp"
#include "ahvfx_vertexarray.hpp"

struct ahvfx_shape_t {
    std::shared_ptr<ahvfx::vertexarray> VAO;
    std::shared_ptr<ahvfx::vertexbuffer> VBO;
    std::shared_ptr<ahvfx::vertexbuffer> IBO;
    std::shared_ptr<ahvfx::primitive> PRIM;
    ahvfx_shape_t() = default;
};
typedef struct ahvfx_shape_t ahvfx_shape_t;

ahvfx_ns_0

class shape : public ahvfx::obj_base , public ahvfx::drawable {
public:
    std::shared_ptr<ahvfx_shape_t> etnl_shape;
public:
    ahvfx_des;
    shape() = default;
    ~shape();
    virtual void draw() = 0;
};

ahvfx_ns_1

#endif /* ahvfx_shape_hpp */
