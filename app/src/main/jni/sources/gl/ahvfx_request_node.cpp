//
//  ahvfx_request_node.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/20.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_request_node.hpp"
#include "ahvfx_util.hpp"
#include "ahvfx_png.hpp"

struct ahvfx_request_node_t {
    std::string name;
    uint32_t fIdx = 0;
    uint32_t request_node_type = 0;
    std::vector<std::string> referred_assets;
    ahvfx::texture_t source_texture;
    ahvfx::req_callback req_callback;
    std::shared_ptr<ahvfx::png> png;
    ahvfx::range_type now_range;
    uint32_t animation_interval = 1;
    ahvfx_des_t;
    ahvfx_request_node_t() {
        
    }
    ~ahvfx_request_node_t() {
        decons_log();
    }
    
    inline uint32_t clamp(uint32_t v, uint32_t m1, uint32_t m2) {
        return v > m1 ? (v < m2 ? v : m2) : m1;
    }
        
//    typedef ahvfx::io_texture (*request_callback_2)(const char *refered_asset, request_asset_type type, void *client, uint32_t fIdx);

    void request_inputs() {
        if (req_callback.get(request_node_type).request_good()) {
            
            ahvfx::request_req_param_t para;
            para.name = name.c_str();
            para.total_count = (uint32_t)referred_assets.size();
            para.type = (ahvfx::request_asset_type)request_node_type;
            para.frame_index = clamp(fIdx - now_range.second.first, 0, 0xffff);
            para.interval = animation_interval;
            para.client = req_callback.get(request_node_type).client;
            
            ahvfx::io_texture t = req_callback.get(request_node_type).request(para);
            
//            ahvfx::io_texture t = req_callback.get(request_node_type).request(name.c_str(), (uint32_t)referred_assets.size(), (ahvfx::request_asset_type)request_node_type, req_callback.get(request_node_type).client, fIdx);
            source_texture = ahvfx::texture_t::move(t.id, t.size.w, t.size.h);
        } else {
            if (request_node_type == ahvfx::request_asset_type::image) {
                source_texture = png->out(clamp(fIdx - now_range.second.first, 0, 0xffff));
//                ahvfx_log("internal png decode impl is requesting %d : {%d x %d}.", source_texture.id, source_texture.width(), source_texture.height());
            }
        }
    }
    
    void request_init() {
        if (req_callback.get(request_node_type).init_good()) {
            uint32_t asset_count = (uint32_t)referred_assets.size();
            ahvfx::foreach<std::string>(referred_assets, [&](std::string path, uint32_t index){
//                req_callback.init(NULL, (ahvfx::request_asset_type)request_node_type, req_callback.client);
//                typedef void (*request_callback_1)(const char *refered_asset, uint32_t refered_asset_index, request_asset_type type, void *client);
//                typedef ahvfx::io_texture (*request_callback_2)(const char *refered_asset, uint32_t refered_asset_index, request_asset_type type, void *client, uint32_t fIdx);
                
                
                ahvfx::request_fi_param_t para;
                para.name = name.c_str();
                para.path = path.c_str();
                para.index = clamp(fIdx - now_range.second.first, 0, 0xffff);
                para.total_count = asset_count;
                para.type = (ahvfx::request_asset_type)request_node_type;
                para.client = req_callback.get(request_node_type).client;
                req_callback.get(request_node_type).init(para);
//                req_callback.get(request_node_type).init(name.c_str(), path.c_str(), index, asset_count, (ahvfx::request_asset_type)request_node_type, req_callback.get(request_node_type).client);
            });
        } else {
            if (request_node_type == ahvfx::request_asset_type::image) {
                png = std::shared_ptr<ahvfx::png>(new ahvfx::png());
                png->set_interval(animation_interval);
                png->load(std::ref(referred_assets));
                ahvfx_log("internal png decode impl is using.");
            }
        }
//        if (req_callback.request && req_callback.request) {
//            std::vector<std::string>::iterator it;
//            int index = 0;
//            for (it = referred_assets.begin(); it < referred_assets.end(); ++it) {
////                req_ctx.init_request(req_ctx.context, it->c_str(), index++);
//                req_callback.init(NULL, (ahvfx::request_asset_type)request_node_type, req_callback.client);
//            }
//        } else {
//            ahvfx_log("request init no callbacks. %d\n", 1);
//        }
    }
    
    void request_uninit() {
//        if (req_ctx.texture_requset && req_ctx.context) {
//            std::vector<std::string>::iterator it;
//            int index = 0;
//            for (it = referred_assets.begin(); it < referred_assets.end(); ++it) {
//                req_ctx.uninit_request(req_ctx.context, it->c_str(), index++);
//            }
//        } else {
//            ahvfx_log("request uninit no callbacks. %d\n", 1);
//        }
        if (req_callback.get(request_node_type).finalize_good()) {
            uint32_t asset_count = (uint32_t)referred_assets.size();
            ahvfx::foreach<std::string>(referred_assets, [&](std::string path, uint32_t index){
                
                ahvfx::request_fi_param_t para;
                para.name = name.c_str();
                para.path = path.c_str();
                para.index = index;
                para.total_count = asset_count;
                para.type = (ahvfx::request_asset_type)request_node_type;
                para.client = req_callback.get(request_node_type).client;
                req_callback.get(request_node_type).finalize(para);
//                req_callback.get(request_node_type).finalize(name.c_str(), path.c_str(), index, asset_count, (ahvfx::request_asset_type)request_node_type, req_callback.get(request_node_type).client);
            });
        } else {
            if (request_node_type == ahvfx::request_asset_type::image) {
                png = NULL;
            }
        }
    }
    
    void add_referred_asset(std::string asset_path) {
        referred_assets.push_back(asset_path);
    }
};
typedef struct ahvfx_request_node_t ahvfx_request_node_t;

using namespace ahvfx;

void request_node::set_request_node_type(uint32_t type) {
    request_node_type = type;
    intl_holder->request_node_type = type;
}

request_node::request_node() {
    intl_holder = std::shared_ptr<ahvfx_request_node_t>(new ahvfx_request_node_t());
    set_initial(true);
    always_update = true;
}

void request_node::set_name(std::string name) {
    graph_node::set_name(name);
    intl_holder->name = name;
}

void request_node::set_name(std::string package, std::string name) {
    graph_node::set_name(name);
    intl_holder->name = package + "." + name;
}

request_node::~request_node() {
    request_uninit();
    decons_log();
}

void request_node::request_init() {
    intl_holder->request_init();
}
void request_node::request_uninit() {
    intl_holder->request_uninit();
}

void request_node::request_inputs() {
    if (inranges()) {
        graph_node::request_inputs();
        intl_holder->now_range = closestrange();
        intl_holder->request_inputs();
        notify();
    } else {
        intl_holder->now_range = ahvfx::make_range(0, 0, 0);
    }
}

void request_node::set_callbacks(ahvfx::req_callback callback) {
    intl_holder->req_callback = callback;
}

const bool request_node::is_pass_node() const {
    return false;
}

const ahvfx::texture_t request_node::out() {
    return intl_holder->source_texture;
}

void request_node::in(ahvfx::texture_t texture, uint32_t index, uint32_t fIdx) {
    intl_holder->fIdx = fIdx;
    graph_node::in(texture, index, fIdx);
}


void request_node::add_referred_asset(std::string asset_path) {
    intl_holder->add_referred_asset(asset_path);
}

void request_node::draw() {
    //never draw
}

void request_node::set_animation_interval(uint32_t interval) {
    intl_holder->animation_interval = interval;
}
