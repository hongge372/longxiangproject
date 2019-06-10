//
//  ahvfx_graph_builder.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/21.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_grahp_builder_hpp
#define ahvfx_grahp_builder_hpp

#include "ahvfx_graph.hpp"

ahvfx_ns_0

class graph_builder {
private:
    graph_builder() = default;
    ~graph_builder() = default;
public:
    ahvfx::req_callback callback;
    static graph_builder * default_builder();
public:
    std::shared_ptr<ahvfx::graph> build_graph(std::string root, std::string pack);
};

ahvfx_ns_1

#endif /* ahvfx_grahp_builder_hpp */
