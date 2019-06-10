//
//  ahvfx_program.hpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/18.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_program_hpp
#define ahvfx_program_hpp

#include "ahvfx_obj.hpp"


typedef struct ahvfx_program_t ahvfx_program_t;

#define k_ahvfx_max_texture_unit_count 16

ahvfx_ns_0

struct program_uniform_t {
    int32_t texture[k_ahvfx_max_texture_unit_count];
    int32_t textureSize[k_ahvfx_max_texture_unit_count];
    int32_t textureEnabled[k_ahvfx_max_texture_unit_count];
    int32_t frameBufferSize;
    int32_t frameIndex;
    int32_t startIndex;
    int32_t endIndex;
    int32_t rangeProgress;
    int32_t instanceRenderIndex;
    int32_t instanceRenderCount;
    int32_t coverageRenderIndex;
    int32_t periodAI;
    int32_t random01;
    program_uniform_t() {
        memset(texture, -1, sizeof(int32_t[k_ahvfx_max_texture_unit_count]));
        memset(textureSize, -1, sizeof(int32_t[k_ahvfx_max_texture_unit_count]));
        memset(textureEnabled, -1, sizeof(int32_t[k_ahvfx_max_texture_unit_count]));
        memset(&frameBufferSize, -1, sizeof(program_uniform_t) - sizeof(int32_t) * 3 * k_ahvfx_max_texture_unit_count);
    }
    ~program_uniform_t() = default;
};
typedef struct program_uniform_t program_uniform_t;

enum {
    ATTRIBUTE_POSITION,
    ATTRIBUTE_TEXCOORD,
    ATTRIBUTE_NORMAL,
    ATTRIBUTE_NUM
};

class program : public obj_base {
private:
    void default_uniforms();
protected:
    std::shared_ptr<ahvfx_program_t> intl_program;
public:
    program_uniform_t uniform;
    std::function<void(ahvfx::program *)> config_callback;
public:
    ahvfx_des;
    program() = default;
    program(const char *vert, const char *frag);
    program(std::string vert, std::string frag);
    ~program();
    bool load_file(const char *vsh, const char *frag);
    
    void use();
    
    const uint32_t uniform_location(const char *name) const;
    const uint32_t attribute_location(const char *name) const;
    
    void uniform1(int32_t loc, int x);
    void uniform1(int32_t loc, float x);
    void uniform1v(int32_t loc, int count, int *v);
    void uniform1v(int32_t loc, int count, float *v);
    void uniform2(int32_t loc, int x, int y);
    void uniform2(int32_t loc, float x, float y);
    void uniform2v(int32_t loc, int count, int *v);
    void uniform2v(int32_t loc, int count, float *v);
    void uniform3(int32_t loc, int x, int y, int z);
    void uniform3(int32_t loc, float x, float y, float z);
    void uniform3v(int32_t loc, int count, int *v);
    void uniform3v(int32_t loc, int count, float *v);
    void uniform4(int32_t loc, int x, int y, int z, int w);
    void uniform4(int32_t loc, float x, float y, float z, float w);
    void uniform4v(int32_t loc, int count, int *v);
    void uniform4v(int32_t loc, int count, float *v);
    
    void uniform1(int32_t loc, uint32_t x);
    void uniform1v(int32_t loc, int count, uint32_t *v);
    void uniform2(int32_t loc, int x, uint32_t y);
    void uniform2v(int32_t loc, int count, uint32_t *v);
    void uniform3(int32_t loc, uint32_t x, uint32_t y, uint32_t z);
    void uniform3v(int32_t loc, int count, uint32_t *v);
    void uniform4(int32_t loc, uint32_t x, uint32_t y, uint32_t z, uint32_t w);
    void uniform4v(int32_t loc, int count, uint32_t *v);
    
};

ahvfx_ns_1

#endif /* ahvfx_program_hpp */
