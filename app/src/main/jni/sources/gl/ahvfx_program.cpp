//
//  ahvfx_program.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/18.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_program.hpp"
#include "ahvfx_shader.hpp"

extern "C" char *readbuf(const char *, long &);

static std::shared_ptr<ahvfx::shader> default_vert = NULL;
static std::shared_ptr<ahvfx::shader> default_frag = NULL;

struct ahvfx_program_t {
    ahvfx_des_t;
    uint32_t id;
    std::shared_ptr<ahvfx::shader> vert_shader;
    std::shared_ptr<ahvfx::shader> frag_shader;
    
    ~ahvfx_program_t() {
        if (id) {
            glDeleteProgram(id);
            id = 0;
        }
        decons_log();
    }
    
    ahvfx_program_t(std::shared_ptr<ahvfx::shader> vertex, std::shared_ptr<ahvfx::shader> fragment) {
        id = 0;
        if (vertex && fragment && vertex->good() && fragment->good()) {
            id = glCreateProgram();
            if (id) {
                vert_shader = vertex;
                frag_shader = fragment;
                glAttachShader(id, vertex->id());
                glAttachShader(id, fragment->id());
                std::string attributes[ahvfx::ATTRIBUTE_NUM] = {"a_Position", "a_TexCoord"
                    , "a_Normal"
                };
                for (int i = ahvfx::ATTRIBUTE_POSITION; i < ahvfx::ATTRIBUTE_NUM; ++i) {
                    glBindAttribLocation(id, i, attributes[i].c_str());
                }
                if (!link()) {
                    if (id) {
                        glDeleteProgram(id);
                        id = 0;
                    }
                    ahvfx_log( "link gl program error.\n");
                } else {
                    
                }
            } else {
                ahvfx_log( "create gl program error.\n");
            }
        } else {
            ahvfx_log( "create gl program shader is not good.\n");
        }
    }
    
    bool link() {
        bool result = true;
        GLint status;
        glLinkProgram(id);
        glGetProgramiv(id, GL_LINK_STATUS, &status);
        if (status == GL_FALSE) {
            result = false;
            GLint logLength = 0, charsWritten = 0;
            
            glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
            if (logLength < 1)
                return false;
            
            char *logBytes = (char *)malloc(logLength);
            glGetProgramInfoLog(id, logLength, &charsWritten, logBytes);
            ahvfx_log( "WARNING: gl link error:%s\n", logBytes);
            free(logBytes);
        }
        frag_shader = NULL;
        vert_shader = NULL;
        return result;
    }

    void use() {
        if (id) {
            glUseProgram(id);
            validate();
        } else {
            ahvfx_log( "WARNING: gl program is 0\n");
        }
    }
    
    void validate() {
#ifdef DEBUG
        GLint logLength;
        
        glValidateProgram(id);
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            GLchar *log = (GLchar *)malloc(logLength);
            glGetProgramInfoLog(id, logLength, &logLength, log);
            ahvfx_log( "Program validate log: %s\n", log);
            free(log);
        }
#endif
    }
    
    const uint32_t uniform_location(const char *name) const {
        return glGetUniformLocation(id, name);
    }
    
    const uint32_t attribute_location(const char *name) const {
        return glGetAttribLocation(id, name);
    }

};
typedef struct ahvfx_program_t ahvfx_program_t;

using namespace ahvfx;

//static std::once_flag once;

program::program(std::string vert, std::string frag) {
#if 0
    std::call_once(once, [&]{
        default_vert = std::shared_ptr<ahvfx::shader>(new shader(ahvfx_glsl( precision highp float;
                                                                            attribute vec4 a_Position;
                                                                            attribute vec2 a_TexCoord;
                                                                            varying vec2 v_TexCoord;
                                                                            void main() {
                                                                                gl_Position = a_Position;
                                                                                v_TexCoord = a_TexCoord;
                                                                            }), GL_VERTEX_SHADER));
        default_frag = std::shared_ptr<ahvfx::shader>(new shader(ahvfx_glsl(precision mediump float;
                                                                            varying vec2 v_TexCoord;
                                                                            uniform sampler2D u_Texture0;
                                                                            void main() {
                                                                                gl_FragColor = texture2D(u_Texture0, v_TexCoord);
                                                                            }), GL_FRAGMENT_SHADER));
    });
    std::shared_ptr<ahvfx::shader> vertex, fragment;
    vertex = !vert.empty() ? std::shared_ptr<ahvfx::shader>(new shader((const char *)(vert.c_str()), GL_VERTEX_SHADER)) : default_vert;
    fragment = !frag.empty() ? std::shared_ptr<ahvfx::shader>(new shader((const char *)(frag.c_str()), GL_FRAGMENT_SHADER)) : default_frag;
    intl_program = std::shared_ptr<ahvfx_program_t>(new ahvfx_program_t(vertex, fragment));
    fragment = NULL;
    vertex = NULL;
    default_uniforms();
#endif
}

