//
//  ahvfx_video_flat.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/19.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_video_flat_hpp
#define ahvfx_video_flat_hpp

#include "ahvfx_obj.hpp"

typedef struct ahvfx_video_flat_t ahvfx_video_flat_t;

namespace ahvfx {
    class video_flat {
    private:
        std::shared_ptr<ahvfx_video_flat_t> intl_flat;
        video_flat();
        ~video_flat() = delete;
        static video_flat *CREATE();
        void in_draw();
    public:
        static void draw();
    };
}

#endif /* ahvfx_video_flat_hpp */
