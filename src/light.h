#pragma once

#include "vec3.h"

class AreaLight {
public:

    Vec3 position;

    double size;

    Vec3 intensity;


    AreaLight(
        const Vec3& position,
        double size,
        const Vec3& intensity
    )
        : position(position),
          size(size),
          intensity(intensity) {}


    Vec3 sample(
        int x,
        int y,
        int samplesPerSide
    ) const {

        // Convert sample coordinates
        // into the range -0.5 to +0.5.

        double u =
            (static_cast<double>(x) + 0.5)
            / samplesPerSide
            - 0.5;

        double v =
            (static_cast<double>(y) + 0.5)
            / samplesPerSide
            - 0.5;


        // Create a square area light.
        //
        // X controls left/right.
        // Z controls front/back.

        return Vec3(
            position.x + u * size,
            position.y,
            position.z + v * size
        );
    }
};