//
//  ahvfx_png.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/4/11.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_png_hpp
#define ahvfx_png_hpp

#include "ahvfx_obj.hpp"
#include "ahvfx_io.hpp"

namespace ahvfx {
    class png_imlp;
    class png : public obj_base {
    private:
        std::shared_ptr<png_imlp> impl;
    public:
    public:
        explicit png();
        ~png();
        void load(const char *path);
        void load(std::vector<std::string> &pathes);
        void set_interval(uint32_t v);
        const uint32_t get_interval() const;
        
        const texture_t out(uint32_t fIdx) const;
    };
}

#endif /* ahvfx_png_hpp */
