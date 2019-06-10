//
//  ahvfx_graph_node.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/19.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_graph_node_hpp
#define ahvfx_graph_node_hpp

#include "ahvfx_obj.hpp"
#include "ahvfx_io.hpp"
#include "ahvfx_drawable.hpp"
#include "ahvfx_program.hpp"
#include "ahvfx_uniform_data.hpp"

typedef struct ahvfx_graph_node_t ahvfx_graph_node_t;
typedef struct ahvfx_graph_node_src_check_info_t ahvfx_graph_node_src_check_info_t;

ahvfx_ns_0

class graph_node;
typedef std::shared_ptr<ahvfx::graph_node> graph_node_ptr;
typedef std::pair<uint32_t, std::pair<uint32_t, uint32_t>> range_type;
static inline ahvfx::range_type make_range(uint32_t i, uint32_t s, uint32_t e) {
    return std::make_pair(i, std::make_pair(s, e));
}
typedef enum {
    merge,
    coverage
} graph_node_render_style;
class graph_node :
public std::enable_shared_from_this<graph_node>,
public ahvfx::obj_base,
public ahvfx::inputable,
public ahvfx::outputable,
public ahvfx::drawable {
public:
    void notify();
    virtual void extra_notify(std::shared_ptr<ahvfx::inputable>, uint32_t, uint32_t);
    bool inranges();
    ahvfx::range_type closestrange();
    virtual const bool is_pass_node() const;
    void set_output_size(uint32_t w, uint32_t h);
public:
    bool always_update = false;
    void set_markable_input_num(uint32_t num);
public:
    std::vector<ahvfx::range_type> ranges;
public:
    ahvfx_des;
    std::shared_ptr<ahvfx_graph_node_t> intl_gn;
    ahvfx::vec4<float> clearColor;
    std::shared_ptr<ahvfx::program> program, coverage_main_program;
    std::vector<ahvfx::uniform_data> uniform_datas;
private:
    bool initial = false;
    bool terminal = false;
    graph_node_render_style render_style = ahvfx::graph_node_render_style::merge;
public:
    virtual const bool is_initial() const;
    virtual const bool is_terminal() const;
    virtual void set_initial(bool w);
    virtual void set_terminal(bool w);
    virtual const ahvfx::graph_node_render_style get_render_style() const;
    virtual void set_render_style(ahvfx::graph_node_render_style style);
public:
    std::shared_ptr<ahvfx_graph_node_src_check_info_t> source_checks_info;
    std::vector<std::pair<std::weak_ptr<ahvfx::outputable>, uint32_t>> srcs;
    std::vector<std::pair<std::weak_ptr<ahvfx::inputable>, uint32_t>> dsts;
    void clean_links();
    
    void push_srcs(std::pair<std::weak_ptr<ahvfx::outputable>, uint32_t> p);
    void push_dsts(std::pair<std::weak_ptr<ahvfx::inputable>, uint32_t> p);
public:
    
    explicit graph_node();
    ~graph_node();
    
    void in(ahvfx::texture_t texture, uint32_t index, uint32_t fIdx);
    const ahvfx::texture_t out();
    
    void draw();
    
    void link(std::weak_ptr<ahvfx::inputable> input, uint32_t index);
    void linked_by(std::weak_ptr<ahvfx::outputable> output, uint32_t index);
    
    virtual void request_inputs();
    virtual void gl_env_configure(std::shared_ptr<ahvfx::program> prog, uint32_t fIdx);
    
    const uint32_t frameidx() const;
    
public:
    std::function<void(void)> before_draw_callback = NULL;
    std::function<void(void)> after_draw_callback = NULL;
};

ahvfx_ns_1

#endif /* ahvfx_graph_node_hpp */
