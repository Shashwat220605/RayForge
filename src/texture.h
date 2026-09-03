#pragma once

#include <cmath>

#include "vec3.h"

class Texture {
public:

    // ========================================================
    // CHECKERBOARD
    // ========================================================

    static Vec3 checker(
        double u,
        double v,
        const Vec3& colorA,
        const Vec3& colorB,
        double scale
    ) {

        int x =
            static_cast<int>(
                std::floor(u * scale)
            );

        int y =
            static_cast<int>(
                std::floor(v * scale)
            );

        if (
            (x + y) % 2 == 0
        ) {
            return colorA;
        }

        return colorB;
    }


    // ========================================================
    // SPHERICAL UV
    // ========================================================

    static void sphericalUV(
        const Vec3& normal,
        double& u,
        double& v
    ) {

        Vec3 n =
            normal.normalized();

        const double pi =
            3.14159265358979323846;

        u =
            0.5
            +
            std::atan2(
                n.z,
                n.x
            )
            / (2.0 * pi);

        v =
            0.5
            -
            std::asin(
                n.y
            )
            / pi;
    }


    // ========================================================
    // SPHERE CHECKER
    // ========================================================

    static Vec3 sphereChecker(
        const Vec3& normal,
        const Vec3& colorA,
        const Vec3& colorB,
        double scale
    ) {

        double u;
        double v;

        sphericalUV(
            normal,
            u,
            v
        );

        return checker(
            u,
            v,
            colorA,
            colorB,
            scale
        );
    }


    // ========================================================
    // PLANAR CHECKER
    // ========================================================

    static Vec3 planeChecker(
        const Vec3& point,
        const Vec3& colorA,
        const Vec3& colorB,
        double scale
    ) {

        int x =
            static_cast<int>(
                std::floor(
                    point.x * scale
                )
            );

        int z =
            static_cast<int>(
                std::floor(
                    point.z * scale
                )
            );

        if (
            (x + z) % 2 == 0
        ) {
            return colorA;
        }

        return colorB;
    }
};