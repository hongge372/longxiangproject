//
//  ahvfx_graph_node.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/19.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_graph_node.hpp"
#include "ahvfx_framebuffer.hpp"
#include "ahvfx_flat.hpp"
#include "ahvfx_global_callback.hpp"
#include <math.h>

extern "C" {
    void ahvfx_gl_make_square_box(int w, int h, int *x, int *y, int *ww, int *hh);
}

struct ahvfx_graph_node_src_texture_info_t {
    ahvfx::texture_t texture;
    uint32_t target_index;
    ahvfx_graph_node_src_texture_info_t()
    : target_index(0) {}
    ~ahvfx_graph_node_src_texture_info_t() = default;
};
typedef struct ahvfx_graph_node_src_texture_info_t ahvfx_graph_node_src_texture_info_t;

struct ahvfx_graph_node_src_check_info_t {
    int32_t fIdx;
    uint32_t markable_input_num = 0;
    ahvfx::texture_t tmp_pass_texture;
    ahvfx::texture_t source_textures[k_ahvfx_max_texture_unit_count];
    ahvfx_graph_node_src_check_info_t()
    : fIdx(0)
    {}
    ~ahvfx_graph_node_src_check_info_t() = default;
    
    void received(ahvfx::texture_t texture, uint32_t index, uint32_t frameIndex) {
        if (texture.texture_parameters_initialized == false) texture.default_texture_parameters();
        source_textures[index] = texture;
        if (index == 0) {
            tmp_pass_texture = texture;
        }
        fIdx = frameIndex;
    }
    
    void clear() {
        memset(source_textures, 0, sizeof(source_textures));
    }
    
    void invalided() {
        
    }
    
    bool getthrough(std::vector<std::pair<std::weak_ptr<ahvfx::outputable>, uint32_t>> &srcs) {
        bool rs = source_textures[0].valid();
        if (rs) {
            std::vector<std::pair<std::weak_ptr<ahvfx::outputable>, uint32_t>>::iterator it;
            for (it = srcs.begin(); it != srcs.end(); ++it) {
                if (!(rs &= (it->second < k_ahvfx_max_texture_unit_count && source_textures[it->second].valid()))) {
                    break;
                }
            }
            for (int i = 0; i < markable_input_num; ++i) {
                if (!(rs &= source_textures[i].valid())) {
                    break;
                }
            }
        }
        if (rs) {
            return true;
        }
        return false;
        return rs;
    }
    
    void use(std::shared_ptr<ahvfx::program> program) {
        for (int i = 0; i < k_ahvfx_max_texture_unit_count; ++i) {
            ahvfx::texture_t texture = source_textures[i];
            if (texture.valid()) {
                ahvfx_active_bind_texture(texture, i);
                program->uniform1(program->uniform.textureEnabled[i], 1);
                program->uniform1(program->uniform.texture[i], i);
                program->uniform2(program->uniform.textureSize[i], (float)texture.size[0], (float)texture.size[1]);
            } else {
                program->uniform1(program->uniform.textureEnabled[i], 0);
            }
        }
    }
    
    void use_main(std::shared_ptr<ahvfx::program> program) {
        
//        for (int i = 0; i < k_ahvfx_max_texture_unit_count; ++i) {
        int i = 0;
        ahvfx::texture_t texture = source_textures[i];
        if (texture.valid()) {
            ahvfx_active_bind_texture(texture, i);
            program->uniform1(program->uniform.texture[i], i);
            program->uniform2(program->uniform.textureSize[i], (float)texture.size[0], (float)texture.size[1]);
        }
//        }
    }
    
    void use_minor(std::shared_ptr<ahvfx::program> program) {
        int start = 1;
        for (int i = start; i < k_ahvfx_max_texture_unit_count; ++i) {
            ahvfx::texture_t texture = source_textures[i];
            if (texture.valid()) {
                ahvfx_active_bind_texture(texture, i - start);
                program->uniform1(program->uniform.texture[i - start], i - start);
                program->uniform2(program->uniform.textureSize[i - start], (float)texture.size[0], (float)texture.size[1]);
            }
        }
    }

    void foreach(std::function<std::shared_ptr<ahvfx::program>(uint32_t)> program_callback, std::function<void(uint32_t, std::shared_ptr<ahvfx::program>)> callback) {
        for (int i = 0; i < k_ahvfx_max_texture_unit_count; ++i) {
            std::shared_ptr<ahvfx::program> rs_program = program_callback(i);
            rs_program->use();
            ahvfx::texture_t texture = source_textures[i];
            if (texture.valid()) {
                ahvfx_active_bind_texture(texture, 0);
                rs_program->uniform1(rs_program->uniform.texture[0], 0);
                rs_program->uniform2(rs_program->uniform.textureSize[0], (float)texture.size[0], (float)texture.size[1]);
                if (i > 0) rs_program->uniform1(rs_program->uniform.coverageRenderIndex, i - 1);
                callback(i, rs_program);
            }
        }
    }
    
    void set_markable_input_num(uint32_t num) {
        markable_input_num = num;
    }
};
typedef struct ahvfx_graph_node_src_check_info_t ahvfx_graph_node_src_check_info_t;

