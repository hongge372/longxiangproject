//
//  ahvfx_object.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/18.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_object_hpp
#define ahvfx_object_hpp

#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <memory>
#include <assert.h>

#include "ahvfx_defs.h"
#include "ahvfx_types.h"

#include "../plateform/ahvfx_plateform.h"

ahvfx_ns_0

class obj_base {
protected:
    std::string name;
public:
    ahvfx_des;
    virtual void set_name(std::string s);
    virtual const std::string get_name() const;
    obj_base();
    virtual ~obj_base();
};

ahvfx_ns_1
#endif /* ahvfx_object_hpp */
