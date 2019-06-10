//
//  ahvfx_interface.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/22.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_interface_hpp
#define ahvfx_interface_hpp

#include <iostream>

namespace ahvfx {
    
    struct io_size {
        uint32_t w;
        uint32_t h;
    };
    
    static const io_size size_zero {0,0};
    
    static inline io_size make_size(uint32_t w, uint32_t h) {
        return io_size{w, h};
    }
    
    struct io_texture {
        uint32_t id;
        io_size size;
    };
    
    static inline io_texture make_texture(uint32_t id, io_size size) {
        return io_texture{id, size};
    }

    class interface {
        virtual void io(uint32_t itex, io_size isize, uint32_t &ot, io_size &osize, uint32_t frameidx = 0) = 0;
        virtual io_size osize() = 0;
        virtual uint32_t otex() = 0;
        virtual uint32_t frameidx() = 0;
    };
}

#endif /* ahvfx_interface_hpp */
