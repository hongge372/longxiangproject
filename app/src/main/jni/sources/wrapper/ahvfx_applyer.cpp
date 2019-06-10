//
//  ahvfx_applyer.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/22.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_applyer.hpp"
#include "ahvfx_graph_builder.hpp"
#include "ahvfx_util.hpp"
#include "ahvfx_global_callback.hpp"
#include <stack>

uint32_t ahvfx_range_positive_inf = 0xFFFF;

struct ahvfx_applyer_t {
    uint32_t VERSION = 0;
    float landmarks[256];
    uint32_t landmarks_count = 0;
    uint32_t car_points_count = 0;
    ahvfx::req_callback callback;
    std::shared_ptr<ahvfx::graph> graph;
    bool landmarks_required = false;
    ahvfx_applyer_t() = default;
    ~ahvfx_applyer_t() {
        ahvfx::global_callback::shared()->func = NULL;
        ahvfx::global_callback::shared()->device_roll_func = NULL;
        ahvfx::global_callback::shared()->landmark_func = NULL;
    };
    bool good = false;
    bool landmark_visiable = false;
    
    bool use(ahvfx::fxmodel &model) {
        ahvfx::graph_builder::default_builder()->callback = callback;
        graph = ahvfx::graph_builder::default_builder()->build_graph(model.root, model.name);
        if (graph) {
            VERSION = graph->build_version();
            version_adpat();
        }
        return (good = graph != NULL);
    }
    
    void convert_ranges(std::vector<ahvfx::range_type>& vector_range, std::stack<std::pair<uint32_t, uint32_t>> stack_range) {
        vector_range.reserve(stack_range.size());
        while (!stack_range.empty()) {
            vector_range.push_back(ahvfx::range_type(0, stack_range.top()));
            stack_range.pop();
        }
    }
    
    std::stack<std::pair<uint32_t, uint32_t>> ranges;
    void add_range(uint32_t start_index, uint32_t end_index) {
        ranges.push(std::make_pair(start_index, end_index));
        std::vector<ahvfx::range_type> vector_range;
        convert_ranges(vector_range, ranges);
        graph->reset_ranges(vector_range);
    }
    
    std::pair<uint32_t, uint32_t> pop_range() {
        std::pair<uint32_t, uint32_t> p(0, 0);
        if (!ranges.empty()) {
            p = ranges.top();
            ranges.pop();
        }
        return p;
    }
    
    void remove_all_range() {
        while (!ranges.empty()) ranges.pop();
    }
    
    bool in_range(uint32_t fIdx) {
        if (!ranges.empty()) {
            std::stack<std::pair<uint32_t, uint32_t>> ranges_cp(ranges);
            while (!ranges_cp.empty()) {
                std::pair<uint32_t, uint32_t> p = ranges_cp.top();
                ranges_cp.pop();
                if (p.first <= fIdx && fIdx <= p.second) {
                    return true;
                }
            }            
        }
        return false;
    }
    
    uint32_t tmp_tex = 0;
    ahvfx::io_size tmp_size;
    uint32_t tmp_fIdx = 0;
    bool receive_tmp(uint32_t itex, ahvfx::io_size isize, uint32_t ifidx) {
        tmp_tex = itex;
        tmp_size = isize;
        tmp_fIdx = ifidx;
        return true;
    }
    
    void version_adpat() {
        if (VERSION > 1000) {
            landmarks_required = false;
        } else {
            landmarks_required = true;
        }
    }
    
    bool further_without_landmarks() {
        if (VERSION > 1000) {
            return true;
        } else {
            return landmarks_count != 0;
        }
    }
};
typedef struct ahvfx_applyer_t ahvfx_applyer_t;

using namespace ahvfx;

applyer::applyer(ahvfx::req_callback callback) {
    requset_callback = callback;
}

bool applyer::use(fxmodel &model) {
    
    if (model.is_empty) {
        skip_process = true;
        return true;
    } else {
        skip_process = false;
    }
    
    if (true
//        || requset_callback.good()
        ) {
        if (model.isgood()) {
            entl_applyer = std::shared_ptr<ahvfx_applyer_t>(new ahvfx_applyer_t());
            entl_applyer->callback = requset_callback;
            set_landmark_visible(false);
            return entl_applyer->use(model);
        }
    } else {
        ahvfx_log("request callbacks is not imp!");
    }
    return false;
}

void applyer::io(uint32_t itex, io_size isize, uint32_t &ot, io_size &osize, uint32_t frameidx) {
    if (!skip_process && entl_applyer && entl_applyer->receive_tmp(itex, isize, frameidx) && (entl_applyer->in_range(frameidx) || always_update) && entl_applyer->good && entl_applyer->further_without_landmarks()) {
        entl_applyer->graph->io(itex, isize, ot, osize, frameidx);
    } else {
        ot = itex;
        osize = isize;
    }
}

io_size applyer::osize() {
    if (entl_applyer && entl_applyer->good) {
        return entl_applyer->graph->osize();
    }
    return entl_applyer->tmp_size;
}

uint32_t applyer::otex() {
    if (entl_applyer && entl_applyer->good) {
        return entl_applyer->graph->otex();
    }
    return entl_applyer->tmp_tex;
}

uint32_t applyer::frameidx() {
    if (entl_applyer && entl_applyer->good) {
        return entl_applyer->graph->frameidx();
    }
    return entl_applyer->tmp_fIdx;
}

void applyer::add_range(uint32_t start_index, uint32_t end_index) {
    if (entl_applyer && start_index < end_index) {
        entl_applyer->add_range(start_index, end_index);
        ahvfx_log("%s add range %d - %d", name.c_str(), start_index, end_index);
    }
}

std::pair<uint32_t, uint32_t> applyer::pop_range() {
    if (entl_applyer) {
        return entl_applyer->pop_range();
    }
    return std::make_pair(0, 0);
}

void applyer::remove_all_range() {
    if (entl_applyer) {
        entl_applyer->remove_all_range();
    }
}

void applyer::set_car_points(float *carPoints, size_t size) {
    if (entl_applyer == NULL) {
        return;
    }
    if (carPoints) {
        entl_applyer->car_points_count = (uint32_t)(size / sizeof(carPoints[0]) / 2);
    } else {
        entl_applyer->car_points_count = 0;
    }
    memcpy(entl_applyer->landmarks, carPoints, size);
    entl_applyer->landmarks_count = (uint32_t)(size / sizeof(carPoints[0]) / 2);
    global_callback::shared()->func = [=](program *p)
    {
        if (entl_applyer.get()) {
            p->uniform2v(p->uniform_location("u_CarPoints[0]"), entl_applyer->landmarks_count, entl_applyer->landmarks);
            p->uniform1(p->uniform_location("u_CarPointsCount"), entl_applyer->landmarks_count);
        } else {
            ahvfx_log("applyer is not exits.");
        }
    };
}

void applyer::set_landmark_visible(bool visible) {
    landmark_visible = visible;
    global_callback::shared()->landmark_func = [=](program *p)
    {
        p->uniform1(p->uniform_location("u_Landmark_Visible"), (int)landmark_visible);
    };
}
