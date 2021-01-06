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
 * Date: 2/4/2020
 */
#pragma once

#include "vec4.h"

#include <AK/String.h>
#include <string.h>
#include <stdio.h>

class Vec4;

class Mat4 final
{
public:
    Mat4(bool identity = true)
    {
        if(identity)
            load_identity();
    }

    Mat4(float val) { memset(m_data, val, sizeof(float) * 16); }

    Mat4(const float data[4][4]) { memcpy(m_data, data, sizeof(float) * 16); }

    Mat4 operator+(const Mat4& rhs)
    {
        Mat4 ret;

        ret.m_data[0][0] = m_data[0][0] + rhs.m_data[0][0];
        ret.m_data[0][1] = m_data[0][1] + rhs.m_data[0][1];
        ret.m_data[0][2] = m_data[0][2] + rhs.m_data[0][2];
        ret.m_data[0][3] = m_data[0][3] + rhs.m_data[0][3];

        ret.m_data[1][0] = m_data[1][0] + rhs.m_data[1][0];
        ret.m_data[1][1] = m_data[1][1] + rhs.m_data[1][1];
        ret.m_data[1][2] = m_data[1][2] + rhs.m_data[1][2];
        ret.m_data[1][3] = m_data[1][3] + rhs.m_data[1][3];

        ret.m_data[2][0] = m_data[2][0] + rhs.m_data[2][0];
        ret.m_data[2][1] = m_data[2][1] + rhs.m_data[2][1];
        ret.m_data[2][2] = m_data[2][2] + rhs.m_data[2][2];
        ret.m_data[2][3] = m_data[2][3] + rhs.m_data[2][3];

        ret.m_data[3][0] = m_data[3][0] + rhs.m_data[3][0];
        ret.m_data[3][1] = m_data[3][1] + rhs.m_data[3][1];
        ret.m_data[3][2] = m_data[3][2] + rhs.m_data[3][2];
        ret.m_data[3][3] = m_data[3][3] + rhs.m_data[3][3];

        return ret;
    }

    Mat4& operator+=(const Mat4& rhs)
    {
        m_data[0][0] += rhs.m_data[0][0];
        m_data[0][1] += rhs.m_data[0][1];
        m_data[0][2] += rhs.m_data[0][2];
        m_data[0][3] += rhs.m_data[0][3];

        m_data[1][0] += rhs.m_data[1][0];
        m_data[1][1] += rhs.m_data[1][1];
        m_data[1][2] += rhs.m_data[1][2];
        m_data[1][3] += rhs.m_data[1][3];

        m_data[2][0] += rhs.m_data[2][0];
        m_data[2][1] += rhs.m_data[2][1];
        m_data[2][2] += rhs.m_data[2][2];
        m_data[2][3] += rhs.m_data[2][3];

        m_data[3][0] += rhs.m_data[3][0];
        m_data[3][1] += rhs.m_data[3][1];
        m_data[3][2] += rhs.m_data[3][2];
        m_data[3][3] += rhs.m_data[3][3];

        return *this;
    }

    Mat4 operator-(const Mat4& rhs)
    {
        Mat4 ret;


        ret.m_data[0][0] = m_data[0][0] - rhs.m_data[0][0];
        ret.m_data[0][1] = m_data[0][1] - rhs.m_data[0][1];
        ret.m_data[0][2] = m_data[0][2] - rhs.m_data[0][2];
        ret.m_data[0][3] = m_data[0][3] - rhs.m_data[0][3];

        ret.m_data[1][0] = m_data[1][0] - rhs.m_data[1][0];
        ret.m_data[1][1] = m_data[1][1] - rhs.m_data[1][1];
        ret.m_data[1][2] = m_data[1][2] - rhs.m_data[1][2];
        ret.m_data[1][3] = m_data[1][3] - rhs.m_data[1][3];

        ret.m_data[2][0] = m_data[2][0] - rhs.m_data[2][0];
        ret.m_data[2][1] = m_data[2][1] - rhs.m_data[2][1];
        ret.m_data[2][2] = m_data[2][2] - rhs.m_data[2][2];
        ret.m_data[2][3] = m_data[2][3] - rhs.m_data[2][3];

        ret.m_data[3][0] = m_data[3][0] - rhs.m_data[3][0];
        ret.m_data[3][1] = m_data[3][1] - rhs.m_data[3][1];
        ret.m_data[3][2] = m_data[3][2] - rhs.m_data[3][2];
        ret.m_data[3][3] = m_data[3][3] - rhs.m_data[3][3];

        return ret;
    }

