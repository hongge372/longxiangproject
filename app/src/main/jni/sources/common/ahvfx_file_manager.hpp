//
//  ahvfx_file_manager.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/21.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_file_manager_hpp
#define ahvfx_file_manager_hpp

#include "ahvfx_obj.hpp"
#include "CJsonObject.hpp"

ahvfx_ns_0

class file_manager {
private:
    file_manager();
public:
    static file_manager * default_manager();
public:
    void add_root_path(std::string &rootPath, std::string &packageName, neb::CJsonObject &Json);
};

ahvfx_ns_1

#endif /* ahvfx_file_manager_hpp */
