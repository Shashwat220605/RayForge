#pragma once

#include <memory>
#include <cmath>

#include "hittable.h"
#include "material.h"

class Plane : public Hittable {
public:

    Vec3 point;
    Vec3 normal;

    std::shared_ptr<Material> material;

    Plane(
        const Vec3& point,
        const Vec3& normal,
        std::shared_ptr<Material> material
    )
        : point(point),
          normal(normal.normalized()),
          material(material) {
    }

    bool hit(
        const Ray& ray,
        HitRecord& record
    ) const override {

        double denominator =
            Vec3::dot(
                normal,
                ray.direction
            );

        if (
            std::abs(denominator)
            < 0.000001
        ) {
            return false;
        }

        double t =
            Vec3::dot(
                point - ray.origin,
                normal
            ) / denominator;

        if (t < 0.001) {
            return false;
        }

        record.t = t;

        record.point =
            ray.at(t);

        record.normal =
            normal;

        record.material =
            material;

        return true;
    }

    bool boundingBox(
        AABB& outputBox
    ) const override {

        // Infinite plane has no finite AABB.
        return false;
    }
};