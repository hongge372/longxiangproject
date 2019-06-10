//
//  ahvfx_vertexarray.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/19.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_vertexarray_hpp
#define ahvfx_vertexarray_hpp

#include "ahvfx_obj.hpp"

typedef struct ahvfx_vertexarray_t ahvfx_vertexarray_t;
#if 0
#ifdef __ANDROID__
typedef GLvoid (EGLAPIENTRYP PFNGLGENVERTEXARRAYSOESPROC)(GLsizei n, GLuint *arrays);
typedef GLvoid (EGLAPIENTRYP PFNGLDELETEVERTEXARRAYSOESPROC)(GLsizei n, const GLuint *arrays);
typedef GLvoid (EGLAPIENTRYP PFNGLBINDVERTEXARRAYOESPROC)(GLuint array);
#endif
#endif

ahvfx_ns_0

class vertexarray : public ahvfx::obj_base {
public:
    std::shared_ptr<ahvfx_vertexarray_t> intl_vao;
public:
    ahvfx_des;
    vertexarray();
    ~vertexarray();
    
    void bind();
    void unbind();
    
    const uint32_t id() const;
};

ahvfx_ns_1

#endif /* ahvfx_vertexarray_hpp */
