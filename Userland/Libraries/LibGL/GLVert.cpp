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
#include "math/vec4.h"

#include <AK/QuickSort.h>
#include <AK/Vector.h>
#include <math.h>

#define NUM_CLIP_PLANES 6

// static std::list<R3D_Triangle> triangle_list;
static Vector<GLVertex> vertex_list;
static Vector<GLTriangle> triangle_list;
static Vector<GLTriangle> processed_triangles;

static Vec4 clip_planes[] = {
    { -1, 0, 0, 1 }, // Left Plane
    { 1, 0, 0, 1 },  // Right Plane
    { 0, 1, 0, 1 },  // Top Plane
    { 0, -1, 0, 1 }, // Bottom plane
    { 0, 0, 1, 1 },  // Near Plane
    { 0, 0, -1, 1 }  // Far Plane
};

static Vec4 clip_plane_normals[] = {
    { 1, 0, 0, 1 },  // Left Plane
    { -1, 0, 0, 1 }, // Right Plane
    { 0, -1, 0, 1 }, // Top Plane
    { 0, 1, 0, 1 },  // Bottom plane
    { 0, 0, -1, 1 }, // Near Plane
    { 0, 0, 1, 1 }   // Far Plane
};

enum ClippingPlane {
    LEFT = 0,
    RIGHT = 1,
    TOP = 2,
    BOTTOM = 3,
    NEAR = 4,
    FAR = 5
};

void glBegin(GLenum mode)
{
    g_gl_state->curr_draw_mode = mode;
}

// TODO: Change this to a vertex!
// Determines whether or not a vertex is inside the frustum for a given plane
static bool vert_inside_plane(const Vec4& vec, ClippingPlane plane)
{
    switch (plane) {
    case ClippingPlane::LEFT:
        return vec.x() > -vec.w();
    case ClippingPlane::RIGHT:
        return vec.x() < vec.w();
    case ClippingPlane::TOP:
        return vec.y() < vec.w();
    case ClippingPlane::BOTTOM:
        return vec.y() > -vec.w();
    case ClippingPlane::NEAR:
        return vec.z() > -vec.w();
    case ClippingPlane::FAR:
        return vec.z() < vec.w();
    }

    return false;
}

// TODO: This needs to interpolate color/UV data as well!
static Vec4 clip_intersection_point(const Vec4& vec, const Vec4& prev_vec, ClippingPlane plane_index)
{
    // https://github.com/fogleman/fauxgl/blob/master/clipping.go#L20
    // How the fuck does this work??????
    Vec4 u, w;
    Vec4 ret = prev_vec;
    Vec4 plane = clip_planes[plane_index];
    Vec4 plane_normal = clip_plane_normals[plane_index];

    u = vec;
    u -= prev_vec;
    w = prev_vec;
    w -= plane;
    float d = plane_normal.dot(u);
    float n = -plane_normal.dot(w);

    ret += (u * (n / d));
    return ret;
}

// https://groups.csail.mit.edu/graphics/classes/6.837/F04/lectures/07_Pipeline_II.pdf
// This is a really rough implementation of the Sutherland-Hodgman algorithm in clip-space
static void clip_triangle_against_frustum(Vector<Vec4>& in_vec)
{
    Vector<Vec4> clipped_polygon = in_vec;    // in_vec = subjectPolygon, clipped_polygon = outputList
    for (int i = 0; i < NUM_CLIP_PLANES; i++) // Test against each clip plane
    {
        ClippingPlane plane = static_cast<ClippingPlane>(i); // Hahaha, what the fuck
        in_vec = clipped_polygon;
        clipped_polygon.clear();

        // Prevent a crash from .at() undeflow
        if (in_vec.size() == 0)
            return;

        Vec4 prev_vec = in_vec.at(in_vec.size() - 1);

        for (size_t j = 0; j < in_vec.size(); j++) // Perform this for each vertex
        {
            const Vec4& vec = in_vec.at(j);
            if (vert_inside_plane(vec, plane)) {
                if (!vert_inside_plane(prev_vec, plane)) {
                    Vec4 intersect = clip_intersection_point(prev_vec, vec, plane);
                    clipped_polygon.append(intersect);
                }

                clipped_polygon.append(vec);
            } else if (vert_inside_plane(prev_vec, plane)) {
                Vec4 intersect = clip_intersection_point(prev_vec, vec, plane);
                clipped_polygon.append(intersect);
            }

            prev_vec = vec;
        }
    }
}

