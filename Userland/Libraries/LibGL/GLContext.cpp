/*
 * Copyright (c) 2021, Jesse Buhagiar <jooster669@gmail.com>
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@gmx.de>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "SoftwareGLContext.h"
#include <LibGfx/Bitmap.h>

__attribute__((visibility("hidden"))) GLContext* g_gl_context;

GLContext::~GLContext()
{
    if (g_gl_context == this)
        gl_make_context_current(nullptr);
}

OwnPtr<GLContext> gl_create_context(Gfx::Bitmap& bitmap)
{
    auto context = adopt_own(*new SoftwareGLContext(bitmap));

    if (!g_gl_context)
        g_gl_context = context;

    return context;
}

void gl_make_context_current(GLContext* context)
{
    g_gl_context = context;
}
