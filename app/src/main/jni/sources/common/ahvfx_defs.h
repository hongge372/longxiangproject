#ifndef ahvideofx_defs_h
#define ahvideofx_defs_h

#include <iostream>
#include <stdio.h>

#define ahvfx_ns_0 namespace ahvfx {
#define ahvfx_ns_1 }
#define lazy_load(variable, func_body) if (variable == NULL) { variable = func_body; }
#if DEBUG
#   define ahvfx_log(format, ...) printf("ahvfx: \t" format "\n", ##__VA_ARGS__)
#   define ahvfx_cond_log(_x, format, ...) if (_x) printf("ahvfx: \t" format "\n", ##__VA_ARGS__)
#   define ahvfx_des void decons_log() {\
        std::cout << "ahvfx: \t" << __PRETTY_FUNCTION__ << "\t[" << this->name << ":" << this << "]\t" << " \treleased." <<std::endl;\
    }
#   define ahvfx_des_t void decons_log() {\
        std::cout << "ahvfx: \t" << __PRETTY_FUNCTION__ << "\t[" << this << "]\t" << " \treleased." <<std::endl;\
    }
#else
#   define ahvfx_log(format, ...)
#   define ahvfx_cond_log(_x, format, ...)
#   define ahvfx_des void decons_log() {}
#   define ahvfx_des_t void decons_log() {}
#endif
#define ahvfx_glsl2(x) #x
#define ahvfx_ns_glsl(x) ahvfx_glsl2(x)
#define ahvfx_glsl(x) ahvfx_ns_glsl(x)
#define ahvfx_makechar(x) @#x
#endif /* ahvideofx_defs_h */
