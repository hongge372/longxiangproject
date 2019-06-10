//
//  ahvfx_ texture_duplicator.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/29.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_texture_duplicator_hpp
#define ahvfx_texture_duplicator_hpp

#include "ahvfx_obj.hpp"
#include "ahvfx_io.hpp"

class ahvfx_texture_duplicator_intl;

ahvfx_ns_0

class  texture_duplicator : public obj_base {
public:
    std::shared_ptr<ahvfx_texture_duplicator_intl> intl_texture_duplicator;
public:
    ahvfx_des;
    explicit  texture_duplicator();
    ~ texture_duplicator();
    
    void copy(ahvfx:: texture_t in, ahvfx:: texture_t &rs);
};

ahvfx_ns_1

#endif /* ahvfx_ texture_duplicator_hpp */
