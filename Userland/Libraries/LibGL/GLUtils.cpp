/*
 * Copyright (c) 2021, Jesse Buhagiar <jooster669@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "GL/gl.h"
#include "GLContext.h"

#include <AK/String.h>
#include <math.h>

// FIXME: Some of these should be fetched from the actual Graphics Card driver
static const String s_vendor = "The SerenityOS Developers";
static const String s_renderer = "SerenityOS OpenGL";
static const String s_version = "OpenGL 1.2 SerenityOS";

void glClear(GLbitfield mask)
{
    if (mask & GL_COLOR_BUFFER_BIT) {
        uint8_t r = static_cast<uint8_t>(floor(g_gl_state->clear_color.r * 255.0f));
        uint8_t g = static_cast<uint8_t>(floor(g_gl_state->clear_color.g * 255.0f));
        uint8_t b = static_cast<uint8_t>(floor(g_gl_state->clear_color.b * 255.0f));

        uint64_t color = r << 16 | g << 8 | b;
        UNUSED_VAR(color);
        //rush3d_register_write(BACK_COLOR_REGISTER, color);
        //rush3d_register_write(CONTROL_STATUS_REGISTER_WRITE, CLEAR_FRAMEBUFFER);
    } else {
        // set gl error here!?
    }
}

void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    g_gl_state->clear_color.r = red;
    g_gl_state->clear_color.g = green;
    g_gl_state->clear_color.b = blue;
    g_gl_state->clear_color.a = alpha;
}

GLubyte* glGetString(GLenum name)
{
    switch (name) {
    case GL_VENDOR:
        return reinterpret_cast<GLubyte*>(const_cast<char*>(s_vendor.characters()));
    case GL_RENDERER:
        return reinterpret_cast<GLubyte*>(const_cast<char*>(s_renderer.characters()));
    case GL_VERSION:
        return reinterpret_cast<GLubyte*>(const_cast<char*>(s_version.characters()));
    default:
#ifdef GL_DEBUG
        dbgln("glGetString(): Unknown enum name!");
#endif
        break;
    }

    // TODO: Set glError to GL_INVALID_ENUM here
    return 0;
}

void glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    UNUSED_VAR(x);
    UNUSED_VAR(y);
    UNUSED_VAR(width);
    UNUSED_VAR(height);
}
