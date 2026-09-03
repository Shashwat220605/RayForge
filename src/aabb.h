#pragma once

#include <algorithm>
#include <cmath>
#include <limits>

#include "ray.h"

class AABB {
public:
    Vec3 minimum;
    Vec3 maximum;

    AABB()
        : minimum(
            std::numeric_limits<double>::infinity(),
            std::numeric_limits<double>::infinity(),
            std::numeric_limits<double>::infinity()
          ),
          maximum(
            -std::numeric_limits<double>::infinity(),
            -std::numeric_limits<double>::infinity(),
            -std::numeric_limits<double>::infinity()
          ) {
    }

    AABB(
        const Vec3& minimum,
        const Vec3& maximum
    )
        : minimum(minimum),
          maximum(maximum) {
    }

    bool hit(
        const Ray& ray,
        double tMin = 0.001,
        double tMax = std::numeric_limits<double>::infinity()
    ) const {
        for (int axis = 0; axis < 3; axis++) {
            double origin;
            double direction;
            double minValue;
            double maxValue;

            if (axis == 0) {
                origin = ray.origin.x;
                direction = ray.direction.x;
                minValue = minimum.x;
                maxValue = maximum.x;
            }
            else if (axis == 1) {
                origin = ray.origin.y;
                direction = ray.direction.y;
                minValue = minimum.y;
                maxValue = maximum.y;
            }
            else {
                origin = ray.origin.z;
                direction = ray.direction.z;
                minValue = minimum.z;
                maxValue = maximum.z;
            }

            if (std::abs(direction) < 1e-12) {
                if (origin < minValue || origin > maxValue) {
                    return false;
                }

                continue;
            }

            double invD = 1.0 / direction;

            double t0 = (minValue - origin) * invD;
            double t1 = (maxValue - origin) * invD;

            if (invD < 0.0) {
                std::swap(t0, t1);
            }

            tMin = std::max(tMin, t0);
            tMax = std::min(tMax, t1);

            if (tMax <= tMin) {
                return false;
            }
        }

        return true;
    }

    static AABB surroundingBox(
        const AABB& a,
        const AABB& b
    ) {
        Vec3 small(
            std::min(a.minimum.x, b.minimum.x),
            std::min(a.minimum.y, b.minimum.y),
            std::min(a.minimum.z, b.minimum.z)
        );

        Vec3 large(
            std::max(a.maximum.x, b.maximum.x),
            std::max(a.maximum.y, b.maximum.y),
            std::max(a.maximum.z, b.maximum.z)
        );

        return AABB(small, large);
    }

    Vec3 centroid() const {
        return (minimum + maximum) * 0.5;
    }
};