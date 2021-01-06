/**
 * This file is part of Rush3D.
 * Copyright (c) 2020 Jack Allan, Matthew Atkin and Jesse Buhagiar
 *
 * Rush3D is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Rush3D is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Rush3D.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Author: Jesse Buhagiar
 * Date: 4/4/2020
 */
#include "GL/gl.h"
#include "GLContext.h"

#include <cmath>
#include <AK/String.h>

// FIXME: Some of these should be fetched from the actual Graphics Card driver
static const String s_vendor = "The SerenityOS Developers";
static const String s_renderer = "SerenityOS OpenGL";
static const String s_version = "OpenGL 1.2 SerenityOS";

void glClear(GLbitfield mask)
{
    if(mask & GL_COLOR_BUFFER_BIT)
    {
        uint8_t r = static_cast<uint8_t>(floor(g_gl_state->clear_color.r * 255.0f));
        uint8_t g = static_cast<uint8_t>(floor(g_gl_state->clear_color.g * 255.0f));
        uint8_t b = static_cast<uint8_t>(floor(g_gl_state->clear_color.b * 255.0f));

        uint64_t color = r << 16 | g << 8 | b;
        UNUSED_VAR(color);
        //rush3d_register_write(BACK_COLOR_REGISTER, color);
        //rush3d_register_write(CONTROL_STATUS_REGISTER_WRITE, CLEAR_FRAMEBUFFER);
    }
    else
    {
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
    switch(name)
    {
    case GL_VENDOR:
        return reinterpret_cast<GLubyte*>(const_cast<char*>(s_vendor.characters()));
    case GL_RENDERER:
        return reinterpret_cast<GLubyte*>(const_cast<char*>(s_renderer.characters()));
    case GL_VERSION:
        return reinterpret_cast<GLubyte*>(const_cast<char*>(s_version.characters()));
    default:
#ifdef GL_DEBUG
        dbg() << "glGetString(): Unknown enum name!\n";
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