program::program(const char *vert, const char *frag) {
#if 0
    std::call_once(once, [&]{
        default_vert = std::shared_ptr<ahvfx::shader>(new shader(ahvfx_glsl( precision highp float;
                                                                            attribute vec4 a_Position;
                                                                            attribute vec2 a_TexCoord;
                                                                            varying vec2 v_TexCoord;
                                                                            void main() {
                                                                                gl_Position = a_Position;
                                                                                v_TexCoord = a_TexCoord;
                                                                            }), GL_VERTEX_SHADER));
        default_frag = std::shared_ptr<ahvfx::shader>(new shader(ahvfx_glsl(precision mediump float;
                                                                            varying vec2 v_TexCoord;
                                                                            uniform sampler2D u_Texture0;
                                                                            void main() {
                                                                                gl_FragColor = texture2D(u_Texture0, v_TexCoord);
                                                                            }), GL_FRAGMENT_SHADER));
    });
    std::shared_ptr<ahvfx::shader> vertex, fragment;
    vertex = vert ? std::shared_ptr<ahvfx::shader>(new shader(vert, GL_VERTEX_SHADER)) : default_vert;
    fragment = frag ? std::shared_ptr<ahvfx::shader>(new shader(frag, GL_FRAGMENT_SHADER)) : default_frag;
    intl_program = std::shared_ptr<ahvfx_program_t>(new ahvfx_program_t(vertex, fragment));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    fragment = NULL;
    vertex = NULL;
    default_uniforms();
#endif
}

bool program::load_file(const char *vert_path, const char *frag_path) {
#if 0
    std::call_once(once, [&]{
        default_vert = std::shared_ptr<ahvfx::shader>(new shader(ahvfx_glsl( precision highp float;
                                                                            attribute vec4 a_Position;
                                                                            attribute vec2 a_TexCoord;
                                                                            varying vec2 v_TexCoord;
                                                                            void main() {
                                                                                gl_Position = a_Position;
                                                                                v_TexCoord = a_TexCoord;
                                                                            }), GL_VERTEX_SHADER));
        default_frag = std::shared_ptr<ahvfx::shader>(new shader(ahvfx_glsl(precision mediump float;
                                                                            varying vec2 v_TexCoord;
                                                                            uniform sampler2D u_Texture0;
                                                                            void main() {
                                                                                gl_FragColor = texture2D(u_Texture0, v_TexCoord);
                                                                            }), GL_FRAGMENT_SHADER));
    });
    std::shared_ptr<ahvfx::shader> vertex, fragment;
    long sz = 0;
    const char *vert = readbuf(vert_path, sz);
    const char *frag = readbuf(frag_path, sz);
    vertex = vert ? std::shared_ptr<ahvfx::shader>(new shader(vert, GL_VERTEX_SHADER)) : default_vert;
    fragment = frag ? std::shared_ptr<ahvfx::shader>(new shader(frag, GL_FRAGMENT_SHADER)) : default_frag;
    intl_program = std::shared_ptr<ahvfx_program_t>(new ahvfx_program_t(vertex, fragment));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    fragment = NULL;
    vertex = NULL;
    default_uniforms();
#endif
    
    return true;
}

void program::default_uniforms() {
    for (int i = 0; i < k_ahvfx_max_texture_unit_count; ++i) {
        char name[64];
        int32_t loc;
        sprintf(name, "u_Texture%d", i);
        loc = uniform_location(name);
        if (loc == -1) {
            sprintf(name, "u_Texture[%d]", i);
            loc = uniform_location(name);
        }
        uniform.texture[i] = loc;
        sprintf(name, "u_TextureSize%d", i);
        loc = uniform_location(name);
        if (loc == -1) {
            sprintf(name, "u_TextureSize[%d]", i);
            loc = uniform_location(name);
        }
        uniform.textureSize[i] = loc;
        sprintf(name, "u_TextureEnabled%d", i);
        loc = uniform_location(name);
        if (loc == -1) {
            sprintf(name, "u_TextureEnabled[%d]", i);
            loc = uniform_location(name);
        }
        uniform.textureEnabled[i] = loc;
    }
    uniform.frameBufferSize = uniform_location("u_FrameBufferSize");
    uniform.frameIndex = uniform_location("u_FrameIndex");
    uniform.startIndex = uniform_location("u_StartIndex");
    uniform.endIndex = uniform_location("u_EndIndex");
    uniform.rangeProgress = uniform_location("u_RangeProgress");
    uniform.instanceRenderIndex = uniform_location("u_InstanceRenderIndex");
    uniform.instanceRenderCount = uniform_location("u_InstanceRenderCount");
    uniform.coverageRenderIndex = uniform_location("u_CoverageRenderIndex");
    uniform.periodAI = uniform_location("u_PeriodAI");
    uniform.random01 = uniform_location("u_Random01");
}

