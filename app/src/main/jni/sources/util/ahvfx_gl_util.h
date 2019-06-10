//
//  ahvfx_gl_util.h
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/22.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_gl_util_h
#define ahvfx_gl_util_h

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
    void ahvfx_GPU_COMMANDS_SYNC(void);
    void ahvfx_UNPACKT_ALIGNMENT(uint32_t aligment);
    void ahvfx_ENABLE_ATTRIB(uint32_t attrib_idx, int32_t sz, uint32_t data_type, int32_t normalized, int32_t stride, const void *ptr);
#ifdef __cplusplus
}
#endif


#endif /* ahvfx_gl_util_h */