struct ahvfx_graph_node_t {
    ahvfx_des_t;
    std::shared_ptr<ahvfx::framebuffer> fbo;
    std::shared_ptr<ahvfx::flat> flat;
    uint32_t output_width = 0;
    uint32_t output_height = 0;
    ahvfx_graph_node_t() {
        fbo = std::shared_ptr<ahvfx::framebuffer>(new ahvfx::framebuffer());
        flat = std::shared_ptr<ahvfx::flat>(new ahvfx::flat());
    }
    ~ahvfx_graph_node_t() {
        decons_log();
    }
    void bind_framebuffer(ahvfx::vec2<uint32_t> sz) {
        fbo->bind(sz);
    }
    
    ahvfx::vec2<uint32_t> get_framebuffer_size() {
        return fbo->buffer_size();
    }
    
    void clearColor(ahvfx::vec4<float> c) {
        glClearColor(c.elem.r, c.elem.g, c.elem.b, c.elem.a);
    }
    void clear() {
        glClear(GL_COLOR_BUFFER_BIT);
    }
    void bind_src() {
        
    }
    
    void viewport(ahvfx::vec4<int32_t> v) {
        glViewport(v[0], v[1], v[2], v[3]);
    }
    
    void set_output_size(uint32_t w, uint32_t h) {
        output_width = w;
        output_height = h;
    }
    
    bool configed_output_size() {
        return output_width * output_height != 0;
    }
    
};
typedef struct ahvfx_graph_node_t ahvfx_graph_node_t;

using namespace ahvfx;

graph_node::graph_node() {
    terminal = false;
    initial = false;
    always_update = false;
    intl_gn = std::shared_ptr<ahvfx_graph_node_t>(new ahvfx_graph_node_t());
    source_checks_info = std::shared_ptr<ahvfx_graph_node_src_check_info_t>(new ahvfx_graph_node_src_check_info_t());
}

const bool graph_node::is_pass_node() const {
    return true;
}

graph_node::~graph_node() {
    decons_log();
}

void graph_node::clean_links() {

}

