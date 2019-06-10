//
//  ahvfx_gl_util.c
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/22.
//  Copyright © 2019 AH. All rights reserved.
//

#include "ahvfx_gl_util.h"
#include "ahvfx_plateform.h"

void ahvfx_gl_enable_attribute(uint32_t attrib_idx, int32_t sz, uint32_t data_type, int32_t normalized, int32_t stride, const void *ptr);
void ahvfx_ENABLE_ATTRIB(uint32_t attrib_idx, int32_t sz, uint32_t data_type, int32_t normalized, int32_t stride, const void *ptr) {
    ahvfx_gl_enable_attribute(attrib_idx, sz, data_type, normalized, stride, ptr);
}

void ahvfx_gl_enable_attribute(uint32_t attrib_idx, int32_t sz, uint32_t data_type, int32_t normalized, int32_t stride, const void *ptr) {
    glEnableVertexAttribArray(attrib_idx);
    glVertexAttribPointer(attrib_idx, sz, data_type, normalized, stride, ptr);
}

void ahvfx_gl_default_config_texture_2d_parameters(uint32_t texture) {
    if (texture != 0) {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void ahvfx_GPU_COMMANDS_SYNC(void) {
#if GL_ES_VERSION_3_0
    GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    glFlush();
    glClientWaitSync(sync, 0, GL_TIMEOUT_IGNORED);
#elif GL_APPLE_sync
    GLsync sync = glFenceSyncAPPLE(GL_SYNC_GPU_COMMANDS_COMPLETE_APPLE, 0);
    glFlush();
    glClientWaitSyncAPPLE(sync, 0, GL_TIMEOUT_IGNORED_APPLE);
#else
    glFlush();
#endif
}

void ahvfx_gl_make_square_box(int w, int h, int *x, int *y, int *ww, int *hh) {
    int max = w > h ? w : h;
    int gap = abs(w - h) / 2;
    if (max == w) {
        *x = 0;
        *y = -gap;
    } else {
        *x = -gap;
        *y = 0;
    }
    *ww = max;
    *hh = max;
}

void ahvfx_UNPACKT_ALIGNMENT(uint32_t aligment) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, aligment);
}
