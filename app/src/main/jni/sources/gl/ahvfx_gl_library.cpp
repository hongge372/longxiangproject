//
//  ahvfx_gl_library.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/26.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_gl_library.hpp"
#include "ahvfx_defs.h"
namespace ahvfx {
    class system_lib {
    
    public:
        
        static bool library_system_ext_framebuffer_fetch(std::string &in) {
            return
            in == ahvfx_glsl([system extension: sffetch]) ||
            in == ahvfx_glsl([system extension: sffetch]\r);
        }
        
        std::string ext_framebuffer_fetch() {
            return std::string("#") + ahvfx_glsl(extension GL_EXT_shader_framebuffer_fetch : require);
        }
        
        static bool library_system_inline_sample_lod(std::string &in) {
            return
            in == ahvfx_glsl([system sample LOD]) ||
            in == ahvfx_glsl([system sample LOD]\r) ||
            in == ahvfx_glsl([system sample]) ||
            in == ahvfx_glsl([system sample]\r);
        }
        
        static std::string system_sample_lod() {
            return ahvfx_glsl
            (
             vec4 SAMP1(int index) {
                 return texture2D(u_Texture[index], v_TexCoord);
             }
             
             vec4 SAMP2(int index, vec2 uv) {
                 return texture2D(u_Texture[index], uv);
             }

            );
        }
                
        static bool library_system_inline_attribute(std::string &s) {
            return
            s == ahvfx_glsl([system attribute]) ||
            s == ahvfx_glsl([system attribute]\r);
        }
        static bool library_system_inline_uniform(std::string &s) {
            return
            s == ahvfx_glsl([system uniform]) ||
            s == ahvfx_glsl([system uniform]\r);
        }
        static std::string system_attribute() {
            return ahvfx_glsl(
                              attribute vec4 a_Position;
                              attribute vec4 a_TexCoord;);
            //    attribute vec4 a_Normal;"
#if GL_ES_VERSION_3_0
            return ahvfx_glsl(\
                              #version 300 es
                              layout(location = 0) in vec4 a_Position;
                              layout(location = 1) in vec4 a_TexCoord;
                              layout(location = 2) in vec4 a_Normal;
                              );
#endif
        }
        static std::string system_uniform() {
            return ahvfx_glsl(
                              uniform sampler2D u_Texture[8];
                              uniform vec2 u_TextureSize[8];
                              uniform int u_TextureEnabled[8];
                              uniform vec2 u_FrameBufferSize;
                              uniform int u_FrameIndex;
                              uniform int u_StartIndex;
                              uniform int u_EndIndex;
                              uniform int u_InstanceRenderIndex;
                              uniform int u_InstanceRenderCount;
                              uniform int u_CoverageRenderIndex;
                              
                              
                              uniform vec2 u_CarPoints[8];
                              );
        }
        
        // varying
        static bool library_system_inline_varying(std::string &in) {
            return
            in == ahvfx_glsl([system varying]) ||
            in == ahvfx_glsl([system varying]\r);
        }
        
        static std::string system_varying() {
            return ahvfx_glsl(varying vec2 v_TexCoord;);
        }
        
        //matrix
        static bool library_system_inline_matrix_funcs(std::string &in) {
            return
            in == ahvfx_glsl([system matrix function]) ||
            in == ahvfx_glsl([system matrix function]\r);
        }
        
