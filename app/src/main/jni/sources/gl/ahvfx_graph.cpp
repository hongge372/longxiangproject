//
//  ahvfx_graph.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/20.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_graph.hpp"

struct ahvfx_graph_t {
    ahvfx_des_t;
    uint32_t m_version = 0;
    std::map<std::string, std::shared_ptr<ahvfx::program>> program_cache;
    std::map<std::string, std::shared_ptr<ahvfx::graph_node>> node_cache;
    std::vector<std::weak_ptr<ahvfx::graph_node>> initial_nodes;
    std::weak_ptr<ahvfx::graph_node> terminal_node;
    std::map<std::string, ahvfx::uniform_data> unid_cache;

    ahvfx_graph_t() = default;
    
    ~ahvfx_graph_t() {
        for (std::map<std::string, ahvfx::uniform_data>::iterator it = unid_cache.begin(); it != unid_cache.end(); ++it) {
            it->second.free_data();
        }
        decons_log();
    }
    
    uint32_t build_version() {
        return m_version;
    }
    
    void set_build_version(uint32_t version) {
        m_version = version;
    }
    
    void store(ahvfx::uniform_data unid) {
        unid_cache[unid.data_name] = unid;
    }
    
    const ahvfx::uniform_data restore_unid(const std::string name) {
        return unid_cache[name];
    }
    
    void store(std::shared_ptr<ahvfx::program> program) {
        program_cache[program->get_name()] = program;
    }
    
    const std::shared_ptr<ahvfx::program> restore2(const std::string name) {
        return program_cache[name];
    }
    
    void store(std::shared_ptr<ahvfx::graph_node> node) {
        if (node->is_terminal()) {
            terminal_node = node;
        }
        if (node->is_initial()) {
            initial_nodes.push_back(node);
        }
        node_cache[node->get_name()] = node;
    }
    
    const std::shared_ptr<ahvfx::graph_node> restore(const std::string name) {
        return node_cache[name];
    }
    
    void link(std::weak_ptr<ahvfx::inputable> input, uint32_t index) {
        if (terminal_node.expired()) {
            terminal_node.lock()->link(input, index);
        }
    }
    
    void linked_by(std::weak_ptr<ahvfx::outputable> output, uint32_t index) {
        std::vector<std::weak_ptr<ahvfx::graph_node>>::iterator it;
        for (it = initial_nodes.begin(); it != initial_nodes.end(); ++it) {
            std::weak_ptr<ahvfx::graph_node> p = *it;
            if (!p.expired()) {
                p.lock()->linked_by(output, index);
            }
        }
    }
    
    void request_inputs() {
        std::vector<std::weak_ptr<ahvfx::graph_node>>::iterator it;
        for (it = initial_nodes.begin(); it != initial_nodes.end(); ++it) {
            std::weak_ptr<ahvfx::graph_node> p = *it;
            if (!p.expired()) {
                p.lock()->request_inputs();
            }
        }
    }
    
    void in(ahvfx::texture_t texture, uint32_t index, uint32_t fIdx) {
        bool need_request = false;
        std::vector<std::weak_ptr<ahvfx::graph_node>>::iterator it;
        for (it = initial_nodes.begin(); it != initial_nodes.end(); ++it) {
            std::weak_ptr<ahvfx::graph_node> p = *it;
            if (!p.expired()) {
                if (p.lock()->inranges() && p.lock()->is_pass_node()) {
                    need_request = true;
                }
                p.lock()->in(texture, index, fIdx);
            }
        }
        if (index == 0 && need_request) {
            request_inputs();
        }
    }
    
    const ahvfx::texture_t out() {
        if (!terminal_node.expired()) {
            return terminal_node.lock()->out();
        }
        ahvfx_log("graph has no termial.\n");
#ifdef DEBUG
        assert(0);
#endif
        return ahvfx::texture_t(0, 10., 10.);
    }
    
    void reset_ranges(std::vector<ahvfx::range_type>& ranges) {
        for (std::map<std::string, std::shared_ptr<ahvfx::graph_node>>::iterator it = node_cache.begin(); it != node_cache.end(); ++it) {
            std::shared_ptr<ahvfx::graph_node> node = it->second;
            if (node) {
                node->ranges = ranges;
            }
        }
    }
};
typedef struct ahvfx_graph_t ahvfx_graph_t;

using namespace ahvfx;

uint32_t graph::build_version() {
    return intl_graph->build_version();
}

void graph::set_build_version(uint32_t version) {
    intl_graph->set_build_version(version);
}

graph::graph() {
    intl_graph = std::shared_ptr<ahvfx_graph_t>(new ahvfx_graph_t());
}

graph::~graph() {
    std::cout << std::endl;
    ahvfx_log( "grahp:%p deconstruction begin:\n", this);
    std::cout << std::endl;

    intl_graph = NULL;
    decons_log();
    std::cout << std::endl;
    ahvfx_log( "grahp:%p deconstruction end.\n", this);
    std::cout << std::endl;
}

void graph::store(ahvfx::uniform_data unid) {
    if (intl_graph) {
        intl_graph->store(unid);
    }
}

const ahvfx::uniform_data graph::restore_unid(const std::string name) const {
    return intl_graph->restore_unid(name);
}

void graph::store(std::shared_ptr<ahvfx::graph_node> node) {
    intl_graph->store(node);
}

const std::shared_ptr<ahvfx::graph_node> graph::restore(const std::string name) const {
    return intl_graph->restore(name);
}

void graph::store(std::shared_ptr<ahvfx::program> program) {
    intl_graph->store(program);
}

const std::shared_ptr<ahvfx::program> graph::restore_program(const std::string name) const {
    return intl_graph->restore2(name);
}

void graph::in(ahvfx::texture_t texture, uint32_t index, uint32_t fIdx) {
//#ifdef DEBUG
//    bool rs =(intl_graph != NULL && !intl_graph->terminal_node.expired());
//    assert(rs);
//#endif
    intl_graph->in(texture, index, fIdx);
}

const ahvfx::texture_t graph::out() {
    return intl_graph->out();
}

void graph::link(std::weak_ptr<ahvfx::inputable> input, uint32_t index) {
    intl_graph->link(input, index);
}

void graph::linked_by(std::weak_ptr<ahvfx::outputable> output, uint32_t index) {
    intl_graph->linked_by(output, index);
}

void graph::reset_ranges(std::vector<ahvfx::range_type>& ranges) {
    if (intl_graph) {
        intl_graph->reset_ranges(ranges);
    }
}

#pragma mark - interface imp

void graph::io(uint32_t itex, io_size isize, uint32_t &ot, io_size &osize, uint32_t frameidx) {
    in(ahvfx::texture_t::move(itex, isize.w, isize.h), 0, frameidx);
    ahvfx::texture_t otexture = out();
    ot = otexture.id;
    osize = ahvfx::make_size(otexture.width(), otexture.height());
}

io_size graph::osize() {
    return make_size(out().width(), out().height());
}
uint32_t graph::otex() {
    return out().id;
}
uint32_t graph::frameidx() {
    if (intl_graph && !intl_graph->terminal_node.expired()) {
        return intl_graph->terminal_node.lock()->frameidx();
    }
    return 0;
}