void glEnd()
{
    // At this point, the user has effectively specified that they are done with defining the geometry
    // of what they want to draw. We now need to do a few things (https://www.khronos.org/opengl/wiki/Rendering_Pipeline_Overview):
    //
    // 1.   Transform all of the vertices in the current vertex list into eye space by mulitplying the model-view matrix
    // 2.   Transform all of the vertices from eye space into clip space by multiplying by the projection matrix
    // 3.   If culling is enabled, we cull the desired faces (https://learnopengl.com/Advanced-OpenGL/Face-culling)
    // 4.   Each element of the vertex is then divided by w to bring the positions into NDC (Normalized Device Coordinates)
    // 5.   The vertices are sorted (for the rasteriser, how are we doing this? 3Dfx did this top to bottom in terms of vertex y co-ordinates)
    // 6.   The vertices are then sent off to the rasteriser and drawn to the screen

    // FIXME: Don't assume screen dimensions
    float scr_width = 640.0f;
    float scr_height = 480.0f;

    // Let's construct some triangles
    if (g_gl_state->curr_draw_mode == GL_TRIANGLES) {
        GLTriangle triangle;
        for (size_t i = 0; i < vertex_list.size(); i += 3) {
            triangle.vertices[0] = vertex_list.at(i);
            triangle.vertices[1] = vertex_list.at(i + 1);
            triangle.vertices[2] = vertex_list.at(i + 2);

            triangle_list.append(triangle);
        }
    } else if (g_gl_state->curr_draw_mode == GL_QUADS) {
        // We need to construct two triangles to form the quad
        GLTriangle triangle;
        VERIFY(vertex_list.size() % 4 == 0);
        for (size_t i = 0; i < vertex_list.size(); i += 4) {
            // Triangle 1
            triangle.vertices[0] = vertex_list.at(i);
            triangle.vertices[1] = vertex_list.at(i + 1);
            triangle.vertices[2] = vertex_list.at(i + 2);
            triangle_list.append(triangle);

            // Triangle 2
            triangle.vertices[0] = vertex_list.at(i + 2);
            triangle.vertices[1] = vertex_list.at(i + 3);
            triangle.vertices[2] = vertex_list.at(i);
            triangle_list.append(triangle);
        }
    } else if (g_gl_state->curr_draw_mode == GL_TRIANGLE_FAN) {
        GLTriangle triangle;
        triangle.vertices[0] = vertex_list.at(0); // Root vertex is always the vertex defined first

        for (size_t i = 1; i < vertex_list.size() - 1; i++) // This is technically `n-2` triangles. We start at index 1
        {
            triangle.vertices[1] = vertex_list.at(i);
            triangle.vertices[2] = vertex_list.at(i + 1);
            triangle_list.append(triangle);
        }
    } else if (g_gl_state->curr_draw_mode == GL_TRIANGLE_STRIP) {
        GLTriangle triangle;
        for (size_t i = 0; i < vertex_list.size() - 2; i++) {
            triangle.vertices[0] = vertex_list.at(i);
            triangle.vertices[1] = vertex_list.at(i + 1);
            triangle.vertices[2] = vertex_list.at(i + 2);
            triangle_list.append(triangle);
        }
    } else {
        VERIFY_NOT_REACHED();
    }

    // Now let's transform each triangle and send that to the GPU
    for (size_t i = 0; i < triangle_list.size(); i++) {
        GLTriangle& triangle = triangle_list.at(i);
        GLVertex& vertexa = triangle.vertices[0];
        GLVertex& vertexb = triangle.vertices[1];
        GLVertex& vertexc = triangle.vertices[2];

        Vec4 veca({ vertexa.x, vertexa.y, vertexa.z, 1.0f });
        Vec4 vecb({ vertexb.x, vertexb.y, vertexb.z, 1.0f });
        Vec4 vecc({ vertexc.x, vertexc.y, vertexc.z, 1.0f });

        // First multiply the vertex by the MODELVIEW matrix and then the PROJECTION matrix
        veca = g_gl_state->model_view_matrix * veca;
        veca = g_gl_state->projection_matrix * veca;

        vecb = g_gl_state->model_view_matrix * vecb;
        vecb = g_gl_state->projection_matrix * vecb;

        vecc = g_gl_state->model_view_matrix * vecc;
        vecc = g_gl_state->projection_matrix * vecc;

        // At this point, we're in clip space
        // Here's where we do the clipping. This is a really crude implementation of the
        // https://learnopengl.com/Getting-started/Coordinate-Systems
        // "Note that if only a part of a primitive e.g. a triangle is outside the clipping volume OpenGL
        // will reconstruct the triangle as one or more triangles to fit inside the clipping range. "
        //
        // ALL VERTICES ARE DEFINED IN A CLOCKWISE ORDER

        // Okay, let's do some face culling first

        Vector<Vec4> vecs;
        Vector<GLVertex> verts;

        vecs.append(veca);
        vecs.append(vecb);
        vecs.append(vecc);
        clip_triangle_against_frustum(vecs);

        // TODO: Copy color and UV information too!
        for (size_t vec_idx = 0; vec_idx < vecs.size(); vec_idx++) {
            Vec4& vec = vecs.at(vec_idx);
            GLVertex vertex;

            // Perform the perspective divide
            if (vec.w() != 0.0f) {
                vec.x(vec.x() / vec.w());
                vec.y(vec.y() / vec.w());
                vec.z(vec.z() / vec.w());
            }

            vertex.x = vec.x();
            vertex.y = vec.y();
            vertex.z = vec.z();
            vertex.w = vec.w();

            // FIXME: This is to suppress any -Wunused errors
            vertex.u = 0.0f;
            vertex.v = 0.0f;

            if (vec_idx == 0) {
                vertex.r = vertexa.r;
                vertex.g = vertexa.g;
                vertex.b = vertexa.b;
            } else if (vec_idx == 1) {
                vertex.r = vertexb.r;
                vertex.g = vertexb.g;
                vertex.b = vertexb.b;
            } else {
                vertex.r = vertexc.r;
                vertex.g = vertexc.g;
                vertex.b = vertexc.b;
            }

            vertex.x = (vec.x() + 1.0f) * (scr_width / 2.0f) + 0.0f; // TODO: 0.0f should be something!?
            vertex.y = scr_height - ((vec.y() + 1.0f) * (scr_height / 2.0f) + 0.0f);
            vertex.z = vec.z();
            verts.append(vertex);
        }

        if (verts.size() == 0) {
            continue;
        } else if (verts.size() == 3) {
            GLTriangle tri;

            tri.vertices[0] = verts.at(0);
            tri.vertices[1] = verts.at(1);
            tri.vertices[2] = verts.at(2);
            processed_triangles.append(tri);
        } else if (verts.size() == 4) {
            GLTriangle tri1;
            GLTriangle tri2;

            tri1.vertices[0] = verts.at(0);
            tri1.vertices[1] = verts.at(1);
            tri1.vertices[2] = verts.at(2);
            processed_triangles.append(tri1);

            tri2.vertices[0] = verts.at(0);
            tri2.vertices[1] = verts.at(2);
            tri2.vertices[2] = verts.at(3);
            processed_triangles.append(tri2);
        }
    }

    for (size_t i = 0; i < processed_triangles.size(); i++) {
        Vector<GLVertex> sort_vert_list;
        GLTriangle& triangle = processed_triangles.at(i);

        // Now we sort the vertices by their y values. A is the vertex that has the least y value,
        // B is the middle and C is the bottom.
        // These are sorted in groups of 3
        sort_vert_list.append(triangle.vertices[0]);
        sort_vert_list.append(triangle.vertices[1]);
        sort_vert_list.append(triangle.vertices[2]);

        AK::quick_sort(sort_vert_list.begin(), sort_vert_list.end(), [](auto& a, auto& b) { return a.y < b.y; });

        triangle.vertices[0] = sort_vert_list.at(0);
        triangle.vertices[1] = sort_vert_list.at(1);
        triangle.vertices[2] = sort_vert_list.at(2);

        // Let's calculate the (signed) area of the triangle
        // https://cp-algorithms.com/geometry/oriented-triangle-area.html
        float dxAB = triangle.vertices[0].x - triangle.vertices[1].x; // A.x - B.x
        float dxBC = triangle.vertices[1].x - triangle.vertices[2].x; // B.X - C.x
        float dyAB = triangle.vertices[0].y - triangle.vertices[1].y;
        float dyBC = triangle.vertices[1].y - triangle.vertices[2].y;
        float area = (dxAB * dyBC) - (dxBC * dyAB);

        if (area == 0.0f)
            continue;

        int32_t vertexAx = triangle.vertices[0].x;
        int32_t vertexAy = triangle.vertices[0].y;
        int32_t vertexBx = triangle.vertices[1].x;
        int32_t vertexBy = triangle.vertices[1].y;
        int32_t vertexCx = triangle.vertices[2].x;
        int32_t vertexCy = triangle.vertices[2].y;
        UNUSED_VAR(vertexAx);
        UNUSED_VAR(vertexAy);
        UNUSED_VAR(vertexBx);
        UNUSED_VAR(vertexBy);
        UNUSED_VAR(vertexCx);
        UNUSED_VAR(vertexCy);
    }

    // We probably need to wait for the card to finish drawing here before we clear
    triangle_list.clear();
    processed_triangles.clear();
    vertex_list.clear();
}

