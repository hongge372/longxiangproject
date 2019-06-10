//
//  ahvfx_holder_node.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/4/1.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_holder_node_hpp
#define ahvfx_holder_node_hpp

#include "ahvfx_graph_node.hpp"

class ahvfx_lagger_node_impl;

ahvfx_ns_0

class lagger_node : public ahvfx::graph_node {
private:
    std::shared_ptr<ahvfx_lagger_node_impl> impl;
    const bool is_pass_node() const;
    uint32_t output_num = 1;
public:
    explicit lagger_node(uint32_t capacity);
    void in(ahvfx::texture_t texture, uint32_t index, uint32_t fIdx);
    const ahvfx::texture_t out();
    void extra_notify(std::shared_ptr<ahvfx::inputable> in, uint32_t target_idx, uint32_t f_idx);
    void set_output_num(uint32_t n);
};

ahvfx_ns_1

#endif /* ahvfx_holder_node_hpp */
