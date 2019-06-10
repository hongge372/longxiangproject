//
//  ahvfx_types.h
//  AHVideoFX
//
//  Created by longxiang on 2019/3/18.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_types_h
#define ahvfx_types_h

#include <memory.h>

namespace ahvfx {
    template <typename T>
    union elems2 {
        T m[2];
        struct {
            T x; T y;
        };
        struct {
            T r; T g;
        };
    };
    
    template <typename T>
    union elems3 {
        T m[3];
        struct {
            T x; T y; T z;
        };
        struct {
            T r; T g; T b;
        };
    };
    
    template <typename T>
    union elems4 {
        T m[4];
        struct {
            T x; T y; T z; T w;
        };
        struct {
            T r; T g; T b; T a;
        };
    };
    
    template <typename T>
    struct vec2 {
        elems2<T> elem;
        vec2() {
            memset(&elem, 0, sizeof(elems2<T>));
        }
        vec2(T v1, T v2) {
            elem.x = v1;
            elem.y = v2;
        }
        bool operator== (const vec2<T> &v2) {
            return (elem.x == v2.elem.x && elem.y == v2.elem.y);
        }
        bool operator!= (const vec2<T> &v2) {
            return !(*this == v2);
        }
        T operator[] (uint32_t index) {
            return elem.m[index];
        }
        const T operator[] (uint32_t index) const {
            return elem.m[index];
        }
        void clear() {
            memset(&elem, 0, sizeof(elems2<T>));
        }
        
        bool valid() {
            return elem.x && elem.y;
        }
        bool valid() const {
            return elem.x && elem.y;
        }

    };
    
    template <typename T>
    struct vec3 {
        elems3<T> elem;
        vec3() {
            memset(&elem, 0, sizeof(elems3<T>));
        }
        vec3(vec2<T> v2, T v3 = 0) {
            elem.x = v2.elem.x;
            elem.y = v2.elem.y;
            elem.z = v3;
        }
        vec3(T v1, T v2, T v3) {
            elem.x = v1;
            elem.y = v2;
            elem.z = v3;
        }
        bool operator== (const vec3<T> &v2) {
            return (elem.x == v2.elem.x && elem.y == v2.elem.y && elem.z == v2.elem.z);
        }
        bool operator!= (const vec3<T> &v2) {
            return !(*this == v2);
        }
        T operator[] (uint32_t index) {
            return elem.m[index];
        }
        const T operator[] (uint32_t index) const {
            return elem.m[index];
        }
        void clear() {
            memset(&elem, 0, sizeof(elems3<T>));
        }
        
        const vec2<T> xy() const {
            return vec2<T>(elem.x, elem.y);
        }
        
        bool valid() {
            return elem.x && elem.y;
        }
        bool valid() const {
            return elem.x && elem.y;
        }
    };
    
    template <typename T>
    struct vec4 {
        elems4<T> elem;
        T operator[] (uint32_t index) {
            return elem.m[index];
        }
        const T operator[] (uint32_t index) const {
            return elem.m[index];
        }
        bool operator== (const vec4<T> &v2) {
            return (elem.x == v2.elem.x && elem.y == v2.elem.y && elem.z == v2.elem.z && elem.w == v2.elem.w);
        }
        bool operator!= (const vec4<T> &v2) {
            return !(*this == v2);
        }
        bool empty() {
            return elem.x + elem.y + elem.z + elem.w == 0;
        }
        vec4() {
            memset(&elem, 0, sizeof(elems4<T>));
        }
        vec4(T v11, T v12, T v13, T v14) {
            elem.x = v11;
            elem.y = v12;
            elem.z = v13;
            elem.w = v14;
        }
        vec4(T v11) {
            elem.x = v11;
            elem.y = v11;
            elem.z = v11;
            elem.w = v11;
        }
        vec4(T v11, T v12, vec2<T> v21) {
            elem.x = v11;
            elem.y = v12;
            elem.z = v21.elem.x;
            elem.w = v21.elem.y;
        }
        vec4(T v11, vec3<T> v32 = vec3<T>()) {
            elem.x = v11;
            elem.y = v32.elem.x;
            elem.z = v32.elem.y;
            elem.w = v32.elem.z;
        }
        vec4(vec2<T> v21, vec2<T> v22 = vec2<T>()) {
            elem.x = v21.elem.x;
            elem.y = v21.elem.y;
            elem.z = v22.elem.x;
            elem.w = v22.elem.y;
        }
        vec4(vec3<T> v31, T v13 = 0) {
            elem.x = v31.elem.x;
            elem.y = v31.elem.y;
            elem.z = v31.elem.z;
            elem.w = v13;
        }
    };
}

#endif /* ahvfx_types_h */
