//
//  ahvfx_file_manager.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/21.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_file_manager.hpp"
#include <pthread.h>

using namespace ahvfx;

static inline bool string_endwith(std::string str1, std::string str2) {
    if (str1.length() >= str2.length()) {
        return str1.compare(str1.size() - str2.size(), str2.size(), str2) == 0;
    }
    return 0;
}

static pthread_mutex_t mtx;
static file_manager *m_Instance = NULL;
file_manager::file_manager() {
    pthread_mutex_init(&mtx, 0);
}

file_manager * file_manager::default_manager() {
    pthread_mutex_lock(&mtx);
    if (m_Instance == NULL) {
        m_Instance = new file_manager();
    }
    pthread_mutex_unlock(&mtx);
    return m_Instance;
}

void file_manager::add_root_path(std::string &rootPath, std::string &name, neb::CJsonObject &Json) {
    if (Json.IsEmpty() == false) {
        if (Json.IsArray()) {
            // ARRAY
            int size = Json.GetArraySize();
            for (int i = 0; i < size; ++i) {
//                file_manager::add_root_path(rootPath, name, std::ref(Json[i]));
                file_manager::add_root_path(rootPath, name, Json[i]);
            }
        } else {
            // MAP
            std::string strTraversing;
            while (Json.GetKey(strTraversing)) {
                if (string_endwith(strTraversing, "path") || string_endwith(strTraversing, "paths")) {
                    std::string stringValue;
                    if (Json.Get(strTraversing, stringValue)) {
                        std::string newValue = rootPath + "/" + name + "/" + stringValue;
                        Json.Replace(strTraversing, newValue);
#if DEBUG1
                        std::string vv;
                        Json.Get(strTraversing, vv);
                        std::cout << strTraversing << "[" << stringValue << "] -->" << vv << std::endl;
#endif
                    }
                }
                file_manager::add_root_path(rootPath, name, Json[strTraversing]);
            }
        }
    }
}
