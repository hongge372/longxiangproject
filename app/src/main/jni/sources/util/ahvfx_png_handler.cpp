//
//  ahvfx_png_handler.cpp
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/18.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_png_handler.hpp"
#include "png.h"
#include <stdio.h>
#include <iostream>
#include <fstream>

extern "C" {
    char *readbuf(const char *, long &);
}

#define PNG_BYTES_TO_CHECK 4

namespace ahvfx {
    class png_handler_imlp {
    private:
        std::string name = "ahvfx_png";
        ahvfx_des;
        unsigned char *img_bytes = NULL;
        uint32_t img_width = 0, img_height = 0, img_component = 0;
    public:
        ~png_handler_imlp() {
            if (img_bytes) {
                free(img_bytes);
                img_bytes = 0;
            }
            img_width = img_height = img_component = 0;
            decons_log();
        }
        
        struct png_source {
            png_byte *data;
            png_int_32 size;
            png_int_32 offset;
        };
        
        static void png_ready_callback(png_structp png_ptr, png_bytep data, png_size_t length)
        {
            png_source *isource = (png_source *)png_get_io_ptr(png_ptr);
            if (isource->offset + length <= isource->size) {
                memcpy(data, isource->data + isource->offset, length);
                isource->offset += length;
            } else {
                png_error(png_ptr, "png callback failed.");
            }
        }
        
        explicit png_handler_imlp(std::string file) {
            
            FILE *fp;
            png_structp png_ptr;
            png_infop info_ptr;
            png_bytep* row_pointers;
            png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, 0, 0, 0 );
            if (!png_ptr) {
                return;
            }
            info_ptr = png_create_info_struct( png_ptr );
            if (!info_ptr) {
                png_destroy_read_struct(&png_ptr, 0, 0);
                return;
            }
            
            if(setjmp(png_jmpbuf(png_ptr))) {
                png_destroy_read_struct(&png_ptr, &info_ptr,0);
            }
            
//            char buf[PNG_BYTES_TO_CHECK];
            int w, h, x, y, color_type;
            
            long bufsz;
            png_source png_src;
            png_src.data = (png_byte *)readbuf(file.c_str(), bufsz);
            if (!png_src.data) {
                printf("file %s not exitst.", file.c_str());
                png_destroy_read_struct(&png_ptr, &info_ptr,0);
                return;
            }
            png_src.size = (png_int_32)bufsz;
            png_src.offset = 0;
            png_set_read_fn(png_ptr, &png_src, png_ready_callback);
            
//            fp = fopen(file.c_str(), "rb");
//            if( fp == NULL ) {
//                assert(0);
//                return;
//            }
//
//
//            setjmp( png_jmpbuf(png_ptr) );
//
//            /* 读取PNG_BYTES_TO_CHECK个字节的数据 */
//            temp = (int)fread( buf, 1, PNG_BYTES_TO_CHECK, fp );
//            /* 若读到的数据并没有PNG_BYTES_TO_CHECK个字节 */
//            if( temp < PNG_BYTES_TO_CHECK ) {
//                fclose(fp);
//                png_destroy_read_struct( &png_ptr, &info_ptr, 0);
//                return/* 返回值 */;
//            }
//            /* 检测数据是否为PNG的签名 */
//            temp = png_sig_cmp( (png_bytep)buf, (png_size_t)0, PNG_BYTES_TO_CHECK );
//            /* 如果不是PNG的签名，则说明该文件不是PNG文件 */
//            if( temp != 0 ) {
//                fclose(fp);
//                png_destroy_read_struct( &png_ptr, &info_ptr, 0);
//                assert(0);
//                return/* 返回值 */;
//            }
            
            /* 复位文件指针 */
//            rewind( fp );
            /* 开始读文件 */
//            png_init_io( png_ptr, fp );
            /* 读取PNG图片信息和像素数据 */
            png_read_png( png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0 );
            /* 获取图像的色彩类型 */
            color_type = png_get_color_type( png_ptr, info_ptr );
            /* 获取图像的宽高 */
            w = png_get_image_width( png_ptr, info_ptr );
            h = png_get_image_height( png_ptr, info_ptr );
            /* 获取图像的所有行像素数据，row_pointers里边就是rgba数据 */
            row_pointers = png_get_rows( png_ptr, info_ptr );
            
            size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
            img_bytes = (png_byte *)malloc(h * rowbytes);
            img_width = w;
            img_height = h;
            
            /* 根据不同的色彩类型进行相应处理 */
            png_byte *pImg = img_bytes;
            switch( color_type ) {
                case PNG_COLOR_TYPE_RGB_ALPHA:
                    img_component = 4;
                    for( y=0; y<h; ++y ) {
                        for( x=0; x<w*4; ) {
                            /* 以下是RGBA数据，需要自己补充代码，保存RGBA数据 */
//                            png_byte v;
                            *pImg++/* 目标内存 */ = row_pointers[y][x++]; // red
                            *pImg++/* 目标内存 */ = row_pointers[y][x++]; // green
                            *pImg++/* 目标内存 */ = row_pointers[y][x++]; // blue
                            *pImg++/* 目标内存 */ = row_pointers[y][x++]; // alpha
                        }
                    }
                    break;
                    
                case PNG_COLOR_TYPE_RGB:
                    img_component = 3;
                    for( y=0; y<h; ++y ) {
                        for( x=0; x<w*3; ) {
                            *pImg++/* 目标内存 */ = row_pointers[y][x++]; // red
                            *pImg++/* 目标内存 */ = row_pointers[y][x++]; // green
                            *pImg++/* 目标内存 */ = row_pointers[y][x++]; // blue
                        }
                    }
                    break;
                    /* 其它色彩类型的图像就不读了 */
                    
                case PNG_COLOR_TYPE_GRAY:
                    img_component = 1;
                    for( y=0; y<h; ++y ) {
                        for( x=0; x<w; ) {
                            *pImg++/* 目标内存 */ = row_pointers[y][x++]; // g
                        }
                    }
                    break;
                default:
                    fclose(fp);
                    free(png_src.data);
                    png_destroy_read_struct( &png_ptr, &info_ptr, 0);
                    return/* 返回值 */;
            }
            free(png_src.data);
            png_destroy_read_struct( &png_ptr, &info_ptr, 0);
            return;
        }
    public:
        const unsigned char * byte() const {
            return img_bytes;
        }
        
        const uint32_t width() const {
            return img_width;
        }
        
        const uint32_t height() const {
            return img_height;
        }
        
        const uint32_t component() const {
            return img_component;
        }
    };
}

using namespace ahvfx;

static inline bool file_exists(const char *file) {
    std::fstream _file;
    _file.open(file, std::ios::in);
    return !!_file;
}

png_handler::png_handler(const std::string file) {
    if (file_exists(file.c_str())) {
        impl = std::shared_ptr<png_handler_imlp>(new png_handler_imlp(file));
    }
}

png_handler::~png_handler() {
    decons_log();
}

const unsigned char * png_handler::byte() const {
    if (impl) {
        return impl->byte();
    }
    return NULL;
}

const uint32_t png_handler::width() const {
    if (impl) {
        return impl->width();
    }
    return 32;
}
const uint32_t png_handler::height() const {
    if (impl) {
        return impl->height();
    }
    return 32;
}

const uint32_t png_handler::component() const {
    if (impl) {
        return impl->component();
    }
    return 0;
}
