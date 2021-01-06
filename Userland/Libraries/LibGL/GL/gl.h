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
#pragma once

#define GL_DEBUG

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GLAPI
    #define GLAPI extern
#endif

// OpenGL related `defines`
#define GL_TRUE  1
#define GL_FALSE 0

// Matrix Modes
#define GL_MODELVIEW  0x0050
#define GL_PROJECTION 0x0051

// glBegin/glEnd primitive types
#define GL_TRIANGLES      0x0100
#define GL_QUADS          0x0101
#define GL_TRIANGLE_FAN   0x0102
#define GL_TRIANGLE_STRIP 0x0103
#define GL_POLYGON        0x0104

// Buffer bits
#define GL_COLOR_BUFFER_BIT 0x0200

// Utility
#define GL_VENDOR   0x1F00
#define GL_RENDERER 0x1F01
#define GL_VERSION  0x1F02

//
// OpenGL typedefs
//
// Defines types used by all OpenGL applications
// https://www.khronos.org/opengl/wiki/OpenGL_Type
typedef char GLchar;
typedef unsigned char GLuchar;
typedef unsigned char GLubyte;
typedef short GLshort;
typedef unsigned short GLushort;
typedef int GLint;
typedef unsigned int GLuint;
typedef int GLfixed;
typedef long long GLint64;
typedef unsigned long long GLuint64;
typedef unsigned long GLsizei;
typedef void GLvoid;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef unsigned int GLenum;
typedef unsigned int GLbitfield;

GLAPI void glBegin(GLenum mode);
GLAPI void glClear(GLbitfield mask);
GLAPI void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
GLAPI void glColor3f(GLfloat r, GLfloat g, GLfloat b);
GLAPI void glEnd();
GLAPI void glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble nearVal, GLdouble farVal);
GLAPI GLubyte* glGetString(GLenum name);
GLAPI void glLoadIdentity();
GLAPI void glMatrixMode(GLenum mode);
GLAPI void glPushMatrix();
GLAPI void glPopMatrix();
GLAPI void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
GLAPI void glTranslatef(GLfloat x, GLfloat y, GLfloat z);
GLAPI void glVertex3f(GLfloat x, GLfloat y, GLfloat z);
GLAPI void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

#ifdef __cplusplus
}
#endif