    Mat4& operator-=(const Mat4& rhs)
    {
        m_data[0][0] -= rhs.m_data[0][0];
        m_data[0][1] -= rhs.m_data[0][1];
        m_data[0][2] -= rhs.m_data[0][2];
        m_data[0][3] -= rhs.m_data[0][3];

        m_data[1][0] -= rhs.m_data[1][0];
        m_data[1][1] -= rhs.m_data[1][1];
        m_data[1][2] -= rhs.m_data[1][2];
        m_data[1][3] -= rhs.m_data[1][3];

        m_data[2][0] -= rhs.m_data[2][0];
        m_data[2][1] -= rhs.m_data[2][1];
        m_data[2][2] -= rhs.m_data[2][2];
        m_data[2][2] -= rhs.m_data[2][2];

        m_data[3][0] -= rhs.m_data[3][0];
        m_data[3][1] -= rhs.m_data[3][1];
        m_data[3][2] -= rhs.m_data[3][2];
        m_data[3][2] -= rhs.m_data[3][2];

        return *this;
    }

    // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0472k/chr1360928373893.html
    // https://github.com/thenifty/neon-guide
    Mat4 operator*(const Mat4& rhs)
    {
        Mat4 ret;

        ret.m_data[0][0] = (m_data[0][0] * rhs.m_data[0][0]) + (m_data[0][1] * rhs.m_data[1][0]) + (m_data[0][2] * rhs.m_data[2][0]) + (m_data[0][3] * rhs.m_data[3][0]);
        ret.m_data[0][1] = (m_data[0][0] * rhs.m_data[0][1]) + (m_data[0][1] * rhs.m_data[1][1]) + (m_data[0][2] * rhs.m_data[2][1]) + (m_data[0][3] * rhs.m_data[3][1]);
        ret.m_data[0][2] = (m_data[0][0] * rhs.m_data[0][2]) + (m_data[0][1] * rhs.m_data[1][2]) + (m_data[0][2] * rhs.m_data[2][2]) + (m_data[0][3] * rhs.m_data[3][2]);
        ret.m_data[0][3] = (m_data[0][0] * rhs.m_data[0][3]) + (m_data[0][1] * rhs.m_data[1][3]) + (m_data[0][2] * rhs.m_data[2][3]) + (m_data[0][3] * rhs.m_data[3][3]);

        ret.m_data[1][0] = (m_data[1][0] * rhs.m_data[0][0]) + (m_data[1][1] * rhs.m_data[1][0]) + (m_data[1][2] * rhs.m_data[2][0]) + (m_data[1][3] * rhs.m_data[3][0]);
        ret.m_data[1][1] = (m_data[1][0] * rhs.m_data[0][1]) + (m_data[1][1] * rhs.m_data[1][1]) + (m_data[1][2] * rhs.m_data[2][1]) + (m_data[1][3] * rhs.m_data[3][1]);
        ret.m_data[1][2] = (m_data[1][0] * rhs.m_data[0][2]) + (m_data[1][1] * rhs.m_data[1][2]) + (m_data[1][2] * rhs.m_data[2][2]) + (m_data[1][3] * rhs.m_data[3][2]);
        ret.m_data[1][3] = (m_data[1][0] * rhs.m_data[0][3]) + (m_data[1][1] * rhs.m_data[1][3]) + (m_data[1][2] * rhs.m_data[2][3]) + (m_data[1][3] * rhs.m_data[3][3]);

        ret.m_data[2][0] = (m_data[2][0] * rhs.m_data[0][0]) + (m_data[2][1] * rhs.m_data[1][0]) + (m_data[2][2] * rhs.m_data[2][0]) + (m_data[2][3] * rhs.m_data[3][0]);
        ret.m_data[2][1] = (m_data[2][0] * rhs.m_data[0][1]) + (m_data[2][1] * rhs.m_data[1][1]) + (m_data[2][2] * rhs.m_data[2][1]) + (m_data[2][3] * rhs.m_data[3][1]);
        ret.m_data[2][2] = (m_data[2][0] * rhs.m_data[0][2]) + (m_data[2][1] * rhs.m_data[1][2]) + (m_data[2][2] * rhs.m_data[2][2]) + (m_data[2][3] * rhs.m_data[3][2]);
        ret.m_data[2][3] = (m_data[2][0] * rhs.m_data[0][3]) + (m_data[2][1] * rhs.m_data[1][3]) + (m_data[2][2] * rhs.m_data[2][3]) + (m_data[2][3] * rhs.m_data[3][3]);

        ret.m_data[3][0] = (m_data[3][0] * rhs.m_data[0][0]) + (m_data[3][1] * rhs.m_data[1][0]) + (m_data[3][2] * rhs.m_data[2][0]) + (m_data[3][3] * rhs.m_data[3][0]);
        ret.m_data[3][1] = (m_data[3][0] * rhs.m_data[0][1]) + (m_data[3][1] * rhs.m_data[1][1]) + (m_data[3][2] * rhs.m_data[2][1]) + (m_data[3][3] * rhs.m_data[3][1]);
        ret.m_data[3][2] = (m_data[3][0] * rhs.m_data[0][2]) + (m_data[3][1] * rhs.m_data[1][2]) + (m_data[3][2] * rhs.m_data[2][2]) + (m_data[3][3] * rhs.m_data[3][2]);
        ret.m_data[3][3] = (m_data[3][0] * rhs.m_data[0][3]) + (m_data[3][1] * rhs.m_data[1][3]) + (m_data[3][2] * rhs.m_data[2][3]) + (m_data[3][3] * rhs.m_data[3][3]);
        return ret;
    }