        static std::string system_matrix_functions() {
            return ahvfx_glsl(
            const float Pi = 3.14159265358;
            float a2r(float angle) {
                return angle / 180.0 * Pi;
            }
            mat4 matrix_translate(vec3 trans) {
                return mat4(1.0, 0.0, 0.0, 0.0,
                            0.0, 1.0, 0.0, 0.0,
                            0.0, 0.0, 1.0, 0.0,
                            trans.x, trans.y, trans.z, 1.0);
            }
             
            mat4 matrix_scale(vec3 scale) {
                return mat4(scale.x, 0.0, 0.0, 0.0,
                            0.0, scale.y, 0.0, 0.0,
                            0.0, 0.0, scale.z, 0.0,
                            0.0, 0.0, 0.0, 1.0);
            }
                
            mat4 matrix_rotateX(float rad) {
                return mat4(1.0, 0.0, 0.0, 0.0,
                            0.0, cos(rad), -sin(rad), 0.0,
                            0.0, sin(rad),  cos(rad), 0.0,
                            0.0, 0.0, 0.0, 1.0);
            }
            
            mat4 matrix_rotateY(float rad) {
                return mat4(cos(rad), 0.0, sin(rad), 0.0,
                            0.0, 1.0, 0.0, 0.0,
                            -sin(rad), 0.0, cos(rad), 0.0,
                            0.0, 0.0, 0.0, 1.0);
            }
            
            mat4 matrix_rotateZ(float rad) {
                return mat4( cos(rad),  sin(rad), 0.0, 0.0,
                            -sin(rad),  cos(rad), 0.0, 0.0,
                            0.0,         0.0, 1.0, 0.0,
                            0.0,         0.0, 0.0, 1.0);
            }
                
            mat4 matrix_rotate(vec3 r) {
                return matrix_rotateX(r.x) * matrix_rotateY(r.y) * matrix_rotateZ(r.z);
            });
            
        }
        
        // commands
        static bool library_system_inline_cmds_begin(std::string &in) {
            return
            in == ahvfx_glsl([system commands begin]) ||
            in == ahvfx_glsl([system commands begin]\r);
        }
        
        static std::string system_commands_begin() {
            return ahvfx_glsl(void main() {);
        }
                              
        static bool library_system_inline_cmds_end(std::string &in) {
            return
            in == ahvfx_glsl([system commands end]) ||
            in == ahvfx_glsl([system commands end]\r);
        }
          
        static std::string system_commands_end() {
            return ahvfx_glsl(});
        }
                              
        // out01
      static bool is_out0(std::string &in) {
          return
          in == ahvfx_glsl([output0]) ||
          in == ahvfx_glsl([output0]\r) ||
          in == ahvfx_glsl([config output0]) ||
          in == ahvfx_glsl([config output0]\r);
      }
      
      static std::string output0() {
          return ahvfx_glsl(gl_Position = );
      }
      
      static bool is_out1(std::string &in) {
          return
          in == ahvfx_glsl([output1]) ||
          in == ahvfx_glsl([output1]\r) ||
          in == ahvfx_glsl([config output1]) ||
          in == ahvfx_glsl([config output1]\r);
      }
      
      static std::string output1() {
          return ahvfx_glsl(gl_FragColor = );
      }
      
      static bool is_frag_header1(std::string &in) {
          return
          in ==  ahvfx_glsl([system header: level 1]) ||
          in ==  ahvfx_glsl([system header: level 1]\r);
      }
      
      static std::string frag_header1() {
          return ahvfx_glsl(
                            precision lowp float;
                            precision lowp int;
                            );
      }
      
      static bool is_frag_header2(std::string &in) {
          return
          in ==  ahvfx_glsl([system header: level 2]) ||
          in ==  ahvfx_glsl([system header: level 2]\r);
      }
      
      static std::string frag_header2() {
          return ahvfx_glsl(
                            precision mediump float;
                            precision mediump int;
                            );
      }

      static bool is_frag_header3(std::string &in) {
          return
          in ==  ahvfx_glsl([system header: level 3]) ||
          in ==  ahvfx_glsl([system header: level 3]\r);
      }

      static std::string frag_header3() {
          return ahvfx_glsl(
                            precision highp float;
                            precision highp int;
                            );
      }
      
      
      static bool has(std::string &str, std::string sub) {
          return str.find(sub) != -1;
      }
      
      static bool begin_with(std::string &str, std::string sub) {
          return str.find(sub) == 0;
      }
      
      static bool commented(std::string &in) {
          return has(in, "$") || has(in, "//") || has(in, "::");
      }
      
      //blend funcs
      static bool is_blend_func_normal(std::string &in) {
          return
          in == ahvfx_glsl([system blend func: normal]) ||
          in == ahvfx_glsl([system blend func: normal]\r);
      }
      
      static std::string blend_func_normal() {
          return ahvfx_glsl(vec4 blend_func_normal(vec4 col1, vec4 col2) {
              return mix(col1, col2, col2.a);
          });
      }
      
      static bool is_blend_func_normal_alpha_premultiply(std::string &in) {
          return
          in == ahvfx_glsl([system blend func: normal_apm]) ||
          in == ahvfx_glsl([system blend func: normal_apm]\r);
      }
      
      static std::string blend_func_normal_alpha_premultiply() {
          return ahvfx_glsl(vec4 blend_func_normal_apm(vec4 col1, vec4 col2) {
              return col1 * (1.0 - col2.a) + col2;
          });
      }

      
      static bool is_blend_func_screen(std::string &in) {
          return
          in == ahvfx_glsl([system blend func: screen]) ||
          in == ahvfx_glsl([system blend func: screen]\r);
      }
      
      static std::string blend_func_screen() {
          return ahvfx_glsl(vec4 blend_func_screen(vec4 col1, vec4 col2) {
              return vec4(1.0) - (vec4(1.0) - col1) * (vec4(1.0) - col2);
          });
      }
      
      static bool is_blend_func_additive(std::string &in) {
          return
          in == ahvfx_glsl([system blend func: additive]) ||
          in == ahvfx_glsl([system blend func: additive]\r);
      }
      
      static std::string blend_func_additive() {
          return ahvfx_glsl(vec4 blend_func_additive(vec4 col1, vec4 col2) {
              return col1 + col2;
          });
      }
      
      static bool is_blend_func_subtract(std::string &in) {
          return
          in == ahvfx_glsl([system blend func: subtract]) ||
          in == ahvfx_glsl([system blend func: subtract]\r);
      }
      
      static std::string blend_func_subtract() {
          return ahvfx_glsl(vec4 blend_func_subtract(vec4 col1, vec4 col2) {
              return col1 - col2;
          });
      }
      
      
      static bool is_blend_func_difference(std::string &in) {
          return
          in == ahvfx_glsl([system blend func: difference]) ||
          in == ahvfx_glsl([system blend func: difference]\r);
      }
      
      static std::string blend_func_difference() {
          return ahvfx_glsl(vec4 blend_func_difference(vec4 col1, vec4 col2) {
              return abs(col1 - col2);
          });
      }
                              
      static bool is_blend_func_overlay(std::string &in) {
          return
          in == ahvfx_glsl([system blend func: overlay]) ||
          in == ahvfx_glsl([system blend func: overlay]\r);
      }
      
      static std::string blend_func_overlay() {
          return ahvfx_glsl(
          float blend_func_overlay_channel(float c1, float c2) {
              return c1 < 0.5 ? (2.0 * c1 * c2) : (1.0 - 2.0 * (1.0 - c1) * (1.0 - c2));
          }
          vec4 blend_func_overlay(vec4 col1, vec4 col2) {
              return vec4(blend_func_overlay_channel(col1.r, col2.r),
                            blend_func_overlay_channel(col1.g, col2.g),
                            blend_func_overlay_channel(col1.b, col2.b),
                           1.0);
          });
      }
                              
    static bool is_blend_func_hardlight(std::string &in) {
          return
          in == ahvfx_glsl([system blend func: hardlight]) ||
          in == ahvfx_glsl([system blend func: hardlight]\r);
      }
      
      static std::string blend_func_hardlight() {
          return ahvfx_glsl(
                            float blend_func_hardlight_pc(float c1, float c2) {
                                return c1 < 0.5 ? 2.0 * c1 * c2 : 1.0 - 2.0 * (1.0 - c1) * (1.0 - c2);
                            }
                            
                            vec4 blend_func_hardlight(vec4 c1, vec4 c2) {
                                return vec4(blend_func_hardlight_pc(c1.r, c2.r),
                                            blend_func_hardlight_pc(c1.g, c2.g),
                                            blend_func_hardlight_pc(c1.b, c2.b), 1.0);
                            }
         );
      }
                              
