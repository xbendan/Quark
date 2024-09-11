#pragma once

#include <mixins/std/math.h>
#include <mixins/std/type_traits.h>

template <typename T, unsigned Dim>
    requires Std::isArithmetic<T>
struct Vec;

using Vec2D = Vec<f64, 2>;
using Vec3D = Vec<f64, 3>;

template <typename T>
    requires Std::isArithmetic<T>
struct Vec<T, 2>
{
    T x, y;

    Vec() = default;
    Vec(T x, T y)
        : x(x)
        , y(y)
    {
    }

    Vec operator+(const Vec& other) const
    {
        return Vec(x + other.x, y + other.y);
    }

    Vec operator-(const Vec& other) const
    {
        return Vec(x - other.x, y - other.y);
    }

    Vec operator*(const Vec& other) const
    {
        return Vec(x * other.x, y * other.y);
    }

    Vec operator/(const Vec& other) const
    {
        return Vec(x / other.x, y / other.y);
    }

    Vec operator+(T other) const { return Vec(x + other, y + other); }

    Vec operator-(T other) const { return Vec(x - other, y - other); }

    Vec operator*(T other) const { return Vec(x * other, y * other); }

    Vec operator/(T other) const { return Vec(x / other, y / other); }

    Vec& operator+=(const Vec& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vec& operator-=(const Vec& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vec& operator*=(const Vec& other)
    {
        x *= other.x;
        y *= other.y;
        return *this;
    }

    Vec& operator/=(const Vec& other)
    {
        x /= other.x;
        y /= other.y;
        return *this;
    }

    Vec& operator+=(T other)
    {
        x += other;
        y += other;
        return *this;
    }

    Vec& operator-=(T other)
    {
        x -= other;
        y -= other;
        return *this;
    }

    Vec& operator*=(T other)
    {
        x *= other;
        y *= other;
        return *this;
    }

    Vec& operator/=(T other)
    {
        x /= other;
        y /= other;
        return *this;
    }

    bool operator==(const Vec& other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Vec& other) const
    {
        return x != other.x || y != other.y;
    }

    T dot(const Vec& other) const { return x * other.x + y * other.y; }

    T length() const { return sqrt(x * x + y * y); }

    T lengthSquared() const { return x * x + y * y; }

    Vec normalize() const
    {
        T len = length();
        return Vec(x / len, y / len);
    }

    Vec reflect(const Vec& normal) const
    {
        return *this - normal * 2 * dot(normal);
    }

    Vec refract(const Vec& normal, T eta) const
    {
        T cosI = -dot(normal);
        T k    = 1 - eta * eta * (1 - cosI * cosI);
        if (k < 0) {
            return Vec(0, 0);
        } else {
            return *this * eta + normal * (eta * cosI - sqrt(k));
        }
    }

    Vec rotate(T angle) const
    {
        T s = sin(angle);
        T c = cos(angle);
        return Vec(x * c - y * s, x * s + y * c);
    }

    Vec lerp(const Vec& other, T t) const
    {
        return *this * (1 - t) + other * t;
    }

    Vec slerp(const Vec& other, T t) const
    {
        T dot        = this->dot(other);
        dot          = clamp(dot, -1, 1);
        T   theta    = acos(dot) * t;
        Vec relative = (other - *this * dot).normalize();
        return *this * cos(theta) + relative * sin(theta);
    }

    Vec nlerp(const Vec& other, T t) const
    {
        return lerp(other, t).normalize();
    }

    Vec nnlerp(const Vec& other, T t) const
    {
        return nlerp(other, t).normalize();
    }

    Vec project(const Vec& other) const
    {
        return other * dot(other) / other.lengthSquared();
    }

    Vec projectPlane(const Vec& normal) const
    {
        return *this - normal * dot(normal);
    }

    Vec projectPlane(const Vec& normal, const Vec& point) const
    {
        return point + projectPlane(normal);
    }

    Vec projectLine(const Vec& point, const Vec& direction) const
    {
        return point +
               direction * dot(*this - point) / direction.lengthSquared();
    }

    Vec projectRay(const Vec& point, const Vec& direction) const
    {
        return point +
               direction * dot(*this - point) / direction.dot(direction);
    }

    Vec projectSegment(const Vec& start, const Vec& end) const
    {
        Vec direction = end - start;
        T   length    = direction.length();
        return start + direction * dot(*this - start) / (length * length);
    }

    Vec projectSegment(const Vec& start, const Vec& end, T& t) const
    {
        Vec direction = end - start;
        T   length    = direction.length();
        t             = dot(*this - start) / (length * length);
        return start + direction * t;
    }

    Vec projectTriangle(const Vec& a, const Vec& b, const Vec& c) const
    {
        Vec normal = (b - a).cross(c - a);
        return *this - normal * normal.dot(*this - a) / normal.lengthSquared();
    }

    Vec projectTriangle(const Vec& a,
                        const Vec& b,
                        const Vec& c,
                        T&         u,
                        T&         v) const
    {
        Vec normal = (b - a).cross(c - a);
        Vec projection =
            *this - normal * normal.dot(*this - a) / normal.lengthSquared();
        Vec ab          = b - a;
        Vec ac          = c - a;
        Vec bc          = c - b;
        Vec ba          = a - b;
        Vec ca          = a - c;
        Vec cb          = b - c;
        Vec projectionA = projection - a;
        Vec projectionB = projection - b;
        Vec projectionC = projection - c;
        u               = 1 - projectionA.dot(ab) / ab.lengthSquared();
        v               = 1 - projectionB.dot(bc) / bc.lengthSquared();
        T w             = 1 - projectionC.dot(ca) / ca.lengthSquared();
        if (u < 0) {
            if (v < 0) {
                if (w < 0) {
                    return a;
                } else {
                    return c;
                }
            } else {
                if (w < 0) {
                    return b;
                } else {
                    return a;
                }
            }
        } else {
            if (v < 0) {
                if (w < 0) {
                    return c;
                } else {
                    return b;
                }
            } else {
                if (w < 0) {
                    return c;
                } else {
                    return projection;
                }
            }
        }
    }

    Vec projectTriangle(const Vec& a,
                        const Vec& b,
                        const Vec& c,
                        T&         u,
                        T&         v,
                        T&         w) const
    {
        Vec normal = (b - a).cross(c - a);
        Vec projection =
            *this - normal * normal.dot(*this - a) / normal.lengthSquared();
        Vec ab          = b - a;
        Vec ac          = c - a;
        Vec bc          = c - b;
        Vec ba          = a - b;
        Vec ca          = a - c;
        Vec cb          = b - c;
        Vec projectionA = projection - a;
        Vec projectionB = projection - b;
        Vec projectionC = projection - c;
        u               = 1 - projectionA.dot(ab) / ab.lengthSquared();
        v               = 1 - projectionB.dot(bc) / bc.lengthSquared();
        w               = 1 - projectionC.dot(ca) / ca.lengthSquared();
        if (u < 0) {
            if (v < 0) {
                if (w < 0) {
                    return a;
                } else {
                    return c;
                }
            } else {
                if (w < 0) {
                    return b;
                } else {
                    return a;
                }
            }
        } else {
            if (v < 0) {
                if (w < 0) {
                    return c;
                } else {
                    return b;
                }
            } else {
                if (w < 0) {
                    return c;
                } else {
                    return projection;
                }
            }
        }
    }
};

template <typename T>
    requires Std::isArithmetic<T>
struct Vec<T, 3>
{
    T x, y, z;

    Vec() = default;
    Vec(T x, T y, T z)
        : x(x)
        , y(y)
        , z(z)
    {
    }

    Vec operator+(const Vec& other) const
    {
        return Vec(x + other.x, y + other.y, z + other.z);
    }

    Vec operator-(const Vec& other) const
    {
        return Vec(x - other.x, y - other.y, z - other.z);
    }

    Vec operator*(const Vec& other) const
    {
        return Vec(x * other.x, y * other.y, z * other.z);
    }

    Vec operator/(const Vec& other) const
    {
        return Vec(x / other.x, y / other.y, z / other.z);
    }

    Vec operator+(T other) const
    {
        return Vec(x + other, y + other, z + other);
    }

    Vec operator-(T other) const
    {
        return Vec(x - other, y - other, z - other);
    }

    Vec operator*(T other) const
    {
        return Vec(x * other, y * other, z * other);
    }

    Vec operator/(T other) const
    {
        return Vec(x / other, y / other, z / other);
    }

    Vec& operator+=(const Vec& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vec& operator-=(const Vec& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    Vec& operator*=(const Vec& other)
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }

    Vec& operator/=(const Vec& other)
    {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        return *this;
    }

    Vec& operator+=(T other)
    {
        x += other;
        y += other;
        z += other;
        return *this;
    }

    Vec& operator-=(T other)
    {
        x -= other;
        y -= other;
        z -= other;
        return *this;
    }

    Vec& operator*=(T other)
    {
        x *= other;
        y *= other;
        z *= other;
        return *this;
    }

    Vec& operator/=(T other)
    {
        x /= other;
        y /= other;
        z /= other;
        return *this;
    }

    bool operator==(const Vec& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator!=(const Vec& other) const
    {
        return x != other.x || y != other.y || z != other.z;
    }

    T dot(const Vec& other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    Vec cross(const Vec& other) const
    {
        return Vec(y * other.z - z * other.y,
                   z * other.x - x * other.z,
                   x * other.y - y * other.x);
    }

    T length() const { return sqrt(x * x + y * y + z * z); }

    T lengthSquared() const { return x * x + y * y + z * z; }

    Vec normalize() const
    {
        T len = length();
        return Vec(x / len, y / len, z / len);
    }

    Vec reflect(const Vec& normal) const
    {
        return *this - normal * 2 * dot(normal);
    }

    Vec refract(const Vec& normal, T eta) const
    {
        T cosI = -dot(normal);
        T k    = 1 - eta * eta * (1 - cosI * cosI);
        if (k < 0) {
            return Vec(0, 0, 0);
        } else {
            return *this * eta + normal * (eta * cosI - sqrt(k));
        }
    }

    Vec rotate(const Vec& axis, T angle) const
    {
        T   s = sin(angle);
        T   c = cos(angle);
        T   t = 1 - c;
        Vec a = axis.normalize();
        return Vec(x * (t * a.x * a.x + c) + y * (t * a.x * a.y - s * a.z) +
                       z * (t * a.x * a.z + s * a.y),
                   x * (t * a.x * a.y + s * a.z) + y * (t * a.y * a.y + c) +
                       z * (t * a.y * a.z - s * a.x),
                   x * (t * a.x * a.z - s * a.y) +
                       y * (t * a.y * a.z + s * a.x) + z * (t * a.z * a.z + c));
    }

    Vec lerp(const Vec& other, T t) const
    {
        return *this * (1 - t) + other * t;
    }

    Vec slerp(const Vec& other, T t) const
    {
        T dot        = this->dot(other);
        dot          = clamp(dot, -1, 1);
        T   theta    = acos(dot) * t;
        Vec relative = (other - *this * dot).normalize();
        return *this * cos(theta) + relative * sin(theta);
    }

    Vec nlerp(const Vec& other, T t) const
    {
        return lerp(other, t).normalize();
    }

    Vec nnlerp(const Vec& other, T t) const
    {
        return nlerp(other, t).normalize();
    }

    Vec project(const Vec& other) const
    {
        return other * dot(other) / other.lengthSquared();
    }

    Vec projectPlane(const Vec& normal) const
    {
        return *this - normal * dot(normal);
    }

    Vec projectPlane(const Vec& normal, const Vec& point) const
    {
        return point + projectPlane(normal);
    }

    Vec projectLine(const Vec& point, const Vec& direction) const
    {
        return point +
               direction * dot(*this - point) / direction.lengthSquared();
    }

    Vec projectRay(const Vec& point, const Vec& direction) const
    {
        return point +
               direction * dot(*this - point) / direction.dot(direction);
    }

    Vec projectSegment(const Vec& start, const Vec& end) const
    {
        Vec direction = end - start;
        T   length    = direction.length();
        return start + direction * dot(*this - start) / (length * length);
    }

    Vec projectSegment(const Vec& start, const Vec& end, T& t) const
    {
        Vec direction = end - start;
        T   length    = direction.length();
        t             = dot(*this - start) / (length * length);
        return start + direction * t;
    }

    Vec projectTriangle(const Vec& a, const Vec& b, const Vec& c) const
    {
        Vec normal = (b - a).cross(c - a);
        return *this - normal * normal.dot(*this - a) / normal.lengthSquared();
    }

    Vec projectTriangle(const Vec& a,
                        const Vec& b,
                        const Vec& c,
                        T&         u,
                        T&         v) const
    {
        Vec normal = (b - a).cross(c - a);
        Vec projection =
            *this - normal * normal.dot(*this - a) / normal.lengthSquared();
        Vec ab          = b - a;
        Vec ac          = c - a;
        Vec bc          = c - b;
        Vec ba          = a - b;
        Vec ca          = a - c;
        Vec cb          = b - c;
        Vec projectionA = projection - a;
        Vec projectionB = projection - b;
        Vec projectionC = projection - c;
        u               = 1 - projectionA.dot(ab) / ab.lengthSquared();
        v               = 1 - projectionB.dot(bc) / bc.lengthSquared();
        T w             = 1 - projectionC.dot(ca) / ca.lengthSquared();
        if (u < 0) {
            if (v < 0) {
                if (w < 0) {
                    return a;
                } else {
                    return c;
                }
            } else {
                if (w < 0) {
                    return b;
                } else {
                    return a;
                }
            }
        } else {
            if (v < 0) {
                if (w < 0) {
                    return c;
                } else {
                    return b;
                }
            } else {
                if (w < 0) {
                    return c;
                } else {
                    return projection;
                }
            }
        }
    }
};