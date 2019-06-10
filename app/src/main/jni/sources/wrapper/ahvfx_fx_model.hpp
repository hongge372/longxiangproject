//
//  ahvfx_fxmodel.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/22.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_fxmodel_hpp
#define ahvfx_fxmodel_hpp

#include <string>

namespace ahvfx {
    class fxmodel {
    public:
        std::string root;
        std::string name;
        explicit fxmodel(std::string root, std::string name);
        const bool isgood() const;
        bool is_empty = false;
        static fxmodel build_empty() {
            fxmodel rs("", "");
            rs.is_empty = true;
            return rs;
        }
    };
}

#endif /* ahvfx_fxmodel_hpp */
