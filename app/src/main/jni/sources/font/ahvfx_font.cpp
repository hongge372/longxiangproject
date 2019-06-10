//
//  ahvfx_font.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/4/9.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_font.hpp"
#include "ahvfx_character.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_BITMAP_H
#include <assert.h>
#include "ahvfx_texture.hpp"
#include "ahvfx_plateform.h"
#include "ahvfx_program.hpp"
#include "ahvfx_flat.hpp"
#include "ahvfx_vertexbuffer.hpp"
#include <sstream>
#include <stdlib.h>
#define max(a, b) (a > b ? a : b)

extern "C" {
    void ahvfx_UNPACKT_ALIGNMENT(uint32_t aligment);
    #include "ahvfx_gl_util.h"
}

static ahvfx::program *Render_Text_Program = NULL;
typedef ahvfx::character CHAR;

namespace ahvfx {
    
    struct glyph_vertex {
        float x = 0;
        float y = 0;
//        float z = 0;
        float u = 0;
        float v = 0;
//        float w = 0;
    };
    
    class font_impl {
    public:
        
        std::shared_ptr<ahvfx::flat> flat;
        std::shared_ptr<ahvfx::vertexbuffer> vertices_buffer;
        std::shared_ptr<ahvfx::vertexbuffer> uvs_buffer;

        FT_Library library = NULL;
        FT_Face face = NULL;
        CHAR CHAR_SET[1 << 16];
        char *font_Buffer = NULL;
        uint32_t font_Size = 0;
        std::shared_ptr<ahvfx::texture> CHAR_TEX[1 << 8];
        uint32_t CHAR_TEX_SZ = 0;
        uint32_t CHAR_TEX_IDX = 0;
        uint32_t x_Start = 0, y_Start = 0;
        uint32_t font_Pixel_SizeX = 0, font_Pixel_SizeY = 0;
    public:
        void destroy_face() {
            if (face) {
                FT_Done_Face(face);
                memset(CHAR_SET, 0, sizeof(CHAR_SET));
            }
            destroy_CHAR_TEX();
        }
        
