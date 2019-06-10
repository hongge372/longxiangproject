//
//  ahvfx_graph.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/20.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_graph_hpp
#define ahvfx_graph_hpp

#include "ahvfx_request_node.hpp"
#include "ahvfx_interface.hpp"
#include "ahvfx_uniform_data.hpp"

typedef struct ahvfx_graph_t ahvfx_graph_t;

ahvfx_ns_0

class graph;
typedef std::shared_ptr<graph> graph_ptr;

class graph
:
public ahvfx::obj_base,
public ahvfx::inputable,
public ahvfx::outputable,
public ahvfx::interface
{
private:
    ahvfx_des;
    std::shared_ptr<ahvfx_graph_t> intl_graph;
public:
public:
    graph();
    ~graph();
public:
    uint32_t build_version();
    void set_build_version(uint32_t version);
    void store(ahvfx::uniform_data unid);
    const ahvfx::uniform_data restore_unid(const std::string name) const;
    
    void store(std::shared_ptr<ahvfx::graph_node> node);
    const std::shared_ptr<ahvfx::graph_node> restore(const std::string name) const;
    
    void store(std::shared_ptr<ahvfx::program> program);
    const std::shared_ptr<ahvfx::program> restore_program(const std::string name) const;
    
    void in(ahvfx::texture_t texture, uint32_t index, uint32_t fIdx);
    const ahvfx::texture_t out();
        
    void link(std::weak_ptr<ahvfx::inputable> input, uint32_t index);
    void linked_by(std::weak_ptr<ahvfx::outputable> output, uint32_t index);
    
    void reset_ranges(std::vector<ahvfx::range_type>& ranges);
public:
    
    virtual void io(uint32_t itex, io_size isize, uint32_t &ot, io_size &osize, uint32_t frameidx = 0);
    virtual io_size osize();
    virtual uint32_t otex();
    virtual uint32_t frameidx();
    
};


ahvfx_ns_1

#endif /* ahvfx_graph_hpp */
