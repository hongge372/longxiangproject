//
//  ahvfx_shader.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/18.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_shader.hpp"

struct ahvfx_shader_t {
    ahvfx_des_t;
    uint32_t id;
    
    ~ahvfx_shader_t() {
        if (id) {
            glDeleteShader(id);
            id = 0;
        }
        decons_log();
    }
    
    ahvfx_shader_t(const char *string, uint32_t type) {
        id = 0;
        GLuint m_shader = glCreateShader(type);
        GLint status = GL_FALSE;
        if (string) {
            glShaderSource(m_shader, 1, &string, NULL);
            glCompileShader(m_shader);
            glGetShaderiv(m_shader, GL_COMPILE_STATUS, &status);
            if (status != GL_TRUE) {
#ifdef DEBUG
                GLint logLength;
                glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &logLength);
                if (logLength > 0)
                {
                    GLchar *log = (GLchar *)malloc(logLength);
                    glGetShaderInfoLog(m_shader, logLength, &logLength, log);
                    ahvfx_log("%s Shader :%s compile log: %s\n", type == GL_VERTEX_SHADER ? "vertex" : "frag", string, log);
                    free(log);
                    glDeleteShader(m_shader);
                    m_shader = 0;
                }
#else
                glDeleteShader(m_shader);
                m_shader = 0;
#endif
            } else {
                id = m_shader;
            }
        } else {
            ahvfx_log("null shader string.\n");
        }
    }
    
    const int handle() const {
        return id;
    }
    
    const bool good() const {
        return id != 0;
    }
};
typedef struct ahvfx_shader_t ahvfx_shader_t;

using namespace ahvfx;

shader::shader(const char *src, uint32_t type) {
    intl_shader = std::shared_ptr<ahvfx_shader_t>(new ahvfx_shader_t(src, type));
}

shader::~shader() {
    decons_log();
}

const int shader::id() const {
    return intl_shader->handle();
}

const bool shader::good() const {
    return intl_shader->good();
}