        font_impl() {
            
            static std::once_flag oc;
            std::call_once(oc, [&]{
                Render_Text_Program = new ahvfx::program(ahvfx_glsl(
                                                                    attribute vec4 a_Position;
                                                                    attribute vec4 a_TexCoord;
                                                                    
                                                                    varying vec2 v_TexCoord;
                                                                    
                                                                    mat4 s(float v) {
                                                                        return mat4 (v, 0.0, 0.0, 0.0, 0.0, v, 0.0, 0.0, 0.0, 0.0, v, 0.0, 0.0, 0.0, 0.0, 1.0);
                                                                    }
                                                                    
                                                                    void main() {
                                                                        gl_Position = s(1.0) * a_Position;
                                                                        v_TexCoord = a_TexCoord.st;
                                                                    }
                                                         ), ahvfx_glsl(
                                                                       precision mediump float;
                                                                       varying vec2 v_TexCoord;
                                                                       
                                                                       uniform sampler2D u_Texture0;
                                                                       uniform sampler2D u_Texture1;
                                                                       
                                                                       void main() {
//                                                                           if (v_TexCoord.x < .5) {
                                                                               gl_FragColor = vec4(vec3(1.0), texture2D(u_Texture0, v_TexCoord).a);
//                                                                           } else {
//                                                                               gl_FragColor = texture2D(u_Texture1, v_TexCoord) * vec4(1.0, 0.0, 0.0, 1.0);
//                                                                           }
//                                                                           gl_FragColor = vec4(v_TexCoord, 0.0, 1.0);
                                                                       }
                                                         ));
            });
            
            glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint *)&CHAR_TEX_SZ);
//            CHAR_TEX_SZ = 2000;
            FT_Error rs = FT_Init_FreeType(&library);
            if (rs != FT_Err_Ok) {
                printf("1\n");
            }
        }
        ~font_impl() {
            FT_Done_FreeType(library);
        }
        
        char * read_file(const char *file_path, unsigned long &length) {
            FILE *file = fopen(file_path, "rb");
            if (file) {
                fseek(file, 0, SEEK_END);
                length = ftell(file);
                fseek(file, 0, SEEK_SET);
                char *buffer = new char[length + 1];
                fread(buffer, 1, length, file);
                buffer[length] = 0;
                fclose(file);
                return buffer;
            } else {
                assert(0);
            }
            return 0;
        }
        
        void create_CHAR_TEX() {
            CHAR_TEX[CHAR_TEX_IDX] = std::shared_ptr<ahvfx::texture>(new ahvfx::texture(GL_ALPHA));
            CHAR_TEX[CHAR_TEX_IDX]->bind(ahvfx::vec2<uint32_t>(CHAR_TEX_SZ, CHAR_TEX_SZ));
        }
        
        void destroy_CHAR_TEX() {
            for (int i = 0; i < 1 << 8; ++i) {
                CHAR_TEX[i] = NULL;
            }
            CHAR_TEX_IDX = 0;
        }
        
        void build_font(const char *font_Path, uint32_t font_Size) {
            unsigned long length = 0;
            font_Buffer = read_file(font_Path, length);
            this->font_Size = font_Size;
            
            destroy_face();
            create_CHAR_TEX();
            
            FT_Error rs = FT_New_Memory_Face(library, (const FT_Byte *)font_Buffer, length, 0, &face);
            if (rs == FT_Err_Ok) {
                
                FT_Select_Charmap(face, FT_ENCODING_UNICODE);
                FT_F26Dot6 ft_Size = (FT_F26Dot6)(font_Size * (1 << 6));
                FT_Set_Char_Size(face, ft_Size, 0, 72, 72);
        
            } else {
                destroy_face();
                assert(0);
            }
        }
        
        CHAR get_Character(wchar_t ch) {
            if (CHAR_SET[ch].invalid()) {
                ahvfx_UNPACKT_ALIGNMENT(1);
                if (x_Start + max(font_Pixel_SizeX, font_Size) > CHAR_TEX_SZ) {
                    x_Start = 0;
                    y_Start += max(font_Pixel_SizeY, font_Size);
                    if (y_Start > CHAR_TEX_SZ) {
                        y_Start = 0;
                        CHAR_TEX_IDX++;
                        create_CHAR_TEX();
                    }
                }
                FT_UInt index = FT_Get_Char_Index(face, ch);
                FT_Load_Glyph(face, index, FT_LOAD_DEFAULT);
                FT_Glyph glyph;
                FT_Get_Glyph(face->glyph, &glyph);
                
                if (!(ch >= L'0' &&  ch <= L'9')) {
                    FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1);
                } else {
                    FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_MONO, 0, 1);
                }
                
                FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
                FT_Bitmap &bitmap = bitmap_glyph->bitmap;
                FT_Bitmap ft_bitmap;
                FT_Bitmap_New(&ft_bitmap);
                
                if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO) {
                    if (FT_Bitmap_Convert(library, &bitmap, &ft_bitmap, 1) == FT_Err_Ok) {
                        for (unsigned char *p = ft_bitmap.buffer, *pt = p + ft_bitmap.width * ft_bitmap.rows; p != pt; ++p) {
                            *p ^= -*p ^ *p;
                        }
                        bitmap = ft_bitmap;
                    }
                }
                
                //not found
                if (bitmap.width == 0 && bitmap.rows == 0) {
                    char mem[1024 * 32];
                    memset(mem, 0, sizeof(mem));
                    CHAR c = CHAR_SET[ch];
                    c.x0 = x_Start;
                    c.y0 = y_Start;
                    c.x1 = x_Start + font_Size / 2;
                    c.y1 = y_Start + font_Size - 1;
                    c.offset_x = font_Size - 1;
                    c.offset_y = 0;
                    c.mip_index = 0;
                    CHAR_TEX[CHAR_TEX_IDX]->bind();
                    CHAR_TEX[CHAR_TEX_IDX]->upload(x_Start, y_Start, font_Size / 2, font_Size, mem);
                    CHAR_SET[ch] = c;
                } else {
                    CHAR c = CHAR_SET[ch];
                    c.x0 = x_Start;
                    c.y0 = y_Start;
                    c.x1 = x_Start + bitmap.width;
                    c.y1 = y_Start + bitmap.rows;
                    c.offset_x = bitmap_glyph->left;
                    c.offset_y = bitmap_glyph->top;
                    c.mip_index = CHAR_TEX_IDX;
                    CHAR_TEX[CHAR_TEX_IDX]->bind();
                    ahvfx_UNPACKT_ALIGNMENT(1);
                    CHAR_TEX[CHAR_TEX_IDX]->upload(x_Start, y_Start, max(1, bitmap.width), max(1, bitmap.rows), bitmap.buffer);
                    x_Start += (bitmap.width + 1);
                    font_Pixel_SizeX = max(font_Pixel_SizeX, bitmap.width);
                    font_Pixel_SizeY = max(font_Pixel_SizeY, bitmap.rows);
                    CHAR_SET[ch] = c;
                }
                FT_Bitmap_Done((FT_Library)library, &ft_bitmap);
            }
            return CHAR_SET[ch];
        }
        
        void begine_Text() {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        
        void end_Text() {
            glDisable(GL_BLEND);
        }
        
        void drawText(uint32_t x, uint32_t y, const wchar_t *text, size_t len = -1) {
            float xS = x;
            float yS = y + font_Size;
            unsigned index = 0;
            unsigned long size = len == -1 ? wcslen(text) : len;
            if (size == 0) {
                return;
            }
            glyph_vertex vert[size * 6];
            for (unsigned i = 0; i < size; ++i) {
                CHAR ch = get_Character(text[i]);
                float w = ch.x1 - ch.x0;
                float h = ch.y1 - ch.y0;
                float offsetY = h - ch.offset_y;
                float u0 = (float)ch.x0 / CHAR_TEX_SZ;
                float v0 = (float)ch.y0 / CHAR_TEX_SZ;
                float u1 = (float)ch.x1 / CHAR_TEX_SZ;
                float v1 = (float)ch.y1 / CHAR_TEX_SZ;
                
                vert[index + 0].x = xS / 720.0;
                vert[index + 0].y = (yS - h + offsetY) / 1280.0;
                vert[index + 0].u = u0;
                vert[index + 0].v = v0;
                
                vert[index + 1].x = (xS + w) / 720.0;
                vert[index + 1].y = (yS - h + offsetY) / 1280.0;
                vert[index + 1].u = u1;
                vert[index + 1].v = v0;
                
                vert[index + 2].x       =   (xS + w) / 720.0;
                vert[index + 2].y       =   (yS  + offsetY) / 1280.0;
                vert[index + 2].u       =   u1;
                vert[index + 2].v       =   v1;
                
                vert[index + 3].x       =   (xS) / 720.0;
                vert[index + 3].y       =   (yS - h + offsetY) / 1280.0;
                vert[index + 3].u       =   u0;
                vert[index + 3].v       =   v0;
                
                vert[index + 4].x       =   (xS + w) / 720.0;
                vert[index + 4].y       =   (yS  + offsetY) / 1280.0;
                vert[index + 4].u       =   u1;
                vert[index + 4].v       =   v1;
                
                // 第四个点
                vert[index + 5].x       =   (xS) / 720.0;
                vert[index + 5].y       =   (yS  + offsetY) / 1280.0;
                vert[index + 5].u       =   u0;
                vert[index + 5].v       =   v1;
                
                index += 6;
                xS  +=  w + (ch.offset_x + 1);
            }
            
            begine_Text();
            
//            glFlush();
            Render_Text_Program->use();
            ahvfx::texture_t t = CHAR_TEX[CHAR_TEX_IDX]->out();
            ahvfx_active_bind_texture(t, 0);
            Render_Text_Program->uniform1(Render_Text_Program->uniform.texture[0], 0);
            
                        ahvfx_ENABLE_ATTRIB(ahvfx::ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(glyph_vertex), vert);
                        ahvfx_ENABLE_ATTRIB(ahvfx::ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(glyph_vertex), &vert[0].u);
//            static const GLfloat vertices[] = {
//                -1.0f, -1.0f,
//                1.0f, -1.0f,
//                -1.0f,  1.0f,
//                1.0f,  1.0f,
//            };
//
//            static const GLfloat texCoords[] = {
//                        0.0f, 0.0f,
//                        1.0f, 0.0f,
//                        0.0f, 1.0f,
//                        1.0f, 1.0f,
//            };
            
//            if (vertices_buffer == 0) {
//                vertices_buffer = std::shared_ptr<ahvfx::vertexbuffer>(new ahvfx::vertexbuffer(GL_ARRAY_BUFFER));
//            }
//            vertices_buffer->bind();
//            vertices_buffer->buffer((GLvoid *)vert, sizeof(vert), GL_STATIC_DRAW);

            
            
//            if (flat == 0) {
//                flat = std::shared_ptr<ahvfx::flat>(new ahvfx::flat());
//            }
//            flat->draw();

            
            
//            glEnableVertexAttribArray(ahvfx::ATTRIBUTE_POSITION);
//            glVertexAttribPointer(ahvfx::ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(glyph_vertex), 0);
//
////            if (uvs_buffer == 0) {
////                uvs_buffer = std::shared_ptr<ahvfx::vertexbuffer>(new ahvfx::vertexbuffer(GL_ARRAY_BUFFER));
////                uvs_buffer->bind();
////                uvs_buffer->buffer((void *)texCoords, sizeof(texCoords), GL_STATIC_DRAW);
////            } else {
////                uvs_buffer->bind();
////            }
//
//
//            glEnableVertexAttribArray(ahvfx::ATTRIBUTE_TEXCOORD);
//            glVertexAttribPointer(ahvfx::ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(glyph_vertex), (void *)(sizeof(float) * 2));
//            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            
            glDrawArrays(GL_TRIANGLES, 0, index);
            
            end_Text();
//            glFinish();

//            GLuint alpha_2;
//            glGenTextures(1, &alpha_2);
//            glBindTexture(GL_TEXTURE_2D, alpha_2);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//            unsigned char alpha[200 * 200];
//            unsigned char *p = alpha;
//            for (int i = 0; i < 200; ++i) {
//                for (int j = 0; j < 200; ++j) {
//                    *p++ = arc4random() % 256;
////                    *p++ = arc4random() % 256;
////                    *p++ = arc4random() % 256;
////                    *p++ = arc4random() % 256;
//                }
//            }
//            glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 200, 200, 0, GL_ALPHA, GL_UNSIGNED_BYTE, alpha);
//
//
//            glActiveTexture(GL_TEXTURE0);
//            glBindTexture(GL_TEXTURE_2D, alpha_2);
//            Render_Text_Program->uniform1(Render_Text_Program->uniform.texture[0], 0);

//            glDeleteTextures(1, &alpha_2);
        }
    };
}


