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
 * Date:
 * Author: Jesse Buhagiar
 */
#pragma once

#include <AK/Assertions.h>
#include "mat4.h"

#include <string.h>
#include <stdio.h>
#include <math.h>

#define UNUSED_VAR(x) (void)x

class Mat4;

class Vec4
{
public:
    Vec4() {}
    Vec4(float x, float y, float z, float w) : m_x(x), m_y(y), m_z(z), m_w(w) {}

    float length() const { return sqrt((m_x * m_x) + (m_y * m_y) + (m_z * m_z) + (m_w * m_w)); }

    void normalize()
    {
        float len = length();
        m_x /= len;
        m_y /= len;
        m_z /= len;
        m_w /= len;
    }

    // float angle() { return atan(m_y / m_x); }

    float x() const { return m_x; }
    float y() const { return m_y; }
    float z() const { return m_z; }
    float w() const { return m_w; }

    void x(float x) { m_x = x; }
    void y(float y) { m_y = y; }
    void z(float z) { m_z = z; }
    void w(float w) { m_w = w; }

    Vec4 operator+(const Vec4& rhs) { return Vec4(m_x + rhs.m_x, m_y + rhs.m_y, m_z + rhs.m_z, m_w + rhs.m_w); }
    Vec4 operator-(const Vec4& rhs) { return Vec4(m_x - rhs.m_x, m_y - rhs.m_y, m_z - rhs.m_z, m_w - rhs.m_w); }
    Vec4 operator*(const float scalar) { return Vec4(m_x * scalar, m_y * scalar, m_z * scalar, m_w * scalar); }
    Vec4 operator/(const float scalar) { return Vec4(m_x / scalar, m_y / scalar, m_z / scalar, m_w / scalar); }

    Vec4& operator+=(const Vec4& rhs)
    {
        m_x += rhs.m_x;
        m_y += rhs.m_y;
        m_z += rhs.m_z;
        m_w += rhs.m_w;
        return *this;
    }

    Vec4& operator-=(const Vec4& rhs)
    {
        m_x -= rhs.m_x;
        m_y -= rhs.m_y;
        m_z -= rhs.m_z;
        m_w -= rhs.m_w;
        return *this;
    }

    Vec4& operator*=(const float scalar)
    {
        m_x *= scalar;
        m_y *= scalar;
        m_z *= scalar;
        m_w *= scalar;
        return *this;
    }

    Vec4& operator/=(const float scalar)
    {
        VERIFY(scalar > 0.0f || scalar < 0.0f);
        m_x /= scalar;
        m_y /= scalar;
        m_z /= scalar;
        m_w /= scalar;
        return *this;
    }

    float dot(const Vec4& b) const { return m_x * b.m_x + m_y * b.m_y + m_z * b.m_z + m_w * b.m_w; }

    void print() const { printf("[%.4f %.4f %.4f %.4f]\n", m_x, m_y, m_z, m_w); }

private:
    float m_x = { 0 };
    float m_y = { 0 };
    float m_z = { 0 };
    float m_w = { 0 };
};
