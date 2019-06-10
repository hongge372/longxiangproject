//
//  ahvfx_nv12_converter.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/19.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_nv12_converter.hpp"

//using namespace ahvfx;
//
//nv12_converter::nv12_converter() {
//    renderPassCount = 1;
//    program = std::shared_ptr<ahvfx::program>(new ahvfx::program(
//                                                                 ahvfx_glsl (
//                                                                          attribute vec4 a_Position;
//                                                                          attribute vec2 a_TexCoord;
//                                                                          
//                                                                          varying vec2 v_TexCoord;
//                                                                          
//                                                                          uniform float u_upsd;
//                                                                          
//                                                                          void main()
//                                                                          {
//                                                                              gl_Position = mat4(1., 0., 0., 0., 0., 1. - 2. * u_upsd, 0., 0., 0., 0., 1., 0., 0., 0., 0., 1.) * a_Position;
//                                                                              v_TexCoord = a_TexCoord;
//                                                                          }),
//                                                                 ahvfx_glsl (
//                                                                                      varying highp vec2 v_TexCoord;
//                                                                                      uniform sampler2D u_Texture0;
//                                                                                      uniform sampler2D u_Texture1;
//                                                                                      
//                                                                                      highp mat3 colorConversionMatrix =
//                                                                                      mat3(  1.164,    1.164,     1.164,
//                                                                                           0.0,      -0.213,    2.112,
//                                                                                           1.793,    -0.533,    0.0);
//                                                                                      
//                                                                                      void main()
//                                                                                      {
//                                                                                          mediump vec3 yuv;
//                                                                                          
//                                                                                          yuv.x = texture2D(u_Texture0, v_TexCoord).r - (16.0/255.0);
//                                                                                          yuv.yz = texture2D(u_Texture1, v_TexCoord).ra - vec2(0.5, 0.5);
//                                                                                          
//                                                                                          lowp vec3 rgb = colorConversionMatrix * yuv;
//                                                                                          
//                                                                                          gl_FragColor = vec4(rgb, 1);
//                                                                                      }
//                                                                                      )));
//}
//
//nv12_converter::~nv12_converter() {
//    decons_log();
//}
//
//void nv12_converter::in(ahvfx::texture_t texture, uint32_t index, uint32_t fIdx) {
//    graph_node::in(texture, index, fIdx);
//}
//
//const ahvfx::texture_t nv12_converter::out() {
//    return graph_node::out();
//}
//
//void nv12_converter::draw() {
//    graph_node::draw();
//}
