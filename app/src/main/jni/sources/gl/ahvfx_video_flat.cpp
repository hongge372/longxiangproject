//
//  ahvfx_video_flat.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/19.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_video_flat.hpp"
#include <pthread.h>
#include <mutex>
#include "ahvfx_flat.hpp"

struct ahvfx_video_flat_t {
    std::shared_ptr<ahvfx::flat> intl_flat;
    ahvfx_video_flat_t() {
        intl_flat = std::shared_ptr<ahvfx::flat>(new ahvfx::flat());
    }
    void draw() {
        intl_flat->draw();
    }
};
typedef struct ahvfx_video_flat_t ahvfx_video_flat_t;

using namespace ahvfx;

video_flat * video_flat::CREATE() {
    static video_flat *ins = NULL;
    static std::once_flag oc;
    std::call_once(oc, [&]{
        ins = new video_flat();
    });
    return ins;
}

void video_flat::draw() {
    CREATE()->in_draw();
}

video_flat::video_flat() {
    intl_flat = std::shared_ptr<ahvfx_video_flat_t>(new ahvfx_video_flat_t());
}

void video_flat::in_draw() {
    intl_flat->draw();
}