      static bool is_lookup_func(std::string &in) {
          return
          in == ahvfx_glsl([system lookup func]) ||
          in == ahvfx_glsl([system lookup func]\r);
      }
      
      static std::string lookup_func() {
          return ahvfx_glsl(
      vec4 look_up_func(vec4 color, sampler2D texture) {
     highp vec4 textureColor = color;
     highp float blueColor = textureColor.b * 63.0;

     highp vec2 quad1;
     quad1.y = floor(floor(blueColor) / 8.0);
     quad1.x = floor(blueColor) - (quad1.y * 8.0);

     highp vec2 quad2;
     quad2.y = floor(ceil(blueColor) / 8.0);
     quad2.x = ceil(blueColor) - (quad2.y * 8.0);

     highp vec2 texPos1;

     texPos1.x = (quad1.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.r);
     texPos1.y = (quad1.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.g);

     highp vec2 texPos2;
     texPos2.x = (quad2.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.r);
     texPos2.y = (quad2.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.g);

     lowp vec4 newColor1 = texture2D(texture, texPos1);
     lowp vec4 newColor2 = texture2D(texture, texPos2);

     lowp vec4 newColor = mix(newColor1, newColor2, fract(blueColor));

    return newColor;
}
         );
      }
                              
                              
                              
    };
}
                              
using namespace ahvfx;

std::string gl_library::library_inline(std::string &in) {
    
    if (system_lib::commented(in)) {
        return "";
    }
    else if (system_lib::library_system_inline_sample_lod(in)) {
        return system_lib::system_sample_lod();
    }
    
    else if (system_lib::library_system_inline_attribute(in)) {
        return system_lib::system_attribute();
    } else if (system_lib::library_system_inline_uniform(in)) {
        return system_lib::system_uniform();
    } else if (system_lib::library_system_inline_matrix_funcs(in)) {
        return system_lib::system_matrix_functions();
    } else if (system_lib::library_system_inline_varying(in)) {
        return system_lib::system_varying();
    } else if (system_lib::library_system_inline_cmds_begin(in)) {
        return system_lib::system_commands_begin();
    } else if (system_lib::library_system_inline_cmds_end(in)) {
        return system_lib::system_commands_end();
    } else if (system_lib::is_out0(in)) {
        return system_lib::output0();
    } else if (system_lib::is_out1(in)) {
        return system_lib::output1();
    }
    else if (system_lib::is_frag_header1(in)) {
        return system_lib::frag_header1();
    }
    else if (system_lib::is_frag_header2(in)) {
        return system_lib::frag_header2();
    }
    else if (system_lib::is_frag_header3(in)) {
        return system_lib::frag_header3();
    }

    else if (system_lib::is_blend_func_normal(in)) {
        return system_lib::blend_func_normal();
    }
    
    else if (system_lib::is_blend_func_normal_alpha_premultiply(in)) {
        return system_lib::blend_func_normal_alpha_premultiply();
    }
    
    else if (system_lib::is_blend_func_screen(in)) {
        return system_lib::blend_func_screen();
    }
    
    else if (system_lib::is_blend_func_additive(in)) {
        return system_lib::blend_func_additive();
    }

    else if (system_lib::is_blend_func_subtract(in)) {
        return system_lib::blend_func_subtract();
    }
    
    else if (system_lib::is_blend_func_difference(in)) {
        return system_lib::blend_func_difference();
    }
    
    else if (system_lib::is_blend_func_overlay(in)) {
        return system_lib::blend_func_overlay();
    }
    
    else if (system_lib::is_blend_func_hardlight(in)) {
        return system_lib::blend_func_hardlight();
    }
    
    else if (system_lib::is_lookup_func(in)) {
        return system_lib::lookup_func();
    }
    
    return in;
}

