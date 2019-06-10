//
//  ahvfx_callbacks.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/25.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_callbacks_hpp
#define ahvfx_callbacks_hpp

#include "ahvfx_interface.hpp"

namespace ahvfx {
    
    typedef enum {
        image,
        video
    } request_asset_type;
    
    typedef struct {
        const char *name;
        const char *path;
        uint32_t index;
        uint32_t total_count;
        ahvfx::request_asset_type type;
        void *client;
    } request_fi_param_t;
    
    typedef void (*request_callback_1)(request_fi_param_t &param);
    
    typedef struct {
        const char *name;
        uint32_t total_count;
        uint32_t frame_index;
        uint32_t interval;
        ahvfx::request_asset_type type;
        void *client;
    } request_req_param_t;
    
    typedef ahvfx::io_texture (*request_callback_2)(request_req_param_t &param);

    struct request_callbacks {
        request_callback_1 init;
        request_callback_1 finalize;
        request_callback_2 request;
        void *client;
        request_callbacks(request_callback_1 a, request_callback_1 b, request_callback_2 c) {
            init = a;
            finalize = b;
            request = c;
        }
        request_callbacks() : init(0), request(0), finalize(0), client(0) {}
        bool init_good() {
            return client && init;
        }
        
        bool request_good() {
            return client && request;
        }

        bool finalize_good() {
            return client && finalize;
        }
        
        bool good() {
            return init_good() && request_good() && finalize_good();
        }
    };
    
    typedef struct request_callbacks request_callbacks;
    
    struct req_callback {
        request_callbacks vid_callback;
        request_callbacks img_callback;
        
        request_callbacks get(uint32_t type) {
            return type == request_asset_type::video ? vid_callback : img_callback;
        }
    };
    typedef struct req_callback req_callback;
}

#endif /* ahvfx_callbacks_hpp */