using namespace ahvfx;

font::font() {
    impl = std::shared_ptr<font_impl>(new font_impl());
}

font::~font() {
    
}

size_t Utf8ToUtf16(const char* src_, wchar_t* dest_, size_t destlen_, size_t srclen_ /*= 0*/)
{
    if (srclen_ == 0)
        srclen_ = strlen(src_);
    size_t destcapacity = destlen_;
    for (size_t idx = 0; ((idx < srclen_) && (destcapacity > 0));)
    {
        wchar_t cp;
        unsigned char   cu = src_[idx++];
        if (cu < 0x80)
            cp = (wchar_t)(cu);
        else if (cu < 0xE0)
        {
            cp = ((cu & 0x1F) << 6);
            cp |= (src_[idx++] & 0x3F);
        }
        else if (cu < 0xF0)
        {
            cp = ((cu & 0x0F) << 12);
            cp |= ((src_[idx++] & 0x3F) << 6);
            cp |= (src_[idx++] & 0x3F);
        }
        else
        {
            cp = L'?';
        }
        *dest_++ = cp;
        --destcapacity;
    }
    return destlen_ - destcapacity;
}

void font::drawText(uint32_t x, uint32_t y, const char *text, size_t len) {
//    impl->begine_Text();
//    std::wstringstream wss;
//    wss << text;
    if (text) {
        size_t sz = len != -1 ? len + 1 : strlen(text) + 1;
        wchar_t *ws = (wchar_t *)malloc(sz * sizeof(wchar_t));
        Utf8ToUtf16(text, ws, sz * sizeof(wchar_t), sz);
        impl->drawText(x, y, ws, len);
        free(ws);
    }
    
//    impl->end_Text();
}

void font::face(const char *font_Path, size_t font_Size) {
    impl->build_font(font_Path, (uint32_t)font_Size);
}
