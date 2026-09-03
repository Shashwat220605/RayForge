#pragma once

#include <algorithm>
#include <cmath>
#include <memory>

#include "hittable.h"
#include "material.h"

class Triangle : public Hittable {
public:

    Vec3 v0;
    Vec3 v1;
    Vec3 v2;

    std::shared_ptr<Material> material;

    Triangle(
        const Vec3& v0,
        const Vec3& v1,
        const Vec3& v2,
        std::shared_ptr<Material> material
    )
        : v0(v0),
          v1(v1),
          v2(v2),
          material(material) {
    }

    bool hit(
        const Ray& ray,
        HitRecord& record
    ) const override {

        const double epsilon = 1e-8;

        Vec3 edge1 =
            v1 - v0;

        Vec3 edge2 =
            v2 - v0;

        Vec3 h =
            Vec3::cross(
                ray.direction,
                edge2
            );

        double a =
            Vec3::dot(
                edge1,
                h
            );

        if (std::abs(a) < epsilon) {
            return false;
        }

        double f =
            1.0 / a;

        Vec3 s =
            ray.origin - v0;

        double u =
            f *
            Vec3::dot(
                s,
                h
            );

        if (u < 0.0 || u > 1.0) {
            return false;
        }

        Vec3 q =
            Vec3::cross(
                s,
                edge1
            );

        double v =
            f *
            Vec3::dot(
                ray.direction,
                q
            );

        if (
            v < 0.0 ||
            u + v > 1.0
        ) {
            return false;
        }

        double t =
            f *
            Vec3::dot(
                edge2,
                q
            );

        if (t < 0.001) {
            return false;
        }

        record.t = t;

        record.point =
            ray.at(t);

        Vec3 normal =
            Vec3::cross(
                edge1,
                edge2
            ).normalized();

        record.normal =
            normal;

        record.material =
            material;

        return true;
    }

    bool boundingBox(
        AABB& outputBox
    ) const override {

        const double padding = 0.0001;

        Vec3 minimum(
            std::min(
                v0.x,
                std::min(v1.x, v2.x)
            ) - padding,

            std::min(
                v0.y,
                std::min(v1.y, v2.y)
            ) - padding,

            std::min(
                v0.z,
                std::min(v1.z, v2.z)
            ) - padding
        );

        Vec3 maximum(
            std::max(
                v0.x,
                std::max(v1.x, v2.x)
            ) + padding,

            std::max(
                v0.y,
                std::max(v1.y, v2.y)
            ) + padding,

            std::max(
                v0.z,
                std::max(v1.z, v2.z)
            ) + padding
        );

        outputBox =
            AABB(
                minimum,
                maximum
            );

        return true;
    }
};