program::~program() {
    decons_log();
}

void program::use() {
    if (intl_program) {
        intl_program->use();
    } else {
        ahvfx_log("%s ?", __func__);
    }
}

const uint32_t program::uniform_location(const char *name) const {
    return intl_program->uniform_location(name);
}

const uint32_t program::attribute_location(const char *name) const {
    return intl_program->attribute_location(name);
}

void program::uniform1(int32_t loc, int x) {
    if (loc != -1)
        glUniform1i(loc, x);
}

void program::uniform1(int32_t loc, float x) {
    if (loc != -1)
        glUniform1f(loc, x);
}

void program::uniform2(int32_t loc, int x, int y) {
    if (loc != -1)
        glUniform2i(loc, x, y);
}

void program::uniform2(int32_t loc, float x, float y) {
    if (loc != -1)
        glUniform2f(loc, x, y);
}

void program::uniform3(int32_t loc, int x, int y, int z) {
    if (loc != -1)
        glUniform3i(loc, x, y, z);
}

void program::uniform3(int32_t loc, float x, float y, float z) {
    if (loc != -1)
        glUniform3i(loc, x, y, z);
}

void program::uniform4(int32_t loc, int x, int y, int z, int w) {
    if (loc != -1)
        glUniform1i(loc, x);

}

void program::uniform4(int32_t loc, float x, float y, float z, float w) {
    if (loc != -1)
        glUniform1i(loc, x);
}

void program::uniform1v(int32_t loc, int count, int *v) {
    if (loc != -1) {
        glUniform1iv(loc, count, v);
    }
}

void program::uniform1v(int32_t loc, int count, float *v) {
    if (loc != -1) {
        glUniform1fv(loc, count, v);
    }
}

void program::uniform2v(int32_t loc, int count, int *v) {
    if (loc != -1) {
        glUniform2iv(loc, count, v);
    }
}

void program::uniform2v(int32_t loc, int count, float *v) {
    if (loc != -1) {
        glUniform2fv(loc, count, v);
    }
}

void program::uniform3v(int32_t loc, int count, int *v) {
    if (loc != -1) {
        glUniform3iv(loc, count, v);
    }
}

void program::uniform3v(int32_t loc, int count, float *v) {
    if (loc != -1) {
        glUniform3fv(loc, count, v);
    }
}

void program::uniform4v(int32_t loc, int count, int *v) {
    if (loc != -1) {
        glUniform4iv(loc, count, v);
    }
}

void program::uniform4v(int32_t loc, int count, float *v) {
    if (loc != -1) {
        glUniform4fv(loc, count, v);
    }
}

void program::uniform1(int32_t loc, uint32_t x) {
    uniform1(loc, (int)x);
}

void program::uniform1v(int32_t loc, int count, uint32_t *v) {
    uniform1v(loc, count, (int *)v);
}

void program::uniform2(int32_t loc, int x, uint32_t y) {
    uniform2(loc, (int)x, (int)y);
}

void program::uniform2v(int32_t loc, int count, uint32_t *v) {
    uniform2v(loc, count, (int *)v);
}

void program::uniform3(int32_t loc, uint32_t x, uint32_t y, uint32_t z) {
    uniform3(loc, (int)x, (int)y, (int)z);
}

void program::uniform3v(int32_t loc, int count, uint32_t *v) {
    uniform3v(loc, count, (int *)v);
}

void program::uniform4(int32_t loc, uint32_t x, uint32_t y, uint32_t z, uint32_t w) {
    uniform4(loc, (int)x, (int)y, (int)z, (int)w);
}

void program::uniform4v(int32_t loc, int count, uint32_t *v) {
    uniform4v(loc, count, (int *)v);
}