    Vec4 operator*(const Vec4& rhs)
    {
        Vec4 ret;

        ret.x(rhs.x() * m_data[0][0] + rhs.y() * m_data[1][0] + rhs.z() * m_data[2][0] + rhs.w() * m_data[3][0]);
        ret.y(rhs.x() * m_data[0][1] + rhs.y() * m_data[1][1] + rhs.z() * m_data[2][1] + rhs.w() * m_data[3][1]);
        ret.z(rhs.x() * m_data[0][2] + rhs.y() * m_data[1][2] + rhs.z() * m_data[2][2] + rhs.w() * m_data[3][2]);
        ret.w(rhs.x() * m_data[0][3] + rhs.y() * m_data[1][3] + rhs.z() * m_data[2][3] + rhs.w() * m_data[3][3]);

        return ret;
    }

    Mat4& operator*=(float val)
    {
        m_data[0][0] *= val;
        m_data[0][1] *= val;
        m_data[0][2] *= val;
        m_data[0][3] *= val;

        m_data[1][0] *= val;
        m_data[1][1] *= val;
        m_data[1][2] *= val;
        m_data[1][3] *= val;

        m_data[2][0] *= val;
        m_data[2][1] *= val;
        m_data[2][2] *= val;
        m_data[2][3] *= val;

        m_data[3][0] *= val;
        m_data[3][1] *= val;
        m_data[3][2] *= val;
        m_data[3][3] *= val;

        return *this;
    }

    float operator()(int col, int row) const { return m_data[col][row]; }

    void operator()(int col, int row, float val) { m_data[col][row] = val; }

    void print() const
    {
        for(int row = 0; row < 4; row++)
        {
            printf("[ ");
            for(int col = 0; col < 4; col++)
            {
                printf("%.4f ", m_data[col][row]);
            }

            printf("]\n");
        }
    }

    void load_identity()
    {
        memset(&m_data, 0, 16 * sizeof(float));

        m_data[0][0] = 1.0f;
        m_data[1][1] = 1.0f;
        m_data[2][2] = 1.0f;
        m_data[3][3] = 1.0f;
    }

private:
    float m_data[4][4] = { { 0 } }; // row x column
};
