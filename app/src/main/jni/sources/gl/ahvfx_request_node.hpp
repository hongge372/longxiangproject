//
//  ahvfx_request_node.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/20.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_request_node_hpp
#define ahvfx_request_node_hpp

#include "ahvfx_graph_node.hpp"
#include "ahvfx_callbacks.hpp"

typedef struct ahvfx_request_node_t ahvfx_request_node_t;

ahvfx_ns_0

typedef struct request_info_t request_info_t;

class request_node : public ahvfx::graph_node {
private:
    ahvfx_des;
    std::shared_ptr<ahvfx_request_node_t> intl_holder;
    uint32_t request_node_type = 0;
public:
    void set_request_node_type(uint32_t type);
public:
    void add_referred_asset(std::string asset_path);
    void set_animation_interval(uint32_t interval);
public:
    void set_name(std::string name);
    void set_name(std::string package, std::string name);
public:
    request_node();
    ~request_node();
    void set_callbacks(ahvfx::req_callback callback);
    void request_init();
    void request_inputs();
    void request_uninit();
    const bool is_pass_node() const;
public:
    const ahvfx::texture_t out();
    void in(ahvfx::texture_t texture, uint32_t index, uint32_t fIdx);
    void draw();
};

ahvfx_ns_1

#endif /* ahvfx_request_node_hpp */
