#pragma once

#include <cmath>

class Vec3 {
public:

    double x;
    double y;
    double z;

    Vec3()
        : x(0.0),
          y(0.0),
          z(0.0) {}

    Vec3(
        double x,
        double y,
        double z
    )
        : x(x),
          y(y),
          z(z) {}


    // =========================
    // Addition
    // =========================

    Vec3 operator+(
        const Vec3& other
    ) const {

        return Vec3(
            x + other.x,
            y + other.y,
            z + other.z
        );
    }


    // =========================
    // Subtraction
    // =========================

    Vec3 operator-(
        const Vec3& other
    ) const {

        return Vec3(
            x - other.x,
            y - other.y,
            z - other.z
        );
    }


    // =========================
    // Multiplication
    // =========================

    Vec3 operator*(
        double value
    ) const {

        return Vec3(
            x * value,
            y * value,
            z * value
        );
    }


    // =========================
    // Division
    // =========================

    Vec3 operator/(
        double value
    ) const {

        return Vec3(
            x / value,
            y / value,
            z / value
        );
    }


    // =========================
    // Length
    // =========================

    double length() const {

        return std::sqrt(
            x * x +
            y * y +
            z * z
        );
    }


    // =========================
    // Normalize
    // =========================

    Vec3 normalized() const {

        double len = length();

        if (len == 0.0) {

            return Vec3(
                0.0,
                0.0,
                0.0
            );
        }

        return *this / len;
    }


    // =========================
    // Dot Product
    // =========================

    static double dot(
        const Vec3& a,
        const Vec3& b
    ) {

        return
            a.x * b.x +
            a.y * b.y +
            a.z * b.z;
    }


    // =========================
    // Cross Product
    // =========================

    static Vec3 cross(
        const Vec3& a,
        const Vec3& b
    ) {

        return Vec3(
            a.y * b.z - a.z * b.y,

            a.z * b.x - a.x * b.z,

            a.x * b.y - a.y * b.x
        );
    }
};