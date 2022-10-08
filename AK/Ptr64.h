/*
 * Copyright (c) 2022, Jesse Buhagiar <jesse.buhagiar@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Assertions.h>
#include <AK/Types.h>

namespace AK {

template<typename T>
class Ptr64 {
public:
    constexpr Ptr64() = default;
    Ptr64(T* const ptr)
        : m_ptr((u64) reinterpret_cast<FlatPtr>(ptr))
    {
        VERIFY((reinterpret_cast<FlatPtr>(ptr) & 0xFFFFFFFFFFFFFFFFULL) == static_cast<FlatPtr>(m_ptr));
    }
    T& operator*() { return *static_cast<T*>(*this); }
    T const& operator*() const { return *static_cast<T const*>(*this); }

    T* operator->() { return *this; }
    T const* operator->() const { return *this; }

    operator T*() { return reinterpret_cast<T*>(static_cast<FlatPtr>(m_ptr)); }
    operator T const*() const { return reinterpret_cast<T const*>(static_cast<FlatPtr>(m_ptr)); }

    T& operator[](size_t index) { return static_cast<T*>(*this)[index]; }
    T const& operator[](size_t index) const { return static_cast<T const*>(*this)[index]; }

    constexpr explicit operator bool() { return m_ptr; }
    template<typename U>
    constexpr bool operator==(Ptr64<U> other) { return m_ptr == other.m_ptr; }

    constexpr Ptr64<T> operator+(u64 other) const
    {
        Ptr64<T> ptr {};
        ptr.m_ptr = m_ptr + other;
        return ptr;
    }
    constexpr Ptr64<T> operator-(u64 other) const
    {
        Ptr64<T> ptr {};
        ptr.m_ptr = m_ptr - other;
        return ptr;
    }

private:
    u64 m_ptr { 0 };
};

}

using AK::Ptr64;
