//
//  ahvfx_holder_node.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/4/1.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_holder_node.hpp"
#include "ahvfx_texture_holder.hpp"

class ahvfx_lagger_node_impl {
public:
    std::shared_ptr<ahvfx::texture_holder> texture_holder;
public:
    ahvfx_lagger_node_impl(uint32_t capacity) {
        texture_holder = std::shared_ptr<ahvfx::texture_holder>(new ahvfx::texture_holder(capacity));
    }
    
    void in(ahvfx::texture_t texture, uint32_t index, uint32_t fIdx) {
        texture_holder->hold(texture, fIdx);
    }
    
    const ahvfx::texture_t out() {
        return texture_holder->out();
    }
};

using namespace ahvfx;

lagger_node::lagger_node(uint32_t capacity) {
    impl = std::shared_ptr<ahvfx_lagger_node_impl>(new ahvfx_lagger_node_impl(capacity));
}

void lagger_node::in(ahvfx::texture_t texture, uint32_t index, uint32_t fIdx) {
    impl->in(texture, index, fIdx);
    graph_node::in(texture, index, fIdx);
}

const ahvfx::texture_t lagger_node::out() {
    return impl->out();
}

const bool lagger_node::is_pass_node() const {
    return false;
}

void lagger_node::extra_notify(std::shared_ptr<ahvfx::inputable> in, uint32_t target_idx, uint32_t f_idx) {
//    ahvfx_log("%d - %d", tgt_idx, f_idx);
    if (impl->texture_holder->full()) {
        impl->texture_holder->begin_next();
        for (int i = 1; i < output_num; ++i) {
            in->in(impl->texture_holder->next_out(), target_idx + i, f_idx);
        }
        impl->texture_holder->end_next();        
    }
}

void lagger_node::set_output_num(uint32_t n) {
    output_num = n;
    set_markable_input_num(n);
}
