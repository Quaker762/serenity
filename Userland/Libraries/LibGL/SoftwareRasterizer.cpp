/*
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@gmx.de>
 * 
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "SoftwareRasterizer.h"
#include <AK/Function.h>
#include <AK/SIMD.h>
#include <LibGfx/Painter.h>

static constexpr size_t RASTERIZER_BLOCK_SIZE = 16;

using AK::SIMD::f32x4;

struct FloatVector2 {
    float x;
    float y;
};

constexpr static float triangle_area(const FloatVector2& a, const FloatVector2& b, const FloatVector2& c)
{
    return ((c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x)) / 2;
}

constexpr static Gfx::RGBA32 to_rgba32(f32x4 v)
{
    u8 r = clamp(v[0], 0.0f, 1.0f) * 255;
    u8 g = clamp(v[1], 0.0f, 1.0f) * 255;
    u8 b = clamp(v[2], 0.0f, 1.0f) * 255;
    u8 a = clamp(v[3], 0.0f, 1.0f) * 255;
    return a << 24 | b << 16 | g << 8 | r;
}

template<typename PS>
static void rasterize_triangle(Gfx::Bitmap& render_target, const GLTriangle& triangle, PS pixel_shader)
{
    // Since the algorithm is based on blocks of uniform size, we need
    // to ensure that our render_target size is actually a multiple of the block size
    VERIFY((render_target.width() % RASTERIZER_BLOCK_SIZE) == 0);
    VERIFY((render_target.height() % RASTERIZER_BLOCK_SIZE) == 0);

    // Calculate area of the triangle for later tests
    FloatVector2 v0 = { triangle.vertices[0].x, triangle.vertices[0].y };
    FloatVector2 v1 = { triangle.vertices[1].x, triangle.vertices[1].y };
    FloatVector2 v2 = { triangle.vertices[2].x, triangle.vertices[2].y };

    float area = triangle_area(v0, v1, v2);
    if (area == 0)
        return;

    float oneOverArea = 1 / area;

    // Obey top-left rule:
    // This sets up "zero" for later pixel coverage tests.
    // Depending on where on the triangle the edge is located
    // it is either tested against 0 or float epsilon, effectively
    // turning "< 0" into "<= 0"
    float constexpr epsilon = AK::NumericLimits<float>::epsilon();
    f32x4 zero { epsilon, epsilon, epsilon, 0.0f };
    if (v1.y > v0.y || (v1.y == v0.y && v1.x < v0.x))
        zero[2] = 0;
    if (v2.y > v1.y || (v2.y == v1.y && v2.x < v1.x))
        zero[0] = 0;
    if (v0.y > v2.y || (v0.y == v2.y && v0.x < v2.x))
        zero[1] = 0;

    // This function calculates the barycentric coordinates for the pixel relative to the triangle.
    auto barycentric_coordinates = [v0, v1, v2, oneOverArea](float x, float y) -> f32x4 {
        FloatVector2 p { x, y };
        return f32x4 {
            triangle_area(v1, v2, p) * oneOverArea,
            triangle_area(v2, v0, p) * oneOverArea,
            triangle_area(v0, v1, p) * oneOverArea,
            0.0f
        };
    };

    // This function tests whether a point lies within the triangle
    auto test_point = [zero](f32x4 point) -> bool {
        return point[0] >= zero[0]
            && point[1] >= zero[1]
            && point[2] >= zero[2];
    };

    // Calculate bounds
    FloatVector2 min { v0 };
    FloatVector2 max { v0 };
    if (v1.x > max.x)
        max.x = v1.x;
    if (v2.x > max.x)
        max.x = v2.x;
    if (v1.y > max.y)
        max.y = v1.y;
    if (v2.y > max.y)
        max.y = v2.y;
    if (v1.x < min.x)
        min.x = v1.x;
    if (v2.x < min.x)
        min.x = v2.x;
    if (v1.y < min.y)
        min.y = v1.y;
    if (v2.y < min.y)
        min.y = v2.y;

    // Calculate block-based bounds
    int iminx = floorf(min.x);
    int iminy = floorf(min.y);
    int imaxx = ceilf(max.x);
    int imaxy = ceilf(max.y);

    iminx = clamp(iminx, 0, render_target.width() - 1);
    imaxx = clamp(imaxx, 0, render_target.width() - 1);
    iminy = clamp(iminy, 0, render_target.height() - 1);
    imaxy = clamp(imaxy, 0, render_target.height() - 1);

    int bx0 = iminx / RASTERIZER_BLOCK_SIZE;
    int bx1 = imaxx / RASTERIZER_BLOCK_SIZE + 1;
    int by0 = iminy / RASTERIZER_BLOCK_SIZE;
    int by1 = imaxy / RASTERIZER_BLOCK_SIZE + 1;

    // Iterate over all blocks within the bounds of the triangle
    for (int by = by0; by < by1; by++) {
        for (int bx = bx0; bx < bx1; bx++) {

            // The 4 block corners
            int x0 = bx * RASTERIZER_BLOCK_SIZE;
            int y0 = by * RASTERIZER_BLOCK_SIZE;
            int x1 = bx * RASTERIZER_BLOCK_SIZE + RASTERIZER_BLOCK_SIZE;
            int y1 = by * RASTERIZER_BLOCK_SIZE + RASTERIZER_BLOCK_SIZE;

            // Barycentric coordinates of the 4 block corners
            auto a = barycentric_coordinates(x0, y0);
            auto b = barycentric_coordinates(x1, y0);
            auto c = barycentric_coordinates(x0, y1);
            auto d = barycentric_coordinates(x1, y1);

            // If the whole block is outside any of the triangle edges we can discard it completely
            if ((a[0] < zero[0] && b[0] < zero[0] && c[0] < zero[0] && d[0] < zero[0])
                || (a[1] < zero[1] && b[1] < zero[1] && c[1] < zero[1] && d[1] < zero[1])
                || (a[2] < zero[2] && b[2] < zero[2] && c[2] < zero[2] && d[2] < zero[2]))
                continue;

            // barycentric coordinate derrivatives
            auto dcdx = (b - a) / RASTERIZER_BLOCK_SIZE;
            auto dcdy = (c - a) / RASTERIZER_BLOCK_SIZE;

            if (test_point(a) && test_point(b) && test_point(c) && test_point(d)) {
                // The block is fully contained within the triangle
                // Fill the block without further coverage tests
                for (int y = y0; y < y1; y++) {
                    auto coords = a;
                    auto* pixels = &render_target.scanline(y)[x0];
                    for (int x = x0; x < x1; x++) {
                        *pixels++ = to_rgba32(pixel_shader(coords, triangle));
                        coords = coords + dcdx;
                    }
                    a = a + dcdy;
                }
            } else {
                // The block overlaps at least one triangle edge
                // We need to test coverage of every pixel within the block
                for (int y = y0; y < y1; y++) {
                    auto coords = a;
                    auto* pixels = &render_target.scanline(y)[x0];
                    for (int x = x0; x < x1; x++) {
                        if (test_point(coords)) {
                            *pixels = to_rgba32(pixel_shader(coords, triangle));
                        }
                        pixels++;
                        coords = coords + dcdx;
                    }
                    a = a + dcdy;
                }
            }
        }
    }
}

static Gfx::IntSize closest_multiple(const Gfx::IntSize& min_size, size_t step)
{
    int width = ((min_size.width() + step - 1) / step) * step;
    int height = ((min_size.height() + step - 1) / step) * step;
    return { width, height };
}

SoftwareRasterizer::SoftwareRasterizer(const Gfx::IntSize& min_size)
    : m_render_target { Gfx::Bitmap::create(Gfx::BitmapFormat::BGRA8888, closest_multiple(min_size, RASTERIZER_BLOCK_SIZE)) }
{
}

void SoftwareRasterizer::submit_triangle(const GLTriangle& triangle)
{
    if (m_options.shade_smooth) {
        rasterize_triangle(*m_render_target, triangle, [](f32x4 v, const GLTriangle& t) -> f32x4 {
            const float r = t.vertices[0].r * v[0] + t.vertices[1].r * v[1] + t.vertices[2].r * v[2];
            const float g = t.vertices[0].g * v[0] + t.vertices[1].g * v[1] + t.vertices[2].g * v[2];
            const float b = t.vertices[0].b * v[0] + t.vertices[1].b * v[1] + t.vertices[2].b * v[2];
            const float a = t.vertices[0].a * v[0] + t.vertices[1].a * v[1] + t.vertices[2].a * v[2];
            return f32x4 { r, g, b, a };
        });
    } else {
        rasterize_triangle(*m_render_target, triangle, [](f32x4, const GLTriangle& t) -> f32x4 {
            return f32x4 { t.vertices[0].r, t.vertices[0].g, t.vertices[0].b, t.vertices[0].a };
        });
    }
}

void SoftwareRasterizer::resize(const Gfx::IntSize& min_size)
{
    wait_for_all_threads();

    m_render_target = Gfx::Bitmap::create(Gfx::BitmapFormat::BGRA8888, closest_multiple(min_size, RASTERIZER_BLOCK_SIZE));
}

void SoftwareRasterizer::clear_color(const FloatVector4& color)
{
    wait_for_all_threads();

    uint8_t r = static_cast<uint8_t>(clamp(color.x(), 0.0f, 1.0f) * 255);
    uint8_t g = static_cast<uint8_t>(clamp(color.y(), 0.0f, 1.0f) * 255);
    uint8_t b = static_cast<uint8_t>(clamp(color.z(), 0.0f, 1.0f) * 255);
    uint8_t a = static_cast<uint8_t>(clamp(color.w(), 0.0f, 1.0f) * 255);

    m_render_target->fill(Gfx::Color(r, g, b, a));
}

void SoftwareRasterizer::clear_depth(float)
{
    wait_for_all_threads();

    // FIXME: implement this
}

void SoftwareRasterizer::blit_to(Gfx::Bitmap& target)
{
    wait_for_all_threads();

    Gfx::Painter painter { target };
    painter.blit({ 0, 0 }, *m_render_target, m_render_target->rect(), 1.0f, false);
}

void SoftwareRasterizer::wait_for_all_threads() const
{
    // FIXME: Wait for all render threads to finish when multithreading is being implemented
}

void SoftwareRasterizer::set_options(const RasterizerOptions& options)
{
    wait_for_all_threads();

    m_options = options;

    // FIXME: Recreate or reinitialize render threads here when multithreading is being implemented
}
