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
#include <AK/Assertions.h>
#include <AK/Format.h>

using AK::dbgln;

void glMatrixMode(GLenum mode)
{
    VERIFY(mode == GL_MODELVIEW || mode == GL_PROJECTION);
    g_gl_state->curr_matrix_mode = mode;
}

/*
 * Push the current matrix (based on the current matrix mode)
 * to its' corresponding matrix stack in the current OpenGL
 * state context
 */
void glPushMatrix()
{
#ifdef GL_DEBUG
    dbgln("glPushMatrix(): Pushing matrix to the matrix stack (matrix_mode {})", g_gl_state->curr_matrix_mode);
#endif

    switch (g_gl_state->curr_matrix_mode) {
    case GL_PROJECTION:
        g_gl_state->projection_matrix_stack.append(g_gl_state->projection_matrix);
        break;
    case GL_MODELVIEW:
        g_gl_state->model_view_matrix_stack.append(g_gl_state->model_view_matrix);
        break;
    default:
        dbgln("glPushMatrix(): Attempt to push matrix with invalid matrix mode {})", g_gl_state->curr_matrix_mode);
        break;
    }
}

/*
 * Pop a matrix from the corresponding matrix stack into the
 * corresponding matrix in the state based on the current
 * matrix mode
 */
void glPopMatrix()
{
#ifdef GL_DEBUG
    dbgln("glPopMatrix(): Popping matrix from matrix stack (matrix_mode = {})", g_gl_state->curr_matrix_mode);
#endif

    // TODO: Make sure stack::top() doesn't cause any  nasty issues if it's empty (that could result in a lockup/hang)
    switch (g_gl_state->curr_matrix_mode) {
    case GL_PROJECTION:
        g_gl_state->projection_matrix = g_gl_state->projection_matrix_stack.take_last();
        ;

        break;
    case GL_MODELVIEW:
        g_gl_state->model_view_matrix = g_gl_state->model_view_matrix_stack.take_last();
        ;
        break;
    default:
        dbgln("glPopMatrix(): Attempt to pop matrix with invalid matrix mode, {}", g_gl_state->curr_matrix_mode);
        break;
    }
}

void glLoadIdentity()
{
    if (g_gl_state->curr_matrix_mode == GL_PROJECTION)
        g_gl_state->projection_matrix = FloatMatrix4x4::identity();
    else if (g_gl_state->curr_matrix_mode == GL_MODELVIEW)
        g_gl_state->model_view_matrix = FloatMatrix4x4::identity();
    else
        VERIFY_NOT_REACHED();
}

/**
 * Create a viewing frustum (a.k.a a "Perspective Matrix") in the current matrix. This
 * is usually done to the projection matrix. The current matrix is then multiplied
 * by this viewing frustum matrix.
 *
 * https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glFrustum.xml
 *
 *
 * TODO: We need to check for some values that could result in a division by zero
 * or some other fuckery that will fuck with the driver here!
 */
void glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble nearVal, GLdouble farVal)
{
    float a;
    float b;
    float c;
    float d;

    // Let's do some math!
    a = ((float)right + (float)left) / ((float)right - (float)left);
    b = ((float)top + (float)bottom) / ((float)top - (float)bottom);
    c = -(((float)farVal + (float)nearVal) / ((float)farVal - (float)nearVal));
    d = -((2 * ((float)farVal * (float)nearVal)) / ((float)farVal - (float)nearVal));

    FloatMatrix4x4 frustum {
        ((2 * (float)nearVal) / ((float)right - (float)left)), 0, a, 0,
        0, ((2 * (float)nearVal) / ((float)top - (float)bottom)), b, 0,
        0, 0, c, d,
        0, 0, -1, 0
    };

    if (g_gl_state->curr_matrix_mode == GL_PROJECTION) {
        g_gl_state->projection_matrix = g_gl_state->projection_matrix * frustum;
    } else if (g_gl_state->curr_matrix_mode == GL_MODELVIEW) {
#ifdef GL_DEBUG
        dbgln("glFrustum(): frustum created with curr_matrix_mode == GL_MODELVIEW!!!");
#endif
        g_gl_state->projection_matrix = g_gl_state->model_view_matrix * frustum;
    }
}
