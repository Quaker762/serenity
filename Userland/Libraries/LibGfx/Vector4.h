/*
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@gmx.de>
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

#include <math.h>

namespace Gfx {
template<typename T>
class Vector4 final {
public:
    Vector4() = default;
    Vector4(T x, T y, T z, T w)
        : m_x(x)
        , m_y(y)
        , m_z(z)
        , m_w(w)
    {
    }

    T x() const { return m_x; }
    T y() const { return m_y; }
    T z() const { return m_z; }
    T w() const { return m_w; }

    void set_x(T value) { m_x = value; }
    void set_y(T value) { m_y = value; }
    void set_z(T value) { m_z = value; }
    void set_w(T value) { m_w = value; }

    Vector4 operator+(const Vector4& other) const
    {
        return Vector4(m_x + other.m_x, m_y + other.m_y, m_z + other.m_z, m_w + other.m_w);
    }

    Vector4 operator-(const Vector4& other) const
    {
        return Vector4(m_x - other.m_x, m_y - other.m_y, m_z - other.m_z, m_w - other.m_w);
    }

    Vector4 operator*(T f) const
    {
        return Vector4(m_x * f, m_y * f, m_z * f, m_w * f);
    }

    Vector4 operator/(T f) const
    {
        return Vector4(m_x / f, m_y / f, m_z / f, m_w / f);
    }

    T dot(const Vector4& other) const
    {
        return m_x * other.m_x + m_y * other.m_y + m_z * other.m_z + m_w * other.m_w;
    }

    Vector4 normalized() const
    {
        T inv_length = 1 / length();
        return *this * inv_length;
    }

    Vector4 clamped(T m, T x) const
    {
        Vector4 copy { *this };
        copy.clamp(m, x);
        return copy;
    }

    void clamp(T min_value, T max_value)
    {
        m_x = max(min_value, m_x);
        m_y = max(min_value, m_y);
        m_z = max(min_value, m_z);
        m_w = max(min_value, m_w);
        m_x = min(max_value, m_x);
        m_y = min(max_value, m_y);
        m_z = min(max_value, m_z);
        m_w = min(max_value, m_w);
    }

    void normalize()
    {
        T inv_length = 1 / length();
        m_x *= inv_length;
        m_y *= inv_length;
        m_z *= inv_length;
        m_w *= inv_length;
    }

    T length() const
    {
        return sqrt(m_x * m_x + m_y * m_y + m_z * m_z + m_w * m_w);
    }

private:
    T m_x;
    T m_y;
    T m_z;
    T m_w;
};

typedef Vector4<float> FloatVector4;
typedef Vector4<double> DoubleVector4;

}

using Gfx::DoubleVector4;
using Gfx::FloatVector4;
using Gfx::Vector4;
