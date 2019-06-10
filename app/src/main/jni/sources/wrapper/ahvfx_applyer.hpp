//
//  ahvfx_applyer.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/22.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_applyer_hpp
#define ahvfx_applyer_hpp
#include <memory>
#include "ahvfx_interface.hpp"
#include "ahvfx_fx_model.hpp"
#include "ahvfx_callbacks.hpp"
#include <string>

typedef struct ahvfx_applyer_t ahvfx_applyer_t;

extern uint32_t ahvfx_range_positive_inf;

namespace ahvfx {
    class applyer;
    typedef std::shared_ptr<applyer> applyer_ptr;
    class applyer : public ahvfx::interface {
    private:
        bool skip_process = false;
        bool landmark_visible = false;
    public:
        static applyer_ptr create(ahvfx::req_callback callback = ahvfx::req_callback()) {
            return applyer_ptr(new applyer(callback));
        }
    protected:
        std::shared_ptr<ahvfx_applyer_t> entl_applyer;
    public:
        ahvfx::req_callback requset_callback;
        std::string name;
    public:
        explicit applyer(ahvfx::req_callback callback = ahvfx::req_callback());
        ~applyer() = default;
    public:
        bool use(ahvfx::fxmodel &model);
    public:
        void io(uint32_t itex, ahvfx::io_size isize, uint32_t &ot, ahvfx::io_size &osize, uint32_t frameidx = 0);
        ahvfx::io_size osize();
        uint32_t otex();
        uint32_t frameidx();
    public:
        void add_range(uint32_t start_index, uint32_t end_index);
        std::pair<uint32_t, uint32_t> pop_range();
        void remove_all_range();
        bool always_update = false;
        
        void set_car_points(float *carPoints, size_t size);
        void set_landmark_visible(bool visible);
    };
}

#endif /* ahvfx_applyer_hpp */
