//
//  ahvfx_png_handler.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/18.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_png_handler_hpp
#define ahvfx_png_handler_hpp

#include "ahvfx_obj.hpp"

ahvfx_ns_0

class png_handler_imlp;

class png_handler : public ahvfx::obj_base {
private:
    ahvfx_des;
    std::shared_ptr<png_handler_imlp> impl;
public:
    explicit png_handler(const std::string file);
    ~png_handler();
    
    const unsigned char * byte() const;
    const uint32_t width() const;
    const uint32_t height() const;
    const uint32_t component() const;
};

ahvfx_ns_1

#endif /* ahvfx_png_handler_hpp */