bool graph_node::inranges() {
    if (always_update) return true;
    std::vector<range_type>::iterator it;
    for (it = ranges.begin(); it != ranges.end(); ++it) {
#if DEBUG
        uint32_t fIdx = source_checks_info->fIdx;
        if (fIdx >= it->second.first && fIdx <= it->second.second) {
#else
        if (source_checks_info->fIdx >= it->second.first && source_checks_info->fIdx <= it->second.second) {
#endif
            return true;
        }
    }
    return false;
}

range_type graph_node::closestrange() {
    std::vector<range_type>::iterator it;
    for (it = ranges.begin(); it != ranges.end(); ++it) {
        if (source_checks_info->fIdx >= it->second.first && source_checks_info->fIdx <= it->second.second) {
            return std::make_pair(it->first, it->second);
        }
    }
    return make_range(0, 0, 0);
}

void graph_node::in(ahvfx::texture_t texture, uint32_t index, uint32_t fIdx) {
    source_checks_info->received(texture, index, fIdx);
    if (is_pass_node() && inranges()) {
        lazy_load(program, std::shared_ptr<ahvfx::program>(new ahvfx::program(0, 0)))
        if (source_checks_info->getthrough(std::ref(srcs))) {
            if (before_draw_callback) {
                before_draw_callback();
            }
            draw();
            if (after_draw_callback) {
                after_draw_callback();
            }
            notify();
            source_checks_info->clear();
        }
    } else {
        if (index == 0) {
            notify();
            source_checks_info->clear();
        }
    }
}

void graph_node::notify() {
    std::vector<std::pair<std::weak_ptr<ahvfx::inputable>, uint32_t>>::iterator it;
    uint32_t index = 0;
    for (it = dsts.begin(); it != dsts.end(); ++it, ++index) {
        if (!it->first.expired()) {
            it->first.lock()->in(out(), it->second, source_checks_info->fIdx);
            extra_notify(it->first.lock(), it->second, source_checks_info->fIdx);
        }
    }
}

void graph_node::extra_notify(std::shared_ptr<ahvfx::inputable> in, uint32_t target_idx, uint32_t f_idx) {
    // imp by cluster
}

const ahvfx::texture_t graph_node::out() {
    if (inranges()) {
        return intl_gn->fbo->out();
    } else if (source_checks_info->tmp_pass_texture.valid()) {
        return source_checks_info->tmp_pass_texture;
    }
    return ahvfx::texture_t_zero;
}

void graph_node::draw() {
    if (is_pass_node() && program && intl_gn) {
        ahvfx::range_type range = closestrange();
        intl_gn->bind_framebuffer(intl_gn->configed_output_size() ? ahvfx::vec2<uint32_t>(intl_gn->output_width, intl_gn->output_height) : source_checks_info->source_textures[0].size.xy());
        intl_gn->clearColor(clearColor);
        intl_gn->clear();
        int index = 0;
        do {
            if (get_render_style() == ahvfx::graph_node_render_style::merge) {
                program->use();
                program->uniform1(program->uniform.instanceRenderIndex, index);
                gl_env_configure(program, source_checks_info->fIdx);
                source_checks_info->use(program);
                intl_gn->flat->draw();
            } else if (get_render_style() == ahvfx::graph_node_render_style::coverage) {
                source_checks_info->foreach([&](uint32_t each_idx) -> std::shared_ptr<ahvfx::program> {
                    lazy_load(coverage_main_program, std::shared_ptr<ahvfx::program>(new ahvfx::program(NULL, NULL)));
                    return each_idx > 0 ? program : coverage_main_program;
                }, [&](uint32_t each_idx, std::shared_ptr<ahvfx::program> p) {
                    if (each_idx > 0) {
                        int32_t x, y, w, h;
                        ahvfx_gl_make_square_box(intl_gn->get_framebuffer_size()[0], intl_gn->get_framebuffer_size()[1], &x, &y, &w, &h);
                        intl_gn->viewport(ahvfx::vec4<int32_t>{x, y, w, h});
                    }
                    p->uniform1(p->uniform.instanceRenderIndex, index);
                    gl_env_configure(p, source_checks_info->fIdx);
                    intl_gn->flat->draw();
                });
            }
        } while (++index < range.first);
    }
//    else if (!is_pass_node) {
//        ahvfx_log( "%s is not pass node.\n", this->name.c_str());
//    }
}

void graph_node::push_srcs(std::pair<std::weak_ptr<ahvfx::outputable>, uint32_t> p) {
    srcs.push_back(p);
}

void graph_node::push_dsts(std::pair<std::weak_ptr<ahvfx::inputable>, uint32_t> p) {
    dsts.push_back(p);
}

void graph_node::link(std::weak_ptr<ahvfx::inputable> input, uint32_t index) {
    if (!input.expired()) {
        input.lock()->linked_by(shared_from_this(), index);
        push_dsts(std::make_pair(input, index));
    } else {
        ahvfx_log( "link input expired.\n");
    }
}

void graph_node::linked_by(std::weak_ptr<ahvfx::outputable> output, uint32_t index) {
    if (
//        true
        !output.expired()
        ) {
        srcs.push_back(std::make_pair(output, index));
    } else {
        ahvfx_log( "output link expired.\n");
    }
}

void graph_node::request_inputs() {
    
}
    
float periodai(uint32_t fIdx) {
    static float ai = 0;
    return sin(ai);
}

float random01() {
    return (rand() % 255) / 254.0;
}

void graph_node::gl_env_configure(std::shared_ptr<ahvfx::program> PRO, uint32_t fIdx) {
//    PRO->uniform1(PRO->uniform.periodAI, periodai(fIdx));
//    PRO->uniform1(PRO->uniform.random01, random01());
    PRO->uniform1(PRO->uniform.frameIndex, (int)fIdx);
    vec2<uint32_t> fbosz = intl_gn->get_framebuffer_size();
    PRO->uniform2(PRO->uniform.frameBufferSize, (float)fbosz.elem.x, (float)fbosz.elem.y);
    ahvfx::range_type p = closestrange();
    if (PRO->uniform.startIndex != -1 || PRO->uniform.endIndex != -1) {
        PRO->uniform1(PRO->uniform.instanceRenderCount, p.first);
        PRO->uniform1(PRO->uniform.startIndex, p.second.first);
        PRO->uniform1(PRO->uniform.endIndex, p.second.second);
        float progress = float(fIdx - p.second.first) / float(p.second.second - p.second.first);
        if (progress < 0) {
            progress = 0;
        } else if (progress > 1) {
            progress = 1;
        }
        PRO->uniform1(PRO->uniform.rangeProgress, progress);
    }
    if (PRO->config_callback) {
        PRO->config_callback(PRO.get());
    }
    if (global_callback::shared()->func) {
        global_callback::shared()->func(PRO.get());
    };
    if (global_callback::shared()->landmark_func) {
        global_callback::shared()->landmark_func(PRO.get());
    }
    if (global_callback::shared()->device_roll_func) {
        global_callback::shared()->device_roll_func(PRO.get());
    }
    for (int i = 0; i < uniform_datas.size(); ++i) {
        uniform_datas[i].unpack(PRO->uniform_location(uniform_datas[i].data_name.c_str()), fIdx - p.second.first);
    }
}
    
const uint32_t graph_node::frameidx() const {
    return source_checks_info->fIdx;
}

const ahvfx::graph_node_render_style graph_node::get_render_style() const {
    return render_style;
}
void graph_node::set_render_style(ahvfx::graph_node_render_style style) {
    render_style = style;
}

const bool graph_node::is_initial() const {
    return initial;
}
const bool graph_node::is_terminal() const {
    return terminal;
}
void graph_node::set_initial(bool w) {
    initial = w;
}
void graph_node::set_terminal(bool w) {
    terminal = w;
}

void graph_node::set_markable_input_num(uint32_t num) {
    source_checks_info->set_markable_input_num(num);
}

void graph_node::set_output_size(uint32_t w, uint32_t h) {
    intl_gn->set_output_size(w, h);
}