void glVertex3f(GLfloat x, GLfloat y, GLfloat z)
{
    GLVertex vertex;

    vertex.x = x;
    vertex.y = y;
    vertex.z = z;
    vertex.r = g_gl_state->curr_vertex_color.r;
    vertex.g = g_gl_state->curr_vertex_color.g;
    vertex.b = g_gl_state->curr_vertex_color.b;

    // FIXME: This is to suppress any -Wunused errors
    vertex.w = 0.0f;
    vertex.u = 0.0f;
    vertex.v = 0.0f;

    vertex_list.append(vertex);
}

void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    Vec4 vec = { x, y, z, 0 };

    Mat4 rotation_mat;
    float cosangle = cos(angle * (M_PI / 180));
    float sinangle = sin(angle * (M_PI / 180));
    float one_minus_cosangle = 1 - cosangle;

    if (vec.length() > 1.0f)
        vec.normalize();

    // Here we go...
    // https://learnopengl.com/Getting-started/Transformations
    // https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glRotate.xml
    rotation_mat(0, 0, ((vec.x() * vec.x()) * one_minus_cosangle) + cosangle);
    rotation_mat(0, 1, ((vec.y() * vec.x()) * one_minus_cosangle) + (vec.z() * sinangle));
    rotation_mat(0, 2, ((vec.x() * vec.z()) * one_minus_cosangle) - (vec.y() * sinangle));
    rotation_mat(0, 3, 0.0f);

    rotation_mat(1, 0, ((vec.x() * vec.y()) * one_minus_cosangle) - (vec.z() * sinangle));
    rotation_mat(1, 1, ((vec.y() * vec.y()) * one_minus_cosangle) + cosangle);
    rotation_mat(1, 2, ((vec.y() * vec.z()) * one_minus_cosangle) + (vec.x() * sinangle));
    rotation_mat(1, 3, 0.0f);

    rotation_mat(2, 0, ((vec.x() * vec.z()) * one_minus_cosangle) + (vec.y() * sinangle));
    rotation_mat(2, 1, ((vec.y() * vec.z()) * one_minus_cosangle) - (vec.x() * sinangle));
    rotation_mat(2, 2, ((vec.z() * vec.z()) * one_minus_cosangle) + cosangle);
    rotation_mat(2, 3, 0.0f);

    rotation_mat(3, 0, 0.0f);
    rotation_mat(3, 1, 0.0f);
    rotation_mat(3, 2, 0.0f);
    rotation_mat(3, 3, 1.0f);

    // Phew...
    if (g_gl_state->curr_matrix_mode == GL_MODELVIEW) {
        g_gl_state->model_view_matrix = g_gl_state->model_view_matrix * rotation_mat;
    } else if (g_gl_state->curr_matrix_mode == GL_PROJECTION) {
        g_gl_state->projection_matrix = g_gl_state->projection_matrix * rotation_mat;
    }
}

void glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
    Mat4 translation_mat;

    translation_mat(0, 0, 1.0f);
    translation_mat(0, 1, 0.0f);
    translation_mat(0, 2, 0.0f);
    translation_mat(0, 3, 0.0f);

    translation_mat(1, 0, 0.0f);
    translation_mat(1, 1, 1.0f);
    translation_mat(1, 2, 0.0f);
    translation_mat(1, 3, 0.0f);

    translation_mat(2, 0, 0.0f);
    translation_mat(2, 1, 0.0f);
    translation_mat(2, 2, 1.0f);
    translation_mat(2, 3, 0.0f);

    translation_mat(3, 0, x);
    translation_mat(3, 1, y);
    translation_mat(3, 2, z);
    translation_mat(3, 3, 1.0f);

    // Phew...
    if (g_gl_state->curr_matrix_mode == GL_MODELVIEW) {
        g_gl_state->model_view_matrix = g_gl_state->model_view_matrix * translation_mat;
    } else if (g_gl_state->curr_matrix_mode == GL_PROJECTION) {
        g_gl_state->projection_matrix = g_gl_state->projection_matrix * translation_mat;
    }
}
