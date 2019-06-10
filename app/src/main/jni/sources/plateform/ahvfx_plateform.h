
//
//  ahvfx_plateform.h
//  AHVideoFX
//
//  Created by longxiang on 2019/3/18.
//  Copyright © 2019 AH. All rights reserved.
//

#ifndef ahvfx_plateform_h
#define ahvfx_plateform_h

#ifdef __APPLE__
#   include <OpenGLES/ES2/gl.h>
#   include <OpenGLES/ES2/glext.h>
#else
//wlz
//#   include <GLES2/gl2.h>
//#   include <GLES2/gl2ext.h>

#   include <GLES3/gl3.h>
#   include <GLES3/gl3ext.h>
#endif


#endif /* ahvfx_plateform_h */
