#ifndef VECTOR2_H
#define VECTOR2_H

#include <functional>

template<typename T>
struct Vector2 {
    T x, y;

    Vector2() : x(0), y(0) { }

    Vector2(T x, T y) : x(x), y(y) { }

    template<typename U>
    explicit Vector2(const Vector2<U> &other)
        : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) { }

    Vector2 operator+(const Vector2 &o) const { return {x + o.x, y + o.y}; }
    Vector2 operator-(const Vector2 &o) const { return {x - o.x, y - o.y}; }
    Vector2 operator*(T s) const { return {x * s, y * s}; }

    [[nodiscard]] T dot(const Vector2 &o) const { return x * o.x + y * o.y; }
    [[nodiscard]] T magnitude() const { return std::sqrt(x * x + y * y); }

    [[nodiscard]] Vector2 normalized() const {
        T m = magnitude();
        return (m > 0) ? Vector2(x / m, y / m) : Vector2();
    }

    bool operator==(const Vector2 &o) const {
        return x == o.x && y == o.y;
    }

    bool operator!=(const Vector2 &o) const {
        return !(*this == o);
    }
};

namespace std {
    template <>
    struct hash<Vector2<int>> {
        size_t operator()(const Vector2<int>& v) const noexcept {
            return (static_cast<size_t>(v.x) << 16) |
                   static_cast<size_t>(v.y);
        }
    };
}

#endif